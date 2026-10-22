#यदि_अपरिभाषितम् शीर्षकम्_CURL_COOKIE_H
#परिभाषय शीर्षकम्_CURL_COOKIE_H
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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "llist.h"

संरचना Cookie {
  संरचना Curl_llist_ग्रन्थिः ग्रन्थिः;    /* for the main cookie list */
  संरचना Curl_llist_ग्रन्थिः getnode; /* for getlist */
  वर्ण *name;                     /* <this> = value */
  वर्ण *मानम्;                    /* name = <this> */
  वर्ण *मार्गः;                     /* canonical path */
  वर्ण *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  अचिह्नित पूर्णाङ्क creationtime;      /* time when the cookie was written */
  BIT(tailmatch);                 /* tail-match the domain name */
  BIT(secure);                    /* the 'secure' keyword was used */
  BIT(livecookie);                /* updated from server, not a stored file */
  BIT(httponly);                  /* the httponly directive is present */
  BIT(उपसर्गः_secure);             /* secure prefix is set */
  BIT(उपसर्गः_host);               /* host prefix is set */
};

/*
 * Available cookie prefixes, as defined in
 * draft-ietf-httpbis-rfc6265bis-02
 */
#परिभाषय COOKIE_उपसर्गः_SECURE (1 << 0)
#परिभाषय COOKIE_उपसर्गः_HOST   (1 << 1)

#परिभाषय COOKIE_HASH_आकार 63

संरचना CookieInfo {
  /* linked lists of cookies we know of */
  संरचना Curl_llist cookielist[COOKIE_HASH_आकार];
  curl_off_t अग्र_expiration; /* the next time at which expiration happens */
  अचिह्नित पूर्णाङ्क numcookies;    /* number of cookies in the "jar" */
  अचिह्नित पूर्णाङ्क lastct;        /* last creation-time used in the jar */
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
#परिभाषय MAX_COOKIE_पङ्क्तिः 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#परिभाषय MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#परिभाषय MAX_समुच्चयः_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#परिभाषय MAX_COOKIE_शीर्षकम्_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#परिभाषय MAX_COOKIE_SEND_AMOUNT 150

संरचना Curl_easy;
संरचना connectdata;

bool Curl_secure_context(संरचना Curl_easy *दत्त, ध्रुव वर्ण *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#परिभाषय COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#परिभाषय COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#परिभाषय COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#परिभाषय COOKIE_NOPSL      (1<<3) /* skip PSL check */
#परिभाषय COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_सङ्कलय(संरचना Curl_easy *दत्त,
                         संरचना CookieInfo *ci,
                         ध्रुव वर्ण *lineptr,
                         ध्रुव वर्ण *domain,
                         ध्रुव वर्ण *मार्गः,
                         ध्रुव पूर्णाङ्क flags) WARN_अप्रयुक्तम्_परिणाम;
CURLcode Curl_cookie_getlist(संरचना Curl_easy *दत्त,
                             bool *okay, ध्रुव वर्ण *host,
                             संरचना Curl_llist *list) WARN_अप्रयुक्तम्_परिणाम;
शून्य Curl_cookie_clearall(संरचना CookieInfo *ci);
शून्य Curl_cookie_clearsess(संरचना CookieInfo *ci);

#यदि defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#परिभाषय Curl_cookie_list(x)      रिक्त
#परिभाषय Curl_cookie_loadfiles(x, y) CURLE_OK
#परिभाषय Curl_cookie_init()       रिक्त
#परिभाषय Curl_cookie_सततम्(x)       Curl_nop_stmt
#परिभाषय Curl_cookie_cleanup(x)   Curl_nop_stmt
#परिभाषय Curl_flush_cookies(x, y) Curl_nop_stmt
#अन्यथा
शून्य Curl_flush_cookies(संरचना Curl_easy *दत्त, bool cleanup);
शून्य Curl_cookie_cleanup(संरचना CookieInfo *ci);
संरचना CookieInfo *Curl_cookie_init(शून्य);
संरचना curl_slist *Curl_cookie_list(संरचना Curl_easy *दत्त);
CURLcode Curl_cookie_loadfiles(संरचना Curl_easy *दत्त,
                               पूर्णाङ्क flags) WARN_अप्रयुक्तम्_परिणाम;
शून्य Curl_cookie_सततम्(संरचना Curl_easy *दत्त);
#यदि_अन्तः

#यदि_अन्तः /* HEADER_CURL_COOKIE_H */
