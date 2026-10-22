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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "altsvc.h"
#ଅନ୍ତର୍ଭାଵ "curl_fopen.h"
#ଅନ୍ତର୍ଭାଵ "curl_get_line.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"

#define MAX_ALTSVC_ଧାଡ଼ି    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#ଯଦି defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ସ୍ଥିର time_t altsvc_debugtime(ଶୂନ୍ୟ *unused)
{
  ଅଚଳ ଅକ୍ଷର *timestr = getenv("CURL_TIME");
  (ଶୂନ୍ୟ)unused;
  ଯଦି(timestr) {
    curl_off_t val;
    curlx_str_ସଂଖ୍ୟା(&timestr, &val, TIME_T_MAX);
    ଫେରନ୍ତୁ (time_t)val;
  }
  ଫେରନ୍ତୁ time(ଖାଲି);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
ଅଚଳ ଅକ୍ଷର *Curl_alpnid2str(ଗଣନା alpnid id)
{
  ବାଛ(id) {
  ସ୍ଥିତି ALPN_h1:
    ଫେରନ୍ତୁ "h1";
  ସ୍ଥିତି ALPN_h2:
    ଫେରନ୍ତୁ "h2";
  ସ୍ଥିତି ALPN_h3:
    ଫେରନ୍ତୁ H3VERSION;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ ""; /* bad */
  }
}

ସ୍ଥିର ଗଣନା alpnid Curl_str2alpnid(ଅଚଳ ସଂରଚନା Curl_str *cstr)
{
  ଫେରନ୍ତୁ Curl_alpn2alpnid((ଅଚଳ unsigned ଅକ୍ଷର *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

ସ୍ଥିର ସଂରଚନା altsvc *altsvc_createid(ଅଚଳ ଅକ୍ଷର *srchost,
                                      size_t hlen,
                                      ଅଚଳ ଅକ୍ଷର *dsthost,
                                      size_t dlen, /* dsthost length */
                                      ଗଣନା alpnid srcalpnid,
                                      ଗଣନା alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ସଂରଚନା altsvc *as;
  ଯଦି((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  ଅନ୍ୟଥା ଯଦି(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  ଯଦି((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  ଯଦି(!hlen || !dlen)
    /* bad input */
    ଫେରନ୍ତୁ ଖାଲି;
  /* struct size plus both strings */
  as = curlx_calloc(1, ଆକାର(ସଂରଚନା altsvc) + (hlen + 1) + (dlen + 1));
  ଯଦି(!as)
    ଫେରନ୍ତୁ ଖାଲି;
  as->src.host = (ଅକ୍ଷର *)as + ଆକାର(ସଂରଚନା altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (ଅକ୍ଷର *)as + ଆକାର(ସଂରଚନା altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  ଫେରନ୍ତୁ as;
}

ସ୍ଥିର ସଂରଚନା altsvc *altsvc_create(ସଂରଚନା Curl_str *srchost,
                                    ସଂରଚନା Curl_str *dsthost,
                                    ସଂରଚନା Curl_str *srcalpn,
                                    ସଂରଚନା Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  ଗଣନା alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  ଗଣନା alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  ଯଦି(!srcalpnid || !dstalpnid)
    ଫେରନ୍ତୁ ଖାଲି;
  ଫେରନ୍ତୁ altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
ସ୍ଥିର ଶୂନ୍ୟ altsvc_append(ସଂରଚନା altsvcinfo *asi, ସଂରଚନା altsvc *as)
{
  ଯେତେବେଳେ(Curl_llist_ଗଣନା(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e = Curl_llist_head(&asi->list);
    ସଂରଚନା altsvc *oldas = Curl_ଗଣ୍ଠି_elem(e);
    Curl_ଗଣ୍ଠି_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ଗଣ୍ଠି);
}

/* only returns SERIOUS errors */
ସ୍ଥିର CURLcode altsvc_add(ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ସଂରଚନା Curl_str srchost;
  ସଂରଚନା Curl_str dsthost;
  ସଂରଚନା Curl_str srcalpn;
  ସଂରଚନା Curl_str dstalpn;
  ସଂରଚନା Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  ଯଦି(curlx_str_word(&ଧାଡ଼ି, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_word(&ଧାଡ଼ି, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_ସଂଖ୍ୟା(&ଧାଡ଼ି, &srcport, 65535) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_word(&ଧାଡ଼ି, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_word(&ଧାଡ଼ି, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_ସଂଖ୍ୟା(&ଧାଡ଼ି, &dstport, 65535) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_quotedword(&ଧାଡ଼ି, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_ସଂଖ୍ୟା(&ଧାଡ଼ି, &persist, 1) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_ସଂଖ୍ୟା(&ଧାଡ଼ି, &prio, 0) ||
     curlx_str_newline(&ଧାଡ଼ି))
    ;
  ଅନ୍ୟଥା {
    ଅକ୍ଷର dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(ଖାଲି);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    ଯଦି(now < expires) {
      ସଂରଚନା altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      ଯଦି(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ସ୍ଥିର CURLcode altsvc_load(ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  ଯଦି(!asi->filename)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ଯଦି(fp) {
    curlx_struct_stat stat;
    ଯଦି((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      ସଂରଚନା dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_ଧାଡ଼ି);
      କର {
        result = Curl_get_ଧାଡ଼ି(&buf, fp, &eof);
        ଯଦି(!result) {
          ଅଚଳ ଅକ୍ଷର *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          ଯଦି(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } ଯେତେବେଳେ(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Write this single altsvc entry to a single output line
 */
ସ୍ଥିର CURLcode altsvc_out(ସଂରଚନା altsvc *as, FILE *fp)
{
  ସଂରଚନା tm stamp;
  ଅଚଳ ଅକ୍ଷର *dst6_pre = "";
  ଅଚଳ ଅକ୍ଷର *dst6_post = "";
  ଅଚଳ ଅକ୍ଷର *src6_pre = "";
  ଅଚଳ ଅକ୍ଷର *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  ଯଦି(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    ଫେରନ୍ତୁ CURLE_OK;
#ifdef USE_IPV6
  ଅନ୍ୟଥା {
    ଅକ୍ଷର ipv6_unused[16];
    ଯଦି(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    ଯଦି(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  ଫେରନ୍ତୁ CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ସଂରଚନା altsvcinfo *Curl_altsvc_init(ଶୂନ୍ୟ)
{
  ସଂରଚନା altsvcinfo *asi = curlx_calloc(1, ଆକାର(ସଂରଚନା altsvcinfo));
  ଯଦି(!asi)
    ଫେରନ୍ତୁ ଖାଲି;
  Curl_llist_init(&asi->list, ଖାଲି);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  ଫେରନ୍ତୁ asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *file)
{
  DEBUGASSERT(asi);
  ଫେରନ୍ତୁ altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ସଂରଚନା Curl_easy *data, ଅଚଳ long ctrl)
{
  DEBUGASSERT(data);
  ଯଦି(!ctrl)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(!data->asi) {
    data->asi = Curl_altsvc_init();
    ଯଦି(!data->asi)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
ଶୂନ୍ୟ Curl_altsvc_cleanup(ସଂରଚନା altsvcinfo **asi)
{
  ଯଦି(*asi) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
    ସଂରଚନା altsvcinfo *altsvc = *asi;
    ପାଇଁ(e = Curl_llist_head(&altsvc->list); e; e = n) {
      ସଂରଚନା altsvc *as = Curl_ଗଣ୍ଠି_elem(e);
      n = Curl_ଗଣ୍ଠି_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ଖାଲି; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  ଅକ୍ଷର *tempstore = ଖାଲି;

  ଯଦି(!asi)
    /* no cache activated */
    ଫେରନ୍ତୁ CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  ଯଦି(!file && asi->filename)
    file = asi->filename;

  ଯଦି((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ଫେରନ୍ତୁ CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  ଯଦି(!result) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    ପାଇଁ(e = Curl_llist_head(&asi->list); e; e = n) {
      ସଂରଚନା altsvc *as = Curl_ଗଣ୍ଠି_elem(e);
      n = Curl_ଗଣ୍ଠି_next(e);
      result = altsvc_out(as, out);
      ଯଦି(result)
        ଭାଙ୍ଗ;
    }
    curlx_fclose(out);
    ଯଦି(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    ଯଦି(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  ଫେରନ୍ତୁ result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ସ୍ଥିର bool hostcompare(ଅଚଳ ଅକ୍ଷର *host, ଅଚଳ ଅକ୍ଷର *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  ଯଦି(hlen && (host[hlen - 1] == '.'))
    hlen--;
  ଯଦି(hlen != clen)
    /* they cannot match if they have different lengths */
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ସ୍ଥିର ଶୂନ୍ୟ altsvc_flush(ସଂରଚନା altsvcinfo *asi,
                         ସଂରଚନା Curl_peer *origin,
                         ଗଣନା alpnid origin_alpnid)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
  ପାଇଁ(e = Curl_llist_head(&asi->list); e; e = n) {
    ସଂରଚନା altsvc *as = Curl_ଗଣ୍ଠି_elem(e);
    n = Curl_ଗଣ୍ଠି_next(e);
    ଯଦି((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ଗଣ୍ଠି_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
ସ୍ଥିର ଶୂନ୍ୟ altsvc_parse_params(ଅଚଳ ଅକ୍ଷର **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  ଯଦି(curlx_str_single(pp, ';'))
    ଫେରନ୍ତୁ;

  ପାଇଁ(;;) {
    ସଂରଚନା Curl_str name;
    ସଂରଚନା Curl_str val;
    curl_off_t ଅଙ୍କ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyword = 0;

    /* allow some extra whitespaces around name and value */
    ଯଦି(curlx_str_until(pp, &name, MAX_ALTSVC_ଧାଡ଼ି, '=') ||
       curlx_str_single(pp, '='))
      ଭାଙ୍ଗ;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    ଯଦି(**pp == '\"') {
      ଯଦି(curlx_str_quotedword(pp, &val, MAX_ALTSVC_ଧାଡ଼ି))
        ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା {
      ଯଦି(curlx_str_cspn(pp, &val, ",;\r\n"))
        ଭାଙ୍ଗ;
    }
    curlx_str_trimblanks(&val);

    ଯଦି(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    ଯଦି(keyword) {
      ଅଚଳ ଅକ୍ଷର *vp = curlx_str(&val);
      ଅଚଳ ଅକ୍ଷର *vend = vp + curlx_strlen(&val);
      ଯଦି(curlx_str_ସଂଖ୍ୟା(&vp, &ଅଙ୍କ, CURL_OFF_T_MAX))
        ଭାଙ୍ଗ; /* not a number, skip further parameter parsing */
      ଯଦି(vp != vend)
        ଭାଙ୍ଗ; /* not entirely a number, skip further parameter parsing */
      ଯଦି(keyword == ALTSVC_MA) {
        ଯଦି(ଅଙ୍କ > CAP_ALTSVC_MAX_AGE)
          ଅଙ୍କ = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)ଅଙ୍କ;
      }
      ଅନ୍ୟଥା ଯଦି(ଅଙ୍କ == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    ଯଦି(curlx_str_single(pp, ';'))
      ଭାଙ୍ଗ; /* no further parameters */
  }
}

ସ୍ଥିର bool altsvc_parse_dest(ଅଚଳ ଅକ୍ଷର **pp,
                              ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା Curl_peer *origin,
                              ସଂରଚନା Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  ଯଦି(curlx_str_single(pp, '\"'))
    ଫେରନ୍ତୁ FALSE;

  /* quoted string, with hostname or just :port ? */
  ଯଦି(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    ଯଦି(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      ଯଦି(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        ଫେରନ୍ତୁ FALSE;
      }
    }
    ଅନ୍ୟଥା { /* IPv6 hostname */
      ଯଦି(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        ଫେରନ୍ତୁ FALSE;
      }
    }
    ଯଦି(curlx_str_single(pp, ':'))
      ଫେରନ୍ତୁ FALSE; /* not followed by ':' */
  }
  ଅନ୍ୟଥା  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  ଯଦି(curlx_str_ସଂଖ୍ୟା(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    ଫେରନ୍ତୁ FALSE;
  }

  *pdstport = (uint16_t)port;
  ଯଦି(curlx_str_single(pp, '\"'))
    ଫେରନ୍ତୁ FALSE; /* quoted string not ending here as expected */
  ଫେରନ୍ତୁ TRUE;
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
CURLcode Curl_altsvc_parse(ସଂରଚନା Curl_easy *data,
                           ସଂରଚନା altsvcinfo *asi, ଅଚଳ ଅକ୍ଷର *ମୂଲ୍ୟ,
                           ସଂରଚନା Curl_peer *origin,
                           ଗଣନା alpnid origin_alpnid)
{
  ସଂରଚନା altsvc *as;
  size_t entries = 0;
  ସଂରଚନା Curl_str alpn;
  ଅଚଳ ଅକ୍ଷର *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = ମୂଲ୍ୟ;
  ଯଦି(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    ଯଦି(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      ଫେରନ୍ତୁ CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  ପାଇଁ(p = ମୂଲ୍ୟ; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    ଗଣନା alpnid dstalpnid;
    ସଂରଚନା Curl_str dsthost;
    uint16_t dstport;

    ଯଦି(curlx_str_until(&p, &alpn, MAX_ALTSVC_ଧାଡ଼ି, '='))
      ଭାଙ୍ଗ; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    ଯଦି(curlx_str_single(&p, '='))
      ଭାଙ୍ଗ;

    /* Parse altsvc hostname:port */
    ଯଦି(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      ଭାଙ୍ଗ;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    ଯଦି(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      ଯଦି(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      ଯଦି(as) {
        time_t secs = time(ଖାଲି);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        ଯଦି(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        ଅନ୍ୟଥା
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      ଅନ୍ୟଥା
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    ଯଦି(curlx_str_single(&p, ','))
      ଭାଙ୍ଗ;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ସଂରଚନା altsvcinfo *asi,
                        ସଂରଚନା Curl_peer *origin,
                        ଗଣନା alpnid origin_alpnid,
                        ସଂରଚନା altsvc **dstentry,
                        ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  ଯଦି(Curl_llist_ଗଣନା(&asi->list)) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
    time_t now = time(ଖାଲି);

    ପାଇଁ(e = Curl_llist_head(&asi->list); e; e = n) {
      ସଂରଚନା altsvc *as = Curl_ଗଣ୍ଠି_elem(e);
      n = Curl_ଗଣ୍ଠି_next(e);
      ଯଦି(as->expires < now) {
        /* an expired entry, remove */
        Curl_ଗଣ୍ଠି_remove(e);
        altsvc_free(as);
        ଜାରି;
      }
      ଯଦି((origin_alpnid == as->src.alpnid) &&
         (versions & (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        ଫେରନ୍ତୁ TRUE;
      }
    }
  }
  ଫେରନ୍ତୁ FALSE;
}

#ଯଦି defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
