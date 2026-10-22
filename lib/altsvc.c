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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಆದರೆ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_ALTSVC)
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "altsvc.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "connect.h"

#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ALTSVC_ಸಾಲು    4095
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ALTSVC_DATELEN 17
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ALTSVC_HOSTLEN 2048
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ALTSVC_ALPNLEN 10

#ವ್ಯಾಖ್ಯಾನಿಸು H3_ಆವೃತ್ತಿ "h3"

#ಆದರೆ defined(DEBUGBUILD) || defined(UNITTESTS)
/* to play well with debug builds, we can *set* a fixed time this will
   return */
ಸ್ಥಿರ time_t altsvc_debugtime(ಶೂನ್ಯ *ಬಳಸದ)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *timestr = getenv("CURL_TIME");
  (ಶೂನ್ಯ)ಬಳಸದ;
  ಆದರೆ(timestr) {
    curl_off_t val;
    curlx_str_ಸಂಖ್ಯೆ(&timestr, &val, ಸಮಯ_T_MAX);
    ಹಿಂತಿರುಗಿ (time_t)val;
  }
  ಹಿಂತಿರುಗಿ ಸಮಯ(ಖಾಲಿ);
}
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸಮಯ
#ವ್ಯಾಖ್ಯಾನಿಸು ಸಮಯ(x) altsvc_debugtime(x)
#ಆದರೆ_ಅಂತ್ಯ

/* Given the ALPN ID, return the name */
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_alpnid2str(ಎಣಿಕೆ alpnid id)
{
  ಆಯ್ಕೆ(id) {
  ಸ್ಥಿತಿ ALPN_h1:
    ಹಿಂತಿರುಗಿ "h1";
  ಸ್ಥಿತಿ ALPN_h2:
    ಹಿಂತಿರುಗಿ "h2";
  ಸ್ಥಿತಿ ALPN_h3:
    ಹಿಂತಿರುಗಿ H3_ಆವೃತ್ತಿ;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ ""; /* bad */
  }
}

ಸ್ಥಿರ ಎಣಿಕೆ alpnid Curl_str2alpnid(ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_str *cstr)
{
  ಹಿಂತಿರುಗಿ Curl_alpn2alpnid((ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)curlx_str(cstr),
                          curlx_strlen(cstr));
}

#ವ್ಯಾಖ್ಯಾನಿಸು altsvc_free(x) curlx_free(x)

ಸ್ಥಿರ ರಚನೆ altsvc *altsvc_createid(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *srchost,
                                      size_t hlen,
                                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dsthost,
                                      size_t dlen, /* dsthost length */
                                      ಎಣಿಕೆ alpnid srcalpnid,
                                      ಎಣಿಕೆ alpnid dstalpnid,
                                      size_t srcport,
                                      size_t dstport)
{
  ರಚನೆ altsvc *as;
  ಆದರೆ((hlen > 2) && srchost[0] == '[') {
    /* IPv6 address, strip off brackets */
    srchost++;
    hlen -= 2;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(hlen && (srchost[hlen - 1] == '.')) {
    /* strip off trailing dot */
    hlen--;
  }
  ಆದರೆ((dlen > 2) && dsthost[0] == '[') {
    /* IPv6 address, strip off brackets */
    dsthost++;
    dlen -= 2;
  }
  ಆದರೆ(!hlen || !dlen)
    /* bad input */
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  /* struct size plus both strings */
  as = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ altsvc) + (hlen + 1) + (dlen + 1));
  ಆದರೆ(!as)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  as->src.host = (ಅಕ್ಷರ *)as + ಗಾತ್ರ(ರಚನೆ altsvc);
  ಸ್ಮರಣೆ_ನಕಲು(as->src.host, srchost, hlen);
  /* the null-terminator is already there */

  as->dst.host = (ಅಕ್ಷರ *)as + ಗಾತ್ರ(ರಚನೆ altsvc) + hlen + 1;
  ಸ್ಮರಣೆ_ನಕಲು(as->dst.host, dsthost, dlen);
  /* the null-terminator is already there */

  as->src.alpnid = srcalpnid;
  as->dst.alpnid = dstalpnid;
  as->src.ಪೋರ್ಟ್ = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)srcport;
  as->dst.ಪೋರ್ಟ್ = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)dstport;

  ಹಿಂತಿರುಗಿ as;
}

