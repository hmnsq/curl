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
#شامل "curl_setup.h"

#اگر !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#شامل "urldata.h"
#شامل "altsvc.h"
#شامل "curl_fopen.h"
#شامل "curl_get_line.h"
#شامل "parsedate.h"
#شامل "curl_trc.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/strparse.h"
#شامل "connect.h"

#تعریف_کرو MAX_ALTSVC_سطر    4095
#تعریف_کرو MAX_ALTSVC_DATELEN 17
#تعریف_کرو MAX_ALTSVC_HOSTLEN 2048
#تعریف_کرو MAX_ALTSVC_ALPNLEN 10

#تعریف_کرو H3_نسخہ "h3"

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ساکن time_t altsvc_debugtime(خالی *غیر_مستعمل)
{
  ثابت حرف *timestr = getenv("CURL_TIME");
  (خالی)غیر_مستعمل;
  اگر(timestr) {
    curl_off_t val;
    curlx_str_نمبر(&timestr, &val, وقت_T_MAX);
    واپس (time_t)val;
  }
  واپس وقت(تہی);
}
#تعریف_منسوخ وقت
#تعریف_کرو وقت(x) altsvc_debugtime(x)
#اگر_ختم

/* Given the ALPN ID, return the name */
ثابت حرف *Curl_alpnid2str(شمار alpnid id)
{
  انتخاب(id) {
  حالت ALPN_h1:
    واپس "h1";
  حالت ALPN_h2:
    واپس "h2";
  حالت ALPN_h3:
    واپس H3_نسخہ;
  طےشدہ:
    واپس ""; /* bad */
  }
}

