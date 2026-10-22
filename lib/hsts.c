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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_HSTS)
#समावेश "urldata.h"
#समावेश "llist.h"
#समावेश "hsts.h"
#समावेश "curl_fopen.h"
#समावेश "curl_get_line.h"
#समावेश "parsedate.h"
#समावेश "curl_share.h"
#समावेश "curlx/strparse.h"
#समावेश "curlx/strcopy.h"

#घोषणा MAX_HSTS_पंक्ति    4095
#घोषणा MAX_HSTS_HOSTLEN 2048
#घोषणा MAX_HSTS_DATELEN 17
#घोषणा UNLIMITED        "unlimited"

#घोषणा CAP_HSTS_MAX_AGE (2*365*24*3600) /* two years cap */

#अगर defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
समय_प्रकार deltatime; /* allow for "adjustments" for unit test purposes */
अटल समय_प्रकार hsts_debugtime(व्योम *unused)
{
  स्थिर अक्षर *timestr = दो_पर्या("CURL_TIME");
  (व्योम)unused;
  अगर(timestr) {
    curl_off_t val;
    अगर(!curlx_str_संख्या(&timestr, &val, TIME_T_MAX))
      val += (curl_off_t)deltatime;
    वापस (समय_प्रकार)val;
  }
  वापस समय(शून्य);
}
#अघोषित समय
#घोषणा समय(x) hsts_debugtime(x)
#पूर्ण_अगर

काष्ठा hsts *Curl_hsts_init(व्योम)
{
  काष्ठा hsts *h = curlx_calloc(1, माप(काष्ठा hsts));
  अगर(h) {
    Curl_llist_init(&h->list, शून्य);
  }
  वापस h;
}

#घोषणा hsts_free(x) curlx_free(x)

व्योम Curl_hsts_cleanup(काष्ठा hsts **hp)
{
  काष्ठा hsts *h = *hp;
  अगर(h) {
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_llist_गाँठ *n;
    क्रम(e = Curl_llist_शीर्ष(&h->list); e; e = n) {
      काष्ठा stsentry *sts = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_अग्र(e);
      hsts_free(sts);
    }
    curlx_free(h->filename);
    curlx_free(h);
    *hp = शून्य;
  }
}

/* append the new entry to the list after possibly removing an old entry
   first */
अटल व्योम hsts_append(काष्ठा hsts *h, काष्ठा stsentry *sts)
{
  अगर(Curl_llist_गिनती(&h->list) == MAX_HSTS_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    काष्ठा Curl_llist_गाँठ *e = Curl_llist_शीर्ष(&h->list);
    काष्ठा stsentry *oldsts = Curl_गाँठ_elem(e);
    Curl_गाँठ_remove(e);
    hsts_free(oldsts);
  }
  Curl_llist_append(&h->list, sts, &sts->गाँठ);
}

अटल CURLcode hsts_create(काष्ठा hsts *h,
                            स्थिर अक्षर *hostname,
                            माप_प्रकार hlen,
                            bool subdomains,
                            curl_off_t expires)
{
  DEBUGASSERT(h);
  DEBUGASSERT(hostname);

  अगर(hlen && (hostname[hlen - 1] == '.'))
    /* strip off any trailing dot */
    --hlen;
  अगर(hlen) {
    काष्ठा stsentry *sts = curlx_calloc(1, माप(काष्ठा stsentry) + hlen);
    अगर(!sts)
      वापस CURLE_OUT_OF_MEMORY;
    /* the null-terminator is already there */
    स_नकल(sts->host, hostname, hlen);
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
    hsts_append(h, sts);
  }
  वापस CURLE_OK;
}

/* Copy all live entries from src into dst. Used by curl_easy_duphandle so the
 * clone inherits entries learned at runtime. E.g. Strict-Transport-Security.
 */
