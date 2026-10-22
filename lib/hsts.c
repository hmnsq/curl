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
 * The Strict-Transport-Security header is defined in RFC 6797:
 * https://datatracker.ietf.org/doc/html/rfc6797
 */
#include "curl_setup.h"

#如果 !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#include "urldata.h"
#include "llist.h"
#include "hsts.h"
#include "curl_fopen.h"
#include "curl_get_line.h"
#include "parsedate.h"
#include "curl_share.h"
#include "curlx/strparse.h"
#include "curlx/strcopy.h"

#define MAX_HSTS_LINE    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#如果 defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
static time_t hsts_debugtime(void *unused)
{
  const char *timestr = getenv("CURL_TIME");
  (void)unused;
  如果(timestr) {
    curl_off_t val;
    如果(!curlx_str_number(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    返回 (time_t)val;
  }
  返回 time(NULL);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

struct hsts *Curl_hsts_init(void)
{
  struct hsts *h = curlx_calloc(1, sizeof(struct hsts));
  如果(h) {
    Curl_llist_init(&h->list, NULL);
  }
  返回 h;
}

#define hsts_free(x) curlx_free(x)

void Curl_hsts_cleanup(struct hsts **hp)
{
  struct hsts *h = *hp;
  如果(h) {
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    为(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      n = Curl_node_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = NULL;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
static void hsts_append(struct hsts *h, struct stsentry *sts)
{
  如果(Curl_llist_count(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    struct Curl_llist_node *e = Curl_llist_head(&h->list);
    struct stsentry *oldsts = Curl_node_elem(e);
    Curl_node_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->node);
}

static CURLcode hsts_create(struct hsts *h,
                            const char *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  如果(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  如果(hlen) {
    struct stsentry *sts = curlx_calloc(1, sizeof(struct stsentry) + hlen);
    如果(!sts)
      返回 CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  返回 CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(struct hsts *dst, struct hsts *src)
{
  struct Curl_llist_node *e;
  time_t now = time(NULL);
  为(e = Curl_llist_head(&src->list); e; e = Curl_node_next(e)) {
    struct stsentry *sts = Curl_node_elem(e);
    如果(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      如果(result)
        返回 result;
    }
  }
  返回 CURLE_OK;
}

/*
 * Return the matching HSTS entry, or NULL if the given hostname is not
 * currently an HSTS one.
 *
 * The 'subdomain' argument tells the function if subdomain matching should be
 * attempted.
 *
 * @unittest 1660
 */
UNITTEST struct stsentry *hsts_check(struct hsts *h, const char *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST struct stsentry *hsts_check(struct hsts *h, const char *hostname,
                                     size_t hlen, bool subdomain)
{
  struct stsentry *bestsub = NULL;
  如果(h) {
    time_t now = time(NULL);
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    size_t blen = 0;

    如果((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      返回 NULL;
    如果(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    为(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      size_t ntail;
      n = Curl_node_next(e);
      如果(sts->expires <= now) {
        /* remove expired entries */
        Curl_node_remove(&sts->node);
        hsts_free(sts);
        continue;
      }
      ntail = strlen(sts->host);
      如果((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        如果((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      如果((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        返回 sts;
    }
  }
  返回 bestsub;
}

CURLcode Curl_hsts_parse(struct hsts *h, const char *hostname,
                         const char *header)
{
  const char *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  struct stsentry *sts;
  time_t now = time(NULL);
  size_t hlen = strlen(hostname);

  如果(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    返回 CURLE_OK;

  do {
    struct Curl_str word;
    struct Curl_str val = { 0 };
    整数 rc;
    bool assign = FALSE;

    do {
      curlx_str_passblanks(&p);
      如果(*p == ';')
        p++;
      否则
        break;
    } 当(1);
    如果(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      break;

    curlx_str_passblanks(&p);
    如果(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      如果(*p == '\"') {
        如果(curlx_str_quotedword(&p, &val, MAX_HSTS_LINE))
          break;
      }
      否则 {
        如果(curlx_str_cspn(&p, &val, ", ;\r\n"))
          break;
      }
    }

    如果(assign && curlx_str_casecompare(&word, "max-age")) {
      const char *vp = curlx_str(&val);
      如果(gotma)
        返回 CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_number(&vp, &expires, CAP_HSTS_MAX_AGE);
      如果(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      否则 如果(rc)
        /* invalid max-age */
        返回 CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    否则 如果(curlx_str_casecompare(&word, "includesubdomains")) {
      如果(gotinc)
        返回 CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } 当(*p);

  如果(!gotma)
    /* max-age is mandatory */
    返回 CURLE_BAD_FUNCTION_ARGUMENT;

  如果(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    如果(sts) {
      Curl_node_remove(&sts->node);
      hsts_free(sts);
    }
    返回 CURLE_OK;
  }

  如果(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  否则
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  如果(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  否则
    返回 hsts_create(h, hostname, hlen, subdomains, expires);

  返回 CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
static CURLcode hsts_push(struct Curl_easy *data,
                          struct curl_index *i,
                          struct stsentry *sts,
                          bool *stop)
{
  struct curl_hstsentry e;
  CURLSTScode sc;
  struct tm stamp;
  CURLcode result;

  e.name = (char *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  如果(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    如果(result)
      返回 result;

    curl_msnprintf(e.expire, sizeof(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  否则
    curlx_strcopy(e.expire, sizeof(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  返回 sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
static void hsts_out(struct stsentry *sts, FILE *fp)
{
  struct tm stamp;
  如果(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    如果(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  否则
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(struct Curl_easy *data, struct hsts *h,
                        const char *file)
{
  struct Curl_llist_node *e;
  struct Curl_llist_node *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  char *tempstore = NULL;

  如果(!h)
    /* no cache activated */
    返回 CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  如果(!file && h->filename)
    file = h->filename;

  如果((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    goto skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  如果(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    为(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      n = Curl_node_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    如果(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    如果(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  如果(data->set.hsts_write) {
    /* if there is a write callback */
    struct curl_index i; /* count */
    i.total = Curl_llist_count(&h->list);
    i.index = 0;
    为(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      bool stop;
      n = Curl_node_next(e);
      result = hsts_push(data, &i, sts, &stop);
      如果(result || stop)
        break;
      i.index++;
    }
  }
  返回 result;
}

/* only returns SERIOUS errors */
static CURLcode hsts_add_host_expire(struct hsts *h,
                                     const char *host, size_t hostlen,
                                     const char *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  struct stsentry *e;
  char dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(NULL);

  /* The date parser works on a null-terminated string. */
  如果(explen > MAX_HSTS_DATELEN)
    返回 CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  如果(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  否则
    Curl_getdate_capped(dbuf, &expires);

  如果(expires <= now)
    /* this entry already expired */
    返回 CURLE_OK;

  如果(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  如果(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  如果(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    如果(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    否则 如果((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      如果(expires > e->expires)
        e->expires = expires;
      如果(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  返回 result;
}

/* only returns SERIOUS errors */
static CURLcode hsts_add(struct hsts *h, const char *line)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  struct Curl_str host;
  struct Curl_str date;

  如果(curlx_str_word(&line, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_quotedword(&line, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&line))
    ;
  否则 {
    返回 hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  返回 CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
static CURLcode hsts_pull(struct Curl_easy *data, struct hsts *h)
{
  /* if the HSTS read callback is set, use it */
  如果(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    do {
      char buffer[MAX_HSTS_HOSTLEN + 1];
      struct curl_hstsentry e;
      e.name = buffer;
      e.namelen = sizeof(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      如果(sc == CURLSTS_OK) {
        CURLcode result;
        const char *date = e.expire;
        如果(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          返回 CURLE_BAD_FUNCTION_ARGUMENT;
        如果(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        如果(result)
          返回 result;
      }
      否则 如果(sc == CURLSTS_FAIL)
        返回 CURLE_ABORTED_BY_CALLBACK;
    } 当(sc == CURLSTS_OK);
  }
  返回 CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
static CURLcode hsts_load(struct hsts *h, const char *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  如果(!h->filename)
    返回 CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  如果(fp) {
    curlx_struct_stat stat;
    如果((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      struct dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_LINE);
      do {
        result = Curl_get_line(&buf, fp, &eof);
        如果(!result) {
          const char *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          如果((*lineptr == '#') || strlen(lineptr) <= 1)
            continue;

          hsts_add(h, lineptr);
        }
      } 当(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  返回 result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(struct Curl_easy *data,
                            struct hsts *h, const char *file)
{
  DEBUGASSERT(h);
  (void)data;
  返回 hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(struct Curl_easy *data, struct hsts *h)
{
  如果(h)
    返回 hsts_pull(data, h);
  返回 CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  struct curl_slist *l = data->state.hstslist;
  如果(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    当(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      如果(result)
        break;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  返回 result;
}

bool Curl_hsts_applies(struct hsts *h, const struct Curl_peer *dest)
{
  返回 !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#如果 defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
