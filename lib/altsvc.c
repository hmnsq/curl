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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "altsvc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_fopen.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_get_line.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "parsedate.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ALTSVC_ਸਤਰ    4095
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ALTSVC_DATELEN 17
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ALTSVC_HOSTLEN 2048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ALTSVC_ALPNLEN 10

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ H3_ਸੰਸਕਰਣ "h3"

#ਜੇ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ਸਥਿਰ time_t altsvc_debugtime(ਖਾਲੀ *ਅਣਵਰਤਿਆ)
{
  ਅਚਲ ਅੱਖਰ *timestr = getenv("CURL_TIME");
  (ਖਾਲੀ)ਅਣਵਰਤਿਆ;
  ਜੇ(timestr) {
    curl_off_t val;
    curlx_str_ਸੰਖਿਆ(&timestr, &val, ਸਮਾਂ_T_MAX);
    ਵਾਪਸ (time_t)val;
  }
  ਵਾਪਸ ਸਮਾਂ(ਨਿਰਾ);
}
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸਮਾਂ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਮਾਂ(x) altsvc_debugtime(x)
#ਜੇ_ਅੰਤ

/* Given the ALPN ID, return the name */
ਅਚਲ ਅੱਖਰ *Curl_alpnid2str(ਗਿਣਤੀ alpnid id)
{
  ਚੋਣ(id) {
  ਹਾਲਤ ALPN_h1:
    ਵਾਪਸ "h1";
  ਹਾਲਤ ALPN_h2:
    ਵਾਪਸ "h2";
  ਹਾਲਤ ALPN_h3:
    ਵਾਪਸ H3_ਸੰਸਕਰਣ;
  ਮੂਲ:
    ਵਾਪਸ ""; /* bad */
  }
}

