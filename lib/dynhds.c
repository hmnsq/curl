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

ساكن بنية dynhds_ثبت *ثبت_new(ثابت حرف *name, size_t namelen,
                                      ثابت حرف *ارزښت, size_t valuelen,
                                      صحيح opts)
{
  بنية dynhds_ثبت *e;
  حرف *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  إذا((SIZE_MAX - namelen - valuelen) < (حجم(*e) + 2))
    أرجع عدم;
  e = curlx_calloc(1, حجم(*e) + namelen + valuelen + 2);
  إذا(!e)
    أرجع عدم;
  e->name = p = (حرف *)e + حجم(*e);
  حافظه_نقل(p, name, namelen);
  e->namelen = namelen;
  e->ارزښت = p += namelen + 1; /* leave a \0 at the end of name */
  إذا(valuelen) {
    حافظه_نقل(p, ارزښت, valuelen);
    e->valuelen = valuelen;
  }
  إذا(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  أرجع e;
}

ساكن فراغ ثبت_free(بنية dynhds_ثبت *e)
{
  curlx_free(e);
}

فراغ Curl_dynhds_init(بنية dynhds *dynhds, size_t max_entries,
                      size_t max_strs_اندازه)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_اندازه);
  dynhds->hds = عدم;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_اندازه = max_strs_اندازه;
  dynhds->opts = 0;
}

فراغ Curl_dynhds_free(بنية dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  إذا(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    لكل(i = 0; i < dynhds->hds_len; ++i) {
      ثبت_free(dynhds->hds[i]);
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
      ثبت_free(dynhds->hds[i]);
      dynhds->hds[i] = عدم;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_شمېر(بنية dynhds *dynhds)
{
  أرجع dynhds->hds_len;
}

فراغ Curl_dynhds_سېټ_opts(بنية dynhds *dynhds, صحيح opts)
{
  dynhds->opts = opts;
}

بنية dynhds_ثبت *Curl_dynhds_getn(بنية dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  أرجع (n < dynhds->hds_len) ? dynhds->hds[n] : عدم;
}

بنية dynhds_ثبت *Curl_dynhds_get(بنية dynhds *dynhds, ثابت حرف *name,
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

بنية dynhds_ثبت *Curl_dynhds_cget(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع Curl_dynhds_get(dynhds, name, مزي_اوږدوالی(name));
}

CURLcode Curl_dynhds_جمع_کړه(بنية dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *ارزښت, size_t valuelen)
{
  بنية dynhds_ثبت *ثبت = عدم;
  CURLcode پایله = CURLE_بهر_د_حافظه;

  /* Accept NULL value with length 0, name must have length */
  إذا(!dynhds || !name || !namelen || (!ارزښت && valuelen)) {
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_دلیل;
  }
  /* Does the new header fit into the limits? */
  إذا((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_اندازه - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_اندازه - dynhds->strs_len - namelen < valuelen))
    أرجع CURLE_بهر_د_حافظه;

  ثبت = ثبت_new(name, namelen, ارزښت, valuelen, dynhds->opts);
  إذا(!ثبت)
    اذهب بهر;

  إذا(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    بنية dynhds_ثبت **nhds;

    إذا(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, حجم(بنية dynhds_ثبت *));
    إذا(!nhds)
      اذهب بهر;
    إذا(dynhds->hds) {
      حافظه_نقل(nhds, dynhds->hds,
             dynhds->hds_len * حجم(بنية dynhds_ثبت *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = ثبت;
  ثبت = عدم;
  dynhds->strs_len += namelen + valuelen;
  پایله = CURLE_OK;

بهر:
  إذا(ثبت)
    ثبت_free(ثبت);
  أرجع پایله;
}

CURLcode Curl_dynhds_cadd(بنية dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *ارزښت)
{
  إذا(!name)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  أرجع Curl_dynhds_جمع_کړه(dynhds, name, مزي_اوږدوالی(name),
                         ارزښت, ارزښت ? مزي_اوږدوالی(ارزښت) : 0);
}

CURLcode Curl_dynhds_h1_جمع_کړه_کرښه(بنية dynhds *dynhds,
                                 ثابت حرف *کرښه, size_t کرښه_len)
{
  ثابت حرف *p;
  ثابت حرف *name;
  size_t namelen;
  ثابت حرف *ارزښت;
  size_t valuelen, i;

  إذا(!کرښه || !کرښه_len)
    أرجع CURLE_OK;

  p = memchr(کرښه, ':', کرښه_len);
  إذا(!p || p == کرښه)
    أرجع CURLE_BAD_FUNCTION_دلیل;
  name = کرښه;
  namelen = p - کرښه;
  p++; /* move past the colon */
  لكل(i = namelen + 1; i < کرښه_len; ++i, ++p) {
    إذا(!ISBLANK(*p))
      اكسر;
  }
  ارزښت = p;
  valuelen = کرښه_len - i;

  p = memchr(ارزښت, '\r', valuelen);
  إذا(!p)
    p = memchr(ارزښت, '\n', valuelen);
  إذا(p)
    valuelen = (size_t)(p - ارزښت);

  أرجع Curl_dynhds_جمع_کړه(dynhds, name, namelen, ارزښت, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_کرښه(بنية dynhds *dynhds, ثابت حرف *کرښه)
{
  أرجع Curl_dynhds_h1_جمع_کړه_کرښه(dynhds, کرښه, کرښه ? مزي_اوږدوالی(کرښه) : 0);
}

#که_تعریف_شوی UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_لري(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST bool dynhds_لري(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  أرجع !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_لري(dynhds, name, مزي_اوږدوالی(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_شمېر_name(بنية dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_شمېر_name(بنية dynhds *dynhds,
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
  أرجع dynhds_شمېر_name(dynhds, name, مزي_اوږدوالی(name));
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
        ثبت_free(dynhds->hds[i]);
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
UNITTEST CURLcode dynhds_سېټ(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *ارزښت, size_t valuelen);
UNITTEST CURLcode dynhds_سېټ(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *ارزښت, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  أرجع Curl_dynhds_جمع_کړه(dynhds, name, namelen, ارزښت, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_remove(dynhds, name, مزي_اوږدوالی(name));
}

#د_که_پای /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(بنية dynhds *dynhds, بنية dynbuf *dbuf)
{
  CURLcode پایله = CURLE_OK;
  size_t i;

  إذا(!dynhds->hds_len)
    أرجع پایله;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    پایله = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    إذا(!پایله)
      پایله = curlx_dyn_addn(dbuf, STRCONST(": "));
    إذا(!پایله)
      پایله = curlx_dyn_addn(dbuf, dynhds->hds[i]->ارزښت,
                              dynhds->hds[i]->valuelen);
    إذا(!پایله)
      پایله = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    إذا(پایله)
      اكسر;
  }

  أرجع پایله;
}

#که_تعریف_شوی USE_NGHTTP2

nghttp2_nv *Curl_dynhds_منزل_nva(بنية dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, حجم(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  إذا(!nva)
    أرجع عدم;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    بنية dynhds_ثبت *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (بینښه حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].ارزښت = (بینښه حرف *)e->ارزښت;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  أرجع nva;
}

#د_که_پای /* USE_NGHTTP2 */
