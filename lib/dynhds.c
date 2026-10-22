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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "dynhds.h"
#अन्तर्भाव "strcase.h"

स्थिर संरचना dynhds_प्रविष्टिः *प्रविष्टिः_new(ध्रुव वर्ण *name, size_t namelen,
                                      ध्रुव वर्ण *मानम्, size_t valuelen,
                                      पूर्णाङ्क opts)
{
  संरचना dynhds_प्रविष्टिः *e;
  वर्ण *p;

  /* namelen + valuelen <= SIZE_MAX has already been checked by caller */
  यदि((SIZE_MAX - namelen - valuelen) < (परिमाण(*e) + 2))
    प्रत्यागम रिक्त;
  e = curlx_calloc(1, परिमाण(*e) + namelen + valuelen + 2);
  यदि(!e)
    प्रत्यागम रिक्त;
  e->name = p = (वर्ण *)e + परिमाण(*e);
  स्मृतिं_प्रतिलिख(p, name, namelen);
  e->namelen = namelen;
  e->मानम् = p += namelen + 1; /* leave a \0 at the end of name */
  यदि(valuelen) {
    स्मृतिं_प्रतिलिख(p, मानम्, valuelen);
    e->valuelen = valuelen;
  }
  यदि(opts & DYNHDS_OPT_LOWERCASE)
    Curl_strntolower(e->name, e->name, e->namelen);
  प्रत्यागम e;
}

स्थिर शून्य प्रविष्टिः_free(संरचना dynhds_प्रविष्टिः *e)
{
  curlx_free(e);
}

शून्य Curl_dynhds_init(संरचना dynhds *dynhds, size_t max_entries,
                      size_t max_strs_आकार)
{
  DEBUGASSERT(dynhds);
  DEBUGASSERT(max_strs_आकार);
  dynhds->hds = रिक्त;
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
  dynhds->max_entries = max_entries;
  dynhds->max_strs_आकार = max_strs_आकार;
  dynhds->opts = 0;
}

शून्य Curl_dynhds_free(संरचना dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  यदि(dynhds->hds && dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    प्रति(i = 0; i < dynhds->hds_len; ++i) {
      प्रविष्टिः_free(dynhds->hds[i]);
    }
  }
  curlx_safefree(dynhds->hds);
  dynhds->hds_len = dynhds->hds_allc = dynhds->strs_len = 0;
}

शून्य Curl_dynhds_reset(संरचना dynhds *dynhds)
{
  DEBUGASSERT(dynhds);
  यदि(dynhds->hds_len) {
    size_t i;
    DEBUGASSERT(dynhds->hds);
    प्रति(i = 0; i < dynhds->hds_len; ++i) {
      प्रविष्टिः_free(dynhds->hds[i]);
      dynhds->hds[i] = रिक्त;
    }
  }
  dynhds->hds_len = dynhds->strs_len = 0;
}

size_t Curl_dynhds_गणनम्(संरचना dynhds *dynhds)
{
  प्रत्यागम dynhds->hds_len;
}

शून्य Curl_dynhds_समुच्चयः_opts(संरचना dynhds *dynhds, पूर्णाङ्क opts)
{
  dynhds->opts = opts;
}

संरचना dynhds_प्रविष्टिः *Curl_dynhds_getn(संरचना dynhds *dynhds, size_t n)
{
  DEBUGASSERT(dynhds);
  प्रत्यागम (n < dynhds->hds_len) ? dynhds->hds[n] : रिक्त;
}

संरचना dynhds_प्रविष्टिः *Curl_dynhds_get(संरचना dynhds *dynhds, ध्रुव वर्ण *name,
                                     size_t namelen)
{
  size_t i;
  प्रति(i = 0; i < dynhds->hds_len; ++i) {
    यदि(dynhds->hds[i]->namelen == namelen &&
       curl_strnequal(dynhds->hds[i]->name, name, namelen)) {
      प्रत्यागम dynhds->hds[i];
    }
  }
  प्रत्यागम रिक्त;
}

संरचना dynhds_प्रविष्टिः *Curl_dynhds_cget(संरचना dynhds *dynhds, ध्रुव वर्ण *name)
{
  प्रत्यागम Curl_dynhds_get(dynhds, name, वाक्यदैर्घ्यम्(name));
}

