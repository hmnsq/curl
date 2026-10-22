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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "headers.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ಶೀರ್ಷಿಕೆಗಳು_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ಸ್ಥಿರ ಶೂನ್ಯ copy_ಶೀರ್ಷಿಕೆ_external(ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *hs,
                                 size_t ಸೂಚ್ಯಂಕ,
                                 size_t amount,
                                 ರಚನೆ Curl_llist_ಗಂಟು *e,
                                 ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *hout)
{
  ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *h = hout;
  h->name = hs->name;
  h->ಮೌಲ್ಯ = hs->ಮೌಲ್ಯ;
  h->amount = amount;
  h->ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_ಶೀರ್ಷಿಕೆ(CURL *curl,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                           size_t nameindex,
                           ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ origin,
                           ಪೂರ್ಣಾಂಕ request,
                           ರಚನೆ curl_ಶೀರ್ಷಿಕೆ **hout)
{
  ರಚನೆ Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_ಶೀರ್ಷಿಕೆ, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_llist_ಗಂಟು *e_pick = ಖಾಲಿ;
    size_t ಹೊಂದಿಕೆ = 0;
    size_t amount = 0;
    ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *hs = ಖಾಲಿ;
    ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *pick = ಖಾಲಿ;
    ಆದರೆ(!name || !hout || !ದತ್ತಾಂಶ ||
       (origin > (CURLH_ಶೀರ್ಷಿಕೆ | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_ವಾದ;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(!Curl_llist_ಎಣಿಕೆ(&ದತ್ತಾಂಶ->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(request > ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು) {
      hresult = CURLHE_NOREQUEST;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ(request == -1)
      request = ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು;

    /* we need a first round to count amount of this header */
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->state.httphdrs); e; e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
      hs = Curl_ಗಂಟು_elem(e);
      ಆದರೆ(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    ಆದರೆ(!amount)
      hresult = CURLHE_MISSING;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    ಆದರೆ(hresult)
      ಹೋಗು ಹೊರಹೋಗು;

    ಆದರೆ(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    ಇಲ್ಲದಿದ್ದರೆ {
      ಗಾಗಿ(e = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->state.httphdrs); e;
          e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
        hs = Curl_ಗಂಟು_elem(e);
        ಆದರೆ(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (ಹೊಂದಿಕೆ++ == nameindex)) {
          e_pick = e;
          ನಿಲ್ಲಿಸು;
        }
      }
      ಆದರೆ(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        ಹೋಗು ಹೊರಹೋಗು;
      }
    }
    /* this is the name we want */
    copy_ಶೀರ್ಷಿಕೆ_external(hs, nameindex, amount, e_pick,
                         &ದತ್ತಾಂಶ->state.headerout[0]);
    *hout = &ದತ್ತಾಂಶ->state.headerout[0];
    hresult = CURLHE_OK;
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    hresult = Curl_eapi_hcode(ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ hresult;
}

ರಚನೆ nextheader_cache_ನಮೂದು {
  ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *ಶೀರ್ಷಿಕೆ;
  size_t ಕ್ರಮ;
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ nextheader_compare(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p1, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *p2)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ nextheader_cache_ನಮೂದು *e1 = p1;
  ಸ್ಥಿರಾಂಕ ರಚನೆ nextheader_cache_ನಮೂದು *e2 = p2;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *h1 = e1->ಶೀರ್ಷಿಕೆ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *h2 = e2->ಶೀರ್ಷಿಕೆ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n1 = h1->name;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *n2 = h2->name;

  ಆಗ(*n1 && *n2) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c1 = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)Curl_raw_toupper(*n1++);
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c2 = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)Curl_raw_toupper(*n2++);
    ಆದರೆ(c1 != c2)
      ಹಿಂತಿರುಗಿ c1 > c2 ? 1 : -1;
  }
  ಆದರೆ(*n1)
    ಹಿಂತಿರುಗಿ 1;
  ಆದರೆ(*n2)
    ಹಿಂತಿರುಗಿ -1;
  ಆದರೆ(e1->ಕ್ರಮ != e2->ಕ್ರಮ)
    ಹಿಂತಿರುಗಿ e1->ಕ್ರಮ > e2->ಕ್ರಮ ? 1 : -1;
  ಹಿಂತಿರುಗಿ 0;
}

ಸ್ಥಿರ bool nextheader_cache_build(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                   ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ origin,
                                   ಪೂರ್ಣಾಂಕ request, size_t ಎಣಿಕೆ_2)
{
  ರಚನೆ nextheader_cache_ನಮೂದು *ಶೀರ್ಷಿಕೆಗಳು;
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  size_t i = 0;
  size_t ಮೊದಲ;

  ಶೀರ್ಷಿಕೆಗಳು = curlx_malloc(ಗಾತ್ರ(*ಶೀರ್ಷಿಕೆಗಳು) * ಎಣಿಕೆ_2);
  ಆದರೆ(!ಶೀರ್ಷಿಕೆಗಳು)
    ಹಿಂತಿರುಗಿ FALSE;

  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->state.httphdrs); e;
      e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
    ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *hs = Curl_ಗಂಟು_elem(e);
    ಆದರೆ((hs->type & origin) && (hs->request == request)) {
      ಶೀರ್ಷಿಕೆಗಳು[i].ಶೀರ್ಷಿಕೆ = hs;
      ಶೀರ್ಷಿಕೆಗಳು[i].ಕ್ರಮ = i;
      i++;
    }
  }

  qsort(ಶೀರ್ಷಿಕೆಗಳು, i, ಗಾತ್ರ(*ಶೀರ್ಷಿಕೆಗಳು), nextheader_compare);
  ಗಾಗಿ(ಮೊದಲ = 0; ಮೊದಲ < i;) {
    size_t ಕೊನೆಯ = ಮೊದಲ + 1;
    size_t ಸೂಚ್ಯಂಕ;
    ಆಗ((ಕೊನೆಯ < i) &&
          curl_strequal(ಶೀರ್ಷಿಕೆಗಳು[ಮೊದಲ].ಶೀರ್ಷಿಕೆ->name,
                        ಶೀರ್ಷಿಕೆಗಳು[ಕೊನೆಯ].ಶೀರ್ಷಿಕೆ->name))
      ಕೊನೆಯ++;
    ಗಾಗಿ(ಸೂಚ್ಯಂಕ = ಮೊದಲ; ಸೂಚ್ಯಂಕ < ಕೊನೆಯ; ಸೂಚ್ಯಂಕ++) {
      ಶೀರ್ಷಿಕೆಗಳು[ಸೂಚ್ಯಂಕ].ಶೀರ್ಷಿಕೆ->nh_amount = ಕೊನೆಯ - ಮೊದಲ;
      ಶೀರ್ಷಿಕೆಗಳು[ಸೂಚ್ಯಂಕ].ಶೀರ್ಷಿಕೆ->nh_ಸೂಚ್ಯಂಕ = ಸೂಚ್ಯಂಕ - ಮೊದಲ;
    }
    ಮೊದಲ = ಕೊನೆಯ;
  }
  curlx_free(ಶೀರ್ಷಿಕೆಗಳು);

  ದತ್ತಾಂಶ->state.nh_origin = origin;
  ದತ್ತಾಂಶ->state.nh_request = request;
  ದತ್ತಾಂಶ->state.nh_ಎಣಿಕೆ = ಎಣಿಕೆ_2;
  ಹಿಂತಿರುಗಿ TRUE;
}

/* public API */
ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *curl_easy_nextheader(CURL *curl,
                                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ origin,
                                         ಪೂರ್ಣಾಂಕ request,
                                         ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *prev)
{
  ರಚನೆ Curl_eapi_guard guard;
  ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *hd = ಖಾಲಿ;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &ಫಲಿತಾಂಶ)) {
    ರಚನೆ Curl_easy *ದತ್ತಾಂಶ = curl;
    ರಚನೆ Curl_llist_ಗಂಟು *pick;
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *hs;
    size_t amount = 0;
    size_t ಸೂಚ್ಯಂಕ = 0;
    size_t ಎಣಿಕೆ_2;

    ಆದರೆ(request > ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು)
      ಹೋಗು ಹೊರಹೋಗು;
    ಆದರೆ(request == -1)
      request = ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು;

    ಆದರೆ(prev) {
      pick = prev->anchor;
      ಆದರೆ(!pick)
        /* something is wrong */
        ಹೋಗು ಹೊರಹೋಗು;
      pick = Curl_ಗಂಟು_ಮುಂದಿನ(pick);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      pick = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->state.httphdrs);

    ಆದರೆ(pick) {
      /* make sure it is the next header of the desired type */
      ಮಾಡು {
        hs = Curl_ಗಂಟು_elem(pick);
        ಆದರೆ((hs->type & origin) && (hs->request == request))
          ನಿಲ್ಲಿಸು;
        pick = Curl_ಗಂಟು_ಮುಂದಿನ(pick);
      } ಆಗ(pick);
    }

    ಆದರೆ(!pick)
      /* no more headers available */
      ಹೋಗು ಹೊರಹೋಗು;

    hs = Curl_ಗಂಟು_elem(pick);
    ಎಣಿಕೆ_2 = Curl_llist_ಎಣಿಕೆ(&ದತ್ತಾಂಶ->state.httphdrs);

    ಆದರೆ(((ದತ್ತಾಂಶ->state.nh_ಎಣಿಕೆ == ಎಣಿಕೆ_2) &&
        (ದತ್ತಾಂಶ->state.nh_origin == origin) &&
        (ದತ್ತಾಂಶ->state.nh_request == request)) ||
       nextheader_cache_build(ದತ್ತಾಂಶ, origin, request, ಎಣಿಕೆ_2)) {
      amount = hs->nh_amount;
      ಸೂಚ್ಯಂಕ = hs->nh_ಸೂಚ್ಯಂಕ;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      ಗಾಗಿ(e = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->state.httphdrs); e;
          e = Curl_ಗಂಟು_ಮುಂದಿನ(e)) {
        ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *check = Curl_ಗಂಟು_elem(e);
        ಆದರೆ(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        ಆದರೆ(e == pick)
          ಸೂಚ್ಯಂಕ = amount - 1;
      }
    }
    copy_ಶೀರ್ಷಿಕೆ_external(hs, ಸೂಚ್ಯಂಕ, amount, pick,
                         &ದತ್ತಾಂಶ->state.headerout[1]);
    hd = &ದತ್ತಾಂಶ->state.headerout[1];
  }
ಹೊರಹೋಗು:
  CURL_EAPI_LEAVE(&guard);
  ಹಿಂತಿರುಗಿ hd;
}

ಸ್ಥಿರ CURLcode namevalue(ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ, size_t hlen, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type,
                          ಅಕ್ಷರ **name, ಅಕ್ಷರ **ಮೌಲ್ಯ)
{
  ಅಕ್ಷರ *ಕೊನೆ = ಶೀರ್ಷಿಕೆ + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = ಶೀರ್ಷಿಕೆ;

  ಆದರೆ(type == CURLH_PSEUDO) {
    ಆದರೆ(*ಶೀರ್ಷಿಕೆ != ':')
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಶೀರ್ಷಿಕೆ++;
  }

  /* Find the end of the header name */
  ಆಗ(*ಶೀರ್ಷಿಕೆ && (*ಶೀರ್ಷಿಕೆ != ':'))
    ++ಶೀರ್ಷಿಕೆ;

  ಆದರೆ(*ಶೀರ್ಷಿಕೆ)
    /* Skip over colon, null it */
    *ಶೀರ್ಷಿಕೆ++ = 0;
  ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  /* skip all leading blank letters */
  ಆಗ(ISBLANK(*ಶೀರ್ಷಿಕೆ))
    ಶೀರ್ಷಿಕೆ++;

  *ಮೌಲ್ಯ = ಶೀರ್ಷಿಕೆ;

  /* skip all trailing space letters */
  ಆಗ((ಕೊನೆ > ಶೀರ್ಷಿಕೆ) && ISBLANK(*ಕೊನೆ))
    *ಕೊನೆ-- = 0; /* null-terminate */
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_ಶೀರ್ಷಿಕೆಗಳು_ತಳ್ಳು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಶೀರ್ಷಿಕೆ,
                           size_t hlen, /* length of header */
                           ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ type)
{
  ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;
  ಅಕ್ಷರ *name = ಖಾಲಿ;
  ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *hs;
  CURLcode ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಥಿರಾಂಕ size_t ilen = hlen;

  ಆದರೆ((ಶೀರ್ಷಿಕೆ[0] == '\r') || (ಶೀರ್ಷಿಕೆ[0] == '\n'))
    /* ignore the body separator */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* trim off newline characters */
  ಆದರೆ(hlen && (ಶೀರ್ಷಿಕೆ[hlen - 1] == '\n'))
    hlen--;
  ಆದರೆ(hlen && (ಶೀರ್ಷಿಕೆ[hlen - 1] == '\r'))
    hlen--;
  ಆದರೆ(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;

  ಆದರೆ(ISBLANK(ಶೀರ್ಷಿಕೆ[0])) {
    /* pass leading blanks */
    ಆಗ(hlen && ISBLANK(*ಶೀರ್ಷಿಕೆ)) {
      ಶೀರ್ಷಿಕೆ++;
      hlen--;
    }
    ಆದರೆ(!hlen)
      ಹಿಂತಿರುಗಿ CURLE_WEIRD_SERVER_ಉತ್ತರ;
  }
  ಆದರೆ(Curl_llist_ಎಣಿಕೆ(&ದತ್ತಾಂಶ->state.httphdrs) >= MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ) {
    failf(ದತ್ತಾಂಶ, "Too many response headers, %d is max",
          MAX_HTTP_RESP_ಶೀರ್ಷಿಕೆ_ಎಣಿಕೆ);
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, ಗಾತ್ರ(*hs) + hlen);
  ಆದರೆ(!hs)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಸ್ಮರಣೆ_ನಕಲು(hs->ಬಫರ್, ಶೀರ್ಷಿಕೆ, hlen);
  hs->ಬಫರ್[hlen] = 0; /* null-terminate */

  ಫಲಿತಾಂಶ = namevalue(hs->ಬಫರ್, hlen, type, &name, &ಮೌಲ್ಯ);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    hs->name = name;
    hs->ಮೌಲ್ಯ = ಮೌಲ್ಯ;
    hs->type = type;
    hs->request = ದತ್ತಾಂಶ->state.ವಿನಂತಿಗಳು;

    /* insert this node into the list of headers */
    Curl_llist_append(&ದತ್ತಾಂಶ->state.httphdrs, hs, &hs->ಗಂಟು);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    failf(ದತ್ತಾಂಶ, "Invalid response header");
    curlx_free(hs);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ಸ್ಥಿರ ಶೂನ್ಯ ಶೀರ್ಷಿಕೆಗಳು_reset(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  Curl_llist_init(&ದತ್ತಾಂಶ->state.httphdrs, ಖಾಲಿ);
  ದತ್ತಾಂಶ->state.nh_ಎಣಿಕೆ = 0;
}

ರಚನೆ hds_cw_collect_ctx {
  ರಚನೆ Curl_cwriter super;
};

ಸ್ಥಿರ CURLcode hds_cw_collect_write(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ Curl_cwriter *writer, ಪೂರ್ಣಾಂಕ type,
                                     ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *buf, size_t blen)
{
  ಆದರೆ((type & CLIENTWRITE_ಶೀರ್ಷಿಕೆ) && !(type & CLIENTWRITE_ಸ್ಥಿತಿ)) {
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ htype = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_ಶೀರ್ಷಿಕೆ)));
    CURLcode ಫಲಿತಾಂಶ = Curl_ಶೀರ್ಷಿಕೆಗಳು_ತಳ್ಳು(ದತ್ತಾಂಶ, buf, blen, htype);
    CURL_TRC_WRITE(ದತ್ತಾಂಶ, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (ಪೂರ್ಣಾಂಕ)ಫಲಿತಾಂಶ);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
  }
  ಹಿಂತಿರುಗಿ Curl_cwriter_write(ದತ್ತಾಂಶ, writer->ಮುಂದಿನ, type, buf, blen);
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype hds_cw_collect = {
  "hds-collect",
  ಖಾಲಿ,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  ಗಾತ್ರ(ರಚನೆ hds_cw_collect_ctx)
};

CURLcode Curl_ಶೀರ್ಷಿಕೆಗಳು_init(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_cwriter *writer;
  CURLcode ಫಲಿತಾಂಶ;

  ಆದರೆ(ದತ್ತಾಂಶ->conn && (ದತ್ತಾಂಶ->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    ಆದರೆ(Curl_cwriter_get_by_name(ದತ್ತಾಂಶ, hds_cw_collect.name))
      ಹಿಂತಿರುಗಿ CURLE_OK;

    ಫಲಿತಾಂಶ = Curl_cwriter_create(&writer, ದತ್ತಾಂಶ, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

    ಫಲಿತಾಂಶ = Curl_cwriter_ಕೂಡಿಸು(ದತ್ತಾಂಶ, writer);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      Curl_cwriter_free(ದತ್ತಾಂಶ, writer);
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_ಶೀರ್ಷಿಕೆಗಳು_cleanup(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  ರಚನೆ Curl_llist_ಗಂಟು *n;

  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&ದತ್ತಾಂಶ->state.httphdrs); e; e = n) {
    ರಚನೆ Curl_ಶೀರ್ಷಿಕೆ_store *hs = Curl_ಗಂಟು_elem(e);
    n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
    curlx_free(hs);
  }
  ಶೀರ್ಷಿಕೆಗಳು_reset(ದತ್ತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

#ಇಲ್ಲದಿದ್ದರೆ /* HTTP-disabled builds below */

CURLHcode curl_easy_ಶೀರ್ಷಿಕೆ(CURL *easy,
                           ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *name,
                           size_t ಸೂಚ್ಯಂಕ,
                           ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ origin,
                           ಪೂರ್ಣಾಂಕ request,
                           ರಚನೆ curl_ಶೀರ್ಷಿಕೆ **hout)
{
  (ಶೂನ್ಯ)easy;
  (ಶೂನ್ಯ)name;
  (ಶೂನ್ಯ)ಸೂಚ್ಯಂಕ;
  (ಶೂನ್ಯ)origin;
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)hout;
  ಹಿಂತಿರುಗಿ CURLHE_NOT_BUILT_ಒಳ;
}

ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *curl_easy_nextheader(CURL *easy,
                                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ type,
                                         ಪೂರ್ಣಾಂಕ request,
                                         ರಚನೆ curl_ಶೀರ್ಷಿಕೆ *prev)
{
  (ಶೂನ್ಯ)easy;
  (ಶೂನ್ಯ)type;
  (ಶೂನ್ಯ)request;
  (ಶೂನ್ಯ)prev;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ
