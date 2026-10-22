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

ساکن ساخت dynhds_اندراج *اندراج_new(ثابت حرف *name, size_t namelen,
                                      ثابت حرف *قدر, size_t valuelen,
                                      صحیح opts)
{
  ساخت dynhds_اندراج *e;
  حرف *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  اگر((SIZE_MAX - namelen - valuelen) < (حجم(*e) + 2))
    واپس تہی;
  e = curlx_calloc(1, حجم(*e) + namelen + valuelen + 2);
  اگر(!e)
    واپس تہی;
  e->name = p = (حرف *)e + حجم(*e);
  حافظہ_نقل(p, name, namelen);
  e->namelen = namelen;
  e->قدر = p += namelen + 1; /* leave a \0 at the end of name */
  اگر(valuelen) {
    حافظہ_نقل(p, قدر, valuelen);
    e->valuelen = valuelen;
  }
  اگر(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  واپس e;
}

ساکن خالی اندراج_free(ساخت dynhds_اندراج *e)
{
  curlx_free(e);
}

خالی Curl_dynhds_init(ساخت dynhds *dynhds, size_t max_entries,
                      size_t max_strs_سائز)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_سائز);
  dynhds->hds = تہی;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_سائز = max_strs_سائز;
  dynhds->opts = 0;
}

