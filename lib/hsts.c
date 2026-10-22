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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "llist.h"
#अन्तर्भाव "hsts.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"

#define MAX_HSTS_पंक्ति    4095
#define MAX_HSTS_HOSTLEN 2048
#define MAX_HSTS_DATELEN 17
#define UNLIMITED        "unlimited"

#define CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#यदि defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
समय_प्रकार deltatime; /* allow for "adjustments" for unit test purposes */
स्थिर समय_प्रकार hsts_debugtime(शून्य *unused)
{
  ध्रुव वर्ण *timestr = दो_पर्या("CURL_TIME");
  (शून्य)unused;
  यदि(timestr) {
    curl_off_t val;
    यदि(!curlx_str_संख्या(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    प्रत्यागम (समय_प्रकार)val;
  }
  प्रत्यागम समय(रिक्त);
}
#undef समय
#define समय(x) hsts_debugtime(x)
#endif

संरचना hsts *Curl_hsts_init(शून्य)
{
  संरचना hsts *h = curlx_calloc(1, परिमाण(संरचना hsts));
  यदि(h) {
    Curl_llist_init(&h->list, रिक्त);
  }
  प्रत्यागम h;
}

#define hsts_free(x) curlx_free(x)

शून्य Curl_hsts_cleanup(संरचना hsts **hp)
{
  संरचना hsts *h = *hp;
  यदि(h) {
    संरचना Curl_llist_गाँठ *e;
    संरचना Curl_llist_गाँठ *n;
    प्रति(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_next(e);
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
  यदि(Curl_llist_गिनती(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    संरचना Curl_llist_गाँठ *e = Curl_llist_head(&h->list);
    संरचना stsentry *oldsts = Curl_गाँठ_elem(e);
    Curl_गाँठ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->गाँठ);
}

स्थिर CURLcode hsts_create(संरचना hsts *h,
                            ध्रुव वर्ण *hostname,
                            माप_प्रकार hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  यदि(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  यदि(hlen) {
    संरचना stsentry *sts = curlx_calloc(1, परिमाण(संरचना stsentry) + hlen);
    यदि(!sts)
      प्रत्यागम CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    स_नकल(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  प्रत्यागम CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(संरचना hsts *dst, संरचना hsts *src)
{
  संरचना Curl_llist_गाँठ *e;
  समय_प्रकार now = समय(रिक्त);
  प्रति(e = Curl_llist_head(&src->list); e; e = Curl_गाँठ_next(e)) {
    संरचना stsentry *sts = Curl_गाँठ_elem(e);
    यदि(sts->expires > now) {
      CURLcode result = hsts_create(dst, sts->host, म_माप(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      यदि(result)
        प्रत्यागम result;
    }
  }
  प्रत्यागम CURLE_OK;
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
UNITTEST संरचना stsentry *hsts_check(संरचना hsts *h, ध्रुव वर्ण *hostname,
                                     माप_प्रकार hlen, bool subdomain);
UNITTEST संरचना stsentry *hsts_check(संरचना hsts *h, ध्रुव वर्ण *hostname,
                                     माप_प्रकार hlen, bool subdomain)
{
  संरचना stsentry *bestsub = रिक्त;
  यदि(h) {
    समय_प्रकार now = समय(रिक्त);
    संरचना Curl_llist_गाँठ *e;
    संरचना Curl_llist_गाँठ *n;
    माप_प्रकार blen = 0;

    यदि((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      प्रत्यागम रिक्त;
    यदि(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    प्रति(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाँठ_elem(e);
      माप_प्रकार ntail;
      n = Curl_गाँठ_next(e);
      यदि(sts->expires <= now) {
        /* remove expired entries */
        Curl_गाँठ_remove(&sts->गाँठ);
        hsts_free(sts);
        अनुवर्त;
      }
      ntail = म_माप(sts->host);
      यदि((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        माप_प्रकार offs = hlen - ntail;
        यदि((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      यदि((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        प्रत्यागम sts;
    }
  }
  प्रत्यागम bestsub;
}

CURLcode Curl_hsts_parse(संरचना hsts *h, ध्रुव वर्ण *hostname,
                         ध्रुव वर्ण *header)
{
  ध्रुव वर्ण *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  संरचना stsentry *sts;
  समय_प्रकार now = समय(रिक्त);
  माप_प्रकार hlen = म_माप(hostname);

  यदि(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    प्रत्यागम CURLE_OK;

  कुरु {
    संरचना Curl_str word;
    संरचना Curl_str val = { 0 };
    पूर्णाङ्क rc;
    bool assign = FALSE;

    कुरु {
      curlx_str_passblanks(&p);
      यदि(*p == ';')
        p++;
      अन्यथा
        भङ्ग;
    } यावत्(1);
    यदि(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      भङ्ग;

    curlx_str_passblanks(&p);
    यदि(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      यदि(*p == '\"') {
        यदि(curlx_str_quotedword(&p, &val, MAX_HSTS_पंक्ति))
          भङ्ग;
      }
      अन्यथा {
        यदि(curlx_str_cspn(&p, &val, ", ;\r\n"))
          भङ्ग;
      }
    }

    यदि(assign && curlx_str_casecompare(&word, "max-age")) {
      ध्रुव वर्ण *vp = curlx_str(&val);
      यदि(gotma)
        प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_संख्या(&vp, &expires, CAP_HSTS_MAX_AGE);
      यदि(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      अन्यथा यदि(rc)
        /* invalid max-age */
        प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    अन्यथा यदि(curlx_str_casecompare(&word, "includesubdomains")) {
      यदि(gotinc)
        प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } यावत्(*p);

  यदि(!gotma)
    /* max-age is mandatory */
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    यदि(sts) {
      Curl_गाँठ_remove(&sts->गाँठ);
      hsts_free(sts);
    }
    प्रत्यागम CURLE_OK;
  }

  यदि(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  अन्यथा
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  यदि(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  अन्यथा
    प्रत्यागम hsts_create(h, hostname, hlen, subdomains, expires);

  प्रत्यागम CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
स्थिर CURLcode hsts_धकेलो(संरचना Curl_easy *data,
                          संरचना curl_index *i,
                          संरचना stsentry *sts,
                          bool *stop)
{
  संरचना curl_hstsentry e;
  CURLSTScode sc;
  संरचना पंचांग stamp;
  CURLcode result;

  e.name = (वर्ण *)sts->host;
  e.namelen = म_माप(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  यदि(sts->expires != TIME_T_MAX) {
    result = curlx_gmtime((समय_प्रकार)sts->expires, &stamp);
    यदि(result)
      प्रत्यागम result;

    curl_msnprintf(e.expire, परिमाण(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  अन्यथा
    curlx_strcopy(e.expire, परिमाण(e.expire), STRCONST(UNLIMITED));

  sc = data->set.hsts_write(data, &e, i, data->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  प्रत्यागम sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
स्थिर शून्य hsts_out(संरचना stsentry *sts, खाता *fp)
{
  संरचना पंचांग stamp;
  यदि(sts->expires != TIME_T_MAX) {
    CURLcode result = curlx_gmtime((समय_प्रकार)sts->expires, &stamp);
    यदि(!result)
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
                        ध्रुव वर्ण *file)
{
  संरचना Curl_llist_गाँठ *e;
  संरचना Curl_llist_गाँठ *n;
  CURLcode result = CURLE_OK;
  खाता *out;
  वर्ण *tempstore = रिक्त;

  यदि(!h)
    /* no cache activated */
    प्रत्यागम CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  यदि(!file && h->filename)
    file = h->filename;

  यदि((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    गच्छ skipsave;

  result = Curl_fopen(data, file, &out, &tempstore);
  यदि(!result) {
    ख_माला_दो("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    प्रति(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_next(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    यदि(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    यदि(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  यदि(data->set.hsts_write) {
    /* if there is a write callback */
    संरचना curl_index i; /*गिनती*/
    i.total = Curl_llist_गिनती(&h->list);
    i.index = 0;
    प्रति(e = Curl_llist_head(&h->list); e; e = n) {
      संरचना stsentry *sts = Curl_गाँठ_elem(e);
      bool stop;
      n = Curl_गाँठ_next(e);
      result = hsts_धकेलो(data, &i, sts, &stop);
      यदि(result || stop)
        भङ्ग;
      i.index++;
    }
  }
  प्रत्यागम result;
}

/* only returns SERIOUS errors */
स्थिर CURLcode hsts_add_host_expire(संरचना hsts *h,
                                     ध्रुव वर्ण *host, माप_प्रकार hostlen,
                                     ध्रुव वर्ण *expire, माप_प्रकार explen,
                                     bool subdomain) /* default */
{
  CURLcode result = CURLE_OK;
  संरचना stsentry *e;
  वर्ण dbuf[MAX_HSTS_DATELEN + 1];
  समय_प्रकार expires = 0;
  समय_प्रकार now = समय(रिक्त);

  /* The date parser works on a null-terminated string. */
  यदि(explen > MAX_HSTS_DATELEN)
    प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
  स_नकल(dbuf, expire, explen);
  dbuf[explen] = 0;

  यदि(!म_भेद(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  अन्यथा
    Curl_getdate_capped(dbuf, &expires);

  यदि(expires <= now)
    /* this entry already expired */
    प्रत्यागम CURLE_OK;

  यदि(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  यदि(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  यदि(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    यदि(!e)
      result = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    अन्यथा यदि((hostlen == म_माप(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      यदि(expires > e->expires)
        e->expires = expires;
      यदि(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  प्रत्यागम result;
}

/* only returns SERIOUS errors */
स्थिर CURLcode hsts_add(संरचना hsts *h, ध्रुव वर्ण *पंक्ति)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  संरचना Curl_str host;
  संरचना Curl_str date;

  यदि(curlx_str_word(&पंक्ति, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_quotedword(&पंक्ति, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&पंक्ति))
    ;
  अन्यथा {
    प्रत्यागम hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  प्रत्यागम CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
स्थिर CURLcode hsts_pull(संरचना Curl_easy *data, संरचना hsts *h)
{
  /* if the HSTS read callback is set, use it */
  यदि(data->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    कुरु {
      वर्ण buffer[MAX_HSTS_HOSTLEN + 1];
      संरचना curl_hstsentry e;
      e.name = buffer;
      e.namelen = परिमाण(buffer) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = data->set.hsts_read(data, &e, data->set.hsts_read_userp);
      यदि(sc == CURLSTS_OK) {
        CURLcode result;
        ध्रुव वर्ण *date = e.expire;
        यदि(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          प्रत्यागम CURLE_BAD_FUNCTION_ARGUMENT;
        यदि(!date[0])
          date = UNLIMITED;
        result = hsts_add_host_expire(h, e.name, म_माप(e.name),
                                      date, म_माप(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        यदि(result)
          प्रत्यागम result;
      }
      अन्यथा यदि(sc == CURLSTS_FAIL)
        प्रत्यागम CURLE_ABORTED_BY_CALLBACK;
    } यावत्(sc == CURLSTS_OK);
  }
  प्रत्यागम CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
स्थिर CURLcode hsts_load(संरचना hsts *h, ध्रुव वर्ण *file)
{
  CURLcode result = CURLE_OK;
  खाता *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  यदि(!h->filename)
    प्रत्यागम CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  यदि(fp) {
    curlx_struct_stat stat;
    यदि((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      संरचना dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_पंक्ति);
      कुरु {
        result = Curl_get_पंक्ति(&buf, fp, &eof);
        यदि(!result) {
          ध्रुव वर्ण *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          यदि((*lineptr == '#') || म_माप(lineptr) <= 1)
            अनुवर्त;

          hsts_add(h, lineptr);
        }
      } यावत्(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  प्रत्यागम result;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(संरचना Curl_easy *data,
                            संरचना hsts *h, ध्रुव वर्ण *file)
{
  DEBUGASSERT(h);
  (शून्य)data;
  प्रत्यागम hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(संरचना Curl_easy *data, संरचना hsts *h)
{
  यदि(h)
    प्रत्यागम hsts_pull(data, h);
  प्रत्यागम CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(संरचना Curl_easy *data)
{
  CURLcode result = CURLE_OK;
  संरचना curl_slist *l = data->state.hstslist;
  यदि(l) {
    Curl_share_lock(data, CURL_LOCK_DATA_HSTS, CURL_LOCK_ACCESS_SINGLE);

    यावत्(l) {
      result = Curl_hsts_loadfile(data, data->hsts, l->data);
      यदि(result)
        भङ्ग;
      l = l->next;
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_HSTS);
  }
  प्रत्यागम result;
}

bool Curl_hsts_applies(संरचना hsts *h, ध्रुव संरचना Curl_peer *dest)
{
  प्रत्यागम !!hsts_check(h, dest->hostname, म_माप(dest->hostname), TRUE);
}

#यदि defined(DEBUGBUILD) || defined(UNITTESTS)
#undef समय
#endif

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
