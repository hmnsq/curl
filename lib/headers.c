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

#ضمن "urldata.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "headers.h"
#ضمن "strcase.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_سرلیکونه_API)

/* Generate the curl_header struct for the user. This function MUST assign all
   struct fields in the output struct. */
ساكن فراغ copy_سرلیک_external(بنية Curl_سرلیک_store *hs,
                                 size_t شاخص,
                                 size_t amount,
                                 بنية Curl_llist_غوټه *e,
                                 بنية curl_سرلیک *hout)
{
  بنية curl_سرلیک *h = hout;
  h->name = hs->name;
  h->ارزښت = hs->ارزښت;
  h->amount = amount;
  h->شاخص = شاخص;
  /* this will randomly OR a reserved bit for the sole purpose of making it
     impossible for applications to do == comparisons, as that would otherwise
     be tempting and then lead to the reserved bits not being reserved
     anymore. */
  h->origin = (بینښه صحيح)(hs->type | (1 << 27));
  h->anchor = e;
}

/* public API */
CURLHcode curl_easy_سرلیک(CURL *curl,
                           ثابت حرف *name,
                           size_t nameindex,
                           بینښه صحيح origin,
                           صحيح request,
                           بنية curl_سرلیک **hout)
{
  بنية Curl_eapi_guard guard;
  CURLHcode hresult = CURLHE_OK;
  CURLcode پایله;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_سرلیک, &پایله)) {
    بنية Curl_easy *معلومات = curl;
    بنية Curl_llist_غوټه *e;
    بنية Curl_llist_غوټه *e_pick = عدم;
    size_t سمون = 0;
    size_t amount = 0;
    بنية Curl_سرلیک_store *hs = عدم;
    بنية Curl_سرلیک_store *pick = عدم;
    إذا(!name || !hout || !معلومات ||
       (origin > (CURLH_سرلیک | CURLH_TRAILER | CURLH_CONNECT | CURLH_1XX |
                  CURLH_PSEUDO)) || !origin || (request < -1)) {
      hresult = CURLHE_BAD_دلیل;
      اذهب بهر;
    }
    إذا(!Curl_llist_شمېر(&معلومات->state.httphdrs)) {
      hresult = CURLHE_NOHEADERS; /* no headers available */
      اذهب بهر;
    }
    إذا(request > معلومات->state.غوښتنې) {
      hresult = CURLHE_NOREQUEST;
      اذهب بهر;
    }
    إذا(request == -1)
      request = معلومات->state.غوښتنې;

    /* we need a first round to count amount of this header */
    لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e; e = Curl_غوټه_بل(e)) {
      hs = Curl_غوټه_elem(e);
      إذا(curl_strequal(hs->name, name) &&
         (hs->type & origin) &&
         (hs->request == request)) {
        amount++;
        pick = hs;
        e_pick = e;
      }
    }
    إذا(!amount)
      hresult = CURLHE_MISSING;
    وإلا إذا(nameindex >= amount)
      hresult = CURLHE_BADINDEX;
    إذا(hresult)
      اذهب بهر;

    إذا(nameindex == amount - 1)
      /* if the last or only occurrence is what's asked for, then we know it */
      hs = pick;
    وإلا {
      لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e;
          e = Curl_غوټه_بل(e)) {
        hs = Curl_غوټه_elem(e);
        إذا(curl_strequal(hs->name, name) &&
           (hs->type & origin) &&
           (hs->request == request) &&
           (سمون++ == nameindex)) {
          e_pick = e;
          اكسر;
        }
      }
      إذا(!e) { /* this should not happen */
        hresult = CURLHE_MISSING;
        اذهب بهر;
      }
    }
    /* this is the name we want */
    copy_سرلیک_external(hs, nameindex, amount, e_pick,
                         &معلومات->state.headerout[0]);
    *hout = &معلومات->state.headerout[0];
    hresult = CURLHE_OK;
  }
بهر:
  CURL_EAPI_LEAVE(&guard);
  إذا(پایله)
    hresult = Curl_eapi_hcode(پایله);
  أرجع hresult;
}

بنية nextheader_cache_ثبت {
  بنية Curl_سرلیک_store *سرلیک;
  size_t ترتیب;
};

