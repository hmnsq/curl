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

ساكن بنية dynhds_مدخل *مدخل_new(ثابت حرف *name, size_t namelen,
                                      ثابت حرف *مقدار, size_t valuelen,
                                      صحيح opts)
{
  بنية dynhds_مدخل *e;
  حرف *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  إذا((SIZE_MAX - namelen - valuelen) < (حجم(*e) + 2))
    أرجع عدم;
  e = curlx_calloc(1, حجم(*e) + namelen + valuelen + 2);
  إذا(!e)
    أرجع عدم;
  e->name = p = (حرف *)e + حجم(*e);
  کپی_حافظه(p, name, namelen);
  e->namelen = namelen;
  e->مقدار = p += namelen + 1; /* leave a \0 at the end of name */
  إذا(valuelen) {
    کپی_حافظه(p, مقدار, valuelen);
    e->valuelen = valuelen;
  }
  إذا(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  أرجع e;
}

ساكن فراغ مدخل_free(بنية dynhds_مدخل *e)
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
      مدخل_free(dynhds->hds[i]);
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
      مدخل_free(dynhds->hds[i]);
      dynhds->hds[i] = عدم;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_شمار(بنية dynhds *dynhds)
{
  أرجع dynhds->hds_len;
}

فراغ Curl_dynhds_مجموعه_opts(بنية dynhds *dynhds, صحيح opts)
{
  dynhds->opts = opts;
}

بنية dynhds_مدخل *Curl_dynhds_getn(بنية dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  أرجع (n < dynhds->hds_len) ? dynhds->hds[n] : عدم;
}

بنية dynhds_مدخل *Curl_dynhds_get(بنية dynhds *dynhds, ثابت حرف *name,
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

بنية dynhds_مدخل *Curl_dynhds_cget(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع Curl_dynhds_get(dynhds, name, طول_رشته(name));
}

CURLcode Curl_dynhds_جمع_کن(بنية dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *مقدار, size_t valuelen)
{
  بنية dynhds_مدخل *مدخل = عدم;
  CURLcode نتیجه = CURLE_بیرون_از_حافظه;

  /* Accept NULL value with length 0, name must have length */
  إذا(!dynhds || !name || !namelen || (!مقدار && valuelen)) {
    DEBUGASSERT(0);
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  }
  /* Does the new header fit into the limits? */
  إذا((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_اندازه - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_اندازه - dynhds->strs_len - namelen < valuelen))
    أرجع CURLE_بیرون_از_حافظه;

  مدخل = مدخل_new(name, namelen, مقدار, valuelen, dynhds->opts);
  إذا(!مدخل)
    اذهب بیرون;

  إذا(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    بنية dynhds_مدخل **nhds;

    إذا(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, حجم(بنية dynhds_مدخل *));
    إذا(!nhds)
      اذهب بیرون;
    إذا(dynhds->hds) {
      کپی_حافظه(nhds, dynhds->hds,
             dynhds->hds_len * حجم(بنية dynhds_مدخل *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = مدخل;
  مدخل = عدم;
  dynhds->strs_len += namelen + valuelen;
  نتیجه = CURLE_OK;

بیرون:
  إذا(مدخل)
    مدخل_free(مدخل);
  أرجع نتیجه;
}

CURLcode Curl_dynhds_cadd(بنية dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *مقدار)
{
  إذا(!name)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  أرجع Curl_dynhds_جمع_کن(dynhds, name, طول_رشته(name),
                         مقدار, مقدار ? طول_رشته(مقدار) : 0);
}

CURLcode Curl_dynhds_h1_جمع_کن_سطر(بنية dynhds *dynhds,
                                 ثابت حرف *سطر, size_t سطر_len)
{
  ثابت حرف *p;
  ثابت حرف *name;
  size_t namelen;
  ثابت حرف *مقدار;
  size_t valuelen, i;

  إذا(!سطر || !سطر_len)
    أرجع CURLE_OK;

  p = memchr(سطر, ':', سطر_len);
  إذا(!p || p == سطر)
    أرجع CURLE_BAD_FUNCTION_آرگومان;
  name = سطر;
  namelen = p - سطر;
  p++; /* move past the colon */
  لكل(i = namelen + 1; i < سطر_len; ++i, ++p) {
    إذا(!ISBLANK(*p))
      اكسر;
  }
  مقدار = p;
  valuelen = سطر_len - i;

  p = memchr(مقدار, '\r', valuelen);
  إذا(!p)
    p = memchr(مقدار, '\n', valuelen);
  إذا(p)
    valuelen = (size_t)(p - مقدار);

  أرجع Curl_dynhds_جمع_کن(dynhds, name, namelen, مقدار, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_سطر(بنية dynhds *dynhds, ثابت حرف *سطر)
{
  أرجع Curl_dynhds_h1_جمع_کن_سطر(dynhds, سطر, سطر ? طول_رشته(سطر) : 0);
}

#اگر_تعریف_شده UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_شامل_است(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST bool dynhds_شامل_است(بنية dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  أرجع !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST bool dynhds_ccontains(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_شامل_است(dynhds, name, طول_رشته(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_شمار_name(بنية dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_شمار_name(بنية dynhds *dynhds,
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
  أرجع dynhds_شمار_name(dynhds, name, طول_رشته(name));
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
        مدخل_free(dynhds->hds[i]);
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
UNITTEST CURLcode dynhds_مجموعه(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *مقدار, size_t valuelen);
UNITTEST CURLcode dynhds_مجموعه(بنية dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *مقدار, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  أرجع Curl_dynhds_جمع_کن(dynhds, name, namelen, مقدار, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name);
UNITTEST size_t dynhds_cremove(بنية dynhds *dynhds, ثابت حرف *name)
{
  أرجع dynhds_remove(dynhds, name, طول_رشته(name));
}

#پایان_اگر /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(بنية dynhds *dynhds, بنية dynbuf *dbuf)
{
  CURLcode نتیجه = CURLE_OK;
  size_t i;

  إذا(!dynhds->hds_len)
    أرجع نتیجه;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    نتیجه = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(dbuf, STRCONST(": "));
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(dbuf, dynhds->hds[i]->مقدار,
                              dynhds->hds[i]->valuelen);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    إذا(نتیجه)
      اكسر;
  }

  أرجع نتیجه;
}

#اگر_تعریف_شده USE_NGHTTP2

nghttp2_nv *Curl_dynhds_مقصد_nva(بنية dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, حجم(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  إذا(!nva)
    أرجع عدم;

  لكل(i = 0; i < dynhds->hds_len; ++i) {
    بنية dynhds_مدخل *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (بیعلامت حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].مقدار = (بیعلامت حرف *)e->مقدار;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  أرجع nva;
}

#پایان_اگر /* USE_NGHTTP2 */
