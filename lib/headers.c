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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "headers.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ਸਿਰਲੇਖਾਂ_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ਸਥਿਰ ਖਾਲੀ copy_ਸਿਰਲੇਖ_external(ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *hs,
                                 size_t ਸੂਚਕਾਂਕ,
                                 size_t amount,
                                 ਬਣਤਰ Curl_llist_ਗੰਢ *e,
                                 ਬਣਤਰ curl_ਸਿਰਲੇਖ *hout)
{
  ਬਣਤਰ curl_ਸਿਰਲੇਖ *h = hout;
  h->name = hs->name;
  h->ਮੁੱਲ = hs->ਮੁੱਲ;
  h->amount = amount;
  h->ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_ਸਿਰਲੇਖ(CURL *curl,
                           ਅਚਲ ਅੱਖਰ *name,
                           size_t nameindex,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ origin,
                           ਪੂਰਨ request,
                           ਬਣਤਰ curl_ਸਿਰਲੇਖ **hout)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_ਸਿਰਲੇਖ, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_llist_ਗੰਢ *e_pick = ਨਿਰਾ;
    size_t ਮੇਲ = 0;
    size_t amount = 0;
    ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *hs = ਨਿਰਾ;
    ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *pick = ਨਿਰਾ;
    ਜੇ(!name || !hout || !ਅੰਕੜੇ ||
       (origin > (CURLH_ਸਿਰਲੇਖ | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ਦਲੀਲ;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(!Curl_llist_ਗਿਣਤੀ(&ਅੰਕੜੇ->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(request > ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ) {
      hresult = CURLHE_NOREQUEST;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ(request == -1)
      request = ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ;

    /* we need a first round to count amount of this header */
    ਲਈ(e = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->state.httphdrs); e; e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
      hs = Curl_ਗੰਢ_elem(e);
      ਜੇ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ਜੇ(!amount)
      hresult = CURLHE_MISSING;
    ਨਹੀਂਤਾਂ ਜੇ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ਜੇ(hresult)
      ਜਾਓ ਬਾਹਰਲਾ;

    ਜੇ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    ਨਹੀਂਤਾਂ {
      ਲਈ(e = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->state.httphdrs); e;
          e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
        hs = Curl_ਗੰਢ_elem(e);
        ਜੇ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (ਮੇਲ++ == nameindex)) {
          e_pick = e;
          ਰੋਕੋ;
        }
      }
      ਜੇ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        ਜਾਓ ਬਾਹਰਲਾ;
      }
    }
    /* this is the name we want */
    copy_ਸਿਰਲੇਖ_external(hs, nameindex, amount, e_pick,
                         &ਅੰਕੜੇ->state.headerout[0]);
    *hout = &ਅੰਕੜੇ->state.headerout[0];
    hresult = CURLHE_OK;
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਜੇ(ਨਤੀਜਾ)
    hresult = Curl_eapi_hcode(ਨਤੀਜਾ);
  ਵਾਪਸ hresult;
}

ਬਣਤਰ nextheader_cache_ਇੰਦਰਾਜ {
  ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *ਸਿਰਲੇਖ;
  size_t ਕ੍ਰਮ;
};

ਸਥਿਰ ਪੂਰਨ nextheader_compare(ਅਚਲ ਖਾਲੀ *p1, ਅਚਲ ਖਾਲੀ *p2)
{
  ਅਚਲ ਬਣਤਰ nextheader_cache_ਇੰਦਰਾਜ *e1 = p1;
  ਅਚਲ ਬਣਤਰ nextheader_cache_ਇੰਦਰਾਜ *e2 = p2;
  ਅਚਲ ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *h1 = e1->ਸਿਰਲੇਖ;
  ਅਚਲ ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *h2 = e2->ਸਿਰਲੇਖ;
  ਅਚਲ ਅੱਖਰ *n1 = h1->name;
  ਅਚਲ ਅੱਖਰ *n2 = h2->name;

  ਜਦੋਂ(*n1 && *n2) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c1 = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)Curl_raw_toupper(*n1++);
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c2 = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)Curl_raw_toupper(*n2++);
    ਜੇ(c1 != c2)
      ਵਾਪਸ c1 > c2 ? 1 : -1;
  }
  ਜੇ(*n1)
    ਵਾਪਸ 1;
  ਜੇ(*n2)
    ਵਾਪਸ -1;
  ਜੇ(e1->ਕ੍ਰਮ != e2->ਕ੍ਰਮ)
    ਵਾਪਸ e1->ਕ੍ਰਮ > e2->ਕ੍ਰਮ ? 1 : -1;
  ਵਾਪਸ 0;
}