ساکن شمار alpnid Curl_str2alpnid(ثابت ساخت Curl_str *cstr)
{
  واپس Curl_alpn2alpnid((ثابت غیرعلامتی حرف *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#تعریف_کرو altsvc_free(x) curlx_free(x)

ساکن ساخت altsvc *altsvc_createid(ثابت حرف *srchost,
                                      size_t hlen,
                                      ثابت حرف *dsthost,
                                      size_t dlen, /* dsthost length */
                                      شمار alpnid srcalpnid,
                                      شمار alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ساخت altsvc *as;
  اگر((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  ورنہ اگر(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  اگر((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  اگر(!hlen || !dlen)
    /* bad input */
    واپس تہی;
  /* struct size plus both strings */
  as = curlx_calloc(1, حجم(ساخت altsvc) + (hlen + 1) + (dlen + 1));
  اگر(!as)
    واپس تہی;
  as->src.host = (حرف *)as + حجم(ساخت altsvc);
  حافظہ_نقل(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (حرف *)as + حجم(ساخت altsvc) + hlen + 1;
  حافظہ_نقل(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.پورٹ = (غیرعلامتی مختصر)srcport;
  as->dst.پورٹ = (غیرعلامتی مختصر)dstport;

  واپس as;
}

ساکن ساخت altsvc *altsvc_create(ساخت Curl_str *srchost,
                                    ساخت Curl_str *dsthost,
                                    ساخت Curl_str *srcalpn,
                                    ساخت Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  شمار alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  شمار alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  اگر(!srcalpnid || !dstalpnid)
    واپس تہی;
  واپس altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
پہلا*/
ساکن خالی altsvc_append(ساخت altsvcinfo *asi, ساخت altsvc *as)
{
  جب(Curl_llist_گنتی(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ساخت Curl_llist_گرہ *e = Curl_llist_سر(&asi->list);
    ساخت altsvc *oldas = Curl_گرہ_elem(e);
    Curl_گرہ_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->گرہ);
}

/* only returns SERIOUS errors */
ساکن CURLcode altsvc_جوڑو(ساخت altsvcinfo *asi, ثابت حرف *سطر)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ساخت Curl_str srchost;
  ساخت Curl_str dsthost;
  ساخت Curl_str srcalpn;
  ساخت Curl_str dstalpn;
  ساخت Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  اگر(curlx_str_لفظ(&سطر, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_لفظ(&سطر, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_نمبر(&سطر, &srcport, 65535) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_لفظ(&سطر, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_لفظ(&سطر, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_نمبر(&سطر, &dstport, 65535) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_quotedword(&سطر, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_نمبر(&سطر, &persist, 1) ||
     curlx_str_singlespace(&سطر) ||
     curlx_str_نمبر(&سطر, &prio, 0) ||
     curlx_str_newline(&سطر))
    ;
  ورنہ {
    حرف dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = وقت(تہی);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    حافظہ_نقل(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    اگر(now < expires) {
      ساخت altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      اگر(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      ورنہ
        واپس CURLE_برآمد_کا_حافظہ;
    }
  }

  واپس CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ساکن CURLcode altsvc_بھرو(ساخت altsvcinfo *asi, ثابت حرف *file)
{
  CURLcode نتیجہ = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  اگر(!asi->filename)
    واپس CURLE_برآمد_کا_حافظہ;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  اگر(fp) {
    curlx_struct_stat stat;
    اگر((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_منوال)) {
      bool eof = FALSE;
      ساخت dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_سطر);
      کرو {
        نتیجہ = Curl_get_سطر(&buf, fp, &eof);
        اگر(!نتیجہ) {
          ثابت حرف *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          اگر(curlx_str_single(&lineptr, '#'))
            altsvc_جوڑو(asi, lineptr);
        }
      } جب(!نتیجہ && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  واپس نتیجہ;
}

/*
 * Write this single altsvc entry to a single output line
 */
ساکن CURLcode altsvc_برآمد(ساخت altsvc *as, FILE *fp)
{
  ساخت tm stamp;
  ثابت حرف *dst6_pre = "";
  ثابت حرف *dst6_post = "";
  ثابت حرف *src6_pre = "";
  ثابت حرف *src6_post = "";
  CURLcode نتیجہ = curlx_gmtime(as->expires, &stamp);
  اگر(نتیجہ)
    /* fail silently for date errors so that only this entry gets
       skipped */
    واپس CURLE_OK;
#اگر_تعریف_شدہ USE_IPV6
  ورنہ {
    حرف ipv6_غیر_مستعمل[16];
    اگر(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_غیر_مستعمل) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    اگر(curlx_inet_pton(AF_INET6, as->src.host, ipv6_غیر_مستعمل) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#اگر_ختم
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.پورٹ,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.پورٹ,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  واپس CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ساخت altsvcinfo *Curl_altsvc_init(خالی)
{
  ساخت altsvcinfo *asi = curlx_calloc(1, حجم(ساخت altsvcinfo));
  اگر(!asi)
    واپس تہی;
  Curl_llist_init(&asi->list, تہی);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#اگر_تعریف_شدہ USE_HTTP2
    | CURLALTSVC_H2
#اگر_ختم
#اگر_تعریف_شدہ USE_HTTP3
    | CURLALTSVC_H3
#اگر_ختم
    ;
  واپس asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_بھرو(ساخت altsvcinfo *asi, ثابت حرف *file)
{
  DEBUGASSERT(asi);
  واپس altsvc_بھرو(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ساخت Curl_easy *ڈیٹا, ثابت طویل ctrl)
{
  DEBUGASSERT(ڈیٹا);
  اگر(!ctrl)
    واپس CURLE_BAD_FUNCTION_دلیل;

  اگر(!ڈیٹا->asi) {
    ڈیٹا->asi = Curl_altsvc_init();
    اگر(!ڈیٹا->asi)
      واپس CURLE_برآمد_کا_حافظہ;
  }
  ڈیٹا->asi->flags = ctrl;
  واپس CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
خالی Curl_altsvc_cleanup(ساخت altsvcinfo **asi)
{
  اگر(*asi) {
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_llist_گرہ *n;
    ساخت altsvcinfo *altsvc = *asi;
    برائے(e = Curl_llist_سر(&altsvc->list); e; e = n) {
      ساخت altsvc *as = Curl_گرہ_elem(e);
      n = Curl_گرہ_اگلا(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = تہی; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ساخت Curl_easy *ڈیٹا,
                          ساخت altsvcinfo *asi, ثابت حرف *file)
{
  CURLcode نتیجہ = CURLE_OK;
  FILE *برآمد;
  حرف *tempstore = تہی;

  اگر(!asi)
    /* no cache activated */
    واپس CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  اگر(!file && asi->filename)
    file = asi->filename;

  اگر((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    واپس CURLE_OK;

  نتیجہ = Curl_fopen(ڈیٹا, file, &برآمد, &tempstore);
  اگر(!نتیجہ) {
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_llist_گرہ *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          برآمد);
    برائے(e = Curl_llist_سر(&asi->list); e; e = n) {
      ساخت altsvc *as = Curl_گرہ_elem(e);
      n = Curl_گرہ_اگلا(e);
      نتیجہ = altsvc_برآمد(as, برآمد);
      اگر(نتیجہ)
        توڑو;
    }
    curlx_fclose(برآمد);
    اگر(!نتیجہ && tempstore && curlx_rename(tempstore, file))
      نتیجہ = CURLE_WRITE_ERROR;

    اگر(نتیجہ && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  واپس نتیجہ;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ساکن bool hostcompare(ثابت حرف *host, ثابت حرف *check)
{
  size_t hlen = لڑی_لمبائی(host);
  size_t clen = لڑی_لمبائی(check);

  اگر(hlen && (host[hlen - 1] == '.'))
    hlen--;
  اگر(hlen != clen)
    /* they cannot match if they have different lengths */
    واپس FALSE;
  واپس curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ساکن خالی altsvc_flush(ساخت altsvcinfo *asi,
                         ساخت Curl_peer *origin,
                         شمار alpnid origin_alpnid)
{
  ساخت Curl_llist_گرہ *e;
  ساخت Curl_llist_گرہ *n;
  برائے(e = Curl_llist_سر(&asi->list); e; e = n) {
    ساخت altsvc *as = Curl_گرہ_elem(e);
    n = Curl_گرہ_اگلا(e);
    اگر((origin_alpnid == as->src.alpnid) &&
       (origin->پورٹ == as->src.پورٹ) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_گرہ_remove(e);
      altsvc_free(as);
    }
  }
}

#تعریف_کرو CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#تعریف_کرو ALTSVC_MA      1
#تعریف_کرو ALTSVC_PERSIST 2
ساکن خالی altsvc_parse_params(ثابت حرف **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  اگر(curlx_str_single(pp, ';'))
    واپس;

  برائے(;;) {
    ساخت Curl_str name;
    ساخت Curl_str val;
    curl_off_t عدد;
    صحیح keyword = 0;

    /* allow some extra whitespaces around name and value */
    اگر(curlx_str_until(pp, &name, MAX_ALTSVC_سطر, '=') ||
       curlx_str_single(pp, '='))
      توڑو;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    اگر(**pp == '\"') {
      اگر(curlx_str_quotedword(pp, &val, MAX_ALTSVC_سطر))
        توڑو;
    }
    ورنہ {
      اگر(curlx_str_cspn(pp, &val, ",;\r\n"))
        توڑو;
    }
    curlx_str_trimblanks(&val);

    اگر(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    ورنہ اگر(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    اگر(keyword) {
      ثابت حرف *vp = curlx_str(&val);
      ثابت حرف *vend = vp + curlx_strlen(&val);
      اگر(curlx_str_نمبر(&vp, &عدد, CURL_OFF_T_MAX))
        توڑو; /* not a number, skip further parameter parsing */
      اگر(vp != vend)
        توڑو; /* not entirely a number, skip further parameter parsing */
      اگر(keyword == ALTSVC_MA) {
        اگر(عدد > CAP_ALTSVC_MAX_AGE)
          عدد = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)عدد;
      }
      ورنہ اگر(عدد == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    اگر(curlx_str_single(pp, ';'))
      توڑو; /* no further parameters */
  }
}

ساکن bool altsvc_parse_dest(ثابت حرف **pp,
                              ساخت Curl_easy *ڈیٹا,
                              ساخت Curl_peer *origin,
                              ساخت Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t پورٹ = 0;

  اگر(curlx_str_single(pp, '\"'))
    واپس FALSE;

  /* quoted string, with hostname or just :port ? */
  اگر(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    اگر(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      اگر(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(ڈیٹا, "Bad alt-svc hostname, ignoring.");
        واپس FALSE;
      }
    }
    ورنہ { /* IPv6 hostname */
      اگر(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(ڈیٹا, "Bad alt-svc IPv6 hostname, ignoring.");
        واپس FALSE;
      }
    }
    اگر(curlx_str_single(pp, ':'))
      واپس FALSE; /* not followed by ':' */
  }
  ورنہ  /* is only :port, hostname is effectively origin */
    curlx_str_تفویض(dsthost, origin->hostname,
                     لڑی_لمبائی(origin->hostname));

  اگر(curlx_str_نمبر(pp, &پورٹ, 0xffff)) {
    infof(ڈیٹا, "Unknown alt-svc port number, ignoring.");
    واپس FALSE;
  }

  *pdstport = (uint16_t)پورٹ;
  اگر(curlx_str_single(pp, '\"'))
    واپس FALSE; /* quoted string not ending here as expected */
  واپس TRUE;
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
CURLcode Curl_altsvc_parse(ساخت Curl_easy *ڈیٹا,
                           ساخت altsvcinfo *asi, ثابت حرف *قدر,
                           ساخت Curl_peer *origin,
                           شمار alpnid origin_alpnid)
{
  ساخت altsvc *as;
  size_t entries = 0;
  ساخت Curl_str alpn;
  ثابت حرف *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = قدر;
  اگر(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    اگر(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      واپس CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  برائے(p = قدر; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    شمار alpnid dstalpnid;
    ساخت Curl_str dsthost;
    uint16_t dstport;

    اگر(curlx_str_until(&p, &alpn, MAX_ALTSVC_سطر, '='))
      توڑو; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    اگر(curlx_str_single(&p, '='))
      توڑو;

    /* Parse altsvc hostname:port */
    اگر(!altsvc_parse_dest(&p, ڈیٹا, origin, &dsthost, &dstport))
      توڑو;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    اگر(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      اگر(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, لڑی_لمبائی(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->پورٹ, dstport);
      اگر(as) {
        time_t secs = وقت(تہی);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        اگر(maxage > (وقت_T_MAX - secs))
          as->expires = وقت_T_MAX;
        ورنہ
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(ڈیٹا, "Added alt-svc: %.*s:%u over %s",
              (صحیح)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      ورنہ
        واپس CURLE_برآمد_کا_حافظہ;
    }

    /* When this is followed by a comma, we expect another entry */
    اگر(curlx_str_single(&p, ','))
      توڑو;
  }

  واپس CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ساخت altsvcinfo *asi,
                        ساخت Curl_peer *origin,
                        شمار alpnid origin_alpnid,
                        ساخت altsvc **dstentry,
                        ثابت صحیح versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  اگر(Curl_llist_گنتی(&asi->list)) {
    ساخت Curl_llist_گرہ *e;
    ساخت Curl_llist_گرہ *n;
    time_t now = وقت(تہی);

    برائے(e = Curl_llist_سر(&asi->list); e; e = n) {
      ساخت altsvc *as = Curl_گرہ_elem(e);
      n = Curl_گرہ_اگلا(e);
      اگر(as->expires < now) {
        /* an expired entry, remove */
        Curl_گرہ_remove(e);
        altsvc_free(as);
        جاری;
      }
      اگر((origin_alpnid == as->src.alpnid) &&
         (versions & (صحیح)as->dst.alpnid) &&
         (origin->پورٹ == as->src.پورٹ) &&
         hostcompare(origin->hostname, as->src.host)) {
        /*مطابقت*/
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->پورٹ == as->dst.پورٹ) &&
                             hostcompare(origin->hostname, as->dst.host);
        واپس TRUE;
      }
    }
  }
  واپس FALSE;
}

#اگر defined(DEBUGBUILD) || defined(UNITTESTS)
#تعریف_منسوخ وقت
#اگر_ختم

#اگر_ختم /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
