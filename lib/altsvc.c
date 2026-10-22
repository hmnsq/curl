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
#समावेश "curl_setup.h"

#अगर !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#समावेश "urldata.h"
#समावेश "altsvc.h"
#समावेश "curl_fopen.h"
#समावेश "curl_get_line.h"
#समावेश "parsedate.h"
#समावेश "curl_trc.h"
#समावेश "curlx/inet_pton.h"
#समावेश "curlx/strparse.h"
#समावेश "connect.h"

#घोषणा MAX_ALTSVC_पंक्ति    4095
#घोषणा MAX_ALTSVC_DATELEN 17
#घोषणा MAX_ALTSVC_HOSTLEN 2048
#घोषणा MAX_ALTSVC_ALPNLEN 10

#घोषणा H3VERSION "h3"

#अगर defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
अटल समय_प्रकार altsvc_debugtime(व्योम *unused)
{
  स्थिर अक्षर *timestr = दो_पर्या("CURL_TIME");
  (व्योम)unused;
  अगर(timestr) {
    curl_off_t val;
    curlx_str_संख्या(&timestr, &val, TIME_T_MAX);
    वापस (समय_प्रकार)val;
  }
  वापस समय(शून्य);
}
#अघोषित समय
#घोषणा समय(x) altsvc_debugtime(x)
#पूर्ण_अगर

/* Given the ALPN ID, return the name */
स्थिर अक्षर *Curl_alpnid2str(क्रमागत alpnid id)
{
  चयन(id) {
  हाल ALPN_h1:
    वापस "h1";
  हाल ALPN_h2:
    वापस "h2";
  हाल ALPN_h3:
    वापस H3VERSION;
  शेष:
    वापस ""; /* bad */
  }
}

