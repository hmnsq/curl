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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "llist.h"

கட்டமைப்பு Cookie {
  கட்டமைப்பு Curl_llist_முனை முனை;    /* for the main cookie list */
  கட்டமைப்பு Curl_llist_முனை getnode; /* for getlist */
  எழுத்து *name;                     /* <this> = value */
  எழுத்து *மதிப்பு;                    /* name = <this> */
  எழுத்து *path;                     /* canonical path */
  எழுத்து *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  unsigned முழு creationtime;      /* time when the cookie was written */
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

கட்டமைப்பு CookieInfo {
  /* linked lists of cookies we know of */
  கட்டமைப்பு Curl_llist cookielist[COOKIE_HASH_SIZE];
  curl_off_t next_expiration; /* the next time at which expiration happens */
  unsigned முழு numcookies;    /* number of cookies in the "jar" */
  unsigned முழு lastct;        /* last creation-time used in the jar */
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
#define MAX_COOKIE_வரி 5000

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

கட்டமைப்பு Curl_easy;
கட்டமைப்பு connectdata;

bool Curl_secure_context(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#define COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#define COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#define COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#define COOKIE_NOPSL      (1<<3) /* skip PSL check */
#define COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_add(கட்டமைப்பு Curl_easy *data,
                         கட்டமைப்பு CookieInfo *ci,
                         மாறிலி எழுத்து *lineptr,
                         மாறிலி எழுத்து *domain,
                         மாறிலி எழுத்து *path,
                         மாறிலி முழு flags) WARN_UNUSED_RESULT;
CURLcode Curl_cookie_getlist(கட்டமைப்பு Curl_easy *data,
                             bool *okay, மாறிலி எழுத்து *host,
                             கட்டமைப்பு Curl_llist *list) WARN_UNUSED_RESULT;
வெறுமை Curl_cookie_clearall(கட்டமைப்பு CookieInfo *ci);
வெறுமை Curl_cookie_clearsess(கட்டமைப்பு CookieInfo *ci);

#எனில் defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#define Curl_cookie_list(x)      வெற்று
#define Curl_cookie_loadfiles(x, y) CURLE_OK
#define Curl_cookie_init()       வெற்று
#define Curl_cookie_run(x)       Curl_nop_stmt
#define Curl_cookie_cleanup(x)   Curl_nop_stmt
#define Curl_flush_cookies(x, y) Curl_nop_stmt
#இல்லையெனில்
வெறுமை Curl_flush_cookies(கட்டமைப்பு Curl_easy *data, bool cleanup);
வெறுமை Curl_cookie_cleanup(கட்டமைப்பு CookieInfo *ci);
கட்டமைப்பு CookieInfo *Curl_cookie_init(வெறுமை);
கட்டமைப்பு curl_slist *Curl_cookie_list(கட்டமைப்பு Curl_easy *data);
CURLcode Curl_cookie_loadfiles(கட்டமைப்பு Curl_easy *data,
                               முழு flags) WARN_UNUSED_RESULT;
வெறுமை Curl_cookie_run(கட்டமைப்பு Curl_easy *data);
#endif

#endif /* HEADER_CURL_COOKIE_H */