ਸਥਿਰ bool nextheader_cache_build(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                   ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ origin,
                                   ਪੂਰਨ request, size_t ਗਿਣਤੀ_2)
{
  ਬਣਤਰ nextheader_cache_ਇੰਦਰਾਜ *ਸਿਰਲੇਖਾਂ;
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  size_t i = 0;
  size_t ਪਹਿਲਾ;

  ਸਿਰਲੇਖਾਂ = curlx_malloc(ਆਕਾਰ(*ਸਿਰਲੇਖਾਂ) * ਗਿਣਤੀ_2);
  ਜੇ(!ਸਿਰਲੇਖਾਂ)
    ਵਾਪਸ FALSE;

  ਲਈ(e = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->state.httphdrs); e;
      e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
    ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *hs = Curl_ਗੰਢ_elem(e);
    ਜੇ((hs->type & origin) && (hs->request == request)) {
      ਸਿਰਲੇਖਾਂ[i].ਸਿਰਲੇਖ = hs;
      ਸਿਰਲੇਖਾਂ[i].ਕ੍ਰਮ = i;
      i++;
    }
  }

  qsort(ਸਿਰਲੇਖਾਂ, i, ਆਕਾਰ(*ਸਿਰਲੇਖਾਂ), nextheader_compare);
  ਲਈ(ਪਹਿਲਾ = 0; ਪਹਿਲਾ < i;) {
    size_t ਆਖਰੀ = ਪਹਿਲਾ + 1;
    size_t ਸੂਚਕਾਂਕ;
    ਜਦੋਂ((ਆਖਰੀ < i) &&
          curl_strequal(ਸਿਰਲੇਖਾਂ[ਪਹਿਲਾ].ਸਿਰਲੇਖ->name,
                        ਸਿਰਲੇਖਾਂ[ਆਖਰੀ].ਸਿਰਲੇਖ->name))
      ਆਖਰੀ++;
    ਲਈ(ਸੂਚਕਾਂਕ = ਪਹਿਲਾ; ਸੂਚਕਾਂਕ < ਆਖਰੀ; ਸੂਚਕਾਂਕ++) {
      ਸਿਰਲੇਖਾਂ[ਸੂਚਕਾਂਕ].ਸਿਰਲੇਖ->nh_amount = ਆਖਰੀ - ਪਹਿਲਾ;
      ਸਿਰਲੇਖਾਂ[ਸੂਚਕਾਂਕ].ਸਿਰਲੇਖ->nh_ਸੂਚਕਾਂਕ = ਸੂਚਕਾਂਕ - ਪਹਿਲਾ;
    }
    ਪਹਿਲਾ = ਆਖਰੀ;
  }
  curlx_free(ਸਿਰਲੇਖਾਂ);

  ਅੰਕੜੇ->state.nh_origin = origin;
  ਅੰਕੜੇ->state.nh_request = request;
  ਅੰਕੜੇ->state.nh_ਗਿਣਤੀ = ਗਿਣਤੀ_2;
  ਵਾਪਸ TRUE;
}