CURLcode Curl_dynhds_सङ्कलय(संरचना dynhds *dynhds,
                         ध्रुव वर्ण *name, size_t namelen,
                         ध्रुव वर्ण *मानम्, size_t valuelen)
{
  संरचना dynhds_प्रविष्टिः *प्रविष्टिः = रिक्त;
  CURLcode परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  /* Accept NULL value with length 0, name must have length */
  यदि(!dynhds || !name || !namelen || (!मानम् && valuelen)) {
    DEBUGASSERT(0);
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  }
  /* Does the new header fit into the limits? */
  यदि((dynhds->max_entries && (dynhds->hds_len >= dynhds->max_entries)) ||
     (dynhds->max_strs_आकार - dynhds->strs_len < namelen) ||
     (dynhds->max_strs_आकार - dynhds->strs_len - namelen < valuelen))
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  प्रविष्टिः = प्रविष्टिः_new(name, namelen, मानम्, valuelen, dynhds->opts);
  यदि(!प्रविष्टिः)
    गच्छ बहिर्गमनम्;

  यदि(dynhds->hds_len + 1 >= dynhds->hds_allc) {
    size_t nallc = dynhds->hds_allc ? dynhds->hds_allc * 2 : 16;
    संरचना dynhds_प्रविष्टिः **nhds;

    यदि(dynhds->max_entries && nallc > dynhds->max_entries)
      nallc = dynhds->max_entries;

    nhds = curlx_calloc(nallc, परिमाण(संरचना dynhds_प्रविष्टिः *));
    यदि(!nhds)
      गच्छ बहिर्गमनम्;
    यदि(dynhds->hds) {
      स्मृतिं_प्रतिलिख(nhds, dynhds->hds,
             dynhds->hds_len * परिमाण(संरचना dynhds_प्रविष्टिः *));
      curlx_safefree(dynhds->hds);
    }
    dynhds->hds = nhds;
    dynhds->hds_allc = nallc;
  }
  dynhds->hds[dynhds->hds_len++] = प्रविष्टिः;
  प्रविष्टिः = रिक्त;
  dynhds->strs_len += namelen + valuelen;
  परिणाम = CURLE_OK;

बहिर्गमनम्:
  यदि(प्रविष्टिः)
    प्रविष्टिः_free(प्रविष्टिः);
  प्रत्यागम परिणाम;
}

CURLcode Curl_dynhds_cadd(संरचना dynhds *dynhds,
                          ध्रुव वर्ण *name, ध्रुव वर्ण *मानम्)
{
  यदि(!name)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  प्रत्यागम Curl_dynhds_सङ्कलय(dynhds, name, वाक्यदैर्घ्यम्(name),
                         मानम्, मानम् ? वाक्यदैर्घ्यम्(मानम्) : 0);
}

CURLcode Curl_dynhds_h1_सङ्कलय_पङ्क्तिः(संरचना dynhds *dynhds,
                                 ध्रुव वर्ण *पङ्क्तिः, size_t पङ्क्तिः_len)
{
  ध्रुव वर्ण *p;
  ध्रुव वर्ण *name;
  size_t namelen;
  ध्रुव वर्ण *मानम्;
  size_t valuelen, i;

  यदि(!पङ्क्तिः || !पङ्क्तिः_len)
    प्रत्यागम CURLE_OK;

  p = memchr(पङ्क्तिः, ':', पङ्क्तिः_len);
  यदि(!p || p == पङ्क्तिः)
    प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
  name = पङ्क्तिः;
  namelen = p - पङ्क्तिः;
  p++; /* move past the colon */
  प्रति(i = namelen + 1; i < पङ्क्तिः_len; ++i, ++p) {
    यदि(!ISBLANK(*p))
      भङ्ग;
  }
  मानम् = p;
  valuelen = पङ्क्तिः_len - i;

  p = memchr(मानम्, '\r', valuelen);
  यदि(!p)
    p = memchr(मानम्, '\n', valuelen);
  यदि(p)
    valuelen = (size_t)(p - मानम्);

  प्रत्यागम Curl_dynhds_सङ्कलय(dynhds, name, namelen, मानम्, valuelen);
}

CURLcode Curl_dynhds_h1_cadd_पङ्क्तिः(संरचना dynhds *dynhds, ध्रुव वर्ण *पङ्क्तिः)
{
  प्रत्यागम Curl_dynhds_h1_सङ्कलय_पङ्क्तिः(dynhds, पङ्क्तिः, पङ्क्तिः ? वाक्यदैर्घ्यम्(पङ्क्तिः) : 0);
}

#यदि_परिभाषितम् UNITTESTS
/* @unittest 2602 */

/**
 * Return TRUE iff one or more headers with the given name exist.
 */
UNITTEST bool dynhds_अन्तर्भवति(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen);
UNITTEST bool dynhds_अन्तर्भवति(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen)
{
  प्रत्यागम !!Curl_dynhds_get(dynhds, name, namelen);
}

/* @unittest 2602 */
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, ध्रुव वर्ण *name);
UNITTEST bool dynhds_ccontains(संरचना dynhds *dynhds, ध्रुव वर्ण *name)
{
  प्रत्यागम dynhds_अन्तर्भवति(dynhds, name, वाक्यदैर्घ्यम्(name));
}