CURLcode Curl_hsts_copy(काष्ठा hsts *dst, काष्ठा hsts *src)
{
  काष्ठा Curl_llist_गाँठ *e;
  समय_प्रकार now = समय(शून्य);
  क्रम(e = Curl_llist_शीर्ष(&src->list); e; e = Curl_गाँठ_अग्र(e)) {
    काष्ठा stsentry *sts = Curl_गाँठ_elem(e);
    अगर(sts->expires > now) {
      CURLcode परिणाम = hsts_create(dst, sts->host, म_माप(sts->host),
                                    sts->includeSubDomains != 0, sts->expires);
      अगर(परिणाम)
        वापस परिणाम;
    }
  }
  वापस CURLE_OK;
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
UNITTEST काष्ठा stsentry *hsts_check(काष्ठा hsts *h, स्थिर अक्षर *hostname,
                                     माप_प्रकार hlen, bool subdomain);
UNITTEST काष्ठा stsentry *hsts_check(काष्ठा hsts *h, स्थिर अक्षर *hostname,
                                     माप_प्रकार hlen, bool subdomain)
{
  काष्ठा stsentry *bestsub = शून्य;
  अगर(h) {
    समय_प्रकार now = समय(शून्य);
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_llist_गाँठ *n;
    माप_प्रकार blen = 0;

    अगर((hlen > MAX_HSTS_HOSTLEN) || !hlen)
      वापस शून्य;
    अगर(hostname[hlen - 1] == '.')
      /* remove the trailing dot */
      --hlen;

    क्रम(e = Curl_llist_शीर्ष(&h->list); e; e = n) {
      काष्ठा stsentry *sts = Curl_गाँठ_elem(e);
      माप_प्रकार ntail;
      n = Curl_गाँठ_अग्र(e);
      अगर(sts->expires <= now) {
        /* remove expired entries */
        Curl_गाँठ_remove(&sts->गाँठ);
        hsts_free(sts);
        जारी;
      }
      ntail = म_माप(sts->host);
      अगर((subdomain && sts->includeSubDomains) && (ntail < hlen)) {
        माप_प्रकार offs = hlen - ntail;
        अगर((hostname[offs - 1] == '.') &&
           curl_strnequal(&hostname[offs], sts->host, ntail) &&
           (ntail > blen)) {
          /* save the tail match with the longest tail */
          bestsub = sts;
          blen = ntail;
        }
      }
      /* avoid curl_strequal because the hostname is not null-terminated */
      अगर((hlen == ntail) && curl_strnequal(hostname, sts->host, hlen))
        वापस sts;
    }
  }
  वापस bestsub;
}

CURLcode Curl_hsts_parse(काष्ठा hsts *h, स्थिर अक्षर *hostname,
                         स्थिर अक्षर *header)
{
  स्थिर अक्षर *p = header;
  curl_off_t expires = 0;
  bool gotma = FALSE;
  bool gotinc = FALSE;
  bool subdomains = FALSE;
  काष्ठा stsentry *sts;
  समय_प्रकार now = समय(शून्य);
  माप_प्रकार hlen = म_माप(hostname);

  अगर(Curl_host_is_ipnum(hostname))
    /* "explicit IP address identification of all forms is excluded."
       / RFC 6797 */
    वापस CURLE_OK;

  करो {
    काष्ठा Curl_str word;
    काष्ठा Curl_str val = { 0 };
    पूर्णांक rc;
    bool assign = FALSE;

    करो {
      curlx_str_passblanks(&p);
      अगर(*p == ';')
        p++;
      अन्यथा
        अवरोध;
    } जबतक(1);
    अगर(curlx_str_cspn(&p, &word, ";=\r\n \t"))
      अवरोध;

    curlx_str_passblanks(&p);
    अगर(!curlx_str_single(&p, '=')) {
      assign = TRUE;
      curlx_str_passblanks(&p);

      अगर(*p == '\"') {
        अगर(curlx_str_quotedword(&p, &val, MAX_HSTS_पंक्ति))
          अवरोध;
      }
      अन्यथा {
        अगर(curlx_str_cspn(&p, &val, ", ;\r\n"))
          अवरोध;
      }
    }

    अगर(assign && curlx_str_casecompare(&word, "max-age")) {
      स्थिर अक्षर *vp = curlx_str(&val);
      अगर(gotma)
        वापस CURLE_BAD_FUNCTION_ARGUMENT;
      rc = curlx_str_संख्या(&vp, &expires, CAP_HSTS_MAX_AGE);
      अगर(rc == STRE_OVERFLOW)
        expires = CAP_HSTS_MAX_AGE;
      अन्यथा अगर(rc)
        /* invalid max-age */
        वापस CURLE_BAD_FUNCTION_ARGUMENT;

      gotma = TRUE;
    }
    अन्यथा अगर(curlx_str_casecompare(&word, "includesubdomains")) {
      अगर(gotinc)
        वापस CURLE_BAD_FUNCTION_ARGUMENT;
      subdomains = TRUE;
      gotinc = TRUE;
    }
  } जबतक(*p);

  अगर(!gotma)
    /* max-age is mandatory */
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(!expires) {
    /* remove the entry if present verbatim (without subdomain match) */
    sts = hsts_check(h, hostname, hlen, FALSE);
    अगर(sts) {
      Curl_गाँठ_remove(&sts->गाँठ);
      hsts_free(sts);
    }
    वापस CURLE_OK;
  }

  अगर(CURL_OFF_T_MAX - now < expires)
    /* would overflow, use maximum value */
    expires = CURL_OFF_T_MAX;
  अन्यथा
    expires += now;

  /* check if it already exists */
  sts = hsts_check(h, hostname, hlen, FALSE);
  अगर(sts) {
    /* update these fields */
    sts->expires = expires;
    sts->includeSubDomains = subdomains;
  }
  अन्यथा
    वापस hsts_create(h, hostname, hlen, subdomains, expires);

  वापस CURLE_OK;
}

/*
 * Send this HSTS entry to the write callback.
 */
अटल CURLcode hsts_धकेलो(काष्ठा Curl_easy *तथ्य,
                          काष्ठा curl_सूचकांक *i,
                          काष्ठा stsentry *sts,
                          bool *stop)
{
  काष्ठा curl_hstsentry e;
  CURLSTScode sc;
  काष्ठा पंचांग stamp;
  CURLcode परिणाम;

  e.name = (अक्षर *)sts->host;
  e.namelen = म_माप(sts->host);
  e.includeSubDomains = sts->includeSubDomains;

  अगर(sts->expires != TIME_T_MAX) {
    परिणाम = curlx_gmtime((समय_प्रकार)sts->expires, &stamp);
    अगर(परिणाम)
      वापस परिणाम;

    curl_msnprintf(e.expire, माप(e.expire), "%d%02d%02d %02d:%02d:%02d",
                   stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                   stamp.tm_hour, stamp.tm_min, stamp.tm_sec);
  }
  अन्यथा
    curlx_strcopy(e.expire, माप(e.expire), STRCONST(UNLIMITED));

  sc = तथ्य->set.hsts_write(तथ्य, &e, i, तथ्य->set.hsts_write_userp);
  *stop = (sc != CURLSTS_OK);
  वापस sc == CURLSTS_FAIL ? CURLE_BAD_FUNCTION_ARGUMENT : CURLE_OK;
}

/*
 * Write this single hsts entry to a single output line
 */
अटल व्योम hsts_out(काष्ठा stsentry *sts, खाता *fp)
{
  काष्ठा पंचांग stamp;
  अगर(sts->expires != TIME_T_MAX) {
    CURLcode परिणाम = curlx_gmtime((समय_प्रकार)sts->expires, &stamp);
    अगर(!परिणाम)
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
CURLcode Curl_hsts_save(काष्ठा Curl_easy *तथ्य, काष्ठा hsts *h,
                        स्थिर अक्षर *file)
{
  काष्ठा Curl_llist_गाँठ *e;
  काष्ठा Curl_llist_गाँठ *n;
  CURLcode परिणाम = CURLE_OK;
  खाता *out;
  अक्षर *tempstore = शून्य;

  अगर(!h)
    /* no cache activated */
    वापस CURLE_OK;

  /* if no new name is given, use the one we stored from the load */
  अगर(!file && h->filename)
    file = h->filename;

  अगर((h->flags & CURLHSTS_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    जाओ skipsave;

  परिणाम = Curl_fopen(तथ्य, file, &out, &tempstore);
  अगर(!परिणाम) {
    ख_माला_दो("# Your HSTS cache. https://curl.se/docs/hsts.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    क्रम(e = Curl_llist_शीर्ष(&h->list); e; e = n) {
      काष्ठा stsentry *sts = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_अग्र(e);
      hsts_out(sts, out);
    }
    curlx_fclose(out);
    अगर(!परिणाम && tempstore && curlx_rename(tempstore, file))
      परिणाम = CURLE_WRITE_ERROR;

    अगर(परिणाम && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
skipsave:
  अगर(तथ्य->set.hsts_write) {
    /* if there is a write callback */
    काष्ठा curl_सूचकांक i; /*गिनती*/
    i.योग = Curl_llist_गिनती(&h->list);
    i.सूचकांक = 0;
    क्रम(e = Curl_llist_शीर्ष(&h->list); e; e = n) {
      काष्ठा stsentry *sts = Curl_गाँठ_elem(e);
      bool stop;
      n = Curl_गाँठ_अग्र(e);
      परिणाम = hsts_धकेलो(तथ्य, &i, sts, &stop);
      अगर(परिणाम || stop)
        अवरोध;
      i.सूचकांक++;
    }
  }
  वापस परिणाम;
}

/* only returns SERIOUS errors */
अटल CURLcode hsts_add_host_expire(काष्ठा hsts *h,
                                     स्थिर अक्षर *host, माप_प्रकार hostlen,
                                     स्थिर अक्षर *expire, माप_प्रकार explen,
                                     bool subdomain) /* default */
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा stsentry *e;
  अक्षर dbuf[MAX_HSTS_DATELEN + 1];
  समय_प्रकार expires = 0;
  समय_प्रकार now = समय(शून्य);

  /* The date parser works on a null-terminated string. */
  अगर(explen > MAX_HSTS_DATELEN)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;
  स_नकल(dbuf, expire, explen);
  dbuf[explen] = 0;

  अगर(!म_भेद(dbuf, UNLIMITED))
    expires = TIME_T_MAX;
  अन्यथा
    Curl_getdate_capped(dbuf, &expires);

  अगर(expires <= now)
    /* this entry already expired */
    वापस CURLE_OK;

  अगर(host[0] == '.') {
    host++;
    hostlen--;
    subdomain = TRUE;
  }
  अगर(hostlen && (host[hostlen - 1] == '.'))
    /* strip off any trailing dot */
    hostlen--;

  अगर(hostlen) {
    /* only add it if not already present */
    e = hsts_check(h, host, hostlen, FALSE);
    अगर(!e)
      परिणाम = hsts_create(h, host, hostlen, subdomain, expires);
    /* 'host' is not necessarily null-terminated */
    अन्यथा अगर((hostlen == म_माप(e->host) &&
             curl_strnequal(host, e->host, hostlen))) {
      /* the same hostname, use the largest expire time and keep the strictest
         subdomain policy */
      अगर(expires > e->expires)
        e->expires = expires;
      अगर(subdomain)
        e->includeSubDomains = TRUE;
    }
  }
  वापस परिणाम;
}

/* only returns SERIOUS errors */
अटल CURLcode hsts_add(काष्ठा hsts *h, स्थिर अक्षर *पंक्ति)
{
  /* Example lines:
     example.com "20191231 10:00:00"
     .example.net "20191231 10:00:00"
   */
  काष्ठा Curl_str host;
  काष्ठा Curl_str date;

  अगर(curlx_str_word(&पंक्ति, &host, MAX_HSTS_HOSTLEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_quotedword(&पंक्ति, &date, MAX_HSTS_DATELEN) ||
     curlx_str_newline(&पंक्ति))
    ;
  अन्यथा {
    वापस hsts_add_host_expire(h, curlx_str(&host), curlx_strlen(&host),
                                curlx_str(&date), curlx_strlen(&date),
                                FALSE);
  }

  वापस CURLE_OK;
}

/*
 * Load HSTS data from callback.
 *
 */
अटल CURLcode hsts_pull(काष्ठा Curl_easy *तथ्य, काष्ठा hsts *h)
{
  /* if the HSTS read callback is set, use it */
  अगर(तथ्य->set.hsts_read) {
    CURLSTScode sc;
    DEBUGASSERT(h);
    करो {
      अक्षर बफर[MAX_HSTS_HOSTLEN + 1];
      काष्ठा curl_hstsentry e;
      e.name = बफर;
      e.namelen = माप(बफर) - 1;
      e.includeSubDomains = FALSE; /* default */
      e.expire[0] = 0;
      e.expire[MAX_HSTS_DATELEN] = 0;
      e.name[0] = 0; /* to make it clean */
      e.name[MAX_HSTS_HOSTLEN] = 0;
      sc = तथ्य->set.hsts_read(तथ्य, &e, तथ्य->set.hsts_read_userp);
      अगर(sc == CURLSTS_OK) {
        CURLcode परिणाम;
        स्थिर अक्षर *date = e.expire;
        अगर(!e.name[0] || e.expire[MAX_HSTS_DATELEN] ||
           e.name[MAX_HSTS_HOSTLEN])
          /* bail out if no name was stored or if a null-terminator is gone */
          वापस CURLE_BAD_FUNCTION_ARGUMENT;
        अगर(!date[0])
          date = UNLIMITED;
        परिणाम = hsts_add_host_expire(h, e.name, म_माप(e.name),
                                      date, म_माप(date),
                                      /* bitfield to bool conversion: */
                                      e.includeSubDomains ? TRUE : FALSE);
        अगर(परिणाम)
          वापस परिणाम;
      }
      अन्यथा अगर(sc == CURLSTS_FAIL)
        वापस CURLE_ABORTED_BY_CALLBACK;
    } जबतक(sc == CURLSTS_OK);
  }
  वापस CURLE_OK;
}

/*
 * Load the HSTS cache from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/hsts.html
 *
 * This function only returns error on major problems that prevent hsts
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
अटल CURLcode hsts_load(काष्ठा hsts *h, स्थिर अक्षर *file)
{
  CURLcode परिणाम = CURLE_OK;
  खाता *fp;

  /* we need a private copy of the filename so that the hsts cache file
     name survives an easy handle reset */
  curlx_free(h->filename);
  h->filename = curlx_strdup(file);
  अगर(!h->filename)
    वापस CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  अगर(fp) {
    curlx_struct_stat stat;
    अगर((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      काष्ठा dynbuf buf;
      bool eof = FALSE;
      curlx_dyn_init(&buf, MAX_HSTS_पंक्ति);
      करो {
        परिणाम = Curl_get_पंक्ति(&buf, fp, &eof);
        अगर(!परिणाम) {
          स्थिर अक्षर *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);

          /* Skip empty or commented lines, since we know the line will have
             a trailing newline from Curl_get_line we can treat length 1 as
             empty. */
          अगर((*lineptr == '#') || म_माप(lineptr) <= 1)
            जारी;

          hsts_add(h, lineptr);
        }
      } जबतक(!परिणाम && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  वापस परिणाम;
}

/*
 * Curl_hsts_loadfile() loads HSTS from file
 */
CURLcode Curl_hsts_loadfile(काष्ठा Curl_easy *तथ्य,
                            काष्ठा hsts *h, स्थिर अक्षर *file)
{
  DEBUGASSERT(h);
  (व्योम)तथ्य;
  वापस hsts_load(h, file);
}

/*
 * Curl_hsts_loadcb() loads HSTS from callback
 */
CURLcode Curl_hsts_loadcb(काष्ठा Curl_easy *तथ्य, काष्ठा hsts *h)
{
  अगर(h)
    वापस hsts_pull(तथ्य, h);
  वापस CURLE_OK;
}

CURLcode Curl_hsts_loadfiles(काष्ठा Curl_easy *तथ्य)
{
  CURLcode परिणाम = CURLE_OK;
  काष्ठा curl_slist *l = तथ्य->state.hstslist;
  अगर(l) {
    Curl_share_lock(तथ्य, CURL_LOCK_तथ्य_HSTS, CURL_LOCK_ACCESS_SINGLE);

    जबतक(l) {
      परिणाम = Curl_hsts_loadfile(तथ्य, तथ्य->hsts, l->तथ्य);
      अगर(परिणाम)
        अवरोध;
      l = l->अग्र;
    }
    Curl_share_unlock(तथ्य, CURL_LOCK_तथ्य_HSTS);
  }
  वापस परिणाम;
}

bool Curl_hsts_applies(काष्ठा hsts *h, स्थिर काष्ठा Curl_peer *dest)
{
  वापस !!hsts_check(h, dest->hostname, म_माप(dest->hostname), TRUE);
}

#अगर defined(DEBUGBUILD) || defined(UNITTESTS)
#अघोषित समय
#पूर्ण_अगर

#पूर्ण_अगर /* CURL_DISABLE_HTTP || CURL_DISABLE_HSTS */
