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
#अन्तर्भाव "curl_setup.h"

#जर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "llist.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"

#define MAX_HSTS_ओळ    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#जर defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
समय_प्रकार deltatime; /* allow for "adjustments" for unit test purposes */
स्थिर समय_प्रकार hsts_debugtime(शून्य *unused)
{
  स्थिरांक अक्षर *timestr = दो_पर्या("CURL_TIME");
  (शून्य)unused;
  जर(timestr) {
    curl_off_t val;
    जर(!curlx_str_संख्या(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    परत (समय_प्रकार)val;
  }
  परत समय(रिक्त);
}
#undef समय
#define समय(x) hsts_debugtime(x)
#endif

संरचना hsts *Curl_hsts_init(शून्य)
{
  संरचना hsts *h = curlx_calloc(1, आकार(संरचना hsts));
  जर(h) {
    Curl_llist_init(&h->list, रिक्त);
  }
  परत h;
}

#define hsts_free(x) curlx_free(x)

शून्य Curl_hsts_cleanup(संरचना hsts **hp)
{
  संरचना hsts *h = *hp;
  जर(h) {
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *n;
    साठी(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाठ_elem(e);
      n = Curl_गाठ_next(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = रिक्त;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
स्थिर शून्य hsts_append(संरचना hsts *h, संरचना stsentry *sts)
{
  जर(Curl_llist_मोजणी(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    संरचना Curl_llist_गाठ *e = Curl_llist_head(&h->list);
    संरचना stsentry *oldsts = Curl_गाठ_elem(e);
    Curl_गाठ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->गाठ);
}

स्थिर CURLcode hsts_create(संरचना hsts *h,
                            स्थिरांक अक्षर *hostname,
                            माप_प्रकार hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  जर(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  जर(hlen) {
    संरचना stsentry *sts = curlx_calloc(1, आकार(संरचना stsentry) + hlen);
    जर(!sts)
      परत CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    स_नकल(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  परत CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(संरचना hsts *dst, संरचना hsts *src)
{
  संरचना Curl_llist_गाठ *e;
  समय_प्रकार now = समय(रिक्त);
  साठी(e = Curl_llist_head(&src->list); e; e = Curl_गाठ_next(e)) {
    संरचना stsentry *sts = Curl_गाठ_elem(e);
    जर(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, म_माप(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      जर(result)
        परत result;
    }
  }
  परत CURLE_OK;
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
UNITTEST संरचना stsentry *hsts_check(संरचना hsts *h, स्थिरांक अक्षर *hostname,
                                     माप_प्रकार hlen, bool subdomain);
UNITTEST संरचना stsentry *hsts_check(संरचना hsts *h, स्थिरांक अक्षर *hostname,
                                     माप_प्रकार hlen, bool subdomain)
{
  संरचना stsentry *bestsub = रिक्त;
  जर(h) {
    समय_प्रकार now = समय(रिक्त);
    संरचना Curl_llist_गाठ *e;
    संरचना Curl_llist_गाठ *n;
    माप_प्रकार blen = 0;

    जर((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      परत रिक्त;
    जर(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    साठी(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाठ_elem(e);
      माप_प्रकार ntail;
      n = Curl_गाठ_next(e);
      जर(sts->expires <= now) {
        /* remove expired entries */
        Curl_गाठ_remove(&sts->गाठ);
        hsts_free(sts);
        सुरूठेवा;
      }
      ntail = म_माप(sts->host);
      जर((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        माप_प्रकार offs = hlen - ntail;
        जर((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      जर((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        परत sts;
    }
  }
  परत bestsub;
}

CURLcode Curl_hsts_parse(संरचना hsts *h, स्थिरांक अक्षर *hostname,
                         स्थिरांक अक्षर *header)
{
  स्थिरांक अक्षर *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  संरचना stsentry *sts;
  समय_प्रकार now = समय(रिक्त);
  माप_प्रकार hlen = म_माप(hostname);

  जर(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    परत CURLE_OK;

  करा {
    संरचना Curl_str word;
    संरचना Curl_str val = { 0 };
    पूर्णांक rc;
    bool assign = FALSE;

    करा {
      curlx_str_passblanks(&p);
      जर(*p == ';')
        p++;
      अन्यथा
        थांबा;
    } पर्यंत(1);
    जर(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      थांबा;

    curlx_str_passblanks(&p);
    जर(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      जर(*p == '\"') {
        जर(curlx_str_quotedword(&p, &val, MAX_HSTS_ओळ))
          थांबा;
      }
      अन्यथा {
        जर(curlx_str_cspn(&p, &val, ", ;\r\n"))
          थांबा;
      }
    }

    जर(assign && curlx_str_casecompare(&word, "max-age")) {
      स्थिरांक अक्षर *vp = curlx_str(&val);
      जर(gotma)
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_संख्या(&vp, &expires, CAP_HSTS_MAX_AGE);
      जर(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      अन्यथा जर(rc)
        /* invalid max-age */
        परत CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    अन्यथा जर(curlx_str_casecompare(&word, "includesubdomains")) {
      जर(gotinc)
        परत CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } पर्यंत(*p);

  जर(!gotma)
    /* max-age is mandatory */
    परत CURLE_BAD_FUNCTION_ARGUMENT;

  जर(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    जर(sts) {
      Curl_गाठ_remove(&sts->गाठ);
      hsts_free(sts);
    }
    परत CURLE_OK;
  }

  जर(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  अन्यथा
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  जर(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  अन्यथा
    परत hsts_create(h, hostname, hlen, subdomains, expires);

  परत CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
स्थिर CURLcode hsts_ढकला(संरचना Curl_easy *data,
                          संरचना curl_index *i,
                          संरचना stsentry *sts,
                          bool *stop)
{
  संरचना curl_hstsentry e;
  CURLSTScode sc;
  संरचना पंचांग stamp;
  CURLcode result;

  e.name = (अक्षर *)sts->host;
  e.namelen = म_माप(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  जर(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((समय_प्रकार)sts->expires, &stamp);
    जर(result)
      परत result;

    curl_msnprintf(e.expire, आकार(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  अन्यथा
    curlx_strcopy(e.expire, आकार(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  परत sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
स्थिर शून्य hsts_out(संरचना stsentry *sts, खाता *fp)
{
  संरचना पंचांग stamp;
  जर(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((समय_प्रकार)sts->expires, &stamp);
    जर(!result)
      /* skip the entry if the date function fails */
      curl_mfprintf(fp, "%s%s \"%d%02d%02d %02d:%02d:%02d\"\n",
                    sts->includeSubDomains ? "." : "", sts->host,
                    stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                    stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  अन्यथा
    curl_mfprintf(fp, "%s%s \"%s\"\n",
                  sts->includeSubDomains ? "." : "", sts->host, UNLIMITED);
}

/*
 * Curl_https_save() writes the HSTS cache to file and callback.
 */
CURLcode Curl_hsts_save(संरचना Curl_easy *data, संरचना hsts *h,
                        स्थिरांक अक्षर *file)
{
  संरचना Curl_llist_गाठ *e;
  संरचना Curl_llist_गाठ *n;
  CURLcode result = CURLE_OK;
  खाता *out;
  अक्षर *tempstore = रिक्त;

  जर(!h)
    /* no cache activated */
    परत CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  जर(!file && h->filename)
    file = h->filename;

  जर((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    जा skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  जर(!result) {
    ख_माला_दो("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    साठी(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाठ_elem(e);
      n = Curl_गाठ_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    जर(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    जर(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  जर(data->set.hsts_write) {
    /* if there is a write callback */
    संरचना curl_index i; /*मोजणी*/
    i.total = Curl_llist_मोजणी(&h->list);
    i.index = 0;
    साठी(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाठ_elem(e);
      bool stop;
      n = Curl_गाठ_next(e);
      result = hsts_ढकला(data, &i, sts, &stop);
      जर(result || stop)
        थांबा;
      i.index++;
    }
  }
  परत result;
}

/* only returns SERIOUS errors */
स्थिर CURLcode hsts_add_host_expire(संरचना hsts *h,
                                     स्थिरांक अक्षर *host, माप_प्रकार hostlen,
                                     स्थिरांक अक्षर *expire, माप_प्रकार explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  संरचना stsentry *e;
  अक्षर dbuf[MAX_HSTS_DATELEN + 1];
  समय_प्रकार expires = 0;
  समय_प्रकार now = समय(रिक्त);

  /* The date parser works on a null-terminated string. */
  जर(explen > MAX_HSTS_DATELEN)
    परत CURLE_BAD_FUNCTION_ARGUMENT;
  स_नकल(dbuf, expire, explen);
  dbuf[explen] = 0;

  जर(!म_भेद(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  अन्यथा
    Curl_getdate_capped(dbuf, &expires);

  जर(expires <= now)
    /* this entry already expired */
    परत CURLE_OK;

  जर(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  जर(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  जर(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    जर(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    अन्यथा जर((hostlen == म_माप(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      जर(expires > e->expires)
        e->expires = expires;
      जर(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  परत result;
}

/* only returns SERIOUS errors */
स्थिर CURLcode hsts_add(संरचना hsts *h, स्थिरांक अक्षर *ओळ)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  संरचना Curl_str host;
  संरचना Curl_str date;

  जर(curlx_str_word(&ओळ, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&ओळ) ||
     curlx_str_quotedword(&ओळ, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&ओळ))
    ;
  अन्यथा {
    परत hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  परत CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
स्थिर CURLcode hsts_pull(संरचना Curl_easy *data, संरचना hsts *h)
{
  /* if the HSTS read callback is set, use it */
  जर(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    करा {
      अक्षर buffer[MAX_HSTS_HOSTLEN + 1];
      संरचना curl_hstsentry e;
      e.name = buffer;
      e.namelen = आकार(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      जर(sc == CURLSTS_OK) {
        CURLcode result;
        स्थिरांक अक्षर *date = e.expire;
        जर(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          परत CURLE_BAD_FUNCTION_ARGUMENT;
        जर(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, म_माप(e.name),
                                      date, म_माप(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        जर(result)
          परत result;
      }
      अन्यथा जर(sc == CURLSTS_FAIL)
        परत CURLE_ABORTED_BY_CALLBACK;
    } पर्यंत(sc == CURLSTS_OK);
  }
  परत CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
स्थिर CURLcode hsts_load(संरचना hsts *h, स्थिरांक अक्षर *file)
{
  CURLcode result = CURLE_OK;
  खाता *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  जर(!h->filename)
    परत CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  जर(fp) {
    curlx_struct_stat stat;
    जर((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      संरचना dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_ओळ);
      करा {
        result = Curl_get_ओळ(&buf, fp, &eof);
        जर(!result) {
          स्थिरांक अक्षर *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          जर((*lineptr == '#') || म_माप(lineptr) <= 1)
            सुरूठेवा;

          hsts_add(h, lineptr);
        }
      } पर्यंत(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  परत result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(संरचना Curl_easy *data,
                            संरचना hsts *h, स्थिरांक अक्षर *file)
{
  DEBUGASSERT(h);
  (शून्य)data;
  परत hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(संरचना Curl_easy *data, संरचना hsts *h)
{
  जर(h)
    परत hsts_pull(data, h);
  परत CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  संरचना curl_slist *l = data->state.hstslist;
  जर(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    पर्यंत(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      जर(result)
        थांबा;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  परत result;
}

bool Curl_hsts_applies(संरचना hsts *h, स्थिरांक संरचना Curl_peer *dest)
{
  परत !!hsts_check(h, dest->hostname, म_माप(dest->hostname), TRUE);
}

#जर defined(DEBUGBUILD) || defined(UNITTESTS)
#undef समय
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