ਸਥਿਰ ਗਿਣਤੀ alpnid Curl_str2alpnid(ਅਚਲ ਬਣਤਰ Curl_str *cstr)
{
  ਵਾਪਸ Curl_alpn2alpnid((ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ altsvc_free(x) curlx_free(x)

ਸਥਿਰ ਬਣਤਰ altsvc *altsvc_createid(ਅਚਲ ਅੱਖਰ *srchost,
                                      size_t hlen,
                                      ਅਚਲ ਅੱਖਰ *dsthost,
                                      size_t dlen, /* dsthost length */
                                      ਗਿਣਤੀ alpnid srcalpnid,
                                      ਗਿਣਤੀ alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ਬਣਤਰ altsvc *as;
  ਜੇ((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  ਨਹੀਂਤਾਂ ਜੇ(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  ਜੇ((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  ਜੇ(!hlen || !dlen)
    /* bad input */
    ਵਾਪਸ ਨਿਰਾ;
  /* struct size plus both strings */
  as = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ altsvc) + (hlen + 1) + (dlen + 1));
  ਜੇ(!as)
    ਵਾਪਸ ਨਿਰਾ;
  as->src.host = (ਅੱਖਰ *)as + ਆਕਾਰ(ਬਣਤਰ altsvc);
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (ਅੱਖਰ *)as + ਆਕਾਰ(ਬਣਤਰ altsvc) + hlen + 1;
  ਸਮ੍ਰਿਤੀ_ਨਕਲ(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.ਪੋਰਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)srcport;
  as->dst.ਪੋਰਟ = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)dstport;

  ਵਾਪਸ as;
}

ਸਥਿਰ ਬਣਤਰ altsvc *altsvc_create(ਬਣਤਰ Curl_str *srchost,
                                    ਬਣਤਰ Curl_str *dsthost,
                                    ਬਣਤਰ Curl_str *srcalpn,
                                    ਬਣਤਰ Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  ਗਿਣਤੀ alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  ਗਿਣਤੀ alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  ਜੇ(!srcalpnid || !dstalpnid)
    ਵਾਪਸ ਨਿਰਾ;
  ਵਾਪਸ altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
ਪਹਿਲਾ*/
ਸਥਿਰ ਖਾਲੀ altsvc_append(ਬਣਤਰ altsvcinfo *asi, ਬਣਤਰ altsvc *as)
{
  ਜਦੋਂ(Curl_llist_ਗਿਣਤੀ(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ਬਣਤਰ Curl_llist_ਗੰਢ *e = Curl_llist_ਸਿਰ(&asi->list);
    ਬਣਤਰ altsvc *oldas = Curl_ਗੰਢ_elem(e);
    Curl_ਗੰਢ_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ਗੰਢ);
}

/* only returns SERIOUS errors */
ਸਥਿਰ CURLcode altsvc_ਜੋੜੋ(ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *ਸਤਰ)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ਬਣਤਰ Curl_str srchost;
  ਬਣਤਰ Curl_str dsthost;
  ਬਣਤਰ Curl_str srcalpn;
  ਬਣਤਰ Curl_str dstalpn;
  ਬਣਤਰ Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  ਜੇ(curlx_str_ਸ਼ਬਦ(&ਸਤਰ, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸ਼ਬਦ(&ਸਤਰ, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸੰਖਿਆ(&ਸਤਰ, &srcport, 65535) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸ਼ਬਦ(&ਸਤਰ, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸ਼ਬਦ(&ਸਤਰ, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸੰਖਿਆ(&ਸਤਰ, &dstport, 65535) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_quotedword(&ਸਤਰ, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸੰਖਿਆ(&ਸਤਰ, &persist, 1) ||
     curlx_str_singlespace(&ਸਤਰ) ||
     curlx_str_ਸੰਖਿਆ(&ਸਤਰ, &prio, 0) ||
     curlx_str_newline(&ਸਤਰ))
    ;
  ਨਹੀਂਤਾਂ {
    ਅੱਖਰ dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = ਸਮਾਂ(ਨਿਰਾ);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    ਜੇ(now < expires) {
      ਬਣਤਰ altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      ਜੇ(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ਸਥਿਰ CURLcode altsvc_ਲੱਦੋ(ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *file)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  ਜੇ(!asi->filename)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ਜੇ(fp) {
    curlx_struct_stat stat;
    ਜੇ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_ਬਹੁਲਕ)) {
      bool eof = FALSE;
      ਬਣਤਰ dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_ਸਤਰ);
      ਕਰੋ {
        ਨਤੀਜਾ = Curl_get_ਸਤਰ(&buf, fp, &eof);
        ਜੇ(!ਨਤੀਜਾ) {
          ਅਚਲ ਅੱਖਰ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          ਜੇ(curlx_str_single(&lineptr, '#'))
            altsvc_ਜੋੜੋ(asi, lineptr);
        }
      } ਜਦੋਂ(!ਨਤੀਜਾ && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Write this single altsvc entry to a single output line
 */
ਸਥਿਰ CURLcode altsvc_ਬਾਹਰਲਾ(ਬਣਤਰ altsvc *as, FILE *fp)
{
  ਬਣਤਰ tm stamp;
  ਅਚਲ ਅੱਖਰ *dst6_pre = "";
  ਅਚਲ ਅੱਖਰ *dst6_post = "";
  ਅਚਲ ਅੱਖਰ *src6_pre = "";
  ਅਚਲ ਅੱਖਰ *src6_post = "";
  CURLcode ਨਤੀਜਾ = curlx_gmtime(as->expires, &stamp);
  ਜੇ(ਨਤੀਜਾ)
    /* fail silently for date errors so that only this entry gets
       skipped */
    ਵਾਪਸ CURLE_OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_IPV6
  ਨਹੀਂਤਾਂ {
    ਅੱਖਰ ipv6_ਅਣਵਰਤਿਆ[16];
    ਜੇ(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_ਅਣਵਰਤਿਆ) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    ਜੇ(curlx_inet_pton(AF_INET6, as->src.host, ipv6_ਅਣਵਰਤਿਆ) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#ਜੇ_ਅੰਤ
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.ਪੋਰਟ,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.ਪੋਰਟ,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  ਵਾਪਸ CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ਬਣਤਰ altsvcinfo *Curl_altsvc_init(ਖਾਲੀ)
{
  ਬਣਤਰ altsvcinfo *asi = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ altsvcinfo));
  ਜੇ(!asi)
    ਵਾਪਸ ਨਿਰਾ;
  Curl_llist_init(&asi->list, ਨਿਰਾ);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP2
    | CURLALTSVC_H2
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_HTTP3
    | CURLALTSVC_H3
#ਜੇ_ਅੰਤ
    ;
  ਵਾਪਸ asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_ਲੱਦੋ(ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *file)
{
  DEBUGASSERT(asi);
  ਵਾਪਸ altsvc_ਲੱਦੋ(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਲੰਮਾ ctrl)
{
  DEBUGASSERT(ਅੰਕੜੇ);
  ਜੇ(!ctrl)
    ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;

  ਜੇ(!ਅੰਕੜੇ->asi) {
    ਅੰਕੜੇ->asi = Curl_altsvc_init();
    ਜੇ(!ਅੰਕੜੇ->asi)
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਅੰਕੜੇ->asi->flags = ctrl;
  ਵਾਪਸ CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
ਖਾਲੀ Curl_altsvc_cleanup(ਬਣਤਰ altsvcinfo **asi)
{
  ਜੇ(*asi) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;
    ਬਣਤਰ altsvcinfo *altsvc = *asi;
    ਲਈ(e = Curl_llist_ਸਿਰ(&altsvc->list); e; e = n) {
      ਬਣਤਰ altsvc *as = Curl_ਗੰਢ_elem(e);
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ਨਿਰਾ; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *file)
{
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  FILE *ਬਾਹਰਲਾ;
  ਅੱਖਰ *tempstore = ਨਿਰਾ;

  ਜੇ(!asi)
    /* no cache activated */
    ਵਾਪਸ CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  ਜੇ(!file && asi->filename)
    file = asi->filename;

  ਜੇ((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ਵਾਪਸ CURLE_OK;

  ਨਤੀਜਾ = Curl_fopen(ਅੰਕੜੇ, file, &ਬਾਹਰਲਾ, &tempstore);
  ਜੇ(!ਨਤੀਜਾ) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          ਬਾਹਰਲਾ);
    ਲਈ(e = Curl_llist_ਸਿਰ(&asi->list); e; e = n) {
      ਬਣਤਰ altsvc *as = Curl_ਗੰਢ_elem(e);
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      ਨਤੀਜਾ = altsvc_ਬਾਹਰਲਾ(as, ਬਾਹਰਲਾ);
      ਜੇ(ਨਤੀਜਾ)
        ਰੋਕੋ;
    }
    curlx_fclose(ਬਾਹਰਲਾ);
    ਜੇ(!ਨਤੀਜਾ && tempstore && curlx_rename(tempstore, file))
      ਨਤੀਜਾ = CURLE_WRITE_ERROR;

    ਜੇ(ਨਤੀਜਾ && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ਸਥਿਰ bool hostcompare(ਅਚਲ ਅੱਖਰ *host, ਅਚਲ ਅੱਖਰ *check)
{
  size_t hlen = ਲੜੀ_ਲੰਬਾਈ(host);
  size_t clen = ਲੜੀ_ਲੰਬਾਈ(check);

  ਜੇ(hlen && (host[hlen - 1] == '.'))
    hlen--;
  ਜੇ(hlen != clen)
    /* they cannot match if they have different lengths */
    ਵਾਪਸ FALSE;
  ਵਾਪਸ curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ਸਥਿਰ ਖਾਲੀ altsvc_flush(ਬਣਤਰ altsvcinfo *asi,
                         ਬਣਤਰ Curl_peer *origin,
                         ਗਿਣਤੀ alpnid origin_alpnid)
{
  ਬਣਤਰ Curl_llist_ਗੰਢ *e;
  ਬਣਤਰ Curl_llist_ਗੰਢ *n;
  ਲਈ(e = Curl_llist_ਸਿਰ(&asi->list); e; e = n) {
    ਬਣਤਰ altsvc *as = Curl_ਗੰਢ_elem(e);
    n = Curl_ਗੰਢ_ਅਗਲਾ(e);
    ਜੇ((origin_alpnid == as->src.alpnid) &&
       (origin->ਪੋਰਟ == as->src.ਪੋਰਟ) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ਗੰਢ_remove(e);
      altsvc_free(as);
    }
  }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALTSVC_MA      1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ALTSVC_PERSIST 2
ਸਥਿਰ ਖਾਲੀ altsvc_parse_params(ਅਚਲ ਅੱਖਰ **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  ਜੇ(curlx_str_single(pp, ';'))
    ਵਾਪਸ;

  ਲਈ(;;) {
    ਬਣਤਰ Curl_str name;
    ਬਣਤਰ Curl_str val;
    curl_off_t ਅੰਕ;
    ਪੂਰਨ keyword = 0;

    /* allow some extra whitespaces around name and value */
    ਜੇ(curlx_str_until(pp, &name, MAX_ALTSVC_ਸਤਰ, '=') ||
       curlx_str_single(pp, '='))
      ਰੋਕੋ;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    ਜੇ(**pp == '\"') {
      ਜੇ(curlx_str_quotedword(pp, &val, MAX_ALTSVC_ਸਤਰ))
        ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(curlx_str_cspn(pp, &val, ",;\r\n"))
        ਰੋਕੋ;
    }
    curlx_str_trimblanks(&val);

    ਜੇ(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    ਨਹੀਂਤਾਂ ਜੇ(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    ਜੇ(keyword) {
      ਅਚਲ ਅੱਖਰ *vp = curlx_str(&val);
      ਅਚਲ ਅੱਖਰ *vend = vp + curlx_strlen(&val);
      ਜੇ(curlx_str_ਸੰਖਿਆ(&vp, &ਅੰਕ, CURL_OFF_T_MAX))
        ਰੋਕੋ; /* not a number, skip further parameter parsing */
      ਜੇ(vp != vend)
        ਰੋਕੋ; /* not entirely a number, skip further parameter parsing */
      ਜੇ(keyword == ALTSVC_MA) {
        ਜੇ(ਅੰਕ > CAP_ALTSVC_MAX_AGE)
          ਅੰਕ = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)ਅੰਕ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(ਅੰਕ == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    ਜੇ(curlx_str_single(pp, ';'))
      ਰੋਕੋ; /* no further parameters */
  }
}

ਸਥਿਰ bool altsvc_parse_dest(ਅਚਲ ਅੱਖਰ **pp,
                              ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                              ਬਣਤਰ Curl_peer *origin,
                              ਬਣਤਰ Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t ਪੋਰਟ = 0;

  ਜੇ(curlx_str_single(pp, '\"'))
    ਵਾਪਸ FALSE;

  /* quoted string, with hostname or just :port ? */
  ਜੇ(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    ਜੇ(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      ਜੇ(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(ਅੰਕੜੇ, "Bad alt-svc hostname, ignoring.");
        ਵਾਪਸ FALSE;
      }
    }
    ਨਹੀਂਤਾਂ { /* IPv6 hostname */
      ਜੇ(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(ਅੰਕੜੇ, "Bad alt-svc IPv6 hostname, ignoring.");
        ਵਾਪਸ FALSE;
      }
    }
    ਜੇ(curlx_str_single(pp, ':'))
      ਵਾਪਸ FALSE; /* not followed by ':' */
  }
  ਨਹੀਂਤਾਂ  /* is only :port, hostname is effectively origin */
    curlx_str_ਸੌਂਪੋ(dsthost, origin->hostname,
                     ਲੜੀ_ਲੰਬਾਈ(origin->hostname));

  ਜੇ(curlx_str_ਸੰਖਿਆ(pp, &ਪੋਰਟ, 0xffff)) {
    infof(ਅੰਕੜੇ, "Unknown alt-svc port number, ignoring.");
    ਵਾਪਸ FALSE;
  }

  *pdstport = (uint16_t)ਪੋਰਟ;
  ਜੇ(curlx_str_single(pp, '\"'))
    ਵਾਪਸ FALSE; /* quoted string not ending here as expected */
  ਵਾਪਸ TRUE;
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
CURLcode Curl_altsvc_parse(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                           ਬਣਤਰ altsvcinfo *asi, ਅਚਲ ਅੱਖਰ *ਮੁੱਲ,
                           ਬਣਤਰ Curl_peer *origin,
                           ਗਿਣਤੀ alpnid origin_alpnid)
{
  ਬਣਤਰ altsvc *as;
  size_t entries = 0;
  ਬਣਤਰ Curl_str alpn;
  ਅਚਲ ਅੱਖਰ *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = ਮੁੱਲ;
  ਜੇ(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    ਜੇ(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      ਵਾਪਸ CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  ਲਈ(p = ਮੁੱਲ; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    ਗਿਣਤੀ alpnid dstalpnid;
    ਬਣਤਰ Curl_str dsthost;
    uint16_t dstport;

    ਜੇ(curlx_str_until(&p, &alpn, MAX_ALTSVC_ਸਤਰ, '='))
      ਰੋਕੋ; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    ਜੇ(curlx_str_single(&p, '='))
      ਰੋਕੋ;

    /* Parse altsvc hostname:port */
    ਜੇ(!altsvc_parse_dest(&p, ਅੰਕੜੇ, origin, &dsthost, &dstport))
      ਰੋਕੋ;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    ਜੇ(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      ਜੇ(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, ਲੜੀ_ਲੰਬਾਈ(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->ਪੋਰਟ, dstport);
      ਜੇ(as) {
        time_t secs = ਸਮਾਂ(ਨਿਰਾ);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        ਜੇ(maxage > (ਸਮਾਂ_T_MAX - secs))
          as->expires = ਸਮਾਂ_T_MAX;
        ਨਹੀਂਤਾਂ
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(ਅੰਕੜੇ, "Added alt-svc: %.*s:%u over %s",
              (ਪੂਰਨ)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      ਨਹੀਂਤਾਂ
        ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }

    /* When this is followed by a comma, we expect another entry */
    ਜੇ(curlx_str_single(&p, ','))
      ਰੋਕੋ;
  }

  ਵਾਪਸ CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ਬਣਤਰ altsvcinfo *asi,
                        ਬਣਤਰ Curl_peer *origin,
                        ਗਿਣਤੀ alpnid origin_alpnid,
                        ਬਣਤਰ altsvc **dstentry,
                        ਅਚਲ ਪੂਰਨ versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  ਜੇ(Curl_llist_ਗਿਣਤੀ(&asi->list)) {
    ਬਣਤਰ Curl_llist_ਗੰਢ *e;
    ਬਣਤਰ Curl_llist_ਗੰਢ *n;
    time_t now = ਸਮਾਂ(ਨਿਰਾ);

    ਲਈ(e = Curl_llist_ਸਿਰ(&asi->list); e; e = n) {
      ਬਣਤਰ altsvc *as = Curl_ਗੰਢ_elem(e);
      n = Curl_ਗੰਢ_ਅਗਲਾ(e);
      ਜੇ(as->expires < now) {
        /* an expired entry, remove */
        Curl_ਗੰਢ_remove(e);
        altsvc_free(as);
        ਜਾਰੀ;
      }
      ਜੇ((origin_alpnid == as->src.alpnid) &&
         (versions & (ਪੂਰਨ)as->dst.alpnid) &&
         (origin->ਪੋਰਟ == as->src.ਪੋਰਟ) &&
         hostcompare(origin->hostname, as->src.host)) {
        /*ਮੇਲ*/
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->ਪੋਰਟ == as->dst.ਪੋਰਟ) &&
                             hostcompare(origin->hostname, as->dst.host);
        ਵਾਪਸ TRUE;
      }
    }
  }
  ਵਾਪਸ FALSE;
}

#ਜੇ defined(DEBUGBUILD) || defined(UNITTESTS)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ ਸਮਾਂ
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
