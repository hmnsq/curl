#ifndef HEADER_CURL_COOKIE_H
#define HEADER_CURL_COOKIE_H
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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "llist.h"

সংরচনা Cookie {
  সংরচনা Curl_llist_গ্রন্থি গ্রন্থি;    /* for the main cookie list */
  সংরচনা Curl_llist_গ্রন্থি getnode; /* for getlist */
  বর্ণ *name;                     /* <this> = value */
  বর্ণ *মান;                    /* name = <this> */
  বর্ণ *path;                     /* canonical path */
  বর্ণ *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  unsigned পূর্ণাঙ্ক creationtime;      /* time when the cookie was written */
  BIT(tailmatch);                 /* tail-match the domain name */
  BIT(secure);                    /* the 'secure' keyword was used */
  BIT(livecookie);                /* updated from server, not a stored file */
  BIT(httponly);                  /* the httponly directive is present */
  BIT(prefix_secure);             /* secure prefix is set */
  BIT(prefix_host);               /* host prefix is set */
};

/*
 * Available cookie prefixes, as defined in
 * draft-ietf-httpbis-rfc6265bis-02
 */
#define COOKIE_PREFIX__SECURE (1 << 0)
#define COOKIE_PREFIX__HOST   (1 << 1)

#define COOKIE_HASH_SIZE 63

সংরচনা CookieInfo {
  /* linked lists of cookies we know of */
  সংরচনা Curl_llist cookielist[COOKIE_HASH_SIZE];
  curl_off_t next_expiration; /* the next time at which expiration happens */
  unsigned পূর্ণাঙ্ক numcookies;    /* number of cookies in the "jar" */
  unsigned পূর্ণাঙ্ক lastct;        /* last creation-time used in the jar */
  BIT(running);               /* state info, for cookie adding information */
  BIT(newsession); /* new session, discard session cookies on load */
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
#define MAX_COOKIE_সারি 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#define MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#define MAX_SET_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#define MAX_COOKIE_HEADER_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#define MAX_COOKIE_SEND_AMOUNT 150

সংরচনা Curl_easy;
সংরচনা connectdata;

bool Curl_secure_context(সংরচনা Curl_easy *data, ধ্রুব বর্ণ *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#define COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#define COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#define COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#define COOKIE_NOPSL      (1<<3) /* skip PSL check */
#define COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_add(সংরচনা Curl_easy *data,
                         সংরচনা CookieInfo *ci,
                         ধ্রুব বর্ণ *lineptr,
                         ধ্রুব বর্ণ *domain,
                         ধ্রুব বর্ণ *path,
                         ধ্রুব পূর্ণাঙ্ক flags) WARN_UNUSED_RESULT;
CURLcode Curl_cookie_getlist(সংরচনা Curl_easy *data,
                             bool *okay, ধ্রুব বর্ণ *host,
                             সংরচনা Curl_llist *list) WARN_UNUSED_RESULT;
শূন্য Curl_cookie_clearall(সংরচনা CookieInfo *ci);
শূন্য Curl_cookie_clearsess(সংরচনা CookieInfo *ci);

#যদি defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#define Curl_cookie_list(x)      রিক্ত
#define Curl_cookie_loadfiles(x, y) CURLE_OK
#define Curl_cookie_init()       রিক্ত
#define Curl_cookie_run(x)       Curl_nop_stmt
#define Curl_cookie_cleanup(x)   Curl_nop_stmt
#define Curl_flush_cookies(x, y) Curl_nop_stmt
#অন্যথা
শূন্য Curl_flush_cookies(সংরচনা Curl_easy *data, bool cleanup);
শূন্য Curl_cookie_cleanup(সংরচনা CookieInfo *ci);
সংরচনা CookieInfo *Curl_cookie_init(শূন্য);
সংরচনা curl_slist *Curl_cookie_list(সংরচনা Curl_easy *data);
CURLcode Curl_cookie_loadfiles(সংরচনা Curl_easy *data,
                               পূর্ণাঙ্ক flags) WARN_UNUSED_RESULT;
শূন্য Curl_cookie_run(সংরচনা Curl_easy *data);
#endif

#endif /* HEADER_CURL_COOKIE_H */