ಸ್ಥಿರ ರಚನೆ altsvc *altsvc_create(ರಚನೆ Curl_str *srchost,
                                    ರಚನೆ Curl_str *dsthost,
                                    ರಚನೆ Curl_str *srcalpn,
                                    ರಚನೆ Curl_str *dstalpn,
                                    size_t srcport,
                                    size_t dstport)
{
  ಎಣಿಕೆ alpnid dstalpnid = Curl_str2alpnid(dstalpn);
  ಎಣಿಕೆ alpnid srcalpnid = Curl_str2alpnid(srcalpn);
  ಆದರೆ(!srcalpnid || !dstalpnid)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  ಹಿಂತಿರುಗಿ altsvc_createid(curlx_str(srchost), curlx_strlen(srchost),
                         curlx_str(dsthost), curlx_strlen(dsthost),
                         srcalpnid, dstalpnid,
                         srcport, dstport);
}

/* append the new entry to the list after possibly removing an old entry
ಮೊದಲ*/
ಸ್ಥಿರ ಶೂನ್ಯ altsvc_append(ರಚನೆ altsvcinfo *asi, ರಚನೆ altsvc *as)
{
  ಆಗ(Curl_llist_ಎಣಿಕೆ(&asi->list) >= MAX_ALTSVC_ENTRIES) {
    /* It is full. Remove the first entry in the list */
    ರಚನೆ Curl_llist_ಗಂಟು *e = Curl_llist_ತಲೆ(&asi->list);
    ರಚನೆ altsvc *oldas = Curl_ಗಂಟು_elem(e);
    Curl_ಗಂಟು_remove(e);
    altsvc_free(oldas);
  }
  Curl_llist_append(&asi->list, as, &as->ಗಂಟು);
}