/* public API */
ਬਣਤਰ curl_ਸਿਰਲੇਖ *curl_easy_nextheader(CURL *curl,
                                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ origin,
                                         ਪੂਰਨ request,
                                         ਬਣਤਰ curl_ਸਿਰਲੇਖ *prev)
{
  ਬਣਤਰ Curl_eapi_guard guard;
  ਬਣਤਰ curl_ਸਿਰਲੇਖ *hd = ਨਿਰਾ;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &ਨਤੀਜਾ)) {
    ਬਣਤਰ Curl_easy *ਅੰਕੜੇ = curl;
    ਬਣਤਰ Curl_llist_ਗੰਢ *pick;
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *hs;
    size_t amount = 0;
    size_t ਸੂਚਕਾਂਕ = 0;
    size_t ਗਿਣਤੀ_2;

    ਜੇ(request > ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ)
      ਜਾਓ ਬਾਹਰਲਾ;
    ਜੇ(request == -1)
      request = ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ;

    ਜੇ(prev) {
      pick = prev->anchor;
      ਜੇ(!pick)
        /* something is wrong */
        ਜਾਓ ਬਾਹਰਲਾ;
      pick = Curl_ਗੰਢ_ਅਗਲਾ(pick);
    }
    ਨਹੀਂਤਾਂ
      pick = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->state.httphdrs);

    ਜੇ(pick) {
      /* make sure it is the next header of the desired type */
      ਕਰੋ {
        hs = Curl_ਗੰਢ_elem(pick);
        ਜੇ((hs->type & origin) && (hs->request == request))
          ਰੋਕੋ;
        pick = Curl_ਗੰਢ_ਅਗਲਾ(pick);
      } ਜਦੋਂ(pick);
    }

    ਜੇ(!pick)
      /* no more headers available */
      ਜਾਓ ਬਾਹਰਲਾ;

    hs = Curl_ਗੰਢ_elem(pick);
    ਗਿਣਤੀ_2 = Curl_llist_ਗਿਣਤੀ(&ਅੰਕੜੇ->state.httphdrs);

    ਜੇ(((ਅੰਕੜੇ->state.nh_ਗਿਣਤੀ == ਗਿਣਤੀ_2) &&
        (ਅੰਕੜੇ->state.nh_origin == origin) &&
        (ਅੰਕੜੇ->state.nh_request == request)) ||
       nextheader_cache_build(ਅੰਕੜੇ, origin, request, ਗਿਣਤੀ_2)) {
      amount = hs->nh_amount;
      ਸੂਚਕਾਂਕ = hs->nh_ਸੂਚਕਾਂਕ;
    }
    ਨਹੀਂਤਾਂ {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      ਲਈ(e = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->state.httphdrs); e;
          e = Curl_ਗੰਢ_ਅਗਲਾ(e)) {
        ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *check = Curl_ਗੰਢ_elem(e);
        ਜੇ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ਜੇ(e == pick)
          ਸੂਚਕਾਂਕ = amount - 1;
      }
    }
    copy_ਸਿਰਲੇਖ_external(hs, ਸੂਚਕਾਂਕ, amount, pick,
                         &ਅੰਕੜੇ->state.headerout[1]);
    hd = &ਅੰਕੜੇ->state.headerout[1];
  }
ਬਾਹਰਲਾ:
  CURL_EAPI_LEAVE(&guard);
  ਵਾਪਸ hd;
}

