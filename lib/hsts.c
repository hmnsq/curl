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
#અન્તર્ભાવ "curl_setup.h"

#જો !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "llist.h"
#અન્તર્ભાવ "hsts.h"
#અન્તર્ભાવ "curl_fopen.h"
#અન્તર્ભાવ "curl_get_line.h"
#અન્તર્ભાવ "parsedate.h"
#અન્તર્ભાવ "curl_share.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curlx/strcopy.h"

#define MAX_HSTS_લીટી    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#જો defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
time_t deltatime; /* allow for "adjustments" for unit test purposes */
સ્થિર time_t hsts_debugtime(શૂન્ય *unused)
{
  અચલ અક્ષર *timestr = getenv("CURL_TIME");
  (શૂન્ય)unused;
  જો(timestr) {
    curl_off_t val;
    જો(!curlx_str_સંખ્યા(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    પાછા (time_t)val;
  }
  પાછા time(ખાલી);
}
#undef time
#define time(x) hsts_debugtime(x)
#endif

સંરચના hsts *Curl_hsts_init(શૂન્ય)
{
  સંરચના hsts *h = curlx_calloc(1, કદ(સંરચના hsts));
  જો(h) {
    Curl_llist_init(&h->list, ખાલી);
  }
  પાછા h;
}

#define hsts_free(x) curlx_free(x)

શૂન્ય Curl_hsts_cleanup(સંરચના hsts **hp)
{
  સંરચના hsts *h = *hp;
  જો(h) {
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_llist_ગાંઠ *n;
    માટે(e = Curl_llist_head(&h->list); e; e = n) {
      સંરચના stsentry *sts = Curl_ગાંઠ_elem(e);
      n = Curl_ગાંઠ_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = ખાલી;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
સ્થિર શૂન્ય hsts_append(સંરચના hsts *h, સંરચના stsentry *sts)
{
  જો(Curl_llist_ગણતરી(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    સંરચના Curl_llist_ગાંઠ *e = Curl_llist_head(&h->list);
    સંરચના stsentry *oldsts = Curl_ગાંઠ_elem(e);
    Curl_ગાંઠ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->ગાંઠ);
}

સ્થિર CURLcode hsts_create(સંરચના hsts *h,
                            અચલ અક્ષર *hostname,
                            size_t hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  જો(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  જો(hlen) {
    સંરચના stsentry *sts = curlx_calloc(1, કદ(સંરચના stsentry) + hlen);
    જો(!sts)
      પાછા CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    memcpy(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  પાછા CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(સંરચના hsts *dst, સંરચના hsts *src)
{
  સંરચના Curl_llist_ગાંઠ *e;
  time_t now = time(ખાલી);
  માટે(e = Curl_llist_head(&src->list); e; e = Curl_ગાંઠ_next(e)) {
    સંરચના stsentry *sts = Curl_ગાંઠ_elem(e);
    જો(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, strlen(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      જો(result)
        પાછા result;
    }
  }
  પાછા CURLE_OK;
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
UNITTEST સંરચના stsentry *hsts_check(સંરચના hsts *h, અચલ અક્ષર *hostname,
                                     size_t hlen, bool subdomain);
UNITTEST સંરચના stsentry *hsts_check(સંરચના hsts *h, અચલ અક્ષર *hostname,
                                     size_t hlen, bool subdomain)
{
  સંરચના stsentry *bestsub = ખાલી;
  જો(h) {
    time_t now = time(ખાલી);
    સંરચના Curl_llist_ગાંઠ *e;
    સંરચના Curl_llist_ગાંઠ *n;
    size_t blen = 0;

    જો((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      પાછા ખાલી;
    જો(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    માટે(e = Curl_llist_head(&h->list); e; e = n) {
      સંરચના stsentry *sts = Curl_ગાંઠ_elem(e);
      size_t ntail;
      n = Curl_ગાંઠ_next(e);
      જો(sts->expires <= now) {
        /* remove expired entries */
        Curl_ગાંઠ_remove(&sts->ગાંઠ);
        hsts_free(sts);
        ચાલુ;
      }
      ntail = strlen(sts->host);
      જો((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        size_t offs = hlen - ntail;
        જો((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      જો((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        પાછા sts;
    }
  }
  પાછા bestsub;
}

CURLcode Curl_hsts_parse(સંરચના hsts *h, અચલ અક્ષર *hostname,
                         અચલ અક્ષર *header)
{
  અચલ અક્ષર *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  સંરચના stsentry *sts;
  time_t now = time(ખાલી);
  size_t hlen = strlen(hostname);

  જો(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    પાછા CURLE_OK;

  કરો {
    સંરચના Curl_str word;
    સંરચના Curl_str val = { 0 };
    પૂર્ણાંક rc;
    bool assign = FALSE;

    કરો {
      curlx_str_passblanks(&p);
      જો(*p == ';')
        p++;
      નહીંતર
        તોડો;
    } જ્યારે(1);
    જો(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      તોડો;

    curlx_str_passblanks(&p);
    જો(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      જો(*p == '\"') {
        જો(curlx_str_quotedword(&p, &val, MAX_HSTS_લીટી))
          તોડો;
      }
      નહીંતર {
        જો(curlx_str_cspn(&p, &val, ", ;\r\n"))
          તોડો;
      }
    }

    જો(assign && curlx_str_casecompare(&word, "max-age")) {
      અચલ અક્ષર *vp = curlx_str(&val);
      જો(gotma)
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_સંખ્યા(&vp, &expires, CAP_HSTS_MAX_AGE);
      જો(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      નહીંતર જો(rc)
        /* invalid max-age */
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    નહીંતર જો(curlx_str_casecompare(&word, "includesubdomains")) {
      જો(gotinc)
        પાછા CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } જ્યારે(*p);

  જો(!gotma)
    /* max-age is mandatory */
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  જો(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    જો(sts) {
      Curl_ગાંઠ_remove(&sts->ગાંઠ);
      hsts_free(sts);
    }
    પાછા CURLE_OK;
  }

  જો(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  નહીંતર
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  જો(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  નહીંતર
    પાછા hsts_create(h, hostname, hlen, subdomains, expires);

  પાછા CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
સ્થિર CURLcode hsts_ધકેલો(સંરચના Curl_easy *data,
                          સંરચના curl_index *i,
                          સંરચના stsentry *sts,
                          bool *stop)
{
  સંરચના curl_hstsentry e;
  CURLSTScode sc;
  સંરચના tm stamp;
  CURLcode result;

  e.name = (અક્ષર *)sts->host;
  e.namelen = strlen(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  જો(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((time_t)sts->expires, &stamp);
    જો(result)
      પાછા result;

    curl_msnprintf(e.expire, કદ(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  નહીંતર
    curlx_strcopy(e.expire, કદ(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  પાછા sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
સ્થિર શૂન્ય hsts_out(સંરચના stsentry *sts, FILE *fp)
{
  સંરચના tm stamp;
  જો(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((time_t)sts->expires, &stamp);
    જો(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  નહીંતર
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(સંરચના Curl_easy *data, સંરચના hsts *h,
                        અચલ અક્ષર *file)
{
  સંરચના Curl_llist_ગાંઠ *e;
  સંરચના Curl_llist_ગાંઠ *n;
  CURLcode result = CURLE_OK;
  FILE *out;
  અક્ષર *tempstore = ખાલી;

  જો(!h)
    /* no cache activated */
    પાછા CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  જો(!file && h->filename)
    file = h->filename;

  જો((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    જાઓ skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  જો(!result) {
    fputs("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    માટે(e = Curl_llist_head(&h->list); e; e = n) {
      સંરચના stsentry *sts = Curl_ગાંઠ_elem(e);
      n = Curl_ગાંઠ_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    જો(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    જો(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  જો(data->set.hsts_write) {
    /* if there is a write callback */
    સંરચના curl_index i; /*ગણતરી*/
    i.total = Curl_llist_ગણતરી(&h->list);
    i.index = 0;
    માટે(e = Curl_llist_head(&h->list); e; e = n) {
      સંરચના stsentry *sts = Curl_ગાંઠ_elem(e);
      bool stop;
      n = Curl_ગાંઠ_next(e);
      result = hsts_ધકેલો(data, &i, sts, &stop);
      જો(result || stop)
        તોડો;
      i.index++;
    }
  }
  પાછા result;
}

/* only returns SERIOUS errors */
સ્થિર CURLcode hsts_add_host_expire(સંરચના hsts *h,
                                     અચલ અક્ષર *host, size_t hostlen,
                                     અચલ અક્ષર *expire, size_t explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  સંરચના stsentry *e;
  અક્ષર dbuf[MAX_HSTS_DATELEN + 1];
  time_t expires = 0;
  time_t now = time(ખાલી);

  /* The date parser works on a null-terminated string. */
  જો(explen > MAX_HSTS_DATELEN)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;
  memcpy(dbuf, expire, explen);
  dbuf[explen] = 0;

  જો(!strcmp(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  નહીંતર
    Curl_getdate_capped(dbuf, &expires);

  જો(expires <= now)
    /* this entry already expired */
    પાછા CURLE_OK;

  જો(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  જો(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  જો(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    જો(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    નહીંતર જો((hostlen == strlen(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      જો(expires > e->expires)
        e->expires = expires;
      જો(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  પાછા result;
}

/* only returns SERIOUS errors */
સ્થિર CURLcode hsts_add(સંરચના hsts *h, અચલ અક્ષર *લીટી)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  સંરચના Curl_str host;
  સંરચના Curl_str date;

  જો(curlx_str_word(&લીટી, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&લીટી) ||
     curlx_str_quotedword(&લીટી, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&લીટી))
    ;
  નહીંતર {
    પાછા hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  પાછા CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
સ્થિર CURLcode hsts_pull(સંરચના Curl_easy *data, સંરચના hsts *h)
{
  /* if the HSTS read callback is set, use it */
  જો(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    કરો {
      અક્ષર buffer[MAX_HSTS_HOSTLEN + 1];
      સંરચના curl_hstsentry e;
      e.name = buffer;
      e.namelen = કદ(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      જો(sc == CURLSTS_OK) {
        CURLcode result;
        અચલ અક્ષર *date = e.expire;
        જો(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          પાછા CURLE_BAD_FUNCTION_ARGUMENT;
        જો(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, strlen(e.name),
                                      date, strlen(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        જો(result)
          પાછા result;
      }
      નહીંતર જો(sc == CURLSTS_FAIL)
        પાછા CURLE_ABORTED_BY_CALLBACK;
    } જ્યારે(sc == CURLSTS_OK);
  }
  પાછા CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
સ્થિર CURLcode hsts_load(સંરચના hsts *h, અચલ અક્ષર *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  જો(!h->filename)
    પાછા CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  જો(fp) {
    curlx_struct_stat stat;
    જો((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      સંરચના dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_લીટી);
      કરો {
        result = Curl_get_લીટી(&buf, fp, &eof);
        જો(!result) {
          અચલ અક્ષર *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          જો((*lineptr == '#') || strlen(lineptr) <= 1)
            ચાલુ;

          hsts_add(h, lineptr);
        }
      } જ્યારે(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  પાછા result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(સંરચના Curl_easy *data,
                            સંરચના hsts *h, અચલ અક્ષર *file)
{
  DEBUGASSERT(h);
  (શૂન્ય)data;
  પાછા hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(સંરચના Curl_easy *data, સંરચના hsts *h)
{
  જો(h)
    પાછા hsts_pull(data, h);
  પાછા CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(સંરચના Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  સંરચના curl_slist *l = data->state.hstslist;
  જો(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    જ્યારે(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      જો(result)
        તોડો;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  પાછા result;
}

bool Curl_hsts_applies(સંરચના hsts *h, અચલ સંરચના Curl_peer *dest)
{
  પાછા !!hsts_check(h, dest->hostname, strlen(dest->hostname), TRUE);
}

#જો defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
