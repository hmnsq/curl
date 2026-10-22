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

#如果 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
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

#如果 defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
static time_t altsvc_debugtime(void *unused)
{
  const char *timestr = getenv("CURL_TIME");
  (void)unused;
  如果(timestr) {
    curl_off_t val;
    curlx_str_number(&timestr, &val, TIME_T_MAX);
    返回 (time_t)val;
  }
  返回 time(NULL);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
const char *Curl_alpnid2str(enum alpnid id)
{
  switch(id) {
  case ALPN_h1:
    返回 "h1";
  case ALPN_h2:
    返回 "h2";
  case ALPN_h3:
    返回 H3VERSION;
  default:
    返回 ""; /* bad */
  }
}

static enum alpnid Curl_str2alpnid(const struct Curl_str *cstr)
{
  返回 Curl_alpn2alpnid((const unsigned char *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

static struct altsvc *altsvc_createid(const char *srchost,
                                      size_t hlen,
                                      const char *dsthost,
                                      size_t dlen, /* dsthost length */
                                      enum alpnid srcalpnid,
                                      enum alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  struct altsvc *as;
  如果((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  否则 如果(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  如果((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  如果(!hlen || !dlen)
    /* bad input */
    返回 NULL;
  /* struct size plus both strings */
  as = curlx_calloc(1, sizeof(struct altsvc) + (hlen + 1) + (dlen + 1));
  如果(!as)
    返回 NULL;
  as->src.host = (char *)as + sizeof(struct altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (char *)as + sizeof(struct altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  返回 as;
}

static struct altsvc *altsvc_create(struct Curl_str *srchost,
                                    struct Curl_str *dsthost,
                                    struct Curl_str *srcalpn,
                                    struct Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  enum alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  enum alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  如果(!srcalpnid || !dstalpnid)
    返回 NULL;
  返回 altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
static void altsvc_append(struct altsvcinfo *asi, struct altsvc *as)
{
  当(Curl_llist_count(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    struct Curl_llist_node *e = Curl_llist_head(&asi->list);
    struct altsvc *oldas = Curl_node_elem(e);
    Curl_node_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->node);
}

/* only returns SERIOUS errors */
static CURLcode altsvc_add(struct altsvcinfo *asi, const char *line)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  struct Curl_str srchost;
  struct Curl_str dsthost;
  struct Curl_str srcalpn;
  struct Curl_str dstalpn;
  struct Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  如果(curlx_str_word(&line, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
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
  否则 {
    char dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(NULL);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    如果(now < expires) {
      struct altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      如果(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      否则
        返回 CURLE_OUT_OF_MEMORY;
    }
  }

  返回 CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
static CURLcode altsvc_load(struct altsvcinfo *asi, const char *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  如果(!asi->filename)
    返回 CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  如果(fp) {
    curlx_struct_stat stat;
    如果((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      struct dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_LINE);
      do {
        result = Curl_get_line(&buf, fp, &eof);
        如果(!result) {
          const char *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          如果(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } 当(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  返回 result;
}

/*
 * Write this single altsvc entry to a single output line
 */
static CURLcode altsvc_out(struct altsvc *as, FILE *fp)
{
  struct tm stamp;
  const char *dst6_pre = "";
  const char *dst6_post = "";
  const char *src6_pre = "";
  const char *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  如果(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    返回 CURLE_OK;
#ifdef USE_IPV6
  否则 {
    char ipv6_unused[16];
    如果(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    如果(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  返回 CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
struct altsvcinfo *Curl_altsvc_init(void)
{
  struct altsvcinfo *asi = curlx_calloc(1, sizeof(struct altsvcinfo));
  如果(!asi)
    返回 NULL;
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
  返回 asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(struct altsvcinfo *asi, const char *file)
{
  DEBUGASSERT(asi);
  返回 altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(struct Curl_easy *data, const long ctrl)
{
  DEBUGASSERT(data);
  如果(!ctrl)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  如果(!data->asi) {
    data->asi = Curl_altsvc_init();
    如果(!data->asi)
      返回 CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  返回 CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
void Curl_altsvc_cleanup(struct altsvcinfo **asi)
{
  如果(*asi) {
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    struct altsvcinfo *altsvc = *asi;
    为(e = Curl_llist_head(&altsvc->list); e; e = n) {
      struct altsvc *as = Curl_node_elem(e);
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
CURLcode Curl_altsvc_save(struct Curl_easy *data,
                          struct altsvcinfo *asi, const char *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  char *tempstore = NULL;

  如果(!asi)
    /* no cache activated */
    返回 CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  如果(!file && asi->filename)
    file = asi->filename;

  如果((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    返回 CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  如果(!result) {
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    为(e = Curl_llist_head(&asi->list); e; e = n) {
      struct altsvc *as = Curl_node_elem(e);
      n = Curl_node_next(e);
      result = altsvc_out(as, out);
      如果(result)
        break;
    }
    curlx_fclose(out);
    如果(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    如果(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  返回 result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
static bool hostcompare(const char *host, const char *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  如果(hlen && (host[hlen - 1] == '.'))
    hlen--;
  如果(hlen != clen)
    /* they cannot match if they have different lengths */
    返回 FALSE;
  返回 curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
static void altsvc_flush(struct altsvcinfo *asi,
                         struct Curl_peer *origin,
                         enum alpnid origin_alpnid)
{
  struct Curl_llist_node *e;
  struct Curl_llist_node *n;
  为(e = Curl_llist_head(&asi->list); e; e = n) {
    struct altsvc *as = Curl_node_elem(e);
    n = Curl_node_next(e);
    如果((origin_alpnid == as->src.alpnid) &&
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
static void altsvc_parse_params(const char **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  如果(curlx_str_single(pp, ';'))
    返回;

  为(;;) {
    struct Curl_str name;
    struct Curl_str val;
    curl_off_t num;
    整数 keyword = 0;

    /* allow some extra whitespaces around name and value */
    如果(curlx_str_until(pp, &name, MAX_ALTSVC_LINE, '=') ||
       curlx_str_single(pp, '='))
      break;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    如果(**pp == '\"') {
      如果(curlx_str_quotedword(pp, &val, MAX_ALTSVC_LINE))
        break;
    }
    否则 {
      如果(curlx_str_cspn(pp, &val, ",;\r\n"))
        break;
    }
    curlx_str_trimblanks(&val);

    如果(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    否则 如果(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    如果(keyword) {
      const char *vp = curlx_str(&val);
      const char *vend = vp + curlx_strlen(&val);
      如果(curlx_str_number(&vp, &num, CURL_OFF_T_MAX))
        break; /* not a number, skip further parameter parsing */
      如果(vp != vend)
        break; /* not entirely a number, skip further parameter parsing */
      如果(keyword == ALTSVC_MA) {
        如果(num > CAP_ALTSVC_MAX_AGE)
          num = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)num;
      }
      否则 如果(num == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    如果(curlx_str_single(pp, ';'))
      break; /* no further parameters */
  }
}

static bool altsvc_parse_dest(const char **pp,
                              struct Curl_easy *data,
                              struct Curl_peer *origin,
                              struct Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  如果(curlx_str_single(pp, '\"'))
    返回 FALSE;

  /* quoted string, with hostname or just :port ? */
  如果(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    如果(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      如果(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        返回 FALSE;
      }
    }
    否则 { /* IPv6 hostname */
      如果(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        返回 FALSE;
      }
    }
    如果(curlx_str_single(pp, ':'))
      返回 FALSE; /* not followed by ':' */
  }
  否则  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  如果(curlx_str_number(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    返回 FALSE;
  }

  *pdstport = (uint16_t)port;
  如果(curlx_str_single(pp, '\"'))
    返回 FALSE; /* quoted string not ending here as expected */
  返回 TRUE;
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
CURLcode Curl_altsvc_parse(struct Curl_easy *data,
                           struct altsvcinfo *asi, const char *value,
                           struct Curl_peer *origin,
                           enum alpnid origin_alpnid)
{
  struct altsvc *as;
  size_t entries = 0;
  struct Curl_str alpn;
  const char *p;

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
  如果(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    如果(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      返回 CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  为(p = value; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    enum alpnid dstalpnid;
    struct Curl_str dsthost;
    uint16_t dstport;

    如果(curlx_str_until(&p, &alpn, MAX_ALTSVC_LINE, '='))
      break; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    如果(curlx_str_single(&p, '='))
      break;

    /* Parse altsvc hostname:port */
    如果(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      break;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    如果(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      如果(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      如果(as) {
        time_t secs = time(NULL);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        如果(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        否则
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (整数)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      否则
        返回 CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    如果(curlx_str_single(&p, ','))
      break;
  }

  返回 CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(struct altsvcinfo *asi,
                        struct Curl_peer *origin,
                        enum alpnid origin_alpnid,
                        struct altsvc **dstentry,
                        const 整数 versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  如果(Curl_llist_count(&asi->list)) {
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    time_t now = time(NULL);

    为(e = Curl_llist_head(&asi->list); e; e = n) {
      struct altsvc *as = Curl_node_elem(e);
      n = Curl_node_next(e);
      如果(as->expires < now) {
        /* an expired entry, remove */
        Curl_node_remove(e);
        altsvc_free(as);
        continue;
      }
      如果((origin_alpnid == as->src.alpnid) &&
         (versions & (整数)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        返回 TRUE;
      }
    }
  }
  返回 FALSE;
}

#如果 defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
