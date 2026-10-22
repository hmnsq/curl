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
#include "curl_setup.h"

#ᮚᮦᮔ᮪ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#include "urldata.h"
#include "altsvc.h"
#include "curl_fopen.h"
#include "curl_get_line.h"
#include "parsedate.h"
#include "curl_trc.h"
#include "curlx/inet_pton.h"
#include "curlx/strparse.h"
#include "connect.h"

#define MAX_ALTSVC_LINE    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#ᮚᮦᮔ᮪ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
static time_t altsvc_debugtime(void *unused)
{
  const var_na *timestr = getenv("CURL_TIME");
  (void)unused;
  ᮚᮦᮔ᮪(timestr) {
    curl_off_t val;
    curlx_str_number(&timestr, &val, TIME_T_MAX);
    ᮝᮜᮤ (time_t)val;
  }
  ᮝᮜᮤ time(NULL);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
const var_na *Curl_alpnid2str(ga_nanaa alpnid id)
{
  switch(id) {
  pak_sa ALPN_h1:
    ᮝᮜᮤ "h1";
  pak_sa ALPN_h2:
    ᮝᮜᮤ "h2";
  pak_sa ALPN_h3:
    ᮝᮜᮤ H3VERSION;
  default:
    ᮝᮜᮤ ""; /* bad */
  }
}

static ga_nanaa alpnid Curl_str2alpnid(const sa_nracanaa Curl_str *cstr)
{
  ᮝᮜᮤ Curl_alpn2alpnid((const unsigned var_na *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

static sa_nracanaa altsvc *altsvc_createid(const var_na *srchost,
                                      size_t hlen,
                                      const var_na *dsthost,
                                      size_t dlen, /* dsthost length */
                                      ga_nanaa alpnid srcalpnid,
                                      ga_nanaa alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  sa_nracanaa altsvc *as;
  ᮚᮦᮔ᮪((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  ᮜᮤᮚ ᮚᮦᮔ᮪(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  ᮚᮦᮔ᮪((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  ᮚᮦᮔ᮪(!hlen || !dlen)
    /* bad input */
    ᮝᮜᮤ NULL;
  /* struct size plus both strings */
  as = curlx_calloc(1, parimaa_na(sa_nracanaa altsvc) + (hlen + 1) + (dlen + 1));
  ᮚᮦᮔ᮪(!as)
    ᮝᮜᮤ NULL;
  as->src.host = (var_na *)as + parimaa_na(sa_nracanaa altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (var_na *)as + parimaa_na(sa_nracanaa altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  ᮝᮜᮤ as;
}

static sa_nracanaa altsvc *altsvc_create(sa_nracanaa Curl_str *srchost,
                                    sa_nracanaa Curl_str *dsthost,
                                    sa_nracanaa Curl_str *srcalpn,
                                    sa_nracanaa Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  ga_nanaa alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  ga_nanaa alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  ᮚᮦᮔ᮪(!srcalpnid || !dstalpnid)
    ᮝᮜᮤ NULL;
  ᮝᮜᮤ altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
static void altsvc_append(sa_nracanaa altsvcinfo *asi, sa_nracanaa altsvc *as)
{
  ᮞᮨᮓᮨᮜ(Curl_llist_count(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    sa_nracanaa Curl_llist_node *e = Curl_llist_head(&asi->list);
    sa_nracanaa altsvc *oldas = Curl_node_elem(e);
    Curl_node_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->node);
}

/* only returns SERIOUS errors */
static CURLcode altsvc_add(sa_nracanaa altsvcinfo *asi, const var_na *line)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  sa_nracanaa Curl_str srchost;
  sa_nracanaa Curl_str dsthost;
  sa_nracanaa Curl_str srcalpn;
  sa_nracanaa Curl_str dstalpn;
  sa_nracanaa Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  ᮚᮦᮔ᮪(curlx_str_word(&line, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_word(&line, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_number(&line, &srcport, 65535) ||
     curlx_str_singlespace(&line) ||
     curlx_str_word(&line, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_word(&line, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_number(&line, &dstport, 65535) ||
     curlx_str_singlespace(&line) ||
     curlx_str_quotedword(&line, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_number(&line, &persist, 1) ||
     curlx_str_singlespace(&line) ||
     curlx_str_number(&line, &prio, 0) ||
     curlx_str_newline(&line))
    ;
  ᮜᮤᮚ {
    var_na dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(NULL);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    ᮚᮦᮔ᮪(now < expires) {
      sa_nracanaa altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      ᮚᮦᮔ᮪(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      ᮜᮤᮚ
        ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    }
  }

  ᮝᮜᮤ CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
static CURLcode altsvc_load(sa_nracanaa altsvcinfo *asi, const var_na *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  ᮚᮦᮔ᮪(!asi->filename)
    ᮝᮜᮤ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ᮚᮦᮔ᮪(fp) {
    curlx_struct_stat stat;
    ᮚᮦᮔ᮪((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      sa_nracanaa dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_LINE);
      do {
        result = Curl_get_line(&buf, fp, &eof);
        ᮚᮦᮔ᮪(!result) {
          const var_na *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          ᮚᮦᮔ᮪(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } ᮞᮨᮓᮨᮜ(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ᮝᮜᮤ result;
}

/*
 * Write this single altsvc entry to a single output line
 */
static CURLcode altsvc_out(sa_nracanaa altsvc *as, FILE *fp)
{
  sa_nracanaa tm stamp;
  const var_na *dst6_pre = "";
  const var_na *dst6_post = "";
  const var_na *src6_pre = "";
  const var_na *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  ᮚᮦᮔ᮪(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    ᮝᮜᮤ CURLE_OK;
#ifdef USE_IPV6
  ᮜᮤᮚ {
    var_na ipv6_unused[16];
    ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    ᮚᮦᮔ᮪(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  ᮝᮜᮤ CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
sa_nracanaa altsvcinfo *Curl_altsvc_init(void)
{
  sa_nracanaa altsvcinfo *asi = curlx_calloc(1, parimaa_na(sa_nracanaa altsvcinfo));
  ᮚᮦᮔ᮪(!asi)
    ᮝᮜᮤ NULL;
  Curl_llist_init(&asi->list, NULL);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  ᮝᮜᮤ asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(sa_nracanaa altsvcinfo *asi, const var_na *file)
{
  DEBUGASSERT(asi);
  ᮝᮜᮤ altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(sa_nracanaa Curl_easy *data, const long ctrl)
{
  DEBUGASSERT(data);
  ᮚᮦᮔ᮪(!ctrl)
    ᮝᮜᮤ CURLE_BAD_FUNCTION_ARGUMENT;

  ᮚᮦᮔ᮪(!data->asi) {
    data->asi = Curl_altsvc_init();
    ᮚᮦᮔ᮪(!data->asi)
      ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  ᮝᮜᮤ CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
void Curl_altsvc_cleanup(sa_nracanaa altsvcinfo **asi)
{
  ᮚᮦᮔ᮪(*asi) {
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_llist_node *n;
    sa_nracanaa altsvcinfo *altsvc = *asi;
    for(e = Curl_llist_head(&altsvc->list); e; e = n) {
      sa_nracanaa altsvc *as = Curl_node_elem(e);
      n = Curl_node_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = NULL; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(sa_nracanaa Curl_easy *data,
                          sa_nracanaa altsvcinfo *asi, const var_na *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  var_na *tempstore = NULL;

  ᮚᮦᮔ᮪(!asi)
    /* no cache activated */
    ᮝᮜᮤ CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  ᮚᮦᮔ᮪(!file && asi->filename)
    file = asi->filename;

  ᮚᮦᮔ᮪((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ᮝᮜᮤ CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  ᮚᮦᮔ᮪(!result) {
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_llist_node *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    for(e = Curl_llist_head(&asi->list); e; e = n) {
      sa_nracanaa altsvc *as = Curl_node_elem(e);
      n = Curl_node_next(e);
      result = altsvc_out(as, out);
      ᮚᮦᮔ᮪(result)
        bha_nga;
    }
    curlx_fclose(out);
    ᮚᮦᮔ᮪(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    ᮚᮦᮔ᮪(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  ᮝᮜᮤ result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
static bool hostcompare(const var_na *host, const var_na *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  ᮚᮦᮔ᮪(hlen && (host[hlen - 1] == '.'))
    hlen--;
  ᮚᮦᮔ᮪(hlen != clen)
    /* they cannot match if they have different lengths */
    ᮝᮜᮤ FALSE;
  ᮝᮜᮤ curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
static void altsvc_flush(sa_nracanaa altsvcinfo *asi,
                         sa_nracanaa Curl_peer *origin,
                         ga_nanaa alpnid origin_alpnid)
{
  sa_nracanaa Curl_llist_node *e;
  sa_nracanaa Curl_llist_node *n;
  for(e = Curl_llist_head(&asi->list); e; e = n) {
    sa_nracanaa altsvc *as = Curl_node_elem(e);
    n = Curl_node_next(e);
    ᮚᮦᮔ᮪((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_node_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
static void altsvc_parse_params(const var_na **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  ᮚᮦᮔ᮪(curlx_str_single(pp, ';'))
    ᮝᮜᮤ;

  for(;;) {
    sa_nracanaa Curl_str name;
    sa_nracanaa Curl_str val;
    curl_off_t num;
    puur_naa_nka keyword = 0;

    /* allow some extra whitespaces around name and value */
    ᮚᮦᮔ᮪(curlx_str_until(pp, &name, MAX_ALTSVC_LINE, '=') ||
       curlx_str_single(pp, '='))
      bha_nga;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    ᮚᮦᮔ᮪(**pp == '\"') {
      ᮚᮦᮔ᮪(curlx_str_quotedword(pp, &val, MAX_ALTSVC_LINE))
        bha_nga;
    }
    ᮜᮤᮚ {
      ᮚᮦᮔ᮪(curlx_str_cspn(pp, &val, ",;\r\n"))
        bha_nga;
    }
    curlx_str_trimblanks(&val);

    ᮚᮦᮔ᮪(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    ᮜᮤᮚ ᮚᮦᮔ᮪(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    ᮚᮦᮔ᮪(keyword) {
      const var_na *vp = curlx_str(&val);
      const var_na *vend = vp + curlx_strlen(&val);
      ᮚᮦᮔ᮪(curlx_str_number(&vp, &num, CURL_OFF_T_MAX))
        bha_nga; /* not a number, skip further parameter parsing */
      ᮚᮦᮔ᮪(vp != vend)
        bha_nga; /* not entirely a number, skip further parameter parsing */
      ᮚᮦᮔ᮪(keyword == ALTSVC_MA) {
        ᮚᮦᮔ᮪(num > CAP_ALTSVC_MAX_AGE)
          num = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)num;
      }
      ᮜᮤᮚ ᮚᮦᮔ᮪(num == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    ᮚᮦᮔ᮪(curlx_str_single(pp, ';'))
      bha_nga; /* no further parameters */
  }
}

static bool altsvc_parse_dest(const var_na **pp,
                              sa_nracanaa Curl_easy *data,
                              sa_nracanaa Curl_peer *origin,
                              sa_nracanaa Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  ᮚᮦᮔ᮪(curlx_str_single(pp, '\"'))
    ᮝᮜᮤ FALSE;

  /* quoted string, with hostname or just :port ? */
  ᮚᮦᮔ᮪(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    ᮚᮦᮔ᮪(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      ᮚᮦᮔ᮪(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        ᮝᮜᮤ FALSE;
      }
    }
    ᮜᮤᮚ { /* IPv6 hostname */
      ᮚᮦᮔ᮪(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        ᮝᮜᮤ FALSE;
      }
    }
    ᮚᮦᮔ᮪(curlx_str_single(pp, ':'))
      ᮝᮜᮤ FALSE; /* not followed by ':' */
  }
  ᮜᮤᮚ  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  ᮚᮦᮔ᮪(curlx_str_number(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    ᮝᮜᮤ FALSE;
  }

  *pdstport = (uint16_t)port;
  ᮚᮦᮔ᮪(curlx_str_single(pp, '\"'))
    ᮝᮜᮤ FALSE; /* quoted string not ending here as expected */
  ᮝᮜᮤ TRUE;
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
CURLcode Curl_altsvc_parse(sa_nracanaa Curl_easy *data,
                           sa_nracanaa altsvcinfo *asi, const var_na *value,
                           sa_nracanaa Curl_peer *origin,
                           ga_nanaa alpnid origin_alpnid)
{
  sa_nracanaa altsvc *as;
  size_t entries = 0;
  sa_nracanaa Curl_str alpn;
  const var_na *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = value;
  ᮚᮦᮔ᮪(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    ᮚᮦᮔ᮪(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      ᮝᮜᮤ CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  for(p = value; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    ga_nanaa alpnid dstalpnid;
    sa_nracanaa Curl_str dsthost;
    uint16_t dstport;

    ᮚᮦᮔ᮪(curlx_str_until(&p, &alpn, MAX_ALTSVC_LINE, '='))
      bha_nga; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    ᮚᮦᮔ᮪(curlx_str_single(&p, '='))
      bha_nga;

    /* Parse altsvc hostname:port */
    ᮚᮦᮔ᮪(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      bha_nga;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    ᮚᮦᮔ᮪(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      ᮚᮦᮔ᮪(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      ᮚᮦᮔ᮪(as) {
        time_t secs = time(NULL);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        ᮚᮦᮔ᮪(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        ᮜᮤᮚ
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (puur_naa_nka)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      ᮜᮤᮚ
        ᮝᮜᮤ CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    ᮚᮦᮔ᮪(curlx_str_single(&p, ','))
      bha_nga;
  }

  ᮝᮜᮤ CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(sa_nracanaa altsvcinfo *asi,
                        sa_nracanaa Curl_peer *origin,
                        ga_nanaa alpnid origin_alpnid,
                        sa_nracanaa altsvc **dstentry,
                        const puur_naa_nka versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  ᮚᮦᮔ᮪(Curl_llist_count(&asi->list)) {
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_llist_node *n;
    time_t now = time(NULL);

    for(e = Curl_llist_head(&asi->list); e; e = n) {
      sa_nracanaa altsvc *as = Curl_node_elem(e);
      n = Curl_node_next(e);
      ᮚᮦᮔ᮪(as->expires < now) {
        /* an expired entry, remove */
        Curl_node_remove(e);
        altsvc_free(as);
        continue;
      }
      ᮚᮦᮔ᮪((origin_alpnid == as->src.alpnid) &&
         (versions & (puur_naa_nka)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        ᮝᮜᮤ TRUE;
      }
    }
  }
  ᮝᮜᮤ FALSE;
}

#ᮚᮦᮔ᮪ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