अटल क्रमागत alpnid Curl_str2alpnid(स्थिर काष्ठा Curl_str *cstr)
{
  वापस Curl_alpn2alpnid((स्थिर अचिन्हित अक्षर *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#घोषणा altsvc_free(x) curlx_free(x)

अटल काष्ठा altsvc *altsvc_createid(स्थिर अक्षर *srchost,
                                      माप_प्रकार hlen,
                                      स्थिर अक्षर *dsthost,
                                      माप_प्रकार dlen, /* dsthost length */
                                      क्रमागत alpnid srcalpnid,
                                      क्रमागत alpnid dstalpnid,
                                      माप_प्रकार srcport,
                                      माप_प्रकार dstport)
{
  काष्ठा altsvc *as;
  अगर((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  अन्यथा अगर(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  अगर((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  अगर(!hlen || !dlen)
    /* bad input */
    वापस शून्य;
  /* struct size plus both strings */
  as = curlx_calloc(1, माप(काष्ठा altsvc) + (hlen + 1) + (dlen + 1));
  अगर(!as)
    वापस शून्य;
  as->src.host = (अक्षर *)as + माप(काष्ठा altsvc);
  स_नकल(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (अक्षर *)as + माप(काष्ठा altsvc) + hlen + 1;
  स_नकल(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.port = (अचिन्हित लघु)srcport;
  as->dst.port = (अचिन्हित लघु)dstport;

  वापस as;
}

अटल काष्ठा altsvc *altsvc_create(काष्ठा Curl_str *srchost,
                                    काष्ठा Curl_str *dsthost,
                                    काष्ठा Curl_str *srcalpn,
                                    काष्ठा Curl_str *dstalpn,
                                    माप_प्रकार srcport,
                                    माप_प्रकार dstport)
{
  क्रमागत alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  क्रमागत alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  अगर(!srcalpnid || !dstalpnid)
    वापस शून्य;
  वापस altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
   first */
अटल व्योम altsvc_append(काष्ठा altsvcinfo *asi, काष्ठा altsvc *as)
{
  जबतक(Curl_llist_गिनती(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    काष्ठा Curl_llist_गाँठ *e = Curl_llist_शीर्ष(&asi->list);
    काष्ठा altsvc *oldas = Curl_गाँठ_elem(e);
    Curl_गाँठ_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->गाँठ);
}

/* only returns SERIOUS errors */
अटल CURLcode altsvc_add(काष्ठा altsvcinfo *asi, स्थिर अक्षर *पंक्ति)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  काष्ठा Curl_str srchost;
  काष्ठा Curl_str dsthost;
  काष्ठा Curl_str srcalpn;
  काष्ठा Curl_str dstalpn;
  काष्ठा Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  अगर(curlx_str_word(&पंक्ति, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_word(&पंक्ति, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_संख्या(&पंक्ति, &srcport, 65535) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_word(&पंक्ति, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_word(&पंक्ति, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_संख्या(&पंक्ति, &dstport, 65535) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_quotedword(&पंक्ति, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_संख्या(&पंक्ति, &persist, 1) ||
     curlx_str_singlespace(&पंक्ति) ||
     curlx_str_संख्या(&पंक्ति, &prio, 0) ||
     curlx_str_newline(&पंक्ति))
    ;
  अन्यथा {
    अक्षर dbuf[MAX_ALTSVC_DATELEN + 1];
    समय_प्रकार expires = 0;
    समय_प्रकार now = समय(शून्य);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    स_नकल(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    अगर(now < expires) {
      काष्ठा altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (माप_प्रकार)srcport, (माप_प्रकार)dstport);
      अगर(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      अन्यथा
        वापस CURLE_OUT_OF_MEMORY;
    }
  }

  वापस CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
अटल CURLcode altsvc_load(काष्ठा altsvcinfo *asi, स्थिर अक्षर *file)
{
  CURLcode परिणाम = CURLE_OK;
  खाता *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  अगर(!asi->filename)
    वापस CURLE_OUT_OF_MEMORY;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  अगर(fp) {
    curlx_struct_stat stat;
    अगर((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_mode)) {
      bool eof = FALSE;
      काष्ठा dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_पंक्ति);
      करो {
        परिणाम = Curl_get_पंक्ति(&buf, fp, &eof);
        अगर(!परिणाम) {
          स्थिर अक्षर *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          अगर(curlx_str_single(&lineptr, '#'))
            altsvc_add(asi, lineptr);
        }
      } जबतक(!परिणाम && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  वापस परिणाम;
}

/*
 * Write this single altsvc entry to a single output line
 */
अटल CURLcode altsvc_out(काष्ठा altsvc *as, खाता *fp)
{
  काष्ठा पंचांग stamp;
  स्थिर अक्षर *dst6_pre = "";
  स्थिर अक्षर *dst6_post = "";
  स्थिर अक्षर *src6_pre = "";
  स्थिर अक्षर *src6_post = "";
  CURLcode परिणाम = curlx_gmtime(as->expires, &stamp);
  अगर(परिणाम)
    /* fail silently for date errors so that only this entry gets
       skipped */
    वापस CURLE_OK;
#अगर_घोषित USE_IPV6
  अन्यथा {
    अक्षर ipv6_unused[16];
    अगर(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_unused) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    अगर(curlx_inet_pton(AF_INET6, as->src.host, ipv6_unused) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#पूर्ण_अगर
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
  वापस CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
काष्ठा altsvcinfo *Curl_altsvc_init(व्योम)
{
  काष्ठा altsvcinfo *asi = curlx_calloc(1, माप(काष्ठा altsvcinfo));
  अगर(!asi)
    वापस शून्य;
  Curl_llist_init(&asi->list, शून्य);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#अगर_घोषित USE_HTTP2
    | CURLALTSVC_H2
#पूर्ण_अगर
#अगर_घोषित USE_HTTP3
    | CURLALTSVC_H3
#पूर्ण_अगर
    ;
  वापस asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_load(काष्ठा altsvcinfo *asi, स्थिर अक्षर *file)
{
  DEBUGASSERT(asi);
  वापस altsvc_load(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(काष्ठा Curl_easy *तथ्य, स्थिर दीर्घ ctrl)
{
  DEBUGASSERT(तथ्य);
  अगर(!ctrl)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(!तथ्य->asi) {
    तथ्य->asi = Curl_altsvc_init();
    अगर(!तथ्य->asi)
      वापस CURLE_OUT_OF_MEMORY;
  }
  तथ्य->asi->flags = ctrl;
  वापस CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
व्योम Curl_altsvc_cleanup(काष्ठा altsvcinfo **asi)
{
  अगर(*asi) {
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_llist_गाँठ *n;
    काष्ठा altsvcinfo *altsvc = *asi;
    क्रम(e = Curl_llist_शीर्ष(&altsvc->list); e; e = n) {
      काष्ठा altsvc *as = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_अग्र(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = शून्य; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(काष्ठा Curl_easy *तथ्य,
                          काष्ठा altsvcinfo *asi, स्थिर अक्षर *file)
{
  CURLcode परिणाम = CURLE_OK;
  खाता *out;
  अक्षर *tempstore = शून्य;

  अगर(!asi)
    /* no cache activated */
    वापस CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  अगर(!file && asi->filename)
    file = asi->filename;

  अगर((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    वापस CURLE_OK;

  परिणाम = Curl_fopen(तथ्य, file, &out, &tempstore);
  अगर(!परिणाम) {
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_llist_गाँठ *n;
    ख_माला_दो("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          out);
    क्रम(e = Curl_llist_शीर्ष(&asi->list); e; e = n) {
      काष्ठा altsvc *as = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_अग्र(e);
      परिणाम = altsvc_out(as, out);
      अगर(परिणाम)
        अवरोध;
    }
    curlx_fclose(out);
    अगर(!परिणाम && tempstore && curlx_rename(tempstore, file))
      परिणाम = CURLE_WRITE_ERROR;

    अगर(परिणाम && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  वापस परिणाम;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
अटल bool hostcompare(स्थिर अक्षर *host, स्थिर अक्षर *check)
{
  माप_प्रकार hlen = म_माप(host);
  माप_प्रकार clen = म_माप(check);

  अगर(hlen && (host[hlen - 1] == '.'))
    hlen--;
  अगर(hlen != clen)
    /* they cannot match if they have different lengths */
    वापस FALSE;
  वापस curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
अटल व्योम altsvc_flush(काष्ठा altsvcinfo *asi,
                         काष्ठा Curl_peer *origin,
                         क्रमागत alpnid origin_alpnid)
{
  काष्ठा Curl_llist_गाँठ *e;
  काष्ठा Curl_llist_गाँठ *n;
  क्रम(e = Curl_llist_शीर्ष(&asi->list); e; e = n) {
    काष्ठा altsvc *as = Curl_गाँठ_elem(e);
    n = Curl_गाँठ_अग्र(e);
    अगर((origin_alpnid == as->src.alpnid) &&
       (origin->port == as->src.port) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_गाँठ_remove(e);
      altsvc_free(as);
    }
  }
}

#घोषणा CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#घोषणा ALTSVC_MA      1
#घोषणा ALTSVC_PERSIST 2
अटल व्योम altsvc_parse_params(स्थिर अक्षर **pp,
                                समय_प्रकार *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  अगर(curlx_str_single(pp, ';'))
    वापस;

  क्रम(;;) {
    काष्ठा Curl_str name;
    काष्ठा Curl_str val;
    curl_off_t अंक;
    पूर्णांक keyword = 0;

    /* allow some extra whitespaces around name and value */
    अगर(curlx_str_until(pp, &name, MAX_ALTSVC_पंक्ति, '=') ||
       curlx_str_single(pp, '='))
      अवरोध;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    अगर(**pp == '\"') {
      अगर(curlx_str_quotedword(pp, &val, MAX_ALTSVC_पंक्ति))
        अवरोध;
    }
    अन्यथा {
      अगर(curlx_str_cspn(pp, &val, ",;\r\n"))
        अवरोध;
    }
    curlx_str_trimblanks(&val);

    अगर(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    अन्यथा अगर(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    अगर(keyword) {
      स्थिर अक्षर *vp = curlx_str(&val);
      स्थिर अक्षर *vend = vp + curlx_strlen(&val);
      अगर(curlx_str_संख्या(&vp, &अंक, CURL_OFF_T_MAX))
        अवरोध; /* not a number, skip further parameter parsing */
      अगर(vp != vend)
        अवरोध; /* not entirely a number, skip further parameter parsing */
      अगर(keyword == ALTSVC_MA) {
        अगर(अंक > CAP_ALTSVC_MAX_AGE)
          अंक = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (समय_प्रकार)अंक;
      }
      अन्यथा अगर(अंक == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    अगर(curlx_str_single(pp, ';'))
      अवरोध; /* no further parameters */
  }
}

अटल bool altsvc_parse_dest(स्थिर अक्षर **pp,
                              काष्ठा Curl_easy *तथ्य,
                              काष्ठा Curl_peer *origin,
                              काष्ठा Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t port = 0;

  अगर(curlx_str_single(pp, '\"'))
    वापस FALSE;

  /* quoted string, with hostname or just :port ? */
  अगर(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    अगर(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      अगर(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(तथ्य, "Bad alt-svc hostname, ignoring.");
        वापस FALSE;
      }
    }
    अन्यथा { /* IPv6 hostname */
      अगर(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(तथ्य, "Bad alt-svc IPv6 hostname, ignoring.");
        वापस FALSE;
      }
    }
    अगर(curlx_str_single(pp, ':'))
      वापस FALSE; /* not followed by ':' */
  }
  अन्यथा  /* is only :port, hostname is effectively origin */
    curlx_str_assign(dsthost, origin->hostname,
                     म_माप(origin->hostname));

  अगर(curlx_str_संख्या(pp, &port, 0xffff)) {
    infof(तथ्य, "Unknown alt-svc port number, ignoring.");
    वापस FALSE;
  }

  *pdstport = (uint16_t)port;
  अगर(curlx_str_single(pp, '\"'))
    वापस FALSE; /* quoted string not ending here as expected */
  वापस TRUE;
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
CURLcode Curl_altsvc_parse(काष्ठा Curl_easy *तथ्य,
                           काष्ठा altsvcinfo *asi, स्थिर अक्षर *मूल्य,
                           काष्ठा Curl_peer *origin,
                           क्रमागत alpnid origin_alpnid)
{
  काष्ठा altsvc *as;
  माप_प्रकार entries = 0;
  काष्ठा Curl_str alpn;
  स्थिर अक्षर *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = मूल्य;
  अगर(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    अगर(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      वापस CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  क्रम(p = मूल्य; *p;) {
    समय_प्रकार maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    क्रमागत alpnid dstalpnid;
    काष्ठा Curl_str dsthost;
    uint16_t dstport;

    अगर(curlx_str_until(&p, &alpn, MAX_ALTSVC_पंक्ति, '='))
      अवरोध; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    अगर(curlx_str_single(&p, '='))
      अवरोध;

    /* Parse altsvc hostname:port */
    अगर(!altsvc_parse_dest(&p, तथ्य, origin, &dsthost, &dstport))
      अवरोध;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    अगर(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      अगर(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, म_माप(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->port, dstport);
      अगर(as) {
        समय_प्रकार secs = समय(शून्य);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        अगर(maxage > (TIME_T_MAX - secs))
          as->expires = TIME_T_MAX;
        अन्यथा
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(तथ्य, "Added alt-svc: %.*s:%u over %s",
              (पूर्णांक)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      अन्यथा
        वापस CURLE_OUT_OF_MEMORY;
    }

    /* When this is followed by a comma, we expect another entry */
    अगर(curlx_str_single(&p, ','))
      अवरोध;
  }

  वापस CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(काष्ठा altsvcinfo *asi,
                        काष्ठा Curl_peer *origin,
                        क्रमागत alpnid origin_alpnid,
                        काष्ठा altsvc **dstentry,
                        स्थिर पूर्णांक versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  अगर(Curl_llist_गिनती(&asi->list)) {
    काष्ठा Curl_llist_गाँठ *e;
    काष्ठा Curl_llist_गाँठ *n;
    समय_प्रकार now = समय(शून्य);

    क्रम(e = Curl_llist_शीर्ष(&asi->list); e; e = n) {
      काष्ठा altsvc *as = Curl_गाँठ_elem(e);
      n = Curl_गाँठ_अग्र(e);
      अगर(as->expires < now) {
        /* an expired entry, remove */
        Curl_गाँठ_remove(e);
        altsvc_free(as);
        जारी;
      }
      अगर((origin_alpnid == as->src.alpnid) &&
         (versions & (पूर्णांक)as->dst.alpnid) &&
         (origin->port == as->src.port) &&
         hostcompare(origin->hostname, as->src.host)) {
        /* match */
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->port == as->dst.port) &&
                             hostcompare(origin->hostname, as->dst.host);
        वापस TRUE;
      }
    }
  }
  वापस FALSE;
}

#अगर defined(DEBUGBUILD) || defined(UNITTESTS)
#अघोषित समय
#पूर्ण_अगर

#पूर्ण_अगर /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