خالی Curl_dynhds_free(ساخت dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  اگر(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    برائے(i = 0; i < dynhds->hds_len; ++i) {
      اندراج_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

خالی Curl_dynhds_reset(ساخت dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  اگر(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    برائے(i = 0; i < dynhds->hds_len; ++i) {
      اندراج_free(dynhds->hds[i]);
      dynhds->hds[i] = تہی;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_گنتی(ساخت dynhds *dynhds)
{
  واپس dynhds->hds_len;
}

خالی Curl_dynhds_سیٹ_opts(ساخت dynhds *dynhds, صحیح opts)
{
  dynhds->opts = opts;
}

ساخت dynhds_اندراج *Curl_dynhds_getn(ساخت dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  واپس (n < dynhds->hds_len) ? dynhds->hds[n] : تہی;
}

ساخت dynhds_اندراج *Curl_dynhds_get(ساخت dynhds *dynhds, ثابت حرف *name,
                                     size_t namelen)
{
  size_t i;
  برائے(i = 0; i < dynhds->hds_len; ++i) {
    اگر(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      واپس dynhds->hds[i];
    }
  }
  واپس تہی;
}

ساخت dynhds_اندراج *Curl_dynhds_cget(ساخت dynhds *dynhds, ثابت حرف *name)
{
  واپس Curl_dynhds_get(dynhds, name, لڑی_لمبائی(name));
}

CURLcode Curl_dynhds_جوڑو(ساخت dynhds *dynhds,
                         ثابت حرف *name, size_t namelen,
                         ثابت حرف *قدر, size_t valuelen)
{
  ساخت dynhds_اندراج *اندراج_2 = تہی;
  CURLcode نتیجہ = CURLE_برآمد_کا_حافظہ;

  /* Accept NULL value with length 0, name must have length */
  اگر(!dynhds || !name || !namelen || (!قدر && valuelen)) {
    DEBUGASSERT(0);
    واپس CURLE_BAD_FUNCTION_دلیل;
  }
  /* Does the new header fit into the limits? */
  اگر((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_سائز - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_سائز - dynhds->strs_len - namelen < valuelen))
    واپس CURLE_برآمد_کا_حافظہ;

  اندراج_2 = اندراج_new(name, namelen, قدر, valuelen, dynhds->opts);
  اگر(!اندراج_2)
    جاؤ برآمد;

  اگر(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    ساخت dynhds_اندراج **nhds;

    اگر(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, حجم(ساخت dynhds_اندراج *));
    اگر(!nhds)
      جاؤ برآمد;
    اگر(dynhds->hds) {
      حافظہ_نقل(nhds, dynhds->hds,
             dynhds->hds_len * حجم(ساخت dynhds_اندراج *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = اندراج_2;
  اندراج_2 = تہی;
  dynhds->strs_len += namelen + valuelen;
  نتیجہ = CURLE_OK;

برآمد:
  اگر(اندراج_2)
    اندراج_free(اندراج_2);
  واپس نتیجہ;
}

CURLcode Curl_dynhds_cadd(ساخت dynhds *dynhds,
                          ثابت حرف *name, ثابت حرف *قدر)
{
  اگر(!name)
    واپس CURLE_BAD_FUNCTION_دلیل;
  واپس Curl_dynhds_جوڑو(dynhds, name, لڑی_لمبائی(name),
                         قدر, قدر ? لڑی_لمبائی(قدر) : 0);
}

CURLcode Curl_dynhds_h1_جوڑو_سطر(ساخت dynhds *dynhds,
                                 ثابت حرف *سطر, size_t سطر_len)
{
  ثابت حرف *p;
  ثابت حرف *name;
  size_t namelen;
  ثابت حرف *قدر;
  size_t valuelen, i;

  اگر(!سطر || !سطر_len)
    واپس CURLE_OK;

  p = memchr(سطر, ':', سطر_len);
  اگر(!p || p == سطر)
    واپس CURLE_BAD_FUNCTION_دلیل;
  name = سطر;
  namelen = p - سطر;
  p++; /* move past the colon */
  برائے(i = namelen + 1; i < سطر_len; ++i, ++p) {
    اگر(!ISBLANK(*p))
      توڑو;
  }
  قدر = p;
  valuelen = سطر_len - i;

  p = memchr(قدر, '\r', valuelen);
  اگر(!p)
    p = memchr(قدر, '\n', valuelen);
  اگر(p)
    valuelen = (size_t)(p - قدر);

  واپس Curl_dynhds_جوڑو(dynhds, name, namelen, قدر, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_سطر(ساخت dynhds *dynhds, ثابت حرف *سطر)
{
  واپس Curl_dynhds_h1_جوڑو_سطر(dynhds, سطر, سطر ? لڑی_لمبائی(سطر) : 0);
}

#اگر_تعریف_شدہ UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_شامل_ہے(ساخت dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST bool dynhds_شامل_ہے(ساخت dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  واپس !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(ساخت dynhds *dynhds, ثابت حرف *name);
UNITTEST bool dynhds_ccontains(ساخت dynhds *dynhds, ثابت حرف *name)
{
  واپس dynhds_شامل_ہے(dynhds, name, لڑی_لمبائی(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_گنتی_name(ساخت dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_گنتی_name(ساخت dynhds *dynhds,
                                  ثابت حرف *name, size_t namelen)
{
  size_t n = 0;
  اگر(dynhds->hds_len) {
    size_t i;
    برائے(i = 0; i < dynhds->hds_len; ++i) {
      اگر((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  واپس n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(ساخت dynhds *dynhds,
                                        ثابت حرف *name);
UNITTEST size_t dynhds_ccount_name(ساخت dynhds *dynhds,
                                        ثابت حرف *name)
{
  واپس dynhds_گنتی_name(dynhds, name, لڑی_لمبائی(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(ساخت dynhds *dynhds,
                              ثابت حرف *name, size_t namelen);
UNITTEST size_t dynhds_remove(ساخت dynhds *dynhds,
                              ثابت حرف *name, size_t namelen)
{
  size_t n = 0;
  اگر(dynhds->hds_len) {
    size_t i, len;
    برائے(i = 0; i < dynhds->hds_len; ++i) {
      اگر((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        اندراج_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        اگر(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * حجم(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  واپس n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_سیٹ(ساخت dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *قدر, size_t valuelen);
UNITTEST CURLcode dynhds_سیٹ(ساخت dynhds *dynhds,
                             ثابت حرف *name, size_t namelen,
                             ثابت حرف *قدر, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  واپس Curl_dynhds_جوڑو(dynhds, name, namelen, قدر, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(ساخت dynhds *dynhds, ثابت حرف *name);
UNITTEST size_t dynhds_cremove(ساخت dynhds *dynhds, ثابت حرف *name)
{
  واپس dynhds_remove(dynhds, name, لڑی_لمبائی(name));
}

#اگر_ختم /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(ساخت dynhds *dynhds, ساخت dynbuf *dbuf)
{
  CURLcode نتیجہ = CURLE_OK;
  size_t i;

  اگر(!dynhds->hds_len)
    واپس نتیجہ;

  برائے(i = 0; i < dynhds->hds_len; ++i) {
    نتیجہ = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(dbuf, STRCONST(": "));
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(dbuf, dynhds->hds[i]->قدر,
                              dynhds->hds[i]->valuelen);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    اگر(نتیجہ)
      توڑو;
  }

  واپس نتیجہ;
}

#اگر_تعریف_شدہ USE_NGHTTP2

nghttp2_nv *Curl_dynhds_مقصد_nva(ساخت dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, حجم(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  اگر(!nva)
    واپس تہی;

  برائے(i = 0; i < dynhds->hds_len; ++i) {
    ساخت dynhds_اندراج *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (غیرعلامتی حرف *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].قدر = (غیرعلامتی حرف *)e->قدر;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  واپس nva;
}

#اگر_ختم /* USE_NGHTTP2 */