/* only returns SERIOUS errors */
ಸ್ಥಿರ CURLcode altsvc_ಕೂಡಿಸು(ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಸಾಲು)
{
  /* Example line:
     h2 example.com 443 h3 shiny.example.com 8443 "20191231 10:00:00" 1
   */
  ರಚನೆ Curl_str srchost;
  ರಚನೆ Curl_str dsthost;
  ರಚನೆ Curl_str srcalpn;
  ರಚನೆ Curl_str dstalpn;
  ರಚನೆ Curl_str date;
  curl_off_t srcport;
  curl_off_t dstport;
  curl_off_t persist;
  curl_off_t prio;

  ಆದರೆ(curlx_str_ಶಬ್ದ(&ಸಾಲು, &srcalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಶಬ್ದ(&ಸಾಲು, &srchost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &srcport, 65535) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಶಬ್ದ(&ಸಾಲು, &dstalpn, MAX_ALTSVC_ALPNLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಶಬ್ದ(&ಸಾಲು, &dsthost, MAX_ALTSVC_HOSTLEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &dstport, 65535) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_quotedword(&ಸಾಲು, &date, MAX_ALTSVC_DATELEN) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &persist, 1) ||
     curlx_str_singlespace(&ಸಾಲು) ||
     curlx_str_ಸಂಖ್ಯೆ(&ಸಾಲು, &prio, 0) ||
     curlx_str_newline(&ಸಾಲು))
    ;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಅಕ್ಷರ dbuf[MAX_ALTSVC_DATELEN + 1];
    time_t expires = 0;
    time_t now = ಸಮಯ(ಖಾಲಿ);

    /* The date parser works on a null-terminated string. The maximum length
       is upheld by curlx_str_quotedword(). */
    ಸ್ಮರಣೆ_ನಕಲು(dbuf, curlx_str(&date), curlx_strlen(&date));
    dbuf[curlx_strlen(&date)] = 0;
    Curl_getdate_capped(dbuf, &expires);

    ಆದರೆ(now < expires) {
      ರಚನೆ altsvc *as = altsvc_create(&srchost, &dsthost, &srcalpn, &dstalpn,
                                        (size_t)srcport, (size_t)dstport);
      ಆದರೆ(as) {
        as->expires = expires;
        as->persist = persist ? 1 : 0;
        altsvc_append(asi, as);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Load alt-svc entries from the given file. The text based line-oriented file
 * format is documented here: https://curl.se/docs/alt-svc.html
 *
 * This function only returns error on major problems that prevent alt-svc
 * handling to work completely. It will ignore individual syntactical errors
 * etc.
 */
ಸ್ಥಿರ CURLcode altsvc_ತುಂಬು(ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  FILE *fp;

  /* we need a private copy of the filename so that the altsvc cache file
     name survives an easy handle reset */
  curlx_free(asi->filename);
  asi->filename = curlx_strdup(file);
  ಆದರೆ(!asi->filename)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  fp = curlx_fopen(file, FOPEN_READTEXT);
  ಆದರೆ(fp) {
    curlx_struct_stat stat;
    ಆದರೆ((curlx_fstat(fileno(fp), &stat) == -1) || !S_ISDIR(stat.st_ಬಹುಲಕ)) {
      bool eof = FALSE;
      ರಚನೆ dynbuf buf;
      curlx_dyn_init(&buf, MAX_ALTSVC_ಸಾಲು);
      ಮಾಡು {
        ಫಲಿತಾಂಶ = Curl_get_ಸಾಲು(&buf, fp, &eof);
        ಆದರೆ(!ಫಲಿತಾಂಶ) {
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lineptr = curlx_dyn_ptr(&buf);
          curlx_str_passblanks(&lineptr);
          ಆದರೆ(curlx_str_single(&lineptr, '#'))
            altsvc_ಕೂಡಿಸು(asi, lineptr);
        }
      } ಆಗ(!ಫಲಿತಾಂಶ && !eof);
      curlx_dyn_free(&buf); /* free the line buffer */
    }
    curlx_fclose(fp);
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Write this single altsvc entry to a single output line
 */
ಸ್ಥಿರ CURLcode altsvc_ಹೊರಹೋಗು(ರಚನೆ altsvc *as, FILE *fp)
{
  ರಚನೆ tm stamp;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dst6_pre = "";
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dst6_post = "";
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src6_pre = "";
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src6_post = "";
  CURLcode ಫಲಿತಾಂಶ = curlx_gmtime(as->expires, &stamp);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    /* fail silently for date errors so that only this entry gets
       skipped */
    ಹಿಂತಿರುಗಿ CURLE_OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_IPV6
  ಇಲ್ಲದಿದ್ದರೆ {
    ಅಕ್ಷರ ipv6_ಬಳಸದ[16];
    ಆದರೆ(curlx_inet_pton(AF_INET6, as->dst.host, ipv6_ಬಳಸದ) == 1) {
      dst6_pre = "[";
      dst6_post = "]";
    }
    ಆದರೆ(curlx_inet_pton(AF_INET6, as->src.host, ipv6_ಬಳಸದ) == 1) {
      src6_pre = "[";
      src6_post = "]";
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  curl_mfprintf(fp,
                "%s %s%s%s %u "
                "%s %s%s%s %u "
                "\"%d%02d%02d "
                "%02d:%02d:%02d\" "
                "%d 0\n", /* prio still always zero */
                Curl_alpnid2str(as->src.alpnid),
                src6_pre, as->src.host, src6_post,
                as->src.ಪೋರ್ಟ್,

                Curl_alpnid2str(as->dst.alpnid),
                dst6_pre, as->dst.host, dst6_post,
                as->dst.ಪೋರ್ಟ್,

                stamp.tm_year + 1900, stamp.tm_mon + 1, stamp.tm_mday,
                stamp.tm_hour, stamp.tm_min, stamp.tm_sec,
                as->persist);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/* ---- library-wide functions below ---- */

/*
 * Curl_altsvc_init() creates a new altsvc cache.
 * It returns the new instance or NULL if something goes wrong.
 */
ರಚನೆ altsvcinfo *Curl_altsvc_init(ಶೂನ್ಯ)
{
  ರಚನೆ altsvcinfo *asi = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ altsvcinfo));
  ಆದರೆ(!asi)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  Curl_llist_init(&asi->list, ಖಾಲಿ);

  /* set default behavior */
  asi->flags = CURLALTSVC_H1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP2
    | CURLALTSVC_H2
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_HTTP3
    | CURLALTSVC_H3
#ಆದರೆ_ಅಂತ್ಯ
    ;
  ಹಿಂತಿರುಗಿ asi;
}

/*
 * Curl_altsvc_load() loads alt-svc from file.
 */
CURLcode Curl_altsvc_ತುಂಬು(ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  DEBUGASSERT(asi);
  ಹಿಂತಿರುಗಿ altsvc_ತುಂಬು(asi, file);
}

/*
 * Curl_altsvc_ctrl() passes on the external bitmask.
 */
CURLcode Curl_altsvc_ctrl(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ದೀರ್ಘ ctrl)
{
  DEBUGASSERT(ದತ್ತಾಂಶ);
  ಆದರೆ(!ctrl)
    ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;

  ಆದರೆ(!ದತ್ತಾಂಶ->asi) {
    ದತ್ತಾಂಶ->asi = Curl_altsvc_init();
    ಆದರೆ(!ದತ್ತಾಂಶ->asi)
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ದತ್ತಾಂಶ->asi->flags = ctrl;
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Curl_altsvc_cleanup() frees an altsvc cache instance and all associated
 * resources.
 */
ಶೂನ್ಯ Curl_altsvc_cleanup(ರಚನೆ altsvcinfo **asi)
{
  ಆದರೆ(*asi) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_llist_ಗಂಟು *n;
    ರಚನೆ altsvcinfo *altsvc = *asi;
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&altsvc->list); e; e = n) {
      ರಚನೆ altsvc *as = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      altsvc_free(as);
    }
    curlx_free(altsvc->filename);
    curlx_free(altsvc);
    *asi = ಖಾಲಿ; /* clear the pointer */
  }
}

/*
 * Curl_altsvc_save() writes the altsvc cache to a file.
 */
CURLcode Curl_altsvc_save(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *file)
{
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  FILE *ಹೊರಹೋಗು;
  ಅಕ್ಷರ *tempstore = ಖಾಲಿ;

  ಆದರೆ(!asi)
    /* no cache activated */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  /* if not new name is given, use the one we stored from the load */
  ಆದರೆ(!file && asi->filename)
    file = asi->filename;

  ಆದರೆ((asi->flags & CURLALTSVC_READONLYFILE) || !file || !file[0])
    /* marked as read-only, no file or zero length filename */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  ಫಲಿತಾಂಶ = Curl_fopen(ದತ್ತಾಂಶ, file, &ಹೊರಹೋಗು, &tempstore);
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_llist_ಗಂಟು *n;
    fputs("# Your alt-svc cache. https://curl.se/docs/alt-svc.html\n"
          "# This file was generated by libcurl! Edit at your own risk.\n",
          ಹೊರಹೋಗು);
    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&asi->list); e; e = n) {
      ರಚನೆ altsvc *as = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      ಫಲಿತಾಂಶ = altsvc_ಹೊರಹೋಗು(as, ಹೊರಹೋಗು);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ನಿಲ್ಲಿಸು;
    }
    curlx_fclose(ಹೊರಹೋಗು);
    ಆದರೆ(!ಫಲಿತಾಂಶ && tempstore && curlx_rename(tempstore, file))
      ಫಲಿತಾಂಶ = CURLE_WRITE_ERROR;

    ಆದರೆ(ಫಲಿತಾಂಶ && tempstore)
      unlink(tempstore);
  }
  curlx_free(tempstore);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* hostcompare() returns true if 'host' matches 'check'. The first host
 * argument may have a trailing dot present that will be ignored.
 */
ಸ್ಥಿರ bool hostcompare(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *check)
{
  size_t hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(host);
  size_t clen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(check);

  ಆದರೆ(hlen && (host[hlen - 1] == '.'))
    hlen--;
  ಆದರೆ(hlen != clen)
    /* they cannot match if they have different lengths */
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ curl_strnequal(host, check, hlen);
}

/* altsvc_flush() removes all alternatives for this source origin from the
   list */
ಸ್ಥಿರ ಶೂನ್ಯ altsvc_flush(ರಚನೆ altsvcinfo *asi,
                         ರಚನೆ Curl_peer *origin,
                         ಎಣಿಕೆ alpnid origin_alpnid)
{
  ರಚನೆ Curl_llist_ಗಂಟು *e;
  ರಚನೆ Curl_llist_ಗಂಟು *n;
  ಗಾಗಿ(e = Curl_llist_ತಲೆ(&asi->list); e; e = n) {
    ರಚನೆ altsvc *as = Curl_ಗಂಟು_elem(e);
    n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
    ಆದರೆ((origin_alpnid == as->src.alpnid) &&
       (origin->ಪೋರ್ಟ್ == as->src.ಪೋರ್ಟ್) &&
       hostcompare(origin->hostname, as->src.host)) {
      Curl_ಗಂಟು_remove(e);
      altsvc_free(as);
    }
  }
}

#ವ್ಯಾಖ್ಯಾನಿಸು CAP_ALTSVC_MAX_AGE (2*365*24*3600) /* two years cap */

#ವ್ಯಾಖ್ಯಾನಿಸು ALTSVC_MA      1
#ವ್ಯಾಖ್ಯಾನಿಸು ALTSVC_PERSIST 2
ಸ್ಥಿರ ಶೂನ್ಯ altsvc_parse_params(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pp,
                                time_t *pmaxage,
                                bool *ppersist)
{
  curlx_str_passblanks(pp);
  ಆದರೆ(curlx_str_single(pp, ';'))
    ಹಿಂತಿರುಗಿ;

  ಗಾಗಿ(;;) {
    ರಚನೆ Curl_str name;
    ರಚನೆ Curl_str val;
    curl_off_t ಅಂಕೆ;
    ಪೂರ್ಣಾಂಕ keyword = 0;

    /* allow some extra whitespaces around name and value */
    ಆದರೆ(curlx_str_until(pp, &name, MAX_ALTSVC_ಸಾಲು, '=') ||
       curlx_str_single(pp, '='))
      ನಿಲ್ಲಿಸು;  /* skip further parameter parsing */

    curlx_str_trimblanks(&name);
    curlx_str_passblanks(pp);
    ಆದರೆ(**pp == '\"') {
      ಆದರೆ(curlx_str_quotedword(pp, &val, MAX_ALTSVC_ಸಾಲು))
        ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(curlx_str_cspn(pp, &val, ",;\r\n"))
        ನಿಲ್ಲಿಸು;
    }
    curlx_str_trimblanks(&val);

    ಆದರೆ(curlx_str_casecompare(&name, "ma"))
      keyword = ALTSVC_MA;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curlx_str_casecompare(&name, "persist"))
      keyword = ALTSVC_PERSIST;

    ಆದರೆ(keyword) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *vp = curlx_str(&val);
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *vend = vp + curlx_strlen(&val);
      ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&vp, &ಅಂಕೆ, CURL_OFF_T_MAX))
        ನಿಲ್ಲಿಸು; /* not a number, skip further parameter parsing */
      ಆದರೆ(vp != vend)
        ನಿಲ್ಲಿಸು; /* not entirely a number, skip further parameter parsing */
      ಆದರೆ(keyword == ALTSVC_MA) {
        ಆದರೆ(ಅಂಕೆ > CAP_ALTSVC_MAX_AGE)
          ಅಂಕೆ = CAP_ALTSVC_MAX_AGE;
        *pmaxage = (time_t)ಅಂಕೆ;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಅಂಕೆ == 1)
        *ppersist = TRUE;
    }

    curlx_str_passblanks(pp);
    ಆದರೆ(curlx_str_single(pp, ';'))
      ನಿಲ್ಲಿಸು; /* no further parameters */
  }
}