/**
 * Return how often the given name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_गणनम्_name(संरचना dynhds *dynhds,
                                  ध्रुव वर्ण *name, size_t namelen);
UNITTEST size_t dynhds_गणनम्_name(संरचना dynhds *dynhds,
                                  ध्रुव वर्ण *name, size_t namelen)
{
  size_t n = 0;
  यदि(dynhds->hds_len) {
    size_t i;
    प्रति(i = 0; i < dynhds->hds_len; ++i) {
      यदि((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen))
        ++n;
    }
  }
  प्रत्यागम n;
}

/**
 * Return how often the given null-terminated name appears in `dynhds`.
 * Names are case-insensitive.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_ccount_name(संरचना dynhds *dynhds,
                                        ध्रुव वर्ण *name);
UNITTEST size_t dynhds_ccount_name(संरचना dynhds *dynhds,
                                        ध्रुव वर्ण *name)
{
  प्रत्यागम dynhds_गणनम्_name(dynhds, name, वाक्यदैर्घ्यम्(name));
}

/**
 * Remove all entries with the given name.
 * Returns number of entries removed.
 *
 * @unittest 2602
 */
UNITTEST size_t dynhds_remove(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen);
UNITTEST size_t dynhds_remove(संरचना dynhds *dynhds,
                              ध्रुव वर्ण *name, size_t namelen)
{
  size_t n = 0;
  यदि(dynhds->hds_len) {
    size_t i, len;
    प्रति(i = 0; i < dynhds->hds_len; ++i) {
      यदि((namelen == dynhds->hds[i]->namelen) &&
         curl_strnequal(name, dynhds->hds[i]->name, namelen)) {
        ++n;
        --dynhds->hds_len;
        dynhds->strs_len -= (dynhds->hds[i]->namelen +
                             dynhds->hds[i]->valuelen);
        प्रविष्टिः_free(dynhds->hds[i]);
        len = dynhds->hds_len - i; /* remaining entries */
        यदि(len) {
          memmove(&dynhds->hds[i], &dynhds->hds[i + 1],
                  len * परिमाण(dynhds->hds[i]));
        }
        --i; /* do this index again */
      }
    }
  }
  प्रत्यागम n;
}

/**
 * Set the give header name and value, replacing any entries with
 * the same name. The header is added at the end of all (remaining)
 * entries.
 *
 * @unittest 2602
 */
UNITTEST CURLcode dynhds_समुच्चयः(संरचना dynhds *dynhds,
                             ध्रुव वर्ण *name, size_t namelen,
                             ध्रुव वर्ण *मानम्, size_t valuelen);
UNITTEST CURLcode dynhds_समुच्चयः(संरचना dynhds *dynhds,
                             ध्रुव वर्ण *name, size_t namelen,
                             ध्रुव वर्ण *मानम्, size_t valuelen)
{
  dynhds_remove(dynhds, name, namelen);
  प्रत्यागम Curl_dynhds_सङ्कलय(dynhds, name, namelen, मानम्, valuelen);
}

/* @unittest 2602 */
UNITTEST size_t dynhds_cremove(संरचना dynhds *dynhds, ध्रुव वर्ण *name);
UNITTEST size_t dynhds_cremove(संरचना dynhds *dynhds, ध्रुव वर्ण *name)
{
  प्रत्यागम dynhds_remove(dynhds, name, वाक्यदैर्घ्यम्(name));
}

#यदि_अन्तः /* UNITTESTS */

CURLcode Curl_dynhds_h1_dprint(संरचना dynhds *dynhds, संरचना dynbuf *dbuf)
{
  CURLcode परिणाम = CURLE_OK;
  size_t i;

  यदि(!dynhds->hds_len)
    प्रत्यागम परिणाम;

  प्रति(i = 0; i < dynhds->hds_len; ++i) {
    परिणाम = curlx_dyn_addn(dbuf, dynhds->hds[i]->name,
                            dynhds->hds[i]->namelen);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(dbuf, STRCONST(": "));
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(dbuf, dynhds->hds[i]->मानम्,
                              dynhds->hds[i]->valuelen);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(dbuf, STRCONST("\r\n"));
    यदि(परिणाम)
      भङ्ग;
  }

  प्रत्यागम परिणाम;
}

#यदि_परिभाषितम् USE_NGHTTP2

nghttp2_nv *Curl_dynhds_गन्तव्यम्_nva(संरचना dynhds *dynhds, size_t *pcount)
{
  nghttp2_nv *nva = curlx_calloc(1, परिमाण(nghttp2_nv) * dynhds->hds_len);
  size_t i;

  *pcount = 0;
  यदि(!nva)
    प्रत्यागम रिक्त;

  प्रति(i = 0; i < dynhds->hds_len; ++i) {
    संरचना dynhds_प्रविष्टिः *e = dynhds->hds[i];
    DEBUGASSERT(e);
    nva[i].name = (अचिह्नित वर्ण *)e->name;
    nva[i].namelen = e->namelen;
    nva[i].मानम् = (अचिह्नित वर्ण *)e->मानम्;
    nva[i].valuelen = e->valuelen;
    nva[i].flags = NGHTTP2_NV_FLAG_NONE;
  }
  *pcount = dynhds->hds_len;
  प्रत्यागम nva;
}

#यदि_अन्तः /* USE_NGHTTP2 */
