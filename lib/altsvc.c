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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#شامل "urldata.h"
#شامل "altsvc.h"
#شامل "curl_fopen.h"
#شامل "curl_get_line.h"
#شامل "parsedate.h"
#شامل "curl_trc.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/strparse.h"
#شامل "connect.h"

#define MAX_ALTSVC_سطر    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ایستا time_t altsvc_debugtime(تهی *unused)
{
  ثابت نویسه *timestr = getenv("CURL_TIME");
  (تهی)unused;
  اگر(timestr) {
    curl_off_t val;
    curlx_str_عدد(&timestr, &val, TIME_T_MAX);
    بازگشت (time_t)val;
  }
  بازگشت time(پوچ);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
ثابت نویسه *Curl_alpnid2str(شمارش alpnid id)
{
  انتخاب(id) {
  حالت ALPN_h1:
    بازگشت "h1";
  حالت ALPN_h2:
    بازگشت "h2";
  حالت ALPN_h3:
    بازگشت H3VERSION;
  پیشفرض:
    بازگشت ""; /* bad */
  }
}

ایستا شمارش alpnid Curl_str2alpnid(ثابت ساختار Curl_str *cstr)
{
  بازگشت Curl_alpn2alpnid((ثابت unsigned نویسه *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

ایستا ساختار altsvc *altsvc_createid(ثابت نویسه *srchost,
                                      size_t hlen,
                                      ثابت نویسه *dsthost,
                                      size_t dlen, /* dsthost length */
                                      شمارش alpnid srcalpnid,
                                      شمارش alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ساختار altsvc *as;
  اگر((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  وگرنه اگر(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  اگر((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  اگر(!hlen || !dlen)
    /* bad input */
    بازگشت پوچ;
  /* struct size plus both strings */
  as = curlx_calloc(1, اندازه(ساختار altsvc) + (hlen + 1) + (dlen + 1));
  اگر(!as)
    بازگشت پوچ;
  as->src.host = (نویسه *)as + اندازه(ساختار altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (نویسه *)as + اندازه(ساختار altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  بازگشت as;
}

ایستا ساختار altsvc *altsvc_create(ساختار Curl_str *srchost,
                                    ساختار Curl_str *dsthost,
                                    ساختار Curl_str *srcalpn,
                                    ساختار Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  شمارش alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  شمارش alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  اگر(!srcalpnid || !dstalpnid)
    بازگشت پوچ;
  بازگشت altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
ایستا تهی altsvc_append(ساختار altsvcinfo *asi, ساختار altsvc *as)
{
  وقتی(Curl_llist_شمار(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ساختار Curl_llist_گره *e = Curl_llist_head(&asi->list);
    ساختار altsvc *oldas = Curl_گره_elem(e);
    Curl_گره_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->گره);
}

/* only returns SERIOUS errors */
ایستا CURLcode altsvc_add(ساختار altsvcinfo *asi, ثابت نویسه *سطر)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ساختار Curl_str srchost;
  ساختار Curl_str dsthost;
  ساختار Curl_str srcalpn;
  ساختار Curl_str dstalpn;
  ساختار Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  اگر(curlx_str_word(&سطر, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_word(&سطر, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_عدد(&سطر, &srcport, 65535) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_word(&سطر, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_word(&سطر, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_عدد(&سطر, &dstport, 65535) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_quotedword(&سطر, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_عدد(&سطر, &persist, 1) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_عدد(&سطر, &prio, 0) ||
     curlx_str_newline(&سطر))
    ;
  وگرنه {
    نویسه dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(پوچ);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    اگر(now < expires) {
      ساختار altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      اگر(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      وگرنه
        بازگشت CURLE_OUT_OF_MEMORY;
    }
  }

  بازگشت CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ایستا CURLcode altsvc_load(ساختار altsvcinfo *asi, ثابت نویسه *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  اگر(!asi->filename)
    بازگشت CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  اگر(fp) {
    curlx_struct_stat stat;
    اگر((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      ساختار dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_سطر);
      انجامبده {
        result = Curl_get_سطر(&buf, fp, &eof);
        اگر(!result) {
          ثابت نویسه *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          اگر(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } وقتی(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  بازگشت result;
}

/*
 * Write this single altsvc entry to a single output line
 */
ایستا CURLcode altsvc_out(ساختار altsvc *as, FILE *fp)
{
  ساختار tm stamp;
  ثابت نویسه *dst6_pre = "";
  ثابت نویسه *dst6_post = "";
  ثابت نویسه *src6_pre = "";
  ثابت نویسه *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  اگر(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    بازگشت CURLE_OK;
#ifdef USE_IPV6
  وگرنه {
    نویسه ipv6_unused[16];
    اگر(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    اگر(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  بازگشت CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ساختار altsvcinfo *Curl_altsvc_init(تهی)
{
  ساختار altsvcinfo *asi = curlx_calloc(1, اندازه(ساختار altsvcinfo));
  اگر(!asi)
    بازگشت پوچ;
  Curl_llist_init(&asi->list, پوچ);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  بازگشت asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(ساختار altsvcinfo *asi, ثابت نویسه *file)
{
  DEBUGASSERT(asi);
  بازگشت altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ساختار Curl_easy *data, ثابت long ctrl)
{
  DEBUGASSERT(data);
  اگر(!ctrl)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(!data->asi) {
    data->asi = Curl_altsvc_init();
    اگر(!data->asi)
      بازگشت CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  بازگشت CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
تهی Curl_altsvc_cleanup(ساختار altsvcinfo **asi)
{
  اگر(*asi) {
    ساختار Curl_llist_گره *e;
    ساختار Curl_llist_گره *n;
    ساختار altsvcinfo *altsvc = *asi;
    برای(e = Curl_llist_head(&altsvc->list); e; e = n) {
      ساختار altsvc *as = Curl_گره_elem(e);
      n = Curl_گره_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = پوچ; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ساختار Curl_easy *data,
                          ساختار altsvcinfo *asi, ثابت نویسه *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  نویسه *tempstore = پوچ;

  اگر(!asi)
    /* no cache activated */
    بازگشت CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  اگر(!file && asi->filename)
    file = asi->filename;

  اگر((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    بازگشت CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  اگر(!result) {
    ساختار Curl_llist_گره *e;
    ساختار Curl_llist_گره *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    برای(e = Curl_llist_head(&asi->list); e; e = n) {
      ساختار altsvc *as = Curl_گره_elem(e);
      n = Curl_گره_next(e);
      result = altsvc_out(as, out);
      اگر(result)
        بشکن;
    }
    curlx_fclose(out);
    اگر(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    اگر(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  بازگشت result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ایستا bool hostcompare(ثابت نویسه *host, ثابت نویسه *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  اگر(hlen && (host[hlen - 1] == '.'))
    hlen--;
  اگر(hlen != clen)
    /* they cannot match if they have different lengths */
    بازگشت FALSE;
  بازگشت curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ایستا تهی altsvc_flush(ساختار altsvcinfo *asi,
                         ساختار Curl_peer *origin,
                         شمارش alpnid origin_alpnid)
{
  ساختار Curl_llist_گره *e;
  ساختار Curl_llist_گره *n;
  برای(e = Curl_llist_head(&asi->list); e; e = n) {
    ساختار altsvc *as = Curl_گره_elem(e);
    n = Curl_گره_next(e);
    اگر((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_گره_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
ایستا تهی altsvc_parse_params(ثابت نویسه **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  اگر(curlx_str_single(pp, ';'))
    بازگشت;

  برای(;;) {
    ساختار Curl_str name;
    ساختار Curl_str val;
    curl_off_t رقم;
    صحیح keyword = 0;

    /* allow some extra whitespaces around name and value */
    اگر(curlx_str_until(pp, &name, MAX_ALTSVC_سطر, '=') ||
       curlx_str_single(pp, '='))
      بشکن;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    اگر(**pp == '\"') {
      اگر(curlx_str_quotedword(pp, &val, MAX_ALTSVC_سطر))
        بشکن;
    }
    وگرنه {
      اگر(curlx_str_cspn(pp, &val, ",;\r\n"))
        بشکن;
    }
    curlx_str_trimblanks(&val);

    اگر(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    وگرنه اگر(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    اگر(keyword) {
      ثابت نویسه *vp = curlx_str(&val);
      ثابت نویسه *vend = vp + curlx_strlen(&val);
      اگر(curlx_str_عدد(&vp, &رقم, CURL_OFF_T_MAX))
        بشکن; /* not a number, skip further parameter parsing */
      اگر(vp != vend)
        بشکن; /* not entirely a number, skip further parameter parsing */
      اگر(keyword == ALTSVC_MA) {
        اگر(رقم > CAP_ALTSVC_MAX_AGE)
          رقم = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)رقم;
      }
      وگرنه اگر(رقم == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    اگر(curlx_str_single(pp, ';'))
      بشکن; /* no further parameters */
  }
}

ایستا bool altsvc_parse_dest(ثابت نویسه **pp,
                              ساختار Curl_easy *data,
                              ساختار Curl_peer *origin,
                              ساختار Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  اگر(curlx_str_single(pp, '\"'))
    بازگشت FALSE;

  /* quoted string, with hostname or just :port ? */
  اگر(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    اگر(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      اگر(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        بازگشت FALSE;
      }
    }
    وگرنه { /* IPv6 hostname */
      اگر(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        بازگشت FALSE;
      }
    }
    اگر(curlx_str_single(pp, ':'))
      بازگشت FALSE; /* not followed by ':' */
  }
  وگرنه  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  اگر(curlx_str_عدد(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    بازگشت FALSE;
  }

  *pdstport = (uint16_t)port;
  اگر(curlx_str_single(pp, '\"'))
    بازگشت FALSE; /* quoted string not ending here as expected */
  بازگشت TRUE;
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
CURLcode Curl_altsvc_parse(ساختار Curl_easy *data,
                           ساختار altsvcinfo *asi, ثابت نویسه *مقدار,
                           ساختار Curl_peer *origin,
                           شمارش alpnid origin_alpnid)
{
  ساختار altsvc *as;
  size_t entries = 0;
  ساختار Curl_str alpn;
  ثابت نویسه *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = مقدار;
  اگر(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    اگر(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      بازگشت CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  برای(p = مقدار; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    شمارش alpnid dstalpnid;
    ساختار Curl_str dsthost;
    uint16_t dstport;

    اگر(curlx_str_until(&p, &alpn, MAX_ALTSVC_سطر, '='))
      بشکن; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    اگر(curlx_str_single(&p, '='))
      بشکن;

    /* Parse altsvc hostname:port */
    اگر(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      بشکن;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    اگر(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      اگر(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      اگر(as) {
        time_t secs = time(پوچ);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        اگر(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        وگرنه
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (صحیح)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      وگرنه
        بازگشت CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    اگر(curlx_str_single(&p, ','))
      بشکن;
  }

  بازگشت CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ساختار altsvcinfo *asi,
                        ساختار Curl_peer *origin,
                        شمارش alpnid origin_alpnid,
                        ساختار altsvc **dstentry,
                        ثابت صحیح versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  اگر(Curl_llist_شمار(&asi->list)) {
    ساختار Curl_llist_گره *e;
    ساختار Curl_llist_گره *n;
    time_t now = time(پوچ);

    برای(e = Curl_llist_head(&asi->list); e; e = n) {
      ساختار altsvc *as = Curl_گره_elem(e);
      n = Curl_گره_next(e);
      اگر(as->expires < now) {
        /* an expired entry, remove */
        Curl_گره_remove(e);
        altsvc_free(as);
        ادامه;
      }
      اگر((origin_alpnid == as->src.alpnid) &&
         (versions & (صحیح)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        بازگشت TRUE;
      }
    }
  }
  بازگشت FALSE;
}

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