ಸ್ಥಿರ bool altsvc_parse_dest(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pp,
                              ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                              ರಚನೆ Curl_peer *origin,
                              ರಚನೆ Curl_str *dsthost,
                              uint16_t *pdstport)
{
  curl_off_t ಪೋರ್ಟ್ = 0;

  ಆದರೆ(curlx_str_single(pp, '\"'))
    ಹಿಂತಿರುಗಿ FALSE;

  /* quoted string, with hostname or just :port ? */
  ಆದರೆ(curlx_str_single(pp, ':')) { /* is hostname:port ? */
    ಆದರೆ(curlx_str_single(pp, '[')) { /* DNS hostname/IPv4 */
      ಆದರೆ(curlx_str_until(pp, dsthost, MAX_ALTSVC_HOSTLEN, ':')) {
        infof(ದತ್ತಾಂಶ, "Bad alt-svc hostname, ignoring.");
        ಹಿಂತಿರುಗಿ FALSE;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ { /* IPv6 hostname */
      ಆದರೆ(curlx_str_until(pp, dsthost, MAX_IPADR_LEN, ']') ||
         curlx_str_single(pp, ']')) {
        infof(ದತ್ತಾಂಶ, "Bad alt-svc IPv6 hostname, ignoring.");
        ಹಿಂತಿರುಗಿ FALSE;
      }
    }
    ಆದರೆ(curlx_str_single(pp, ':'))
      ಹಿಂತಿರುಗಿ FALSE; /* not followed by ':' */
  }
  ಇಲ್ಲದಿದ್ದರೆ  /* is only :port, hostname is effectively origin */
    curlx_str_ನಿಯೋಜಿಸು(dsthost, origin->hostname,
                     ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(origin->hostname));

  ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(pp, &ಪೋರ್ಟ್, 0xffff)) {
    infof(ದತ್ತಾಂಶ, "Unknown alt-svc port number, ignoring.");
    ಹಿಂತಿರುಗಿ FALSE;
  }

  *pdstport = (uint16_t)ಪೋರ್ಟ್;
  ಆದರೆ(curlx_str_single(pp, '\"'))
    ಹಿಂತಿರುಗಿ FALSE; /* quoted string not ending here as expected */
  ಹಿಂತಿರುಗಿ TRUE;
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
CURLcode Curl_altsvc_parse(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                           ರಚನೆ altsvcinfo *asi, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮೌಲ್ಯ,
                           ರಚನೆ Curl_peer *origin,
                           ಎಣಿಕೆ alpnid origin_alpnid)
{
  ರಚನೆ altsvc *as;
  size_t entries = 0;
  ರಚನೆ Curl_str alpn;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;

  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  /* RFC 7838, The "Alt-Svc" header field value is basically
   * Alt-Svc: (clear|alpn="(host)?:port"\s*(;\s*parameter=value)*)
   * This can be repeated, comma-separated.
   *
   * We parse "best effort", ignoring values we do not recognize.
   */

  /* Try to catch a standalone "clear" */
  p = ಮೌಲ್ಯ;
  ಆದರೆ(!curlx_str_cspn(&p, &alpn, ";\n\r")) {
    curlx_str_trimblanks(&alpn);
    /* "clear" is a magic keyword */
    ಆದರೆ(curlx_str_casecompare(&alpn, "clear")) {
      /* Flush cached alternatives for this source origin */
      altsvc_flush(asi, origin, origin_alpnid);
      ಹಿಂತಿರುಗಿ CURLE_OK;
    }
  }

  /* Not a standalone "clear", parse from start for alpn entries */
  ಗಾಗಿ(p = ಮೌಲ್ಯ; *p;) {
    time_t maxage = 24 * 3600; /* default is 24 hours */
    bool persist = FALSE;
    ಎಣಿಕೆ alpnid dstalpnid;
    ರಚನೆ Curl_str dsthost;
    uint16_t dstport;

    ಆದರೆ(curlx_str_until(&p, &alpn, MAX_ALTSVC_ಸಾಲು, '='))
      ನಿಲ್ಲಿಸು; /* not another entry, leave */
    curlx_str_trimblanks(&alpn);
    dstalpnid = Curl_str2alpnid(&alpn);

    ಆದರೆ(curlx_str_single(&p, '='))
      ನಿಲ್ಲಿಸು;

    /* Parse altsvc hostname:port */
    ಆದರೆ(!altsvc_parse_dest(&p, ದತ್ತಾಂಶ, origin, &dsthost, &dstport))
      ನಿಲ್ಲಿಸು;

    /* Parse optional parameters */
    altsvc_parse_params(&p, &maxage, &persist);

    ಆದರೆ(dstalpnid) { /* this is a known ALPN id, e.g. not ALPN_none */
      ಆದರೆ(!entries++)
        /* Flush cached alternatives for this source origin, if any - when
           this is the first entry of the line. */
        altsvc_flush(asi, origin, origin_alpnid);

      as = altsvc_createid(origin->hostname, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(origin->hostname),
                           curlx_str(&dsthost),
                           curlx_strlen(&dsthost),
                           origin_alpnid, dstalpnid,
                           origin->ಪೋರ್ಟ್, dstport);
      ಆದರೆ(as) {
        time_t secs = ಸಮಯ(ಖಾಲಿ);
        /* The expires time also needs to take the Age: value (if any)
           into account. [See RFC 7838 section 3.1] */
        ಆದರೆ(maxage > (ಸಮಯ_T_MAX - secs))
          as->expires = ಸಮಯ_T_MAX;
        ಇಲ್ಲದಿದ್ದರೆ
          as->expires = maxage + secs;
        as->persist = persist;
        altsvc_append(asi, as);
        infof(ದತ್ತಾಂಶ, "Added alt-svc: %.*s:%u over %s",
              (ಪೂರ್ಣಾಂಕ)curlx_strlen(&dsthost), curlx_str(&dsthost),
              dstport, Curl_alpnid2str(dstalpnid));
      }
      ಇಲ್ಲದಿದ್ದರೆ
        ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }

    /* When this is followed by a comma, we expect another entry */
    ಆದರೆ(curlx_str_single(&p, ','))
      ನಿಲ್ಲಿಸು;
  }

  ಹಿಂತಿರುಗಿ CURLE_OK;
}

/*
 * Return TRUE on a match
 */
bool Curl_altsvc_lookup(ರಚನೆ altsvcinfo *asi,
                        ರಚನೆ Curl_peer *origin,
                        ಎಣಿಕೆ alpnid origin_alpnid,
                        ರಚನೆ altsvc **dstentry,
                        ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ versions, /* one or more bits */
                        bool *psame_destination)
{
  DEBUGASSERT(asi);
  DEBUGASSERT(origin);
  DEBUGASSERT(dstentry);
  *psame_destination = FALSE;

  ಆದರೆ(Curl_llist_ಎಣಿಕೆ(&asi->list)) {
    ರಚನೆ Curl_llist_ಗಂಟು *e;
    ರಚನೆ Curl_llist_ಗಂಟು *n;
    time_t now = ಸಮಯ(ಖಾಲಿ);

    ಗಾಗಿ(e = Curl_llist_ತಲೆ(&asi->list); e; e = n) {
      ರಚನೆ altsvc *as = Curl_ಗಂಟು_elem(e);
      n = Curl_ಗಂಟು_ಮುಂದಿನ(e);
      ಆದರೆ(as->expires < now) {
        /* an expired entry, remove */
        Curl_ಗಂಟು_remove(e);
        altsvc_free(as);
        ಮುಂದುವರಿ;
      }
      ಆದರೆ((origin_alpnid == as->src.alpnid) &&
         (versions & (ಪೂರ್ಣಾಂಕ)as->dst.alpnid) &&
         (origin->ಪೋರ್ಟ್ == as->src.ಪೋರ್ಟ್) &&
         hostcompare(origin->hostname, as->src.host)) {
        /*ಹೊಂದಿಕೆ*/
        *dstentry = as;
        /* alt-svc on the same host+port or another one? */
        *psame_destination = (origin->ಪೋರ್ಟ್ == as->dst.ಪೋರ್ಟ್) &&
                             hostcompare(origin->hostname, as->dst.host);
        ಹಿಂತಿರುಗಿ TRUE;
      }
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ defined(DEBUGBUILD) || defined(UNITTESTS)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು ಸಮಯ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_HTTP && !CURL_DISABLE_ALTSVC */
