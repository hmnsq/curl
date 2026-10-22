#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_COOKIE_H
#وصف_ڪريو سرو_CURL_COOKIE_H
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
#ضمن "curl_setup.h"

#ضمن "llist.h"

بنية Cookie {
  بنية Curl_llist_ڳنڍ ڳنڍ;    /* for the main cookie list */
  بنية Curl_llist_ڳنڍ getnode; /* for getlist */
  حرف *name;                     /* <this> = value */
  حرف *قدر;                    /* name = <this> */
  حرف *رستو;                     /* canonical path */
  حرف *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  بنانشان صحيح creationtime;      /* time when the cookie was written */
  ٻٽو_انگ(tailmatch);                 /* tail-match the domain name */
  ٻٽو_انگ(secure);                    /* the 'secure' keyword was used */
  ٻٽو_انگ(livecookie);                /* updated from server, not a stored file */
  ٻٽو_انگ(httponly);                  /* the httponly directive is present */
  ٻٽو_انگ(اڳياڙي_secure);             /* secure prefix is set */
  ٻٽو_انگ(اڳياڙي_host);               /* host prefix is set */
};

/*
 * Available cookie prefixes, as defined in
 * draft-ietf-httpbis-rfc6265bis-02
 */
#وصف_ڪريو COOKIE_اڳياڙي_SECURE (1 << 0)
#وصف_ڪريو COOKIE_اڳياڙي_HOST   (1 << 1)

#وصف_ڪريو COOKIE_HASH_سائيز 63

بنية CookieInfo {
  /* linked lists of cookies we know of */
  بنية Curl_llist cookielist[COOKIE_HASH_سائيز];
  curl_off_t ايندڙ_expiration; /* the next time at which expiration happens */
  بنانشان صحيح numcookies;    /* number of cookies in the "jar" */
  بنانشان صحيح lastct;        /* last creation-time used in the jar */
  ٻٽو_انگ(running);               /* state info, for cookie adding information */
  ٻٽو_انگ(newsession); /* new session, discard session cookies on load */
};

/* The maximum sizes we accept for cookies. RFC 6265 section 6.1 says
   "general-use user agents SHOULD provide each of the following minimum
   capabilities":

   - At least 4096 bytes per cookie (as measured by the sum of the length of
     the cookie's name, value, and attributes).
   In the 6265bis draft document section 5.4 it is phrased even stronger: "If
   the sum of the lengths of the name string and the value string is more than
   4096 octets, abort these steps and ignore the set-cookie-string entirely."
 */

/** Limits for INCOMING cookies **/

/* The longest we allow a line to be when reading a cookie from an HTTP header
   or from a cookie jar */
#وصف_ڪريو MAX_COOKIE_سٽ 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#وصف_ڪريو MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#وصف_ڪريو MAX_سيٽ_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#وصف_ڪريو MAX_COOKIE_سرو_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#وصف_ڪريو MAX_COOKIE_SEND_AMOUNT 150

بنية Curl_easy;
بنية connectdata;

bool Curl_secure_context(بنية Curl_easy *معلومات, ثابت حرف *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#وصف_ڪريو COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#وصف_ڪريو COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#وصف_ڪريو COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#وصف_ڪريو COOKIE_NOPSL      (1<<3) /* skip PSL check */
#وصف_ڪريو COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_جوڙيو(بنية Curl_easy *معلومات,
                         بنية CookieInfo *ci,
                         ثابت حرف *lineptr,
                         ثابت حرف *domain,
                         ثابت حرف *رستو,
                         ثابت صحيح flags) WARN_اڻ_استعمال_ٿيل_نتيجو;
CURLcode Curl_cookie_getlist(بنية Curl_easy *معلومات,
                             bool *okay, ثابت حرف *host,
                             بنية Curl_llist *list) WARN_اڻ_استعمال_ٿيل_نتيجو;
فراغ Curl_cookie_clearall(بنية CookieInfo *ci);
فراغ Curl_cookie_clearsess(بنية CookieInfo *ci);

#إذا defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#وصف_ڪريو Curl_cookie_list(x)      عدم
#وصف_ڪريو Curl_cookie_loadfiles(x, y) CURLE_OK
#وصف_ڪريو Curl_cookie_init()       عدم
#وصف_ڪريو Curl_cookie_لاڳيتو(x)       Curl_nop_stmt
#وصف_ڪريو Curl_cookie_cleanup(x)   Curl_nop_stmt
#وصف_ڪريو Curl_flush_cookies(x, y) Curl_nop_stmt
#وإلا
فراغ Curl_flush_cookies(بنية Curl_easy *معلومات, bool cleanup);
فراغ Curl_cookie_cleanup(بنية CookieInfo *ci);
بنية CookieInfo *Curl_cookie_init(فراغ);
بنية curl_slist *Curl_cookie_list(بنية Curl_easy *معلومات);
CURLcode Curl_cookie_loadfiles(بنية Curl_easy *معلومات,
                               صحيح flags) WARN_اڻ_استعمال_ٿيل_نتيجو;
فراغ Curl_cookie_لاڳيتو(بنية Curl_easy *معلومات);
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* HEADER_CURL_COOKIE_H */
