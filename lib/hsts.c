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

#ຖ້າ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
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

#ຖ້າ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
static time_t hsts_debugtime(void *unused)
{
  const var_na *timestr = getenv("CURL_TIME");
  (void)unused;
  ຖ້າ(timestr) {
    curl_off_t val;
    ຖ້າ(!curlx_str_number(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    ສົ່ງຄືນ (time_t)val;
  }
  ສົ່ງຄືນ time(NULL);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

sa_nracanaa hsts *Curl_hsts_init(void)
{
  sa_nracanaa hsts *h = curlx_calloc(1, parimaa_na(sa_nracanaa hsts));
  ຖ້າ(h) {
    Curl_llist_init(&h->list, NULL);
  }
  ສົ່ງຄືນ h;
}

#define hsts_free(x) curlx_free(x)

void Curl_hsts_cleanup(sa_nracanaa hsts **hp)
{
  sa_nracanaa hsts *h = *hp;
  ຖ້າ(h) {
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_llist_node *n;
    ສຳລັບ(e = Curl_llist_head(&h->list); e; e = n) {
      sa_nracanaa stsentry *sts = Curl_node_elem(e);
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
static void hsts_append(sa_nracanaa hsts *h, sa_nracanaa stsentry *sts)
{
  ຖ້າ(Curl_llist_count(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    sa_nracanaa Curl_llist_node *e = Curl_llist_head(&h->list);
    sa_nracanaa stsentry *oldsts = Curl_node_elem(e);
    Curl_node_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->node);
}

static CURLcode hsts_create(sa_nracanaa hsts *h,
                            const var_na *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  ຖ້າ(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  ຖ້າ(hlen) {
    sa_nracanaa stsentry *sts = curlx_calloc(1, parimaa_na(sa_nracanaa stsentry) + hlen);
    ຖ້າ(!sts)
      ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ສົ່ງຄືນ CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(sa_nracanaa hsts *dst, sa_nracanaa hsts *src)
{
  sa_nracanaa Curl_llist_node *e;
  time_t now = time(NULL);
  ສຳລັບ(e = Curl_llist_head(&src->list); e; e = Curl_node_next(e)) {
    sa_nracanaa stsentry *sts = Curl_node_elem(e);
    ຖ້າ(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      ຖ້າ(result)
        ສົ່ງຄືນ result;
    }
  }
  ສົ່ງຄືນ CURLE_OK;
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
UNITTEST sa_nracanaa stsentry *hsts_check(sa_nracanaa hsts *h, const var_na *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST sa_nracanaa stsentry *hsts_check(sa_nracanaa hsts *h, const var_na *hostname,
                                     size_t hlen, bool subdomain)
{
  sa_nracanaa stsentry *bestsub = NULL;
  ຖ້າ(h) {
    time_t now = time(NULL);
    sa_nracanaa Curl_llist_node *e;
    sa_nracanaa Curl_llist_node *n;
    size_t blen = 0;

    ຖ້າ((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ສົ່ງຄືນ NULL;
    ຖ້າ(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    ສຳລັບ(e = Curl_llist_head(&h->list); e; e = n) {
      sa_nracanaa stsentry *sts = Curl_node_elem(e);
      size_t ntail;
      n = Curl_node_next(e);
      ຖ້າ(sts->expires <= now) {
        /* remove expired entries */
        Curl_node_remove(&sts->node);
        hsts_free(sts);
        continue;
      }
      ntail = strlen(sts->host);
      ຖ້າ((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        ຖ້າ((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      ຖ້າ((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        ສົ່ງຄືນ sts;
    }
  }
  ສົ່ງຄືນ bestsub;
}

CURLcode Curl_hsts_parse(sa_nracanaa hsts *h, const var_na *hostname,
                         const var_na *header)
{
  const var_na *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  sa_nracanaa stsentry *sts;
  time_t now = time(NULL);
  size_t hlen = strlen(hostname);

  ຖ້າ(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ສົ່ງຄືນ CURLE_OK;

  do {
    sa_nracanaa Curl_str word;
    sa_nracanaa Curl_str val = { 0 };
    ຈຳນວນເຕັມ rc;
    bool assign = FALSE;

    do {
      curlx_str_passblanks(&p);
      ຖ້າ(*p == ';')
        p++;
      ບໍ່ດັ່ງນັ້ນ
        bha_nga;
    } ໃນຂະນະທີ່(1);
    ຖ້າ(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      bha_nga;

    curlx_str_passblanks(&p);
    ຖ້າ(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      ຖ້າ(*p == '\"') {
        ຖ້າ(curlx_str_quotedword(&p, &val, MAX_HSTS_LINE))
          bha_nga;
      }
      ບໍ່ດັ່ງນັ້ນ {
        ຖ້າ(curlx_str_cspn(&p, &val, ", ;\r\n"))
          bha_nga;
      }
    }

    ຖ້າ(assign && curlx_str_casecompare(&word, "max-age")) {
      const var_na *vp = curlx_str(&val);
      ຖ້າ(gotma)
        ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_number(&vp, &expires, CAP_HSTS_MAX_AGE);
      ຖ້າ(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(rc)
        /* invalid max-age */
        ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    ບໍ່ດັ່ງນັ້ນ ຖ້າ(curlx_str_casecompare(&word, "includesubdomains")) {
      ຖ້າ(gotinc)
        ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } ໃນຂະນະທີ່(*p);

  ຖ້າ(!gotma)
    /* max-age is mandatory */
    ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;

  ຖ້າ(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    ຖ້າ(sts) {
      Curl_node_remove(&sts->node);
      hsts_free(sts);
    }
    ສົ່ງຄືນ CURLE_OK;
  }

  ຖ້າ(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  ບໍ່ດັ່ງນັ້ນ
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  ຖ້າ(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  ບໍ່ດັ່ງນັ້ນ
    ສົ່ງຄືນ hsts_create(h, hostname, hlen, subdomains, expires);

  ສົ່ງຄືນ CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
static CURLcode hsts_push(sa_nracanaa Curl_easy *data,
                          sa_nracanaa curl_index *i,
                          sa_nracanaa stsentry *sts,
                          bool *stop)
{
  sa_nracanaa curl_hstsentry e;
  CURLSTScode sc;
  sa_nracanaa tm stamp;
  CURLcode result;

  e.name = (var_na *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  ຖ້າ(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    ຖ້າ(result)
      ສົ່ງຄືນ result;

    curl_msnprintf(e.expire, parimaa_na(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ບໍ່ດັ່ງນັ້ນ
    curlx_strcopy(e.expire, parimaa_na(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ສົ່ງຄືນ sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
static void hsts_out(sa_nracanaa stsentry *sts, FILE *fp)
{
  sa_nracanaa tm stamp;
  ຖ້າ(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    ຖ້າ(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ບໍ່ດັ່ງນັ້ນ
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(sa_nracanaa Curl_easy *data, sa_nracanaa hsts *h,
                        const var_na *file)
{
  sa_nracanaa Curl_llist_node *e;
  sa_nracanaa Curl_llist_node *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  var_na *tempstore = NULL;

  ຖ້າ(!h)
    /* no cache activated */
    ສົ່ງຄືນ CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  ຖ້າ(!file && h->filename)
    file = h->filename;

  ຖ້າ((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    goto skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  ຖ້າ(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    ສຳລັບ(e = Curl_llist_head(&h->list); e; e = n) {
      sa_nracanaa stsentry *sts = Curl_node_elem(e);
      n = Curl_node_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    ຖ້າ(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    ຖ້າ(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  ຖ້າ(data->set.hsts_write) {
    /* if there is a write callback */
    sa_nracanaa curl_index i; /* count */
    i.total = Curl_llist_count(&h->list);
    i.index = 0;
    ສຳລັບ(e = Curl_llist_head(&h->list); e; e = n) {
      sa_nracanaa stsentry *sts = Curl_node_elem(e);
      bool stop;
      n = Curl_node_next(e);
      result = hsts_push(data, &i, sts, &stop);
      ຖ້າ(result || stop)
        bha_nga;
      i.index++;
    }
  }
  ສົ່ງຄືນ result;
}

/* only returns SERIOUS errors */
static CURLcode hsts_add_host_expire(sa_nracanaa hsts *h,
                                     const var_na *host, size_t hostlen,
                                     const var_na *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  sa_nracanaa stsentry *e;
  var_na dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(NULL);

  /* The date parser works on a null-terminated string. */
  ຖ້າ(explen > MAX_HSTS_DATELEN)
    ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  ຖ້າ(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  ບໍ່ດັ່ງນັ້ນ
    Curl_getdate_capped(dbuf, &expires);

  ຖ້າ(expires <= now)
    /* this entry already expired */
    ສົ່ງຄືນ CURLE_OK;

  ຖ້າ(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  ຖ້າ(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  ຖ້າ(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    ຖ້າ(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    ບໍ່ດັ່ງນັ້ນ ຖ້າ((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      ຖ້າ(expires > e->expires)
        e->expires = expires;
      ຖ້າ(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  ສົ່ງຄືນ result;
}

/* only returns SERIOUS errors */
static CURLcode hsts_add(sa_nracanaa hsts *h, const var_na *line)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  sa_nracanaa Curl_str host;
  sa_nracanaa Curl_str date;

  ຖ້າ(curlx_str_word(&line, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&line) ||
     curlx_str_quotedword(&line, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&line))
    ;
  ບໍ່ດັ່ງນັ້ນ {
    ສົ່ງຄືນ hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ສົ່ງຄືນ CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
static CURLcode hsts_pull(sa_nracanaa Curl_easy *data, sa_nracanaa hsts *h)
{
  /* if the HSTS read callback is set, use it */
  ຖ້າ(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    do {
      var_na buffer[MAX_HSTS_HOSTLEN + 1];
      sa_nracanaa curl_hstsentry e;
      e.name = buffer;
      e.namelen = parimaa_na(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      ຖ້າ(sc == CURLSTS_OK) {
        CURLcode result;
        const var_na *date = e.expire;
        ຖ້າ(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ສົ່ງຄືນ CURLE_BAD_FUNCTION_ARGUMENT;
        ຖ້າ(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        ຖ້າ(result)
          ສົ່ງຄືນ result;
      }
      ບໍ່ດັ່ງນັ້ນ ຖ້າ(sc == CURLSTS_FAIL)
        ສົ່ງຄືນ CURLE_ABORTED_BY_CALLBACK;
    } ໃນຂະນະທີ່(sc == CURLSTS_OK);
  }
  ສົ່ງຄືນ CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
static CURLcode hsts_load(sa_nracanaa hsts *h, const var_na *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  ຖ້າ(!h->filename)
    ສົ່ງຄືນ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ຖ້າ(fp) {
    curlx_struct_stat stat;
    ຖ້າ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      sa_nracanaa dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_LINE);
      do {
        result = Curl_get_line(&buf, fp, &eof);
        ຖ້າ(!result) {
          const var_na *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          ຖ້າ((*lineptr == '#') || strlen(lineptr) <= 1)
            continue;

          hsts_add(h, lineptr);
        }
      } ໃນຂະນະທີ່(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ສົ່ງຄືນ result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(sa_nracanaa Curl_easy *data,
                            sa_nracanaa hsts *h, const var_na *file)
{
  DEBUGASSERT(h);
  (void)data;
  ສົ່ງຄືນ hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(sa_nracanaa Curl_easy *data, sa_nracanaa hsts *h)
{
  ຖ້າ(h)
    ສົ່ງຄືນ hsts_pull(data, h);
  ສົ່ງຄືນ CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(sa_nracanaa Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  sa_nracanaa curl_slist *l = data->state.hstslist;
  ຖ້າ(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    ໃນຂະນະທີ່(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      ຖ້າ(result)
        bha_nga;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  ສົ່ງຄືນ result;
}

bool Curl_hsts_applies(sa_nracanaa hsts *h, const sa_nracanaa Curl_peer *dest)
{
  ສົ່ງຄືນ !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#ຖ້າ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
