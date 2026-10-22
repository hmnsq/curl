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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "altsvc.h"
#અન્તર્ભાવ "curl_fopen.h"
#અન્તર્ભાવ "curl_get_line.h"
#અન્તર્ભાવ "parsedate.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "connect.h"

#define MAX_ALTSVC_લીટી    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#જો defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
સ્થિર time_t altsvc_debugtime(શૂન્ય *unused)
{
  અચલ અક્ષર *timestr = getenv("CURL_TIME");
  (શૂન્ય)unused;
  જો(timestr) {
    curl_off_t val;
    curlx_str_સંખ્યા(&timestr, &val, TIME_T_MAX);
    પાછા (time_t)val;
  }
  પાછા time(ખાલી);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
અચલ અક્ષર *Curl_alpnid2str(ગણના alpnid id)
{
  પસંદગી(id) {
  સ્થિતિ ALPN_h1:
    પાછા "h1";
  સ્થિતિ ALPN_h2:
    પાછા "h2";
  સ્થિતિ ALPN_h3:
    પાછા H3VERSION;
  મૂળભૂત:
    પાછા ""; /* bad */
  }
}

સ્થિર ગણના alpnid Curl_str2alpnid(અચલ સંરચના Curl_str *cstr)
{
  પાછા Curl_alpn2alpnid((અચલ unsigned અક્ષર *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

સ્થિર સંરચના altsvc *altsvc_createid(અચલ અક્ષર *srchost,
                                      size_t hlen,
                                      અચલ અક્ષર *dsthost,
                                      size_t dlen, /* dsthost length */
                                      ગણના alpnid srcalpnid,
                                      ગણના alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  સંરચના altsvc *as;
  જો((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  નહીંતર જો(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  જો((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  જો(!hlen || !dlen)
    /* bad input */
    પાછા ખાલી;
  /* struct size plus both strings */
  as = curlx_calloc(1, કદ(સંરચના altsvc) + (hlen + 1) + (dlen + 1));
  જો(!as)
    પાછા ખાલી;
  as->src.host = (અક્ષર *)as + કદ(સંરચના altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (અક્ષર *)as + કદ(સંરચના altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  પાછા as;
}

સ્થિર સંરચના altsvc *altsvc_create(સંરચના Curl_str *srchost,
                                    સંરચના Curl_str *dsthost,
                                    સંરચના Curl_str *srcalpn,
                                    સંરચના Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  ગણના alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  ગણના alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  જો(!srcalpnid || !dstalpnid)
    પાછા ખાલી;
  પાછા altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
સ્થિર શૂન્ય altsvc_append(સંરચના altsvcinfo *asi, સંરચના altsvc *as)
{
  જ્યારે(Curl_llist_ગણતરી(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    સંરચના Curl_llist_ગાંઠ *e = Curl_llist_head(&asi->list);
    સંરચના altsvc *oldas = Curl_ગાંઠ_elem(e);
    Curl_ગાંઠ_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ગાંઠ);
}

/* only returns SERIOUS errors */
સ્થિર CURLcode altsvc_add(સંરચના altsvcinfo *asi, અચલ અક્ષર *લીટી)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  સંરચના Curl_str srchost;
  સંરચના Curl_str dsthost;
  સંરચના Curl_str srcalpn;
  સંરચના Curl_str dstalpn;
  સંરચના Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  જો(curlx_str_word(&લીટી, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_word(&લીટી, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_સંખ્યા(&લીટી, &srcport, 65535) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_word(&લીટી, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_word(&લીટી, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_સંખ્યા(&લીટી, &dstport, 65535) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_quotedword(&લીટી, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_સંખ્યા(&લીટી, &persist, 1) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_સંખ્યા(&લીટી, &prio, 0) ||
     curlx_str_newline(&લીટી))
    ;
  નહીંતર {
    અક્ષર dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(ખાલી);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    જો(now < expires) {
      સંરચના altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      જો(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      નહીંતર
        પાછા CURLE_OUT_OF_MEMORY;
    }
  }

  પાછા CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
સ્થિર CURLcode altsvc_load(સંરચના altsvcinfo *asi, અચલ અક્ષર *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  જો(!asi->filename)
    પાછા CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  જો(fp) {
    curlx_struct_stat stat;
    જો((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      સંરચના dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_લીટી);
      કરો {
        result = Curl_get_લીટી(&buf, fp, &eof);
        જો(!result) {
          અચલ અક્ષર *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          જો(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } જ્યારે(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  પાછા result;
}

/*
 * Write this single altsvc entry to a single output line
 */
સ્થિર CURLcode altsvc_out(સંરચના altsvc *as, FILE *fp)
{
  સંરચના tm stamp;
  અચલ અક્ષર *dst6_pre = "";
  અચલ અક્ષર *dst6_post = "";
  અચલ અક્ષર *src6_pre = "";
  અચલ અક્ષર *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  જો(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    પાછા CURLE_OK;
#ifdef USE_IPV6
  નહીંતર {
    અક્ષર ipv6_unused[16];
    જો(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    જો(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  પાછા CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
સંરચના altsvcinfo *Curl_altsvc_init(શૂન્ય)
{
  સંરચના altsvcinfo *asi = curlx_calloc(1, કદ(સંરચના altsvcinfo));
  જો(!asi)
    પાછા ખાલી;
  Curl_llist_init(&asi->list, ખાલી);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  પાછા asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(સંરચના altsvcinfo *asi, અચલ અક્ષર *file)
{
  DEBUGASSERT(asi);
  પાછા altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(સંરચના Curl_easy *data, અચલ long ctrl)
{
  DEBUGASSERT(data);
  જો(!ctrl)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(!data->asi) {
    data->asi = Curl_altsvc_init();
    જો(!data->asi)
      પાછા CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  પાછા CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
શૂન્ય Curl_altsvc_cleanup(સંરચના altsvcinfo **asi)
{
  જો(*asi) {
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_llist_ગાંઠ *n;
    સંરચના altsvcinfo *altsvc = *asi;
    માટે(e = Curl_llist_head(&altsvc->list); e; e = n) {
      સંરચના altsvc *as = Curl_ગાંઠ_elem(e);
      n = Curl_ગાંઠ_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ખાલી; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(સંરચના Curl_easy *data,
                          સંરચના altsvcinfo *asi, અચલ અક્ષર *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  અક્ષર *tempstore = ખાલી;

  જો(!asi)
    /* no cache activated */
    પાછા CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  જો(!file && asi->filename)
    file = asi->filename;

  જો((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    પાછા CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  જો(!result) {
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_llist_ગાંઠ *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    માટે(e = Curl_llist_head(&asi->list); e; e = n) {
      સંરચના altsvc *as = Curl_ગાંઠ_elem(e);
      n = Curl_ગાંઠ_next(e);
      result = altsvc_out(as, out);
      જો(result)
        તોડો;
    }
    curlx_fclose(out);
    જો(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    જો(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  પાછા result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
સ્થિર bool hostcompare(અચલ અક્ષર *host, અચલ અક્ષર *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  જો(hlen && (host[hlen - 1] == '.'))
    hlen--;
  જો(hlen != clen)
    /* they cannot match if they have different lengths */
    પાછા FALSE;
  પાછા curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
સ્થિર શૂન્ય altsvc_flush(સંરચના altsvcinfo *asi,
                         સંરચના Curl_peer *origin,
                         ગણના alpnid origin_alpnid)
{
  સંરચના Curl_llist_ગાંઠ *e;
  સંરચના Curl_llist_ગાંઠ *n;
  માટે(e = Curl_llist_head(&asi->list); e; e = n) {
    સંરચના altsvc *as = Curl_ગાંઠ_elem(e);
    n = Curl_ગાંઠ_next(e);
    જો((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ગાંઠ_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
સ્થિર શૂન્ય altsvc_parse_params(અચલ અક્ષર **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  જો(curlx_str_single(pp, ';'))
    પાછા;

  માટે(;;) {
    સંરચના Curl_str name;
    સંરચના Curl_str val;
    curl_off_t અંક;
    પૂર્ણાંક keyword = 0;

    /* allow some extra whitespaces around name and value */
    જો(curlx_str_until(pp, &name, MAX_ALTSVC_લીટી, '=') ||
       curlx_str_single(pp, '='))
      તોડો;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    જો(**pp == '\"') {
      જો(curlx_str_quotedword(pp, &val, MAX_ALTSVC_લીટી))
        તોડો;
    }
    નહીંતર {
      જો(curlx_str_cspn(pp, &val, ",;\r\n"))
        તોડો;
    }
    curlx_str_trimblanks(&val);

    જો(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    નહીંતર જો(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    જો(keyword) {
      અચલ અક્ષર *vp = curlx_str(&val);
      અચલ અક્ષર *vend = vp + curlx_strlen(&val);
      જો(curlx_str_સંખ્યા(&vp, &અંક, CURL_OFF_T_MAX))
        તોડો; /* not a number, skip further parameter parsing */
      જો(vp != vend)
        તોડો; /* not entirely a number, skip further parameter parsing */
      જો(keyword == ALTSVC_MA) {
        જો(અંક > CAP_ALTSVC_MAX_AGE)
          અંક = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)અંક;
      }
      નહીંતર જો(અંક == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    જો(curlx_str_single(pp, ';'))
      તોડો; /* no further parameters */
  }
}

સ્થિર bool altsvc_parse_dest(અચલ અક્ષર **pp,
                              સંરચના Curl_easy *data,
                              સંરચના Curl_peer *origin,
                              સંરચના Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  જો(curlx_str_single(pp, '\"'))
    પાછા FALSE;

  /* quoted string, with hostname or just :port ? */
  જો(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    જો(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      જો(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        પાછા FALSE;
      }
    }
    નહીંતર { /* IPv6 hostname */
      જો(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        પાછા FALSE;
      }
    }
    જો(curlx_str_single(pp, ':'))
      પાછા FALSE; /* not followed by ':' */
  }
  નહીંતર  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  જો(curlx_str_સંખ્યા(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    પાછા FALSE;
  }

  *pdstport = (uint16_t)port;
  જો(curlx_str_single(pp, '\"'))
    પાછા FALSE; /* quoted string not ending here as expected */
  પાછા TRUE;
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
CURLcode Curl_altsvc_parse(સંરચના Curl_easy *data,
                           સંરચના altsvcinfo *asi, અચલ અક્ષર *કિંમત,
                           સંરચના Curl_peer *origin,
                           ગણના alpnid origin_alpnid)
{
  સંરચના altsvc *as;
  size_t entries = 0;
  સંરચના Curl_str alpn;
  અચલ અક્ષર *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = કિંમત;
  જો(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    જો(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      પાછા CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  માટે(p = કિંમત; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    ગણના alpnid dstalpnid;
    સંરચના Curl_str dsthost;
    uint16_t dstport;

    જો(curlx_str_until(&p, &alpn, MAX_ALTSVC_લીટી, '='))
      તોડો; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    જો(curlx_str_single(&p, '='))
      તોડો;

    /* Parse altsvc hostname:port */
    જો(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      તોડો;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    જો(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      જો(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      જો(as) {
        time_t secs = time(ખાલી);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        જો(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        નહીંતર
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (પૂર્ણાંક)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      નહીંતર
        પાછા CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    જો(curlx_str_single(&p, ','))
      તોડો;
  }

  પાછા CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(સંરચના altsvcinfo *asi,
                        સંરચના Curl_peer *origin,
                        ગણના alpnid origin_alpnid,
                        સંરચના altsvc **dstentry,
                        અચલ પૂર્ણાંક versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  જો(Curl_llist_ગણતરી(&asi->list)) {
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_llist_ગાંઠ *n;
    time_t now = time(ખાલી);

    માટે(e = Curl_llist_head(&asi->list); e; e = n) {
      સંરચના altsvc *as = Curl_ગાંઠ_elem(e);
      n = Curl_ગાંઠ_next(e);
      જો(as->expires < now) {
        /* an expired entry, remove */
        Curl_ગાંઠ_remove(e);
        altsvc_free(as);
        ચાલુ;
      }
      જો((origin_alpnid == as->src.alpnid) &&
         (versions & (પૂર્ણાંક)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        પાછા TRUE;
      }
    }
  }
  પાછા FALSE;
}

#જો defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