ਸਥਿਰ CURLcode namevalue(ਅੱਖਰ *ਸਿਰਲੇਖ, size_t hlen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type,
                          ਅੱਖਰ **name, ਅੱਖਰ **ਮੁੱਲ)
{
  ਅੱਖਰ *ਸਿਰਾ = ਸਿਰਲੇਖ + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = ਸਿਰਲੇਖ;

  ਜੇ(type == CURLH_PSEUDO) {
    ਜੇ(*ਸਿਰਲੇਖ != ':')
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਸਿਰਲੇਖ++;
  }

  /* Find the end of the header name */
  ਜਦੋਂ(*ਸਿਰਲੇਖ && (*ਸਿਰਲੇਖ != ':'))
    ++ਸਿਰਲੇਖ;

  ਜੇ(*ਸਿਰਲੇਖ)
    /* Skip over colon, null it */
    *ਸਿਰਲੇਖ++ = 0;
  ਨਹੀਂਤਾਂ
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  /* skip all leading blank letters */
  ਜਦੋਂ(ISBLANK(*ਸਿਰਲੇਖ))
    ਸਿਰਲੇਖ++;

  *ਮੁੱਲ = ਸਿਰਲੇਖ;

  /* skip all trailing space letters */
  ਜਦੋਂ((ਸਿਰਾ > ਸਿਰਲੇਖ) && ISBLANK(*ਸਿਰਾ))
    *ਸਿਰਾ-- = 0; /* null-terminate */
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_ਸਿਰਲੇਖਾਂ_ਧੱਕੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *ਸਿਰਲੇਖ,
                           size_t hlen, /* length of header */
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ type)
{
  ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;
  ਅੱਖਰ *name = ਨਿਰਾ;
  ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *hs;
  CURLcode ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਅਚਲ size_t ilen = hlen;

  ਜੇ((ਸਿਰਲੇਖ[0] == '\r') || (ਸਿਰਲੇਖ[0] == '\n'))
    /* ignore the body separator */
    ਵਾਪਸ CURLE_OK;

  /* trim off newline characters */
  ਜੇ(hlen && (ਸਿਰਲੇਖ[hlen - 1] == '\n'))
    hlen--;
  ਜੇ(hlen && (ਸਿਰਲੇਖ[hlen - 1] == '\r'))
    hlen--;
  ਜੇ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;

  ਜੇ(ISBLANK(ਸਿਰਲੇਖ[0])) {
    /* pass leading blanks */
    ਜਦੋਂ(hlen && ISBLANK(*ਸਿਰਲੇਖ)) {
      ਸਿਰਲੇਖ++;
      hlen--;
    }
    ਜੇ(!hlen)
      ਵਾਪਸ CURLE_WEIRD_SERVER_ਜਵਾਬ;
  }
  ਜੇ(Curl_llist_ਗਿਣਤੀ(&ਅੰਕੜੇ->state.httphdrs) >= MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਗਿਣਤੀ) {
    failf(ਅੰਕੜੇ, "Too many response headers, %d is max",
          MAX_HTTP_RESP_ਸਿਰਲੇਖ_ਗਿਣਤੀ);
    ਵਾਪਸ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, ਆਕਾਰ(*hs) + hlen);
  ਜੇ(!hs)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(hs->ਬਫਰ, ਸਿਰਲੇਖ, hlen);
  hs->ਬਫਰ[hlen] = 0; /* null-terminate */

  ਨਤੀਜਾ = namevalue(hs->ਬਫਰ, hlen, type, &name, &ਮੁੱਲ);
  ਜੇ(!ਨਤੀਜਾ) {
    hs->name = name;
    hs->ਮੁੱਲ = ਮੁੱਲ;
    hs->type = type;
    hs->request = ਅੰਕੜੇ->state.ਬੇਨਤੀਆਂ;

    /* insert this node into the list of headers */
    Curl_llist_append(&ਅੰਕੜੇ->state.httphdrs, hs, &hs->ਗੰਢ);
  }
  ਨਹੀਂਤਾਂ {
    failf(ਅੰਕੜੇ, "Invalid response header");
    curlx_free(hs);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ਸਥਿਰ ਖਾਲੀ ਸਿਰਲੇਖਾਂ_reset(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  Curl_llist_init(&ਅੰਕੜੇ->state.httphdrs, ਨਿਰਾ);
  ਅੰਕੜੇ->state.nh_ਗਿਣਤੀ = 0;
}

ਬਣਤਰ hds_cw_collect_ctx {
  ਬਣਤਰ Curl_cwriter super;
};

ਸਥਿਰ CURLcode hds_cw_collect_write(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ Curl_cwriter *writer, ਪੂਰਨ type,
                                     ਅਚਲ ਅੱਖਰ *buf, size_t blen)
{
  ਜੇ((type & CLIENTWRITE_ਸਿਰਲੇਖ) && !(type & CLIENTWRITE_ਸਥਿਤੀ)) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ htype = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_ਸਿਰਲੇਖ)));
    CURLcode ਨਤੀਜਾ = Curl_ਸਿਰਲੇਖਾਂ_ਧੱਕੋ(ਅੰਕੜੇ, buf, blen, htype);
    CURL_TRC_WRITE(ਅੰਕੜੇ, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (ਪੂਰਨ)ਨਤੀਜਾ);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;
  }
  ਵਾਪਸ Curl_cwriter_write(ਅੰਕੜੇ, writer->ਅਗਲਾ, type, buf, blen);
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ਨਿਰਾ,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ਆਕਾਰ(ਬਣਤਰ hds_cw_collect_ctx)
};

