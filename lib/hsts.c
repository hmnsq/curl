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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "hsts.h"
#ଅନ୍ତର୍ଭାଵ "curl_fopen.h"
#ଅନ୍ତର୍ଭାଵ "curl_get_line.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

#define MAX_HSTS_ଧାଡ଼ି    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#ଯଦି defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
ସ୍ଥିର time_t hsts_debugtime(ଶୂନ୍ୟ *unused)
{
  ଅଚଳ ଅକ୍ଷର *timestr = getenv("CURL_TIME");
  (ଶୂନ୍ୟ)unused;
  ଯଦି(timestr) {
    curl_off_t val;
    ଯଦି(!curlx_str_ସଂଖ୍ୟା(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    ଫେରନ୍ତୁ (time_t)val;
  }
  ଫେରନ୍ତୁ time(ଖାଲି);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

ସଂରଚନା hsts *Curl_hsts_init(ଶୂନ୍ୟ)
{
  ସଂରଚନା hsts *h = curlx_calloc(1, ଆକାର(ସଂରଚନା hsts));
  ଯଦି(h) {
    Curl_llist_init(&h->list, ଖାଲି);
  }
  ଫେରନ୍ତୁ h;
}

#define hsts_free(x) curlx_free(x)

ଶୂନ୍ୟ Curl_hsts_cleanup(ସଂରଚନା hsts **hp)
{
  ସଂରଚନା hsts *h = *hp;
  ଯଦି(h) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
    ପାଇଁ(e = Curl_llist_head(&h->list); e; e = n) {
      ସଂରଚନା stsentry *sts = Curl_ଗଣ୍ଠି_elem(e);
      n = Curl_ଗଣ୍ଠି_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ଖାଲି;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
ସ୍ଥିର ଶୂନ୍ୟ hsts_append(ସଂରଚନା hsts *h, ସଂରଚନା stsentry *sts)
{
  ଯଦି(Curl_llist_ଗଣନା(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e = Curl_llist_head(&h->list);
    ସଂରଚନା stsentry *oldsts = Curl_ଗଣ୍ଠି_elem(e);
    Curl_ଗଣ୍ଠି_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->ଗଣ୍ଠି);
}

ସ୍ଥିର CURLcode hsts_create(ସଂରଚନା hsts *h,
                            ଅଚଳ ଅକ୍ଷର *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  ଯଦି(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  ଯଦି(hlen) {
    ସଂରଚନା stsentry *sts = curlx_calloc(1, ଆକାର(ସଂରଚନା stsentry) + hlen);
    ଯଦି(!sts)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(ସଂରଚନା hsts *dst, ସଂରଚନା hsts *src)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  time_t now = time(ଖାଲି);
  ପାଇଁ(e = Curl_llist_head(&src->list); e; e = Curl_ଗଣ୍ଠି_next(e)) {
    ସଂରଚନା stsentry *sts = Curl_ଗଣ୍ଠି_elem(e);
    ଯଦି(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      ଯଦି(result)
        ଫେରନ୍ତୁ result;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
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
UNITTEST ସଂରଚନା stsentry *hsts_check(ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST ସଂରଚନା stsentry *hsts_check(ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *hostname,
                                     size_t hlen, bool subdomain)
{
  ସଂରଚନା stsentry *bestsub = ଖାଲି;
  ଯଦି(h) {
    time_t now = time(ଖାଲି);
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
    size_t blen = 0;

    ଯଦି((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    ପାଇଁ(e = Curl_llist_head(&h->list); e; e = n) {
      ସଂରଚନା stsentry *sts = Curl_ଗଣ୍ଠି_elem(e);
      size_t ntail;
      n = Curl_ଗଣ୍ଠି_next(e);
      ଯଦି(sts->expires <= now) {
        /* remove expired entries */
        Curl_ଗଣ୍ଠି_remove(&sts->ଗଣ୍ଠି);
        hsts_free(sts);
        ଜାରି;
      }
      ntail = strlen(sts->host);
      ଯଦି((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        ଯଦି((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      ଯଦି((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        ଫେରନ୍ତୁ sts;
    }
  }
  ଫେରନ୍ତୁ bestsub;
}

CURLcode Curl_hsts_parse(ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *hostname,
                         ଅଚଳ ଅକ୍ଷର *header)
{
  ଅଚଳ ଅକ୍ଷର *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  ସଂରଚନା stsentry *sts;
  time_t now = time(ଖାଲି);
  size_t hlen = strlen(hostname);

  ଯଦି(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    ଫେରନ୍ତୁ CURLE_OK;

  କର {
    ସଂରଚନା Curl_str word;
    ସଂରଚନା Curl_str val = { 0 };
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    bool assign = FALSE;

    କର {
      curlx_str_passblanks(&p);
      ଯଦି(*p == ';')
        p++;
      ଅନ୍ୟଥା
        ଭାଙ୍ଗ;
    } ଯେତେବେଳେ(1);
    ଯଦି(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      ଭାଙ୍ଗ;

    curlx_str_passblanks(&p);
    ଯଦି(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      ଯଦି(*p == '\"') {
        ଯଦି(curlx_str_quotedword(&p, &val, MAX_HSTS_ଧାଡ଼ି))
          ଭାଙ୍ଗ;
      }
      ଅନ୍ୟଥା {
        ଯଦି(curlx_str_cspn(&p, &val, ", ;\r\n"))
          ଭାଙ୍ଗ;
      }
    }

    ଯଦି(assign && curlx_str_casecompare(&word, "max-age")) {
      ଅଚଳ ଅକ୍ଷର *vp = curlx_str(&val);
      ଯଦି(gotma)
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_ସଂଖ୍ୟା(&vp, &expires, CAP_HSTS_MAX_AGE);
      ଯଦି(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      ଅନ୍ୟଥା ଯଦି(rc)
        /* invalid max-age */
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&word, "includesubdomains")) {
      ଯଦି(gotinc)
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } ଯେତେବେଳେ(*p);

  ଯଦି(!gotma)
    /* max-age is mandatory */
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    ଯଦି(sts) {
      Curl_ଗଣ୍ଠି_remove(&sts->ଗଣ୍ଠି);
      hsts_free(sts);
    }
    ଫେରନ୍ତୁ CURLE_OK;
  }

  ଯଦି(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  ଅନ୍ୟଥା
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  ଯଦି(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  ଅନ୍ୟଥା
    ଫେରନ୍ତୁ hsts_create(h, hostname, hlen, subdomains, expires);

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
ସ୍ଥିର CURLcode hsts_ଠେଲ(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା curl_index *i,
                          ସଂରଚନା stsentry *sts,
                          bool *stop)
{
  ସଂରଚନା curl_hstsentry e;
  CURLSTScode sc;
  ସଂରଚନା tm stamp;
  CURLcode result;

  e.name = (ଅକ୍ଷର *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  ଯଦି(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    ଯଦି(result)
      ଫେରନ୍ତୁ result;

    curl_msnprintf(e.expire, ଆକାର(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ଅନ୍ୟଥା
    curlx_strcopy(e.expire, ଆକାର(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  ଫେରନ୍ତୁ sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
ସ୍ଥିର ଶୂନ୍ୟ hsts_out(ସଂରଚନା stsentry *sts, FILE *fp)
{
  ସଂରଚନା tm stamp;
  ଯଦି(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    ଯଦି(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  ଅନ୍ୟଥା
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(ସଂରଚନା Curl_easy *data, ସଂରଚନା hsts *h,
                        ଅଚଳ ଅକ୍ଷର *file)
{
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  ଅକ୍ଷର *tempstore = ଖାଲି;

  ଯଦି(!h)
    /* no cache activated */
    ଫେରନ୍ତୁ CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  ଯଦି(!file && h->filename)
    file = h->filename;

  ଯଦି((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ଯାଅ skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  ଯଦି(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    ପାଇଁ(e = Curl_llist_head(&h->list); e; e = n) {
      ସଂରଚନା stsentry *sts = Curl_ଗଣ୍ଠି_elem(e);
      n = Curl_ଗଣ୍ଠି_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    ଯଦି(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    ଯଦି(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  ଯଦି(data->set.hsts_write) {
    /* if there is a write callback */
    ସଂରଚନା curl_index i; /*ଗଣନା*/
    i.total = Curl_llist_ଗଣନା(&h->list);
    i.index = 0;
    ପାଇଁ(e = Curl_llist_head(&h->list); e; e = n) {
      ସଂରଚନା stsentry *sts = Curl_ଗଣ୍ଠି_elem(e);
      bool stop;
      n = Curl_ଗଣ୍ଠି_next(e);
      result = hsts_ଠେଲ(data, &i, sts, &stop);
      ଯଦି(result || stop)
        ଭାଙ୍ଗ;
      i.index++;
    }
  }
  ଫେରନ୍ତୁ result;
}

/* only returns SERIOUS errors */
ସ୍ଥିର CURLcode hsts_add_host_expire(ସଂରଚନା hsts *h,
                                     ଅଚଳ ଅକ୍ଷର *host, size_t hostlen,
                                     ଅଚଳ ଅକ୍ଷର *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା stsentry *e;
  ଅକ୍ଷର dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(ଖାଲି);

  /* The date parser works on a null-terminated string. */
  ଯଦି(explen > MAX_HSTS_DATELEN)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  ଯଦି(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  ଅନ୍ୟଥା
    Curl_getdate_capped(dbuf, &expires);

  ଯଦି(expires <= now)
    /* this entry already expired */
    ଫେରନ୍ତୁ CURLE_OK;

  ଯଦି(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  ଯଦି(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  ଯଦି(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    ଯଦି(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    ଅନ୍ୟଥା ଯଦି((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      ଯଦି(expires > e->expires)
        e->expires = expires;
      ଯଦି(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  ଫେରନ୍ତୁ result;
}

/* only returns SERIOUS errors */
ସ୍ଥିର CURLcode hsts_add(ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *ଧାଡ଼ି)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  ସଂରଚନା Curl_str host;
  ସଂରଚନା Curl_str date;

  ଯଦି(curlx_str_word(&ଧାଡ଼ି, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&ଧାଡ଼ି) ||
     curlx_str_quotedword(&ଧାଡ଼ି, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&ଧାଡ଼ି))
    ;
  ଅନ୍ୟଥା {
    ଫେରନ୍ତୁ hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
ସ୍ଥିର CURLcode hsts_pull(ସଂରଚନା Curl_easy *data, ସଂରଚନା hsts *h)
{
  /* if the HSTS read callback is set, use it */
  ଯଦି(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    କର {
      ଅକ୍ଷର buffer[MAX_HSTS_HOSTLEN + 1];
      ସଂରଚନା curl_hstsentry e;
      e.name = buffer;
      e.namelen = ଆକାର(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      ଯଦି(sc == CURLSTS_OK) {
        CURLcode result;
        ଅଚଳ ଅକ୍ଷର *date = e.expire;
        ଯଦି(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
        ଯଦି(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
      }
      ଅନ୍ୟଥା ଯଦି(sc == CURLSTS_FAIL)
        ଫେରନ୍ତୁ CURLE_ABORTED_BY_CALLBACK;
    } ଯେତେବେଳେ(sc == CURLSTS_OK);
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ସ୍ଥିର CURLcode hsts_load(ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  ଯଦି(!h->filename)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ଯଦି(fp) {
    curlx_struct_stat stat;
    ଯଦି((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      ସଂରଚନା dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_ଧାଡ଼ି);
      କର {
        result = Curl_get_ଧାଡ଼ି(&buf, fp, &eof);
        ଯଦି(!result) {
          ଅଚଳ ଅକ୍ଷର *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          ଯଦି((*lineptr == '#') || strlen(lineptr) <= 1)
            ଜାରି;

          hsts_add(h, lineptr);
        }
      } ଯେତେବେଳେ(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(ସଂରଚନା Curl_easy *data,
                            ସଂରଚନା hsts *h, ଅଚଳ ଅକ୍ଷର *file)
{
  DEBUGASSERT(h);
  (ଶୂନ୍ୟ)data;
  ଫେରନ୍ତୁ hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(ସଂରଚନା Curl_easy *data, ସଂରଚନା hsts *h)
{
  ଯଦି(h)
    ଫେରନ୍ତୁ hsts_pull(data, h);
  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(ସଂରଚନା Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା curl_slist *l = data->state.hstslist;
  ଯଦି(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    ଯେତେବେଳେ(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      ଯଦି(result)
        ଭାଙ୍ଗ;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  ଫେରନ୍ତୁ result;
}

bool Curl_hsts_applies(ସଂରଚନା hsts *h, ଅଚଳ ସଂରଚନା Curl_peer *dest)
{
  ଫେରନ୍ତୁ !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#ଯଦି defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
