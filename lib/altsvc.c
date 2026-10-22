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
#അന്തര്ഭാവ "curl_setup.h"

#എങ്കിൽ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "altsvc.h"
#അന്തര്ഭാവ "curl_fopen.h"
#അന്തര്ഭാവ "curl_get_line.h"
#അന്തര്ഭാവ "parsedate.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "connect.h"

#define MAX_ALTSVC_വരി    4095
#define MAX_ALTSVC_DATELEN 17
#define MAX_ALTSVC_HOSTLEN 2048
#define MAX_ALTSVC_ALPNLEN 10

#define H3VERSION "h3"

#എങ്കിൽ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
സ്ഥിതം time_t altsvc_debugtime(ശൂന്യം *unused)
{
  സ്ഥിരം അക്ഷരം *timestr = getenv("CURL_TIME");
  (ശൂന്യം)unused;
  എങ്കിൽ(timestr) {
    curl_off_t val;
    curlx_str_സംഖ്യ(&timestr, &val, TIME_T_MAX);
    തിരികെ (time_t)val;
  }
  തിരികെ time(ശൂന്യ);
}
#undef time
#define time(x) altsvc_debugtime(x)
#endif

/* Given the ALPN ID, return the name */
സ്ഥിരം അക്ഷരം *Curl_alpnid2str(എണ്ണൽ alpnid id)
{
  തിരഞ്ഞെടുപ്പ്(id) {
  അവസ്ഥ ALPN_h1:
    തിരികെ "h1";
  അവസ്ഥ ALPN_h2:
    തിരികെ "h2";
  അവസ്ഥ ALPN_h3:
    തിരികെ H3VERSION;
  സ്ഥിരസ്ഥിതി:
    തിരികെ ""; /* bad */
  }
}