ساكن صحيح nextheader_compare(ثابت فراغ *p1, ثابت فراغ *p2)
{
  ثابت بنية nextheader_cache_ثبت *e1 = p1;
  ثابت بنية nextheader_cache_ثبت *e2 = p2;
  ثابت بنية Curl_سرلیک_store *h1 = e1->سرلیک;
  ثابت بنية Curl_سرلیک_store *h2 = e2->سرلیک;
  ثابت حرف *n1 = h1->name;
  ثابت حرف *n2 = h2->name;

  طالما(*n1 && *n2) {
    بینښه حرف c1 = (بینښه حرف)Curl_raw_toupper(*n1++);
    بینښه حرف c2 = (بینښه حرف)Curl_raw_toupper(*n2++);
    إذا(c1 != c2)
      أرجع c1 > c2 ? 1 : -1;
  }
  إذا(*n1)
    أرجع 1;
  إذا(*n2)
    أرجع -1;
  إذا(e1->ترتیب != e2->ترتیب)
    أرجع e1->ترتیب > e2->ترتیب ? 1 : -1;
  أرجع 0;
}

ساكن bool nextheader_cache_build(بنية Curl_easy *معلومات,
                                   بینښه صحيح origin,
                                   صحيح request, size_t شمېر)
{
  بنية nextheader_cache_ثبت *سرلیکونه;
  بنية Curl_llist_غوټه *e;
  size_t i = 0;
  size_t لومړی;

  سرلیکونه = curlx_malloc(حجم(*سرلیکونه) * شمېر);
  إذا(!سرلیکونه)
    أرجع FALSE;

  لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e;
      e = Curl_غوټه_بل(e)) {
    بنية Curl_سرلیک_store *hs = Curl_غوټه_elem(e);
    إذا((hs->type & origin) && (hs->request == request)) {
      سرلیکونه[i].سرلیک = hs;
      سرلیکونه[i].ترتیب = i;
      i++;
    }
  }

  qsort(سرلیکونه, i, حجم(*سرلیکونه), nextheader_compare);
  لكل(لومړی = 0; لومړی < i;) {
    size_t وروستی = لومړی + 1;
    size_t شاخص;
    طالما((وروستی < i) &&
          curl_strequal(سرلیکونه[لومړی].سرلیک->name,
                        سرلیکونه[وروستی].سرلیک->name))
      وروستی++;
    لكل(شاخص = لومړی; شاخص < وروستی; شاخص++) {
      سرلیکونه[شاخص].سرلیک->nh_amount = وروستی - لومړی;
      سرلیکونه[شاخص].سرلیک->nh_شاخص = شاخص - لومړی;
    }
    لومړی = وروستی;
  }
  curlx_free(سرلیکونه);

  معلومات->state.nh_origin = origin;
  معلومات->state.nh_request = request;
  معلومات->state.nh_شمېر = شمېر;
  أرجع TRUE;
}

/* public API */
بنية curl_سرلیک *curl_easy_nextheader(CURL *curl,
                                         بینښه صحيح origin,
                                         صحيح request,
                                         بنية curl_سرلیک *prev)
{
  بنية Curl_eapi_guard guard;
  بنية curl_سرلیک *hd = عدم;
  CURLcode پایله;

  إذا(CURL_EAPI_ENTER(&guard, curl, easy_nextheader, &پایله)) {
    بنية Curl_easy *معلومات = curl;
    بنية Curl_llist_غوټه *pick;
    بنية Curl_llist_غوټه *e;
    بنية Curl_سرلیک_store *hs;
    size_t amount = 0;
    size_t شاخص = 0;
    size_t شمېر;

    إذا(request > معلومات->state.غوښتنې)
      اذهب بهر;
    إذا(request == -1)
      request = معلومات->state.غوښتنې;

    إذا(prev) {
      pick = prev->anchor;
      إذا(!pick)
        /* something is wrong */
        اذهب بهر;
      pick = Curl_غوټه_بل(pick);
    }
    وإلا
      pick = Curl_llist_سر(&معلومات->state.httphdrs);

    إذا(pick) {
      /* make sure it is the next header of the desired type */
      افعل {
        hs = Curl_غوټه_elem(pick);
        إذا((hs->type & origin) && (hs->request == request))
          اكسر;
        pick = Curl_غوټه_بل(pick);
      } طالما(pick);
    }

    إذا(!pick)
      /* no more headers available */
      اذهب بهر;

    hs = Curl_غوټه_elem(pick);
    شمېر = Curl_llist_شمېر(&معلومات->state.httphdrs);

    إذا(((معلومات->state.nh_شمېر == شمېر) &&
        (معلومات->state.nh_origin == origin) &&
        (معلومات->state.nh_request == request)) ||
       nextheader_cache_build(معلومات, origin, request, شمېر)) {
      amount = hs->nh_amount;
      شاخص = hs->nh_شاخص;
    }
    وإلا {
      /* count number of occurrences of this name within the mask and figure
         out the index for the currently selected entry */
      لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e;
          e = Curl_غوټه_بل(e)) {
        بنية Curl_سرلیک_store *check = Curl_غوټه_elem(e);
        إذا(curl_strequal(hs->name, check->name) &&
           (check->request == request) &&
           (check->type & origin))
          amount++;
        إذا(e == pick)
          شاخص = amount - 1;
      }
    }
    copy_سرلیک_external(hs, شاخص, amount, pick,
                         &معلومات->state.headerout[1]);
    hd = &معلومات->state.headerout[1];
  }
بهر:
  CURL_EAPI_LEAVE(&guard);
  أرجع hd;
}

ساكن CURLcode namevalue(حرف *سرلیک, size_t hlen, بینښه صحيح type,
                          حرف **name, حرف **ارزښت)
{
  حرف *پای_ټکی = سرلیک + hlen - 1; /* point to the last byte */
  DEBUGASSERT(hlen);
  *name = سرلیک;

  إذا(type == CURLH_PSEUDO) {
    إذا(*سرلیک != ':')
      أرجع CURLE_BAD_FUNCTION_دلیل;
    سرلیک++;
  }

  /* Find the end of the header name */
  طالما(*سرلیک && (*سرلیک != ':'))
    ++سرلیک;

  إذا(*سرلیک)
    /* Skip over colon, null it */
    *سرلیک++ = 0;
  وإلا
    أرجع CURLE_BAD_FUNCTION_دلیل;

  /* skip all leading blank letters */
  طالما(ISBLANK(*سرلیک))
    سرلیک++;

  *ارزښت = سرلیک;

  /* skip all trailing space letters */
  طالما((پای_ټکی > سرلیک) && ISBLANK(*پای_ټکی))
    *پای_ټکی-- = 0; /* null-terminate */
  أرجع CURLE_OK;
}

/*
 * Curl_headers_push() gets passed a full HTTP header to store. It gets called
 * immediately before the header callback. The header is CRLF, CR or LF
 * terminated.
 */
CURLcode Curl_سرلیکونه_ټېل(بنية Curl_easy *معلومات, ثابت حرف *سرلیک,
                           size_t hlen, /* length of header */
                           بینښه حرف type)
{
  حرف *ارزښت = عدم;
  حرف *name = عدم;
  بنية Curl_سرلیک_store *hs;
  CURLcode پایله = CURLE_بهر_د_حافظه;
  ثابت size_t ilen = hlen;

  إذا((سرلیک[0] == '\r') || (سرلیک[0] == '\n'))
    /* ignore the body separator */
    أرجع CURLE_OK;

  /* trim off newline characters */
  إذا(hlen && (سرلیک[hlen - 1] == '\n'))
    hlen--;
  إذا(hlen && (سرلیک[hlen - 1] == '\r'))
    hlen--;
  إذا(hlen == ilen)
    /* neither CR nor LF as terminator is not a valid header */
    أرجع CURLE_WEIRD_SERVER_ځواب;

  إذا(ISBLANK(سرلیک[0])) {
    /* pass leading blanks */
    طالما(hlen && ISBLANK(*سرلیک)) {
      سرلیک++;
      hlen--;
    }
    إذا(!hlen)
      أرجع CURLE_WEIRD_SERVER_ځواب;
  }
  إذا(Curl_llist_شمېر(&معلومات->state.httphdrs) >= MAX_HTTP_RESP_سرلیک_شمېر) {
    failf(معلومات, "Too many response headers, %d is max",
          MAX_HTTP_RESP_سرلیک_شمېر);
    أرجع CURLE_TOO_LARGE;
  }

  hs = curlx_calloc(1, حجم(*hs) + hlen);
  إذا(!hs)
    أرجع CURLE_بهر_د_حافظه;
  حافظه_نقل(hs->بفر, سرلیک, hlen);
  hs->بفر[hlen] = 0; /* null-terminate */

  پایله = namevalue(hs->بفر, hlen, type, &name, &ارزښت);
  إذا(!پایله) {
    hs->name = name;
    hs->ارزښت = ارزښت;
    hs->type = type;
    hs->request = معلومات->state.غوښتنې;

    /* insert this node into the list of headers */
    Curl_llist_append(&معلومات->state.httphdrs, hs, &hs->غوټه);
  }
  وإلا {
    failf(معلومات, "Invalid response header");
    curlx_free(hs);
  }
  أرجع پایله;
}

/*
 * Curl_headers_reset(). Reset the headers subsystem.
 */
ساكن فراغ سرلیکونه_reset(بنية Curl_easy *معلومات)
{
  Curl_llist_init(&معلومات->state.httphdrs, عدم);
  معلومات->state.nh_شمېر = 0;
}

بنية hds_cw_collect_ctx {
  بنية Curl_cwriter super;
};

ساكن CURLcode hds_cw_collect_write(بنية Curl_easy *معلومات,
                                     بنية Curl_cwriter *writer, صحيح type,
                                     ثابت حرف *buf, size_t blen)
{
  إذا((type & CLIENTWRITE_سرلیک) && !(type & CLIENTWRITE_حالت)) {
    بینښه حرف htype = (بینښه حرف)
      (type & CLIENTWRITE_CONNECT ? CURLH_CONNECT :
       (type & CLIENTWRITE_1XX ? CURLH_1XX :
        (type & CLIENTWRITE_TRAILER ? CURLH_TRAILER :
         CURLH_سرلیک)));
    CURLcode پایله = Curl_سرلیکونه_ټېل(معلومات, buf, blen, htype);
    CURL_TRC_WRITE(معلومات, "header_collect pushed(type=%x, len=%zu) -> %d",
                   htype, blen, (صحيح)پایله);
    إذا(پایله)
      أرجع پایله;
  }
  أرجع Curl_cwriter_write(معلومات, writer->بل, type, buf, blen);
}

ساكن ثابت بنية Curl_cwtype hds_cw_collect = {
  "hds-collect",
  عدم,
  0,
  Curl_cwriter_def_init,
  hds_cw_collect_write,
  Curl_cwriter_def_flush,
  Curl_cwriter_def_close,
  حجم(بنية hds_cw_collect_ctx)
};

CURLcode Curl_سرلیکونه_init(بنية Curl_easy *معلومات)
{
  بنية Curl_cwriter *writer;
  CURLcode پایله;

  إذا(معلومات->conn && (معلومات->conn->scheme->protocol & PROTO_FAMILY_HTTP)) {
    /* avoid installing it twice */
    إذا(Curl_cwriter_get_by_name(معلومات, hds_cw_collect.name))
      أرجع CURLE_OK;

    پایله = Curl_cwriter_create(&writer, معلومات, &hds_cw_collect,
                                 CURL_CW_PROTOCOL);
    إذا(پایله)
      أرجع پایله;

    پایله = Curl_cwriter_جمع_کړه(معلومات, writer);
    إذا(پایله) {
      Curl_cwriter_free(معلومات, writer);
      أرجع پایله;
    }
  }
  أرجع CURLE_OK;
}

/*
 * Curl_headers_cleanup(). Free all stored headers and associated memory.
 */
CURLcode Curl_سرلیکونه_cleanup(بنية Curl_easy *معلومات)
{
  بنية Curl_llist_غوټه *e;
  بنية Curl_llist_غوټه *n;

  لكل(e = Curl_llist_سر(&معلومات->state.httphdrs); e; e = n) {
    بنية Curl_سرلیک_store *hs = Curl_غوټه_elem(e);
    n = Curl_غوټه_بل(e);
    curlx_free(hs);
  }
  سرلیکونه_reset(معلومات);
  أرجع CURLE_OK;
}

#وإلا /* HTTP-disabled builds below */

CURLHcode curl_easy_سرلیک(CURL *easy,
                           ثابت حرف *name,
                           size_t شاخص,
                           بینښه صحيح origin,
                           صحيح request,
                           بنية curl_سرلیک **hout)
{
  (فراغ)easy;
  (فراغ)name;
  (فراغ)شاخص;
  (فراغ)origin;
  (فراغ)request;
  (فراغ)hout;
  أرجع CURLHE_NOT_BUILT_دننه;
}

بنية curl_سرلیک *curl_easy_nextheader(CURL *easy,
                                         بینښه صحيح type,
                                         صحيح request,
                                         بنية curl_سرلیک *prev)
{
  (فراغ)easy;
  (فراغ)type;
  (فراغ)request;
  (فراغ)prev;
  أرجع عدم;
}
#د_که_پای
