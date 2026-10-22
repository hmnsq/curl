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
#அந்தர்பாவ "curl_setup.h"

#எனில் !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "altsvc.h"
#அந்தர்பாவ "curl_fopen.h"
#அந்தர்பாவ "curl_get_line.h"
#அந்தர்பாவ "parsedate.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "connect.h"

#define MAX_ALTSVC_வரி    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#எனில் defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
நிலையான time_t altsvc_debugtime(வெறுமை *unused)
{
  மாறிலி எழுத்து *timestr = getenv("CURL_TIME");
  (வெறுமை)unused;
  எனில்(timestr) {
    curl_off_t val;
    curlx_str_எண்(&timestr, &val, TIME_T_MAX);
    திரும்பு (time_t)val;
  }
  திரும்பு time(வெற்று);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
மாறிலி எழுத்து *Curl_alpnid2str(எண்ணிக்கை alpnid id)
{
  தேர்வு(id) {
  நிலை ALPN_h1:
    திரும்பு "h1";
  நிலை ALPN_h2:
    திரும்பு "h2";
  நிலை ALPN_h3:
    திரும்பு H3VERSION;
  இயல்பு:
    திரும்பு ""; /* bad */
  }
}

நிலையான எண்ணிக்கை alpnid Curl_str2alpnid(மாறிலி கட்டமைப்பு Curl_str *cstr)
{
  திரும்பு Curl_alpn2alpnid((மாறிலி unsigned எழுத்து *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

நிலையான கட்டமைப்பு altsvc *altsvc_createid(மாறிலி எழுத்து *srchost,
                                      size_t hlen,
                                      மாறிலி எழுத்து *dsthost,
                                      size_t dlen, /* dsthost length */
                                      எண்ணிக்கை alpnid srcalpnid,
                                      எண்ணிக்கை alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  கட்டமைப்பு altsvc *as;
  எனில்((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  இல்லையெனில் எனில்(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  எனில்((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  எனில்(!hlen || !dlen)
    /* bad input */
    திரும்பு வெற்று;
  /* struct size plus both strings */
  as = curlx_calloc(1, அளவு(கட்டமைப்பு altsvc) + (hlen + 1) + (dlen + 1));
  எனில்(!as)
    திரும்பு வெற்று;
  as->src.host = (எழுத்து *)as + அளவு(கட்டமைப்பு altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (எழுத்து *)as + அளவு(கட்டமைப்பு altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  திரும்பு as;
}

நிலையான கட்டமைப்பு altsvc *altsvc_create(கட்டமைப்பு Curl_str *srchost,
                                    கட்டமைப்பு Curl_str *dsthost,
                                    கட்டமைப்பு Curl_str *srcalpn,
                                    கட்டமைப்பு Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  எண்ணிக்கை alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  எண்ணிக்கை alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  எனில்(!srcalpnid || !dstalpnid)
    திரும்பு வெற்று;
  திரும்பு altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
நிலையான வெறுமை altsvc_append(கட்டமைப்பு altsvcinfo *asi, கட்டமைப்பு altsvc *as)
{
  போது(Curl_llist_மொத்தம்(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    கட்டமைப்பு Curl_llist_முனை *e = Curl_llist_head(&asi->list);
    கட்டமைப்பு altsvc *oldas = Curl_முனை_elem(e);
    Curl_முனை_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->முனை);
}

/* only returns SERIOUS errors */
நிலையான CURLcode altsvc_add(கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *வரி)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  கட்டமைப்பு Curl_str srchost;
  கட்டமைப்பு Curl_str dsthost;
  கட்டமைப்பு Curl_str srcalpn;
  கட்டமைப்பு Curl_str dstalpn;
  கட்டமைப்பு Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  எனில்(curlx_str_word(&வரி, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_word(&வரி, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_எண்(&வரி, &srcport, 65535) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_word(&வரி, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_word(&வரி, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_எண்(&வரி, &dstport, 65535) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_quotedword(&வரி, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_எண்(&வரி, &persist, 1) ||
     curlx_str_singlespace(&வரி) ||
     curlx_str_எண்(&வரி, &prio, 0) ||
     curlx_str_newline(&வரி))
    ;
  இல்லையெனில் {
    எழுத்து dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(வெற்று);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    எனில்(now < expires) {
      கட்டமைப்பு altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      எனில்(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      இல்லையெனில்
        திரும்பு CURLE_OUT_OF_MEMORY;
    }
  }

  திரும்பு CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
நிலையான CURLcode altsvc_load(கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  எனில்(!asi->filename)
    திரும்பு CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  எனில்(fp) {
    curlx_struct_stat stat;
    எனில்((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      கட்டமைப்பு dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_வரி);
      செய் {
        result = Curl_get_வரி(&buf, fp, &eof);
        எனில்(!result) {
          மாறிலி எழுத்து *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          எனில்(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } போது(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  திரும்பு result;
}

/*
 * Write this single altsvc entry to a single output line
 */
நிலையான CURLcode altsvc_out(கட்டமைப்பு altsvc *as, FILE *fp)
{
  கட்டமைப்பு tm stamp;
  மாறிலி எழுத்து *dst6_pre = "";
  மாறிலி எழுத்து *dst6_post = "";
  மாறிலி எழுத்து *src6_pre = "";
  மாறிலி எழுத்து *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  எனில்(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    திரும்பு CURLE_OK;
#ifdef USE_IPV6
  இல்லையெனில் {
    எழுத்து ipv6_unused[16];
    எனில்(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    எனில்(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  திரும்பு CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
கட்டமைப்பு altsvcinfo *Curl_altsvc_init(வெறுமை)
{
  கட்டமைப்பு altsvcinfo *asi = curlx_calloc(1, அளவு(கட்டமைப்பு altsvcinfo));
  எனில்(!asi)
    திரும்பு வெற்று;
  Curl_llist_init(&asi->list, வெற்று);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  திரும்பு asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *file)
{
  DEBUGASSERT(asi);
  திரும்பு altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(கட்டமைப்பு Curl_easy *data, மாறிலி long ctrl)
{
  DEBUGASSERT(data);
  எனில்(!ctrl)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(!data->asi) {
    data->asi = Curl_altsvc_init();
    எனில்(!data->asi)
      திரும்பு CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  திரும்பு CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
வெறுமை Curl_altsvc_cleanup(கட்டமைப்பு altsvcinfo **asi)
{
  எனில்(*asi) {
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_llist_முனை *n;
    கட்டமைப்பு altsvcinfo *altsvc = *asi;
    க்கு(e = Curl_llist_head(&altsvc->list); e; e = n) {
      கட்டமைப்பு altsvc *as = Curl_முனை_elem(e);
      n = Curl_முனை_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = வெற்று; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(கட்டமைப்பு Curl_easy *data,
                          கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  எழுத்து *tempstore = வெற்று;

  எனில்(!asi)
    /* no cache activated */
    திரும்பு CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  எனில்(!file && asi->filename)
    file = asi->filename;

  எனில்((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    திரும்பு CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  எனில்(!result) {
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_llist_முனை *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    க்கு(e = Curl_llist_head(&asi->list); e; e = n) {
      கட்டமைப்பு altsvc *as = Curl_முனை_elem(e);
      n = Curl_முனை_next(e);
      result = altsvc_out(as, out);
      எனில்(result)
        நிறுத்து;
    }
    curlx_fclose(out);
    எனில்(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    எனில்(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  திரும்பு result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
நிலையான bool hostcompare(மாறிலி எழுத்து *host, மாறிலி எழுத்து *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  எனில்(hlen && (host[hlen - 1] == '.'))
    hlen--;
  எனில்(hlen != clen)
    /* they cannot match if they have different lengths */
    திரும்பு FALSE;
  திரும்பு curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
நிலையான வெறுமை altsvc_flush(கட்டமைப்பு altsvcinfo *asi,
                         கட்டமைப்பு Curl_peer *origin,
                         எண்ணிக்கை alpnid origin_alpnid)
{
  கட்டமைப்பு Curl_llist_முனை *e;
  கட்டமைப்பு Curl_llist_முனை *n;
  க்கு(e = Curl_llist_head(&asi->list); e; e = n) {
    கட்டமைப்பு altsvc *as = Curl_முனை_elem(e);
    n = Curl_முனை_next(e);
    எனில்((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_முனை_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
நிலையான வெறுமை altsvc_parse_params(மாறிலி எழுத்து **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  எனில்(curlx_str_single(pp, ';'))
    திரும்பு;

  க்கு(;;) {
    கட்டமைப்பு Curl_str name;
    கட்டமைப்பு Curl_str val;
    curl_off_t இலக்கம்;
    முழு keyword = 0;

    /* allow some extra whitespaces around name and value */
    எனில்(curlx_str_until(pp, &name, MAX_ALTSVC_வரி, '=') ||
       curlx_str_single(pp, '='))
      நிறுத்து;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    எனில்(**pp == '\"') {
      எனில்(curlx_str_quotedword(pp, &val, MAX_ALTSVC_வரி))
        நிறுத்து;
    }
    இல்லையெனில் {
      எனில்(curlx_str_cspn(pp, &val, ",;\r\n"))
        நிறுத்து;
    }
    curlx_str_trimblanks(&val);

    எனில்(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    இல்லையெனில் எனில்(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    எனில்(keyword) {
      மாறிலி எழுத்து *vp = curlx_str(&val);
      மாறிலி எழுத்து *vend = vp + curlx_strlen(&val);
      எனில்(curlx_str_எண்(&vp, &இலக்கம், CURL_OFF_T_MAX))
        நிறுத்து; /* not a number, skip further parameter parsing */
      எனில்(vp != vend)
        நிறுத்து; /* not entirely a number, skip further parameter parsing */
      எனில்(keyword == ALTSVC_MA) {
        எனில்(இலக்கம் > CAP_ALTSVC_MAX_AGE)
          இலக்கம் = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)இலக்கம்;
      }
      இல்லையெனில் எனில்(இலக்கம் == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    எனில்(curlx_str_single(pp, ';'))
      நிறுத்து; /* no further parameters */
  }
}

நிலையான bool altsvc_parse_dest(மாறிலி எழுத்து **pp,
                              கட்டமைப்பு Curl_easy *data,
                              கட்டமைப்பு Curl_peer *origin,
                              கட்டமைப்பு Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  எனில்(curlx_str_single(pp, '\"'))
    திரும்பு FALSE;

  /* quoted string, with hostname or just :port ? */
  எனில்(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    எனில்(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      எனில்(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        திரும்பு FALSE;
      }
    }
    இல்லையெனில் { /* IPv6 hostname */
      எனில்(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        திரும்பு FALSE;
      }
    }
    எனில்(curlx_str_single(pp, ':'))
      திரும்பு FALSE; /* not followed by ':' */
  }
  இல்லையெனில்  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  எனில்(curlx_str_எண்(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    திரும்பு FALSE;
  }

  *pdstport = (uint16_t)port;
  எனில்(curlx_str_single(pp, '\"'))
    திரும்பு FALSE; /* quoted string not ending here as expected */
  திரும்பு TRUE;
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
CURLcode Curl_altsvc_parse(கட்டமைப்பு Curl_easy *data,
                           கட்டமைப்பு altsvcinfo *asi, மாறிலி எழுத்து *மதிப்பு,
                           கட்டமைப்பு Curl_peer *origin,
                           எண்ணிக்கை alpnid origin_alpnid)
{
  கட்டமைப்பு altsvc *as;
  size_t entries = 0;
  கட்டமைப்பு Curl_str alpn;
  மாறிலி எழுத்து *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = மதிப்பு;
  எனில்(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    எனில்(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      திரும்பு CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  க்கு(p = மதிப்பு; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    எண்ணிக்கை alpnid dstalpnid;
    கட்டமைப்பு Curl_str dsthost;
    uint16_t dstport;

    எனில்(curlx_str_until(&p, &alpn, MAX_ALTSVC_வரி, '='))
      நிறுத்து; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    எனில்(curlx_str_single(&p, '='))
      நிறுத்து;

    /* Parse altsvc hostname:port */
    எனில்(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      நிறுத்து;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    எனில்(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      எனில்(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      எனில்(as) {
        time_t secs = time(வெற்று);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        எனில்(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        இல்லையெனில்
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (முழு)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      இல்லையெனில்
        திரும்பு CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    எனில்(curlx_str_single(&p, ','))
      நிறுத்து;
  }

  திரும்பு CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(கட்டமைப்பு altsvcinfo *asi,
                        கட்டமைப்பு Curl_peer *origin,
                        எண்ணிக்கை alpnid origin_alpnid,
                        கட்டமைப்பு altsvc **dstentry,
                        மாறிலி முழு versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  எனில்(Curl_llist_மொத்தம்(&asi->list)) {
    கட்டமைப்பு Curl_llist_முனை *e;
    கட்டமைப்பு Curl_llist_முனை *n;
    time_t now = time(வெற்று);

    க்கு(e = Curl_llist_head(&asi->list); e; e = n) {
      கட்டமைப்பு altsvc *as = Curl_முனை_elem(e);
      n = Curl_முனை_next(e);
      எனில்(as->expires < now) {
        /* an expired entry, remove */
        Curl_முனை_remove(e);
        altsvc_free(as);
        தொடர்;
      }
      எனில்((origin_alpnid == as->src.alpnid) &&
         (versions & (முழு)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        திரும்பு TRUE;
      }
    }
  }
  திரும்பு FALSE;
}

#எனில் defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