സ്ഥിതം എണ്ണൽ alpnid Curl_str2alpnid(സ്ഥിരം ഘടന Curl_str *cstr)
{
  തിരികെ Curl_alpn2alpnid((സ്ഥിരം unsigned അക്ഷരം *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#define altsvc_free(x) curlx_free(x)

സ്ഥിതം ഘടന altsvc *altsvc_createid(സ്ഥിരം അക്ഷരം *srchost,
                                      size_t hlen,
                                      സ്ഥിരം അക്ഷരം *dsthost,
                                      size_t dlen, /* dsthost length */
                                      എണ്ണൽ alpnid srcalpnid,
                                      എണ്ണൽ alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ഘടന altsvc *as;
  എങ്കിൽ((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  എങ്കിൽ((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  എങ്കിൽ(!hlen || !dlen)
    /* bad input */
    തിരികെ ശൂന്യ;
  /* struct size plus both strings */
  as = curlx_calloc(1, വലുപ്പം(ഘടന altsvc) + (hlen + 1) + (dlen + 1));
  എങ്കിൽ(!as)
    തിരികെ ശൂന്യ;
  as->src.host = (അക്ഷരം *)as + വലുപ്പം(ഘടന altsvc);
  memcpy(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (അക്ഷരം *)as + വലുപ്പം(ഘടന altsvc) + hlen + 1;
  memcpy(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (unsigned short)srcport;
  as->dst.port = (unsigned short)dstport;

  തിരികെ as;
}

സ്ഥിതം ഘടന altsvc *altsvc_create(ഘടന Curl_str *srchost,
                                    ഘടന Curl_str *dsthost,
                                    ഘടന Curl_str *srcalpn,
                                    ഘടന Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  എണ്ണൽ alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  എണ്ണൽ alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  എങ്കിൽ(!srcalpnid || !dstalpnid)
    തിരികെ ശൂന്യ;
  തിരികെ altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
സ്ഥിതം ശൂന്യം altsvc_append(ഘടന altsvcinfo *asi, ഘടന altsvc *as)
{
  ആകുമ്പോൾ(Curl_llist_എണ്ണം(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ഘടന Curl_llist_കണ്ണി *e = Curl_llist_head(&asi->list);
    ഘടന altsvc *oldas = Curl_കണ്ണി_elem(e);
    Curl_കണ്ണി_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->കണ്ണി);
}

/* only returns SERIOUS errors */
സ്ഥിതം CURLcode altsvc_add(ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *വരി)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ഘടന Curl_str srchost;
  ഘടന Curl_str dsthost;
  ഘടന Curl_str srcalpn;
  ഘടന Curl_str dstalpn;
  ഘടന Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  എങ്കിൽ(curlx_str_word(&വരി, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_word(&വരി, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_സംഖ്യ(&വരി, &srcport, 65535) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_word(&വരി, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_word(&വരി, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_സംഖ്യ(&വരി, &dstport, 65535) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_quotedword(&വരി, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_സംഖ്യ(&വരി, &persist, 1) ||
     curlx_str_singlespace(&വരി) ||
     curlx_str_സംഖ്യ(&വരി, &prio, 0) ||
     curlx_str_newline(&വരി))
    ;
  അല്ലെങ്കിൽ {
    അക്ഷരം dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = time(ശൂന്യ);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    memcpy(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    എങ്കിൽ(now < expires) {
      ഘടന altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      എങ്കിൽ(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_OUT_OF_MEMORY;
    }
  }

  തിരികെ CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
സ്ഥിതം CURLcode altsvc_load(ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *file)
{
  CURLcode result = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  എങ്കിൽ(!asi->filename)
    തിരികെ CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  എങ്കിൽ(fp) {
    curlx_struct_stat stat;
    എങ്കിൽ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      ഘടന dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_വരി);
      ചെയ്യുക {
        result = Curl_get_വരി(&buf, fp, &eof);
        എങ്കിൽ(!result) {
          സ്ഥിരം അക്ഷരം *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          എങ്കിൽ(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } ആകുമ്പോൾ(!result && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  തിരികെ result;
}

/*
 * Write this single altsvc entry to a single output line
 */
സ്ഥിതം CURLcode altsvc_out(ഘടന altsvc *as, FILE *fp)
{
  ഘടന tm stamp;
  സ്ഥിരം അക്ഷരം *dst6_pre = "";
  സ്ഥിരം അക്ഷരം *dst6_post = "";
  സ്ഥിരം അക്ഷരം *src6_pre = "";
  സ്ഥിരം അക്ഷരം *src6_post = "";
  CURLcode result = curlx_gmtime(as->expires, &stamp);
  എങ്കിൽ(result)
    /* fail silently for date errors so that only this entry gets
       skipped */
    തിരികെ CURLE_OK;
#ifdef USE_IPV6
  അല്ലെങ്കിൽ {
    അക്ഷരം ipv6_unused[16];
    എങ്കിൽ(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    എങ്കിൽ(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
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
  തിരികെ CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ഘടന altsvcinfo *Curl_altsvc_init(ശൂന്യം)
{
  ഘടന altsvcinfo *asi = curlx_calloc(1, വലുപ്പം(ഘടന altsvcinfo));
  എങ്കിൽ(!asi)
    തിരികെ ശൂന്യ;
  Curl_llist_init(&asi->list, ശൂന്യ);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ifdef USE_HTTP2
    | CURLALTSVC_H2
#endif
#ifdef USE_HTTP3
    | CURLALTSVC_H3
#endif
    ;
  തിരികെ asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *file)
{
  DEBUGASSERT(asi);
  തിരികെ altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ഘടന Curl_easy *data, സ്ഥിരം long ctrl)
{
  DEBUGASSERT(data);
  എങ്കിൽ(!ctrl)
    തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;

  എങ്കിൽ(!data->asi) {
    data->asi = Curl_altsvc_init();
    എങ്കിൽ(!data->asi)
      തിരികെ CURLE_OUT_OF_MEMORY;
  }
  data->asi->flags = ctrl;
  തിരികെ CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
ശൂന്യം Curl_altsvc_cleanup(ഘടന altsvcinfo **asi)
{
  എങ്കിൽ(*asi) {
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_llist_കണ്ണി *n;
    ഘടന altsvcinfo *altsvc = *asi;
    വേണ്ടി(e = Curl_llist_head(&altsvc->list); e; e = n) {
      ഘടന altsvc *as = Curl_കണ്ണി_elem(e);
      n = Curl_കണ്ണി_next(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ശൂന്യ; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ഘടന Curl_easy *data,
                          ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *file)
{
  CURLcode result = CURLE_OK;
  FILE *out;
  അക്ഷരം *tempstore = ശൂന്യ;

  എങ്കിൽ(!asi)
    /* no cache activated */
    തിരികെ CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  എങ്കിൽ(!file && asi->filename)
    file = asi->filename;

  എങ്കിൽ((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    തിരികെ CURLE_OK;

  result = Curl_fopen(data, file, &out, &tempstore);
  എങ്കിൽ(!result) {
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_llist_കണ്ണി *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    വേണ്ടി(e = Curl_llist_head(&asi->list); e; e = n) {
      ഘടന altsvc *as = Curl_കണ്ണി_elem(e);
      n = Curl_കണ്ണി_next(e);
      result = altsvc_out(as, out);
      എങ്കിൽ(result)
        നിർത്തുക;
    }
    curlx_fclose(out);
    എങ്കിൽ(!result && tempstore && curlx_rename(tempstore, file))
      result = CURLE_WRITE_ERROR;

    എങ്കിൽ(result && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  തിരികെ result;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
സ്ഥിതം bool hostcompare(സ്ഥിരം അക്ഷരം *host, സ്ഥിരം അക്ഷരം *check)
{
  size_t hlen = strlen(host);
  size_t clen = strlen(check);

  എങ്കിൽ(hlen && (host[hlen - 1] == '.'))
    hlen--;
  എങ്കിൽ(hlen != clen)
    /* they cannot match if they have different lengths */
    തിരികെ FALSE;
  തിരികെ curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
സ്ഥിതം ശൂന്യം altsvc_flush(ഘടന altsvcinfo *asi,
                         ഘടന Curl_peer *origin,
                         എണ്ണൽ alpnid origin_alpnid)
{
  ഘടന Curl_llist_കണ്ണി *e;
  ഘടന Curl_llist_കണ്ണി *n;
  വേണ്ടി(e = Curl_llist_head(&asi->list); e; e = n) {
    ഘടന altsvc *as = Curl_കണ്ണി_elem(e);
    n = Curl_കണ്ണി_next(e);
    എങ്കിൽ((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_കണ്ണി_remove(e);
      altsvc_free(as);
    }
  }
}

#define CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#define ALTSVC_MA      1
#define ALTSVC_PERSIST 2
സ്ഥിതം ശൂന്യം altsvc_parse_params(സ്ഥിരം അക്ഷരം **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  എങ്കിൽ(curlx_str_single(pp, ';'))
    തിരികെ;

  വേണ്ടി(;;) {
    ഘടന Curl_str name;
    ഘടന Curl_str val;
    curl_off_t അക്കം;
    പൂർണ്ണസംഖ്യ keyword = 0;

    /* allow some extra whitespaces around name and value */
    എങ്കിൽ(curlx_str_until(pp, &name, MAX_ALTSVC_വരി, '=') ||
       curlx_str_single(pp, '='))
      നിർത്തുക;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    എങ്കിൽ(**pp == '\"') {
      എങ്കിൽ(curlx_str_quotedword(pp, &val, MAX_ALTSVC_വരി))
        നിർത്തുക;
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(curlx_str_cspn(pp, &val, ",;\r\n"))
        നിർത്തുക;
    }
    curlx_str_trimblanks(&val);

    എങ്കിൽ(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    അല്ലെങ്കിൽ എങ്കിൽ(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    എങ്കിൽ(keyword) {
      സ്ഥിരം അക്ഷരം *vp = curlx_str(&val);
      സ്ഥിരം അക്ഷരം *vend = vp + curlx_strlen(&val);
      എങ്കിൽ(curlx_str_സംഖ്യ(&vp, &അക്കം, CURL_OFF_T_MAX))
        നിർത്തുക; /* not a number, skip further parameter parsing */
      എങ്കിൽ(vp != vend)
        നിർത്തുക; /* not entirely a number, skip further parameter parsing */
      എങ്കിൽ(keyword == ALTSVC_MA) {
        എങ്കിൽ(അക്കം > CAP_ALTSVC_MAX_AGE)
          അക്കം = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)അക്കം;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(അക്കം == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    എങ്കിൽ(curlx_str_single(pp, ';'))
      നിർത്തുക; /* no further parameters */
  }
}

സ്ഥിതം bool altsvc_parse_dest(സ്ഥിരം അക്ഷരം **pp,
                              ഘടന Curl_easy *data,
                              ഘടന Curl_peer *origin,
                              ഘടന Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  എങ്കിൽ(curlx_str_single(pp, '\"'))
    തിരികെ FALSE;

  /* quoted string, with hostname or just :port ? */
  എങ്കിൽ(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    എങ്കിൽ(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      എങ്കിൽ(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(data, "Bad alt-svc hostname, ignoring.");
        തിരികെ FALSE;
      }
    }
    അല്ലെങ്കിൽ { /* IPv6 hostname */
      എങ്കിൽ(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(data, "Bad alt-svc IPv6 hostname, ignoring.");
        തിരികെ FALSE;
      }
    }
    എങ്കിൽ(curlx_str_single(pp, ':'))
      തിരികെ FALSE; /* not followed by ':' */
  }
  അല്ലെങ്കിൽ  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     strlen(origin->hostname));

  എങ്കിൽ(curlx_str_സംഖ്യ(pp, &port, 0xffff)) {
    infof(data, "Unknown alt-svc port number, ignoring.");
    തിരികെ FALSE;
  }

  *pdstport = (uint16_t)port;
  എങ്കിൽ(curlx_str_single(pp, '\"'))
    തിരികെ FALSE; /* quoted string not ending here as expected */
  തിരികെ TRUE;
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
CURLcode Curl_altsvc_parse(ഘടന Curl_easy *data,
                           ഘടന altsvcinfo *asi, സ്ഥിരം അക്ഷരം *മൂല്യം,
                           ഘടന Curl_peer *origin,
                           എണ്ണൽ alpnid origin_alpnid)
{
  ഘടന altsvc *as;
  size_t entries = 0;
  ഘടന Curl_str alpn;
  സ്ഥിരം അക്ഷരം *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = മൂല്യം;
  എങ്കിൽ(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    എങ്കിൽ(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      തിരികെ CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  വേണ്ടി(p = മൂല്യം; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    എണ്ണൽ alpnid dstalpnid;
    ഘടന Curl_str dsthost;
    uint16_t dstport;

    എങ്കിൽ(curlx_str_until(&p, &alpn, MAX_ALTSVC_വരി, '='))
      നിർത്തുക; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    എങ്കിൽ(curlx_str_single(&p, '='))
      നിർത്തുക;

    /* Parse altsvc hostname:port */
    എങ്കിൽ(!altsvc_parse_dest(&p, data, origin, &dsthost, &dstport))
      നിർത്തുക;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    എങ്കിൽ(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      എങ്കിൽ(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, strlen(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      എങ്കിൽ(as) {
        time_t secs = time(ശൂന്യ);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        എങ്കിൽ(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        അല്ലെങ്കിൽ
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(data, "Added alt-svc: %.*s:%u over %s",
              (പൂർണ്ണസംഖ്യ)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      അല്ലെങ്കിൽ
        തിരികെ CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    എങ്കിൽ(curlx_str_single(&p, ','))
      നിർത്തുക;
  }

  തിരികെ CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ഘടന altsvcinfo *asi,
                        ഘടന Curl_peer *origin,
                        എണ്ണൽ alpnid origin_alpnid,
                        ഘടന altsvc **dstentry,
                        സ്ഥിരം പൂർണ്ണസംഖ്യ versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  എങ്കിൽ(Curl_llist_എണ്ണം(&asi->list)) {
    ഘടന Curl_llist_കണ്ണി *e;
    ഘടന Curl_llist_കണ്ണി *n;
    time_t now = time(ശൂന്യ);

    വേണ്ടി(e = Curl_llist_head(&asi->list); e; e = n) {
      ഘടന altsvc *as = Curl_കണ്ണി_elem(e);
      n = Curl_കണ്ണി_next(e);
      എങ്കിൽ(as->expires < now) {
        /* an expired entry, remove */
        Curl_കണ്ണി_remove(e);
        altsvc_free(as);
        തുടരുക;
      }
      എങ്കിൽ((origin_alpnid == as->src.alpnid) &&
         (versions & (പൂർണ്ണസംഖ്യ)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        തിരികെ TRUE;
      }
    }
  }
  തിരികെ FALSE;
}

#എങ്കിൽ defined(DEBUGBUILD) || defined(UNITTESTS)
#undef time
#endif

#endif /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
