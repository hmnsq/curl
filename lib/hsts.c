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

#если !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
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

#если defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
static time_t hsts_debugtime(void *unused)
{
  const char *timestr = getenv("CURL_TIME");
  (void)unused;
  если(timestr) {
    curl_off_t val;
    если(!curlx_str_number(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    вернуть (time_t)val;
  }
  вернуть time(NULL);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

struct hsts *Curl_hsts_init(void)
{
  struct hsts *h = curlx_calloc(1, sizeof(struct hsts));
  если(h) {
    Curl_llist_init(&h->list, NULL);
  }
  вернуть h;
}

#define hsts_free(x) curlx_free(x)

void Curl_hsts_cleanup(struct hsts **hp)
{
  struct hsts *h = *hp;
  если(h) {
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    для(e = Curl_llist_head(&h->list); e; e = n) {
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
  если(Curl_llist_count(&h->list) == MAX_HSTS_ENTRIES) {
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

  если(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  если(hlen) {
    struct stsentry *sts = curlx_calloc(1, sizeof(struct stsentry) + hlen);
    если(!sts)
      вернуть CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  вернуть CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(struct hsts *dst, struct hsts *src)
{
  struct Curl_llist_node *e;
  time_t now = time(NULL);
  для(e = Curl_llist_head(&src->list); e; e = Curl_node_next(e)) {
    struct stsentry *sts = Curl_node_elem(e);
    если(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      если(result)
        вернуть result;
    }
  }
  вернуть CURLE_OK;
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
  если(h) {
    time_t now = time(NULL);
    struct Curl_llist_node *e;
    struct Curl_llist_node *n;
    size_t blen = 0;

    если((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      вернуть NULL;
    если(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    для(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      size_t ntail;
      n = Curl_node_next(e);
      если(sts->expires <= now) {
        /* remove expired entries */
        Curl_node_remove(&sts->node);
        hsts_free(sts);
        continue;
      }
      ntail = strlen(sts->host);
      если((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        если((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      если((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        вернуть sts;
    }
  }
  вернуть bestsub;
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

  если(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    вернуть CURLE_OK;

  do {
    struct Curl_str word;
    struct Curl_str val = { 0 };
    целое rc;
    bool assign = FALSE;

    do {
      curlx_str_passblanks(&p);
      если(*p == ';')
        p++;
      иначе
        break;
    } пока(1);
    если(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      break;

    curlx_str_passblanks(&p);
    если(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      если(*p == '\"') {
        если(curlx_str_quotedword(&p, &val, MAX_HSTS_LINE))
          break;
      }
      иначе {
        если(curlx_str_cspn(&p, &val, ", ;\r\n"))
          break;
      }
    }

    если(assign && curlx_str_casecompare(&word, "max-age")) {
      const char *vp = curlx_str(&val);
      если(gotma)
        вернуть CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_number(&vp, &expires, CAP_HSTS_MAX_AGE);
      если(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      иначе если(rc)
        /* invalid max-age */
        вернуть CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    иначе если(curlx_str_casecompare(&word, "includesubdomains")) {
      если(gotinc)
        вернуть CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } пока(*p);

  если(!gotma)
    /* max-age is mandatory */
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;

  если(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    если(sts) {
      Curl_node_remove(&sts->node);
      hsts_free(sts);
    }
    вернуть CURLE_OK;
  }

  если(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  иначе
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  если(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  иначе
    вернуть hsts_create(h, hostname, hlen, subdomains, expires);

  вернуть CURLE_OK;
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

  если(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    если(result)
      вернуть result;

    curl_msnprintf(e.expire, sizeof(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  иначе
    curlx_strcopy(e.expire, sizeof(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  вернуть sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
static void hsts_out(struct stsentry *sts, FILE *fp)
{
  struct tm stamp;
  если(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    если(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  иначе
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

  если(!h)
    /* no cache activated */
    вернуть CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  если(!file && h->filename)
    file = h->filename;

  если((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    goto skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  если(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    для(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      n = Curl_node_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    если(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    если(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  если(data->set.hsts_write) {
    /* if there is a write callback */
    struct curl_index i; /* count */
    i.total = Curl_llist_count(&h->list);
    i.index = 0;
    для(e = Curl_llist_head(&h->list); e; e = n) {
      struct stsentry *sts = Curl_node_elem(e);
      bool stop;
      n = Curl_node_next(e);
      result = hsts_push(data, &i, sts, &stop);
      если(result || stop)
        break;
      i.index++;
    }
  }
  вернуть result;
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
  если(explen > MAX_HSTS_DATELEN)
    вернуть CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  если(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  иначе
    Curl_getdate_capped(dbuf, &expires);

  если(expires <= now)
    /* this entry already expired */
    вернуть CURLE_OK;

  если(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  если(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  если(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    если(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    иначе если((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      если(expires > e->expires)
        e->expires = expires;
      если(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  вернуть result;
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

  если(curlx_str_word(&line, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_quotedword(&line, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&line))
    ;
  иначе {
    вернуть hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  вернуть CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
static CURLcode hsts_pull(struct Curl_easy *data, struct hsts *h)
{
  /* if the HSTS read callback is set, use it */
  если(data->set.hsts_read) {
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
      если(sc == CURLSTS_OK) {
        CURLcode result;
        const char *date = e.expire;
        если(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          вернуть CURLE_BAD_FUNCTION_ARGUMENT;
        если(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        если(result)
          вернуть result;
      }
      иначе если(sc == CURLSTS_FAIL)
        вернуть CURLE_ABORTED_BY_CALLBACK;
    } пока(sc == CURLSTS_OK);
  }
  вернуть CURLE_OK;
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
  если(!h->filename)
    вернуть CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  если(fp) {
    curlx_struct_stat stat;
    если((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      struct dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_LINE);
      do {
        result = Curl_get_line(&buf, fp, &eof);
        если(!result) {
          const char *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          если((*lineptr == '#') || strlen(lineptr) <= 1)
            continue;

          hsts_add(h, lineptr);
        }
      } пока(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  вернуть result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(struct Curl_easy *data,
                            struct hsts *h, const char *file)
{
  DEBUGASSERT(h);
  (void)data;
  вернуть hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(struct Curl_easy *data, struct hsts *h)
{
  если(h)
    вернуть hsts_pull(data, h);
  вернуть CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(struct Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  struct curl_slist *l = data->state.hstslist;
  если(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    пока(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      если(result)
        break;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  вернуть result;
}

bool Curl_hsts_applies(struct hsts *h, const struct Curl_peer *dest)
{
  вернуть !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#если defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
