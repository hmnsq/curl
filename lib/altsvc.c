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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

#define MAX_ALTSVC_ಸಾಲು    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#ಯದಿ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ಸ್ಥಿರ time_t altsvc_debugtime(ಶೂನ್ಯ *unused)
{
  ಧ್ರುವ ವರ್ಣ *timestr = getenv("CURL_TIME");
  (ಶೂನ್ಯ)unused;
  ಯದಿ(timestr) {
    curl_off_t val;
    curlx_str_ಸಂಖ್ಯೆ(&timestr, &val, TIME_T_MAX);
    ಪ್ರತ್ಯಾಗಮ (time_t)val;
  }
  ಪ್ರತ್ಯಾಗಮ time(ರಿಕ್ತ);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
ಧ್ರುವ ವರ್ಣ *Curl_alpnid2str(ಗಣನಾ alpnid id)
{
  ವಿಕಲ್ಪ(id) {
  ಪಕ್ಷ ALPN_h1:
    ಪ್ರತ್ಯಾಗಮ "h1";
  ಪಕ್ಷ ALPN_h2:
    ಪ್ರತ್ಯಾಗಮ "h2";
  ಪಕ್ಷ ALPN_h3:
    ಪ್ರತ್ಯಾಗಮ H3VERSION;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ ""; /* bad */
  }
}

ಸ್ಥಿರ ಗಣನಾ alpnid Curl_str2alpnid(ಧ್ರುವ ಸಂರಚನಾ Curl_str *cstr)
{
  ಪ್ರತ್ಯಾಗಮ Curl_alpn2alpnid((ಧ್ರುವ unsigned ವರ್ಣ *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

ಸ್ಥಿರ ಸಂರಚನಾ altsvc *altsvc_createid(ಧ್ರುವ ವರ್ಣ *srchost,
                                      size_t hlen,
                                      ಧ್ರುವ ವರ್ಣ *dsthost,
                                      size_t dlen, /* dsthost length */
                                      ಗಣನಾ alpnid srcalpnid,
                                      ಗಣನಾ alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ಸಂರಚನಾ altsvc *as;
  ಯದಿ((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  ಅನ್ಯಥಾ ಯದಿ(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  ಯದಿ((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  ಯದಿ(!hlen || !dlen)
    /* bad input */
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  /* struct size plus both strings */
  as = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ altsvc) + (hlen + 1) + (dlen + 1));
  ಯದಿ(!as)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  as->src.host = (ವರ್ಣ *)as + ಪರಿಮಾಣ(ಸಂರಚನಾ altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (ವರ್ಣ *)as + ಪರಿಮಾಣ(ಸಂರಚನಾ altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  ಪ್ರತ್ಯಾಗಮ as;
}

ಸ್ಥಿರ ಸಂರಚನಾ altsvc *altsvc_create(ಸಂರಚನಾ Curl_str *srchost,
                                    ಸಂರಚನಾ Curl_str *dsthost,
                                    ಸಂರಚನಾ Curl_str *srcalpn,
                                    ಸಂರಚನಾ Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  ಗಣನಾ alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  ಗಣನಾ alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  ಯದಿ(!srcalpnid || !dstalpnid)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  ಪ್ರತ್ಯಾಗಮ altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
ಸ್ಥಿರ ಶೂನ್ಯ altsvc_append(ಸಂರಚನಾ altsvcinfo *asi, ಸಂರಚನಾ altsvc *as)
{
  ಯಾವತ್(Curl_llist_ಎಣಿಕೆ(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e = Curl_llist_head(&asi->list);
    ಸಂರಚನಾ altsvc *oldas = Curl_ಗಂಟು_elem(e);
    Curl_ಗಂಟು_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ಗಂಟು);
}

/* only returns SERIOUS errors */
ಸ್ಥಿರ CURLcode altsvc_add(ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *ಸಾಲು)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ಸಂರಚನಾ Curl_str srchost;
  ಸಂರಚನಾ Curl_str dsthost;
  ಸಂರಚನಾ Curl_str srcalpn;
  ಸಂರಚನಾ Curl_str dstalpn;
  ಸಂರಚನಾ Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  ಯದಿ(curlx_str_word(&ಸಾಲು, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_word(&ಸಾಲು, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &srcport, 65535) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_word(&ಸಾಲು, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_word(&ಸಾಲು, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &dstport, 65535) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_quotedword(&ಸಾಲು, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &persist, 1) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &prio, 0) ||
     curlx_str_newline(&ಸಾಲು))
    ;
  ಅನ್ಯಥಾ {
    ವರ್ಣ dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(ರಿಕ್ತ);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    ಯದಿ(now < expires) {
      ಸಂರಚನಾ altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      ಯದಿ(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ಸ್ಥಿರ CURLcode altsvc_load(ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  ಯದಿ(!asi->filename)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ಯದಿ(fp) {
    curlx_struct_stat stat;
    ಯದಿ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      ಸಂರಚನಾ dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_ಸಾಲು);
      ಕುರು {
        result = Curl_get_ಸಾಲು(&buf, fp, &eof);
        ಯದಿ(!result) {
          ಧ್ರುವ ವರ್ಣ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          ಯದಿ(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } ಯಾವತ್(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Write this single altsvc entry to a single output line
 */
ಸ್ಥಿರ CURLcode altsvc_out(ಸಂರಚನಾ altsvc *as, FILE *fp)
{
  ಸಂರಚನಾ tm stamp;
  ಧ್ರುವ ವರ್ಣ *dst6_pre = "";
  ಧ್ರುವ ವರ್ಣ *dst6_post = "";
  ಧ್ರುವ ವರ್ಣ *src6_pre = "";
  ಧ್ರುವ ವರ್ಣ *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  ಯದಿ(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;
#ifdef USE_IPV6
  ಅನ್ಯಥಾ {
    ವರ್ಣ ipv6_unused[16];
    ಯದಿ(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    ಯದಿ(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ಸಂರಚನಾ altsvcinfo *Curl_altsvc_init(ಶೂನ್ಯ)
{
  ಸಂರಚನಾ altsvcinfo *asi = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ altsvcinfo));
  ಯದಿ(!asi)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  Curl_llist_init(&asi->list, ರಿಕ್ತ);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  ಪ್ರತ್ಯಾಗಮ asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *file)
{
  DEBUGASSERT(asi);
  ಪ್ರತ್ಯಾಗಮ altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ long ctrl)
{
  DEBUGASSERT(data);
  ಯದಿ(!ctrl)
    ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;

  ಯದಿ(!data->asi) {
    data->asi = Curl_altsvc_init();
    ಯದಿ(!data->asi)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
ಶೂನ್ಯ Curl_altsvc_cleanup(ಸಂರಚನಾ altsvcinfo **asi)
{
  ಯದಿ(*asi) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
    ಸಂರಚನಾ altsvcinfo *altsvc = *asi;
    ಪ್ರತಿ(e = Curl_llist_head(&altsvc->list); e; e = n) {
      ಸಂರಚನಾ altsvc *as = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ರಿಕ್ತ; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  ವರ್ಣ *tempstore = ರಿಕ್ತ;

  ಯದಿ(!asi)
    /* no cache activated */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  ಯದಿ(!file && asi->filename)
    file = asi->filename;

  ಯದಿ((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  ಯದಿ(!result) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    ಪ್ರತಿ(e = Curl_llist_head(&asi->list); e; e = n) {
      ಸಂರಚನಾ altsvc *as = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_next(e);
      result = altsvc_out(as, out);
      ಯದಿ(result)
        ಭಙ್ಗ;
    }
    curlx_fclose(out);
    ಯದಿ(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    ಯದಿ(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  ಪ್ರತ್ಯಾಗಮ result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ಸ್ಥಿರ bool hostcompare(ಧ್ರುವ ವರ್ಣ *host, ಧ್ರುವ ವರ್ಣ *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  ಯದಿ(hlen && (host[hlen - 1] == '.'))
    hlen--;
  ಯದಿ(hlen != clen)
    /* they cannot match if they have different lengths */
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ಸ್ಥಿರ ಶೂನ್ಯ altsvc_flush(ಸಂರಚನಾ altsvcinfo *asi,
                         ಸಂರಚನಾ Curl_peer *origin,
                         ಗಣನಾ alpnid origin_alpnid)
{
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
  ಪ್ರತಿ(e = Curl_llist_head(&asi->list); e; e = n) {
    ಸಂರಚನಾ altsvc *as = Curl_ಗಂಟು_elem(e);
    n = Curl_ಗಂಟು_next(e);
    ಯದಿ((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ಗಂಟು_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
ಸ್ಥಿರ ಶೂನ್ಯ altsvc_parse_params(ಧ್ರುವ ವರ್ಣ **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  ಯದಿ(curlx_str_single(pp, ';'))
    ಪ್ರತ್ಯಾಗಮ;

  ಪ್ರತಿ(;;) {
    ಸಂರಚನಾ Curl_str name;
    ಸಂರಚನಾ Curl_str val;
    curl_off_t ಅಂಕೆ;
    ಪೂರ್ಣಾಙ್ಕ keyword = 0;

    /* allow some extra whitespaces around name and value */
    ಯದಿ(curlx_str_until(pp, &name, MAX_ALTSVC_ಸಾಲು, '=') ||
       curlx_str_single(pp, '='))
      ಭಙ್ಗ;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    ಯದಿ(**pp == '\"') {
      ಯದಿ(curlx_str_quotedword(pp, &val, MAX_ALTSVC_ಸಾಲು))
        ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(curlx_str_cspn(pp, &val, ",;\r\n"))
        ಭಙ್ಗ;
    }
    curlx_str_trimblanks(&val);

    ಯದಿ(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    ಯದಿ(keyword) {
      ಧ್ರುವ ವರ್ಣ *vp = curlx_str(&val);
      ಧ್ರುವ ವರ್ಣ *vend = vp + curlx_strlen(&val);
      ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&vp, &ಅಂಕೆ, CURL_OFF_T_MAX))
        ಭಙ್ಗ; /* not a number, skip further parameter parsing */
      ಯದಿ(vp != vend)
        ಭಙ್ಗ; /* not entirely a number, skip further parameter parsing */
      ಯದಿ(keyword == ALTSVC_MA) {
        ಯದಿ(ಅಂಕೆ > CAP_ALTSVC_MAX_AGE)
          ಅಂಕೆ = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)ಅಂಕೆ;
      }
      ಅನ್ಯಥಾ ಯದಿ(ಅಂಕೆ == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    ಯದಿ(curlx_str_single(pp, ';'))
      ಭಙ್ಗ; /* no further parameters */
  }
}

ಸ್ಥಿರ bool altsvc_parse_dest(ಧ್ರುವ ವರ್ಣ **pp,
                              ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ Curl_peer *origin,
                              ಸಂರಚನಾ Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  ಯದಿ(curlx_str_single(pp, '\"'))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /* quoted string, with hostname or just :port ? */
  ಯದಿ(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    ಯದಿ(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      ಯದಿ(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        ಪ್ರತ್ಯಾಗಮ FALSE;
      }
    }
    ಅನ್ಯಥಾ { /* IPv6 hostname */
      ಯದಿ(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        ಪ್ರತ್ಯಾಗಮ FALSE;
      }
    }
    ಯದಿ(curlx_str_single(pp, ':'))
      ಪ್ರತ್ಯಾಗಮ FALSE; /* not followed by ':' */
  }
  ಅನ್ಯಥಾ  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  *pdstport = (uint16_t)port;
  ಯದಿ(curlx_str_single(pp, '\"'))
    ಪ್ರತ್ಯಾಗಮ FALSE; /* quoted string not ending here as expected */
  ಪ್ರತ್ಯಾಗಮ TRUE;
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
CURLcode Curl_altsvc_parse(ಸಂರಚನಾ Curl_easy *data,
                           ಸಂರಚನಾ altsvcinfo *asi, ಧ್ರುವ ವರ್ಣ *ಮೌಲ್ಯ,
                           ಸಂರಚನಾ Curl_peer *origin,
                           ಗಣನಾ alpnid origin_alpnid)
{
  ಸಂರಚನಾ altsvc *as;
  size_t entries = 0;
  ಸಂರಚನಾ Curl_str alpn;
  ಧ್ರುವ ವರ್ಣ *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = ಮೌಲ್ಯ;
  ಯದಿ(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    ಯದಿ(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      ಪ್ರತ್ಯಾಗಮ CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  ಪ್ರತಿ(p = ಮೌಲ್ಯ; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    ಗಣನಾ alpnid dstalpnid;
    ಸಂರಚನಾ Curl_str dsthost;
    uint16_t dstport;

    ಯದಿ(curlx_str_until(&p, &alpn, MAX_ALTSVC_ಸಾಲು, '='))
      ಭಙ್ಗ; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    ಯದಿ(curlx_str_single(&p, '='))
      ಭಙ್ಗ;

    /* Parse altsvc hostname:port */
    ಯದಿ(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      ಭಙ್ಗ;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    ಯದಿ(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      ಯದಿ(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      ಯದಿ(as) {
        time_t secs = time(ರಿಕ್ತ);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        ಯದಿ(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        ಅನ್ಯಥಾ
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (ಪೂರ್ಣಾಙ್ಕ)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      ಅನ್ಯಥಾ
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    ಯದಿ(curlx_str_single(&p, ','))
      ಭಙ್ಗ;
  }

  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ಸಂರಚನಾ altsvcinfo *asi,
                        ಸಂರಚನಾ Curl_peer *origin,
                        ಗಣನಾ alpnid origin_alpnid,
                        ಸಂರಚನಾ altsvc **dstentry,
                        ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  ಯದಿ(Curl_llist_ಎಣಿಕೆ(&asi->list)) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
    time_t now = time(ರಿಕ್ತ);

    ಪ್ರತಿ(e = Curl_llist_head(&asi->list); e; e = n) {
      ಸಂರಚನಾ altsvc *as = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_next(e);
      ಯದಿ(as->expires < now) {
        /* an expired entry, remove */
        Curl_ಗಂಟು_remove(e);
        altsvc_free(as);
        ಅನುವರ್ತ;
      }
      ಯದಿ((origin_alpnid == as->src.alpnid) &&
         (versions & (ಪೂರ್ಣಾಙ್ಕ)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        ಪ್ರತ್ಯಾಗಮ TRUE;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
