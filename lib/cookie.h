#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_COOKIE_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_COOKIE_H
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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "llist.h"

ਬਣਤਰ Cookie {
  ਬਣਤਰ Curl_llist_ਗੰਢ ਗੰਢ;    /* for the main cookie list */
  ਬਣਤਰ Curl_llist_ਗੰਢ getnode; /* for getlist */
  ਅੱਖਰ *name;                     /* <this> = value */
  ਅੱਖਰ *ਮੁੱਲ;                    /* name = <this> */
  ਅੱਖਰ *ਰਸਤਾ;                     /* canonical path */
  ਅੱਖਰ *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ creationtime;      /* time when the cookie was written */
  ਦੋਆਧਾਰੀ_ਅੰਕ(tailmatch);                 /* tail-match the domain name */
  ਦੋਆਧਾਰੀ_ਅੰਕ(secure);                    /* the 'secure' keyword was used */
  ਦੋਆਧਾਰੀ_ਅੰਕ(livecookie);                /* updated from server, not a stored file */
  ਦੋਆਧਾਰੀ_ਅੰਕ(httponly);                  /* the httponly directive is present */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਗੇਤਰ_secure);             /* secure prefix is set */
  ਦੋਆਧਾਰੀ_ਅੰਕ(ਅਗੇਤਰ_host);               /* host prefix is set */
};

/*
 * Available cookie prefixes, as defined in
 * draft-ietf-httpbis-rfc6265bis-02
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_ਅਗੇਤਰ_SECURE (1 << 0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_ਅਗੇਤਰ_HOST   (1 << 1)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_HASH_ਮਾਪ 63

ਬਣਤਰ CookieInfo {
  /* linked lists of cookies we know of */
  ਬਣਤਰ Curl_llist cookielist[COOKIE_HASH_ਮਾਪ];
  curl_off_t ਅਗਲਾ_expiration; /* the next time at which expiration happens */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ numcookies;    /* number of cookies in the "jar" */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ lastct;        /* last creation-time used in the jar */
  ਦੋਆਧਾਰੀ_ਅੰਕ(running);               /* state info, for cookie adding information */
  ਦੋਆਧਾਰੀ_ਅੰਕ(newsession); /* new session, discard session cookies on load */
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_COOKIE_ਸਤਰ 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ਸੈੱਟ_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_COOKIE_ਸਿਰਲੇਖ_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_COOKIE_SEND_AMOUNT 150

ਬਣਤਰ Curl_easy;
ਬਣਤਰ connectdata;

bool Curl_secure_context(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_NOPSL      (1<<3) /* skip PSL check */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_ਜੋੜੋ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                         ਬਣਤਰ CookieInfo *ci,
                         ਅਚਲ ਅੱਖਰ *lineptr,
                         ਅਚਲ ਅੱਖਰ *domain,
                         ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                         ਅਚਲ ਪੂਰਨ flags) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
CURLcode Curl_cookie_getlist(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                             bool *okay, ਅਚਲ ਅੱਖਰ *host,
                             ਬਣਤਰ Curl_llist *list) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
ਖਾਲੀ Curl_cookie_clearall(ਬਣਤਰ CookieInfo *ci);
ਖਾਲੀ Curl_cookie_clearsess(ਬਣਤਰ CookieInfo *ci);

#ਜੇ defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cookie_list(x)      ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cookie_loadfiles(x, y) CURLE_OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cookie_init()       ਨਿਰਾ
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cookie_ਲੜੀਵਾਰ(x)       Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_cookie_cleanup(x)   Curl_nop_stmt
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ Curl_flush_cookies(x, y) Curl_nop_stmt
#ਨਹੀਂਤਾਂ
ਖਾਲੀ Curl_flush_cookies(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool cleanup);
ਖਾਲੀ Curl_cookie_cleanup(ਬਣਤਰ CookieInfo *ci);
ਬਣਤਰ CookieInfo *Curl_cookie_init(ਖਾਲੀ);
ਬਣਤਰ curl_slist *Curl_cookie_list(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
CURLcode Curl_cookie_loadfiles(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                               ਪੂਰਨ flags) WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
ਖਾਲੀ Curl_cookie_ਲੜੀਵਾਰ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਅੰਤ /* HEADER_CURL_COOKIE_H */
