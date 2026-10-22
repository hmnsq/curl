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
#অন্তর্ভাব "curl_setup.h"

#যদি !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "altsvc.h"
#অন্তর্ভাব "curl_fopen.h"
#অন্তর্ভাব "curl_get_line.h"
#অন্তর্ভাব "parsedate.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "curlx/inet_pton.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "connect.h"

#define MAX_ALTSVC_সারি    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
স্থির time_t altsvc_debugtime(শূন্য *unused)
{
  ধ্রুব বর্ণ *timestr = getenv("CURL_TIME");
  (শূন্য)unused;
  যদি(timestr) {
    curl_off_t val;
    curlx_str_সংখ্যা(&timestr, &val, TIME_T_MAX);
    প্রত্যাগম (time_t)val;
  }
  প্রত্যাগম time(রিক্ত);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
ধ্রুব বর্ণ *Curl_alpnid2str(গণনা alpnid id)
{
  বিকল্প(id) {
  পক্ষ ALPN_h1:
    প্রত্যাগম "h1";
  পক্ষ ALPN_h2:
    প্রত্যাগম "h2";
  পক্ষ ALPN_h3:
    প্রত্যাগম H3VERSION;
  স্বভাব:
    প্রত্যাগম ""; /* bad */
  }
}

স্থির গণনা alpnid Curl_str2alpnid(ধ্রুব সংরচনা Curl_str *cstr)
{
  প্রত্যাগম Curl_alpn2alpnid((ধ্রুব unsigned বর্ণ *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

স্থির সংরচনা altsvc *altsvc_createid(ধ্রুব বর্ণ *srchost,
                                      size_t hlen,
                                      ধ্রুব বর্ণ *dsthost,
                                      size_t dlen, /* dsthost length */
                                      গণনা alpnid srcalpnid,
                                      গণনা alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  সংরচনা altsvc *as;
  যদি((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  অন্যথা যদি(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  যদি((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  যদি(!hlen || !dlen)
    /* bad input */
    প্রত্যাগম রিক্ত;
  /* struct size plus both strings */
  as = curlx_calloc(1, পরিমাণ(সংরচনা altsvc) + (hlen + 1) + (dlen + 1));
  যদি(!as)
    প্রত্যাগম রিক্ত;
  as->src.host = (বর্ণ *)as + পরিমাণ(সংরচনা altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (বর্ণ *)as + পরিমাণ(সংরচনা altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  প্রত্যাগম as;
}

স্থির সংরচনা altsvc *altsvc_create(সংরচনা Curl_str *srchost,
                                    সংরচনা Curl_str *dsthost,
                                    সংরচনা Curl_str *srcalpn,
                                    সংরচনা Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  গণনা alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  গণনা alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  যদি(!srcalpnid || !dstalpnid)
    প্রত্যাগম রিক্ত;
  প্রত্যাগম altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
স্থির শূন্য altsvc_append(সংরচনা altsvcinfo *asi, সংরচনা altsvc *as)
{
  যাবত্(Curl_llist_গুনতি(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    সংরচনা Curl_llist_গ্রন্থি *e = Curl_llist_head(&asi->list);
    সংরচনা altsvc *oldas = Curl_গ্রন্থি_elem(e);
    Curl_গ্রন্থি_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->গ্রন্থি);
}

/* only returns SERIOUS errors */
স্থির CURLcode altsvc_add(সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *সারি)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  সংরচনা Curl_str srchost;
  সংরচনা Curl_str dsthost;
  সংরচনা Curl_str srcalpn;
  সংরচনা Curl_str dstalpn;
  সংরচনা Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  যদি(curlx_str_word(&সারি, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_word(&সারি, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_সংখ্যা(&সারি, &srcport, 65535) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_word(&সারি, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_word(&সারি, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_সংখ্যা(&সারি, &dstport, 65535) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_quotedword(&সারি, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_সংখ্যা(&সারি, &persist, 1) ||
     curlx_str_singlespace(&সারি) ||
     curlx_str_সংখ্যা(&সারি, &prio, 0) ||
     curlx_str_newline(&সারি))
    ;
  অন্যথা {
    বর্ণ dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(রিক্ত);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    যদি(now < expires) {
      সংরচনা altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      যদি(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      অন্যথা
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }
  }

  প্রত্যাগম CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
স্থির CURLcode altsvc_load(সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  যদি(!asi->filename)
    প্রত্যাগম CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  যদি(fp) {
    curlx_struct_stat stat;
    যদি((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      সংরচনা dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_সারি);
      কুরু {
        result = Curl_get_সারি(&buf, fp, &eof);
        যদি(!result) {
          ধ্রুব বর্ণ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          যদি(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } যাবত্(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  প্রত্যাগম result;
}

/*
 * Write this single altsvc entry to a single output line
 */
স্থির CURLcode altsvc_out(সংরচনা altsvc *as, FILE *fp)
{
  সংরচনা tm stamp;
  ধ্রুব বর্ণ *dst6_pre = "";
  ধ্রুব বর্ণ *dst6_post = "";
  ধ্রুব বর্ণ *src6_pre = "";
  ধ্রুব বর্ণ *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  যদি(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    প্রত্যাগম CURLE_OK;
#ifdef USE_IPV6
  অন্যথা {
    বর্ণ ipv6_unused[16];
    যদি(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    যদি(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#endif
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.port,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.port,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  প্রত্যাগম CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
সংরচনা altsvcinfo *Curl_altsvc_init(শূন্য)
{
  সংরচনা altsvcinfo *asi = curlx_calloc(1, পরিমাণ(সংরচনা altsvcinfo));
  যদি(!asi)
    প্রত্যাগম রিক্ত;
  Curl_llist_init(&asi->list, রিক্ত);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  প্রত্যাগম asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *file)
{
  DEBUGASSERT(asi);
  প্রত্যাগম altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(সংরচনা Curl_easy *data, ধ্রুব long ctrl)
{
  DEBUGASSERT(data);
  যদি(!ctrl)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!data->asi) {
    data->asi = Curl_altsvc_init();
    যদি(!data->asi)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  প্রত্যাগম CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
শূন্য Curl_altsvc_cleanup(সংরচনা altsvcinfo **asi)
{
  যদি(*asi) {
    সংরচনা Curl_llist_গ্রন্থি *e;
    সংরচনা Curl_llist_গ্রন্থি *n;
    সংরচনা altsvcinfo *altsvc = *asi;
    প্রতি(e = Curl_llist_head(&altsvc->list); e; e = n) {
      সংরচনা altsvc *as = Curl_গ্রন্থি_elem(e);
      n = Curl_গ্রন্থি_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = রিক্ত; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(সংরচনা Curl_easy *data,
                          সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  বর্ণ *tempstore = রিক্ত;

  যদি(!asi)
    /* no cache activated */
    প্রত্যাগম CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  যদি(!file && asi->filename)
    file = asi->filename;

  যদি((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    প্রত্যাগম CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  যদি(!result) {
    সংরচনা Curl_llist_গ্রন্থি *e;
    সংরচনা Curl_llist_গ্রন্থি *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    প্রতি(e = Curl_llist_head(&asi->list); e; e = n) {
      সংরচনা altsvc *as = Curl_গ্রন্থি_elem(e);
      n = Curl_গ্রন্থি_next(e);
      result = altsvc_out(as, out);
      যদি(result)
        ভঙ্গ;
    }
    curlx_fclose(out);
    যদি(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    যদি(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  প্রত্যাগম result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
স্থির bool hostcompare(ধ্রুব বর্ণ *host, ধ্রুব বর্ণ *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  যদি(hlen && (host[hlen - 1] == '.'))
    hlen--;
  যদি(hlen != clen)
    /* they cannot match if they have different lengths */
    প্রত্যাগম FALSE;
  প্রত্যাগম curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
স্থির শূন্য altsvc_flush(সংরচনা altsvcinfo *asi,
                         সংরচনা Curl_peer *origin,
                         গণনা alpnid origin_alpnid)
{
  সংরচনা Curl_llist_গ্রন্থি *e;
  সংরচনা Curl_llist_গ্রন্থি *n;
  প্রতি(e = Curl_llist_head(&asi->list); e; e = n) {
    সংরচনা altsvc *as = Curl_গ্রন্থি_elem(e);
    n = Curl_গ্রন্থি_next(e);
    যদি((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_গ্রন্থি_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
স্থির শূন্য altsvc_parse_params(ধ্রুব বর্ণ **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  যদি(curlx_str_single(pp, ';'))
    প্রত্যাগম;

  প্রতি(;;) {
    সংরচনা Curl_str name;
    সংরচনা Curl_str val;
    curl_off_t অঙ্ক;
    পূর্ণাঙ্ক keyword = 0;

    /* allow some extra whitespaces around name and value */
    যদি(curlx_str_until(pp, &name, MAX_ALTSVC_সারি, '=') ||
       curlx_str_single(pp, '='))
      ভঙ্গ;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    যদি(**pp == '\"') {
      যদি(curlx_str_quotedword(pp, &val, MAX_ALTSVC_সারি))
        ভঙ্গ;
    }
    অন্যথা {
      যদি(curlx_str_cspn(pp, &val, ",;\r\n"))
        ভঙ্গ;
    }
    curlx_str_trimblanks(&val);

    যদি(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    অন্যথা যদি(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    যদি(keyword) {
      ধ্রুব বর্ণ *vp = curlx_str(&val);
      ধ্রুব বর্ণ *vend = vp + curlx_strlen(&val);
      যদি(curlx_str_সংখ্যা(&vp, &অঙ্ক, CURL_OFF_T_MAX))
        ভঙ্গ; /* not a number, skip further parameter parsing */
      যদি(vp != vend)
        ভঙ্গ; /* not entirely a number, skip further parameter parsing */
      যদি(keyword == ALTSVC_MA) {
        যদি(অঙ্ক > CAP_ALTSVC_MAX_AGE)
          অঙ্ক = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)অঙ্ক;
      }
      অন্যথা যদি(অঙ্ক == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    যদি(curlx_str_single(pp, ';'))
      ভঙ্গ; /* no further parameters */
  }
}

স্থির bool altsvc_parse_dest(ধ্রুব বর্ণ **pp,
                              সংরচনা Curl_easy *data,
                              সংরচনা Curl_peer *origin,
                              সংরচনা Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  যদি(curlx_str_single(pp, '\"'))
    প্রত্যাগম FALSE;

  /* quoted string, with hostname or just :port ? */
  যদি(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    যদি(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      যদি(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        প্রত্যাগম FALSE;
      }
    }
    অন্যথা { /* IPv6 hostname */
      যদি(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        প্রত্যাগম FALSE;
      }
    }
    যদি(curlx_str_single(pp, ':'))
      প্রত্যাগম FALSE; /* not followed by ':' */
  }
  অন্যথা  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  যদি(curlx_str_সংখ্যা(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    প্রত্যাগম FALSE;
  }

  *pdstport = (uint16_t)port;
  যদি(curlx_str_single(pp, '\"'))
    প্রত্যাগম FALSE; /* quoted string not ending here as expected */
  প্রত্যাগম TRUE;
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
CURLcode Curl_altsvc_parse(সংরচনা Curl_easy *data,
                           সংরচনা altsvcinfo *asi, ধ্রুব বর্ণ *মান,
                           সংরচনা Curl_peer *origin,
                           গণনা alpnid origin_alpnid)
{
  সংরচনা altsvc *as;
  size_t entries = 0;
  সংরচনা Curl_str alpn;
  ধ্রুব বর্ণ *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = মান;
  যদি(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    যদি(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      প্রত্যাগম CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  প্রতি(p = মান; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    গণনা alpnid dstalpnid;
    সংরচনা Curl_str dsthost;
    uint16_t dstport;

    যদি(curlx_str_until(&p, &alpn, MAX_ALTSVC_সারি, '='))
      ভঙ্গ; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    যদি(curlx_str_single(&p, '='))
      ভঙ্গ;

    /* Parse altsvc hostname:port */
    যদি(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      ভঙ্গ;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    যদি(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      যদি(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      যদি(as) {
        time_t secs = time(রিক্ত);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        যদি(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        অন্যথা
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (পূর্ণাঙ্ক)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      অন্যথা
        প্রত্যাগম CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    যদি(curlx_str_single(&p, ','))
      ভঙ্গ;
  }

  প্রত্যাগম CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(সংরচনা altsvcinfo *asi,
                        সংরচনা Curl_peer *origin,
                        গণনা alpnid origin_alpnid,
                        সংরচনা altsvc **dstentry,
                        ধ্রুব পূর্ণাঙ্ক versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  যদি(Curl_llist_গুনতি(&asi->list)) {
    সংরচনা Curl_llist_গ্রন্থি *e;
    সংরচনা Curl_llist_গ্রন্থি *n;
    time_t now = time(রিক্ত);

    প্রতি(e = Curl_llist_head(&asi->list); e; e = n) {
      সংরচনা altsvc *as = Curl_গ্রন্থি_elem(e);
      n = Curl_গ্রন্থি_next(e);
      যদি(as->expires < now) {
        /* an expired entry, remove */
        Curl_গ্রন্থি_remove(e);
        altsvc_free(as);
        অনুবর্ত;
      }
      যদি((origin_alpnid == as->src.alpnid) &&
         (versions & (পূর্ণাঙ্ক)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        প্রত্যাগম TRUE;
      }
    }
  }
  প্রত্যাগম FALSE;
}

#যদি defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