CURLcode Curl_ਸਿਰਲੇਖਾਂ_init(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_cwriter *writer;
  CURLcode ਨਤੀਜਾ;

  ਜੇ(ਅੰਕੜੇ->conn && (ਅੰਕੜੇ->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ਜੇ(Curl_cwriter_get_by_name(ਅੰਕੜੇ, hds_cw_collect.name))
      ਵਾਪਸ CURLE_OK;

    ਨਤੀਜਾ = Curl_cwriter_create(&writer, ਅੰਕੜੇ, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਤੀਜਾ;

    ਨਤੀਜਾ = Curl_cwriter_ਜੋੜੋ(ਅੰਕੜੇ, writer);
    ਜੇ(ਨਤੀਜਾ) {
      Curl_cwriter_free(ਅੰਕੜੇ, writer);
      ਵਾਪਸ ਨਤੀਜਾ;
    }
  }
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_ਸਿਰਲੇਖਾਂ_cleanup(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;

  ਲਈ(e = Curl_llist_ਸਿਰ(&ਅੰਕੜੇ->state.httphdrs); e; e = n) {
    ਬਣਤਰ Curl_ਸਿਰਲੇਖ_store *hs = Curl_ਗੰਢ_elem(e);
    n = Curl_ਗੰਢ_ਅਗਲਾ(e);
    curlx_free(hs);
  }
  ਸਿਰਲੇਖਾਂ_reset(ਅੰਕੜੇ);
  ਵਾਪਸ CURLE_OK;
}

#ਨਹੀਂਤਾਂ /* HTTP-disabled builds below */

CURLHcode curl_easy_ਸਿਰਲੇਖ(CURL *easy,
                           ਅਚਲ ਅੱਖਰ *name,
                           size_t ਸੂਚਕਾਂਕ,
                           ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ origin,
                           ਪੂਰਨ request,
                           ਬਣਤਰ curl_ਸਿਰਲੇਖ **hout)
{
  (ਖਾਲੀ)easy;
  (ਖਾਲੀ)name;
  (ਖਾਲੀ)ਸੂਚਕਾਂਕ;
  (ਖਾਲੀ)origin;
  (ਖਾਲੀ)request;
  (ਖਾਲੀ)hout;
  ਵਾਪਸ CURLHE_NOT_BUILT_ਵਿੱਚ;
}

ਬਣਤਰ curl_ਸਿਰਲੇਖ *curl_easy_nextheader(CURL *easy,
                                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ type,
                                         ਪੂਰਨ request,
                                         ਬਣਤਰ curl_ਸਿਰਲੇਖ *prev)
{
  (ਖਾਲੀ)easy;
  (ਖਾਲੀ)type;
  (ਖਾਲੀ)request;
  (ਖਾਲੀ)prev;
  ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ
