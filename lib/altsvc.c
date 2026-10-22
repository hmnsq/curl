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
/*
 * The Alt-Svc: header is defined in RFC 7838:
 * https://datatracker.ietf.org/doc/html/rfc7838
 */
#ضمن "curl_setup.h"

#إذا !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ضمن "urldata.h"
#ضمن "altsvc.h"
#ضمن "curl_fopen.h"
#ضمن "curl_get_line.h"
#ضمن "parsedate.h"
#ضمن "curl_trc.h"
#ضمن "curlx/inet_pton.h"
#ضمن "curlx/strparse.h"
#ضمن "connect.h"

#وصف_ڪريو MAX_ALTSVC_سٽ    4095
#وصف_ڪريو MAX_ALTSVC_DATELEN 17
#وصف_ڪريو MAX_ALTSVC_HOSTLEN 2048
#وصف_ڪريو MAX_ALTSVC_ALPNLEN 10

#وصف_ڪريو H3_نسخو "h3"

#إذا defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ساكن time_t altsvc_debugtime(فراغ *اڻ_استعمال_ٿيل)
{
  ثابت حرف *timestr = getenv("CURL_TIME");
  (فراغ)اڻ_استعمال_ٿيل;
  إذا(timestr) {
    curl_off_t val;
    curlx_str_عدد(&timestr, &val, وقت_T_MAX);
    أرجع (time_t)val;
  }
  أرجع وقت(عدم);
}
#وصف_رد وقت
#وصف_ڪريو وقت(x) altsvc_debugtime(x)
#جيڪڏهن_پورو

/* Given the ALPN ID, return the name */
ثابت حرف *Curl_alpnid2str(تعداد alpnid id)
{
  بدل(id) {
  حالة ALPN_h1:
    أرجع "h1";
  حالة ALPN_h2:
    أرجع "h2";
  حالة ALPN_h3:
    أرجع H3_نسخو;
  افتراضي:
    أرجع ""; /* bad */
  }
}

ساكن تعداد alpnid Curl_str2alpnid(ثابت بنية Curl_str *cstr)
{
  أرجع Curl_alpn2alpnid((ثابت بنانشان حرف *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#وصف_ڪريو altsvc_free(x) curlx_free(x)

ساكن بنية altsvc *altsvc_createid(ثابت حرف *srchost,
                                      size_t hlen,
                                      ثابت حرف *dsthost,
                                      size_t dlen, /* dsthost length */
                                      تعداد alpnid srcalpnid,
                                      تعداد alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  بنية altsvc *as;
  إذا((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  وإلا إذا(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  إذا((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  إذا(!hlen || !dlen)
    /* bad input */
    أرجع عدم;
  /* struct size plus both strings */
  as = curlx_calloc(1, حجم(بنية altsvc) + (hlen + 1) + (dlen + 1));
  إذا(!as)
    أرجع عدم;
  as->src.host = (حرف *)as + حجم(بنية altsvc);
  يادگيري_نقل(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (حرف *)as + حجم(بنية altsvc) + hlen + 1;
  يادگيري_نقل(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.پورٽ = (بنانشان ننڍو)srcport;
  as->dst.پورٽ = (بنانشان ننڍو)dstport;

  أرجع as;
}

ساكن بنية altsvc *altsvc_create(بنية Curl_str *srchost,
                                    بنية Curl_str *dsthost,
                                    بنية Curl_str *srcalpn,
                                    بنية Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  تعداد alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  تعداد alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  إذا(!srcalpnid || !dstalpnid)
    أرجع عدم;
  أرجع altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
پهريون*/
ساكن فراغ altsvc_append(بنية altsvcinfo *asi, بنية altsvc *as)
{
  طالما(Curl_llist_ڳڻپ(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    بنية Curl_llist_ڳنڍ *e = Curl_llist_سر(&asi->list);
    بنية altsvc *oldas = Curl_ڳنڍ_elem(e);
    Curl_ڳنڍ_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ڳنڍ);
}

/* only returns SERIOUS errors */
ساكن CURLcode altsvc_جوڙيو(بنية altsvcinfo *asi, ثابت حرف *سٽ)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  بنية Curl_str srchost;
  بنية Curl_str dsthost;
  بنية Curl_str srcalpn;
  بنية Curl_str dstalpn;
  بنية Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  إذا(curlx_str_لفظ(&سٽ, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_لفظ(&سٽ, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_عدد(&سٽ, &srcport, 65535) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_لفظ(&سٽ, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_لفظ(&سٽ, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_عدد(&سٽ, &dstport, 65535) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_quotedword(&سٽ, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_عدد(&سٽ, &persist, 1) ||
     curlx_str_singlespace(&سٽ) ||
     curlx_str_عدد(&سٽ, &prio, 0) ||
     curlx_str_newline(&سٽ))
    ;
  وإلا {
    حرف dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = وقت(عدم);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    يادگيري_نقل(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    إذا(now < expires) {
      بنية altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      إذا(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      وإلا
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
  }

  أرجع CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ساكن CURLcode altsvc_ڀريو(بنية altsvcinfo *asi, ثابت حرف *file)
{
  CURLcode نتيجو = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  إذا(!asi->filename)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  إذا(fp) {
    curlx_struct_stat stat;
    إذا((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_گهڻائي)) {
      bool eof = FALSE;
      بنية dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_سٽ);
      افعل {
        نتيجو = Curl_get_سٽ(&buf, fp, &eof);
        إذا(!نتيجو) {
          ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          إذا(curlx_str_single(&lineptr, '#'))
            altsvc_جوڙيو(asi, lineptr);
        }
      } طالما(!نتيجو && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  أرجع نتيجو;
}

/*
 * Write this single altsvc entry to a single output line
 */
ساكن CURLcode altsvc_ٻاهريون(بنية altsvc *as, FILE *fp)
{
  بنية tm stamp;
  ثابت حرف *dst6_pre = "";
  ثابت حرف *dst6_post = "";
  ثابت حرف *src6_pre = "";
  ثابت حرف *src6_post = "";
  CURLcode نتيجو = curlx_gmtime(as->expires, &stamp);
  إذا(نتيجو)
    /* fail silently for date errors so that only this entry gets
       skipped */
    أرجع CURLE_OK;
#جيڪڏهن_وصف_ٿيل USE_IPV6
  وإلا {
    حرف ipv6_اڻ_استعمال_ٿيل[16];
    إذا(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_اڻ_استعمال_ٿيل) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    إذا(curlx_inet_pton(AF_INET6, as->src.host, ipv6_اڻ_استعمال_ٿيل) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#جيڪڏهن_پورو
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.پورٽ,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.پورٽ,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  أرجع CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
بنية altsvcinfo *Curl_altsvc_init(فراغ)
{
  بنية altsvcinfo *asi = curlx_calloc(1, حجم(بنية altsvcinfo));
  إذا(!asi)
    أرجع عدم;
  Curl_llist_init(&asi->list, عدم);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#جيڪڏهن_وصف_ٿيل USE_HTTP2
    | CURLALTSVC_H2
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_HTTP3
    | CURLALTSVC_H3
#جيڪڏهن_پورو
    ;
  أرجع asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_ڀريو(بنية altsvcinfo *asi, ثابت حرف *file)
{
  DEBUGASSERT(asi);
  أرجع altsvc_ڀريو(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(بنية Curl_easy *معلومات, ثابت ڊگهو ctrl)
{
  DEBUGASSERT(معلومات);
  إذا(!ctrl)
    أرجع CURLE_BAD_FUNCTION_دليل;

  إذا(!معلومات->asi) {
    معلومات->asi = Curl_altsvc_init();
    إذا(!معلومات->asi)
      أرجع CURLE_ٻاهريون_جو_يادگيري;
  }
  معلومات->asi->flags = ctrl;
  أرجع CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
فراغ Curl_altsvc_cleanup(بنية altsvcinfo **asi)
{
  إذا(*asi) {
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_llist_ڳنڍ *n;
    بنية altsvcinfo *altsvc = *asi;
    لكل(e = Curl_llist_سر(&altsvc->list); e; e = n) {
      بنية altsvc *as = Curl_ڳنڍ_elem(e);
      n = Curl_ڳنڍ_ايندڙ(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = عدم; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(بنية Curl_easy *معلومات,
                          بنية altsvcinfo *asi, ثابت حرف *file)
{
  CURLcode نتيجو = CURLE_OK;
  FILE *ٻاهريون;
  حرف *tempstore = عدم;

  إذا(!asi)
    /* no cache activated */
    أرجع CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  إذا(!file && asi->filename)
    file = asi->filename;

  إذا((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    أرجع CURLE_OK;

  نتيجو = Curl_fopen(معلومات, file, &ٻاهريون, &tempstore);
  إذا(!نتيجو) {
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_llist_ڳنڍ *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          ٻاهريون);
    لكل(e = Curl_llist_سر(&asi->list); e; e = n) {
      بنية altsvc *as = Curl_ڳنڍ_elem(e);
      n = Curl_ڳنڍ_ايندڙ(e);
      نتيجو = altsvc_ٻاهريون(as, ٻاهريون);
      إذا(نتيجو)
        اكسر;
    }
    curlx_fclose(ٻاهريون);
    إذا(!نتيجو && tempstore && curlx_rename(tempstore, file))
      نتيجو = CURLE_WRITE_ERROR;

    إذا(نتيجو && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  أرجع نتيجو;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ساكن bool hostcompare(ثابت حرف *host, ثابت حرف *check)
{
  size_t hlen = لڙي_ڊيگهه(host);
  size_t clen = لڙي_ڊيگهه(check);

  إذا(hlen && (host[hlen - 1] == '.'))
    hlen--;
  إذا(hlen != clen)
    /* they cannot match if they have different lengths */
    أرجع FALSE;
  أرجع curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ساكن فراغ altsvc_flush(بنية altsvcinfo *asi,
                         بنية Curl_peer *origin,
                         تعداد alpnid origin_alpnid)
{
  بنية Curl_llist_ڳنڍ *e;
  بنية Curl_llist_ڳنڍ *n;
  لكل(e = Curl_llist_سر(&asi->list); e; e = n) {
    بنية altsvc *as = Curl_ڳنڍ_elem(e);
    n = Curl_ڳنڍ_ايندڙ(e);
    إذا((origin_alpnid == as->src.alpnid) &&
       (origin->پورٽ == as->src.پورٽ) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ڳنڍ_remove(e);
      altsvc_free(as);
    }
  }
}

#وصف_ڪريو CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#وصف_ڪريو ALTSVC_MA      1
#وصف_ڪريو ALTSVC_PERSIST 2
ساكن فراغ altsvc_parse_params(ثابت حرف **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  إذا(curlx_str_single(pp, ';'))
    أرجع;

  لكل(;;) {
    بنية Curl_str name;
    بنية Curl_str val;
    curl_off_t انگ;
    صحيح keyword = 0;

    /* allow some extra whitespaces around name and value */
    إذا(curlx_str_until(pp, &name, MAX_ALTSVC_سٽ, '=') ||
       curlx_str_single(pp, '='))
      اكسر;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    إذا(**pp == '\"') {
      إذا(curlx_str_quotedword(pp, &val, MAX_ALTSVC_سٽ))
        اكسر;
    }
    وإلا {
      إذا(curlx_str_cspn(pp, &val, ",;\r\n"))
        اكسر;
    }
    curlx_str_trimblanks(&val);

    إذا(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    وإلا إذا(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    إذا(keyword) {
      ثابت حرف *vp = curlx_str(&val);
      ثابت حرف *vend = vp + curlx_strlen(&val);
      إذا(curlx_str_عدد(&vp, &انگ, CURL_OFF_T_MAX))
        اكسر; /* not a number, skip further parameter parsing */
      إذا(vp != vend)
        اكسر; /* not entirely a number, skip further parameter parsing */
      إذا(keyword == ALTSVC_MA) {
        إذا(انگ > CAP_ALTSVC_MAX_AGE)
          انگ = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)انگ;
      }
      وإلا إذا(انگ == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    إذا(curlx_str_single(pp, ';'))
      اكسر; /* no further parameters */
  }
}

ساكن bool altsvc_parse_dest(ثابت حرف **pp,
                              بنية Curl_easy *معلومات,
                              بنية Curl_peer *origin,
                              بنية Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t پورٽ = 0;

  إذا(curlx_str_single(pp, '\"'))
    أرجع FALSE;

  /* quoted string, with hostname or just :port ? */
  إذا(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    إذا(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      إذا(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(معلومات, "Bad alt-svc hostname, ignoring.");
        أرجع FALSE;
      }
    }
    وإلا { /* IPv6 hostname */
      إذا(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(معلومات, "Bad alt-svc IPv6 hostname, ignoring.");
        أرجع FALSE;
      }
    }
    إذا(curlx_str_single(pp, ':'))
      أرجع FALSE; /* not followed by ':' */
  }
  وإلا  /* is only :port, hostname is effectively origin */
    curlx_str_سونپيو(dsthost, origin->hostname,
                     لڙي_ڊيگهه(origin->hostname));

  إذا(curlx_str_عدد(pp, &پورٽ, 0xffff)) {
    infof(معلومات, "Unknown alt-svc port number, ignoring.");
    أرجع FALSE;
  }

  *pdstport = (uint16_t)پورٽ;
  إذا(curlx_str_single(pp, '\"'))
    أرجع FALSE; /* quoted string not ending here as expected */
  أرجع TRUE;
}

/*
 * Curl_altsvc_parse() takes an incoming alt-svc response header and stores
 * the data correctly in the cache.
 *
 * 'value' points to the header *value*. That is contents to the right of the
 * header name.
 *
 * Currently this function rejects invalid data without returning an error.
 * Invalid hostname, port number will result in the specific alternative
 * being rejected. Unknown protocols are skipped.
 */
CURLcode Curl_altsvc_parse(بنية Curl_easy *معلومات,
                           بنية altsvcinfo *asi, ثابت حرف *قدر,
                           بنية Curl_peer *origin,
                           تعداد alpnid origin_alpnid)
{
  بنية altsvc *as;
  size_t entries = 0;
  بنية Curl_str alpn;
  ثابت حرف *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = قدر;
  إذا(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    إذا(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      أرجع CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  لكل(p = قدر; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    تعداد alpnid dstalpnid;
    بنية Curl_str dsthost;
    uint16_t dstport;

    إذا(curlx_str_until(&p, &alpn, MAX_ALTSVC_سٽ, '='))
      اكسر; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    إذا(curlx_str_single(&p, '='))
      اكسر;

    /* Parse altsvc hostname:port */
    إذا(!altsvc_parse_dest(&p, معلومات, origin, &dsthost, &dstport))
      اكسر;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    إذا(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      إذا(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, لڙي_ڊيگهه(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->پورٽ, dstport);
      إذا(as) {
        time_t secs = وقت(عدم);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        إذا(maxage > (وقت_T_MAX - secs))
          as->expires = وقت_T_MAX;
        وإلا
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(معلومات, "Added alt-svc: %.*s:%u over %s",
              (صحيح)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      وإلا
        أرجع CURLE_ٻاهريون_جو_يادگيري;
    }

    /* When this is followed by a comma, we expect another entry */
    إذا(curlx_str_single(&p, ','))
      اكسر;
  }

  أرجع CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(بنية altsvcinfo *asi,
                        بنية Curl_peer *origin,
                        تعداد alpnid origin_alpnid,
                        بنية altsvc **dstentry,
                        ثابت صحيح versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  إذا(Curl_llist_ڳڻپ(&asi->list)) {
    بنية Curl_llist_ڳنڍ *e;
    بنية Curl_llist_ڳنڍ *n;
    time_t now = وقت(عدم);

    لكل(e = Curl_llist_سر(&asi->list); e; e = n) {
      بنية altsvc *as = Curl_ڳنڍ_elem(e);
      n = Curl_ڳنڍ_ايندڙ(e);
      إذا(as->expires < now) {
        /* an expired entry, remove */
        Curl_ڳنڍ_remove(e);
        altsvc_free(as);
        استمر;
      }
      إذا((origin_alpnid == as->src.alpnid) &&
         (versions & (صحيح)as->dst.alpnid) &&
         (origin->پورٽ == as->src.پورٽ) &&
         hostcompare(origin->hostname, as->src.host)) {
        /*ميلاپ*/
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->پورٽ == as->dst.پورٽ) &&
                             hostcompare(origin->hostname, as->dst.host);
        أرجع TRUE;
      }
    }
  }
  أرجع FALSE;
}

#إذا defined(DEBUGBUILD) || defined(UNITTESTS)
#وصف_رد وقت
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
