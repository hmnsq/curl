#నిర్వచితం_కాకపోతే శీర్షిక_CURL_COOKIE_H
#నిర్వచించు శీర్షిక_CURL_COOKIE_H
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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "llist.h"

నిర్మాణం Cookie {
  నిర్మాణం Curl_llist_కణుపు కణుపు;    /* for the main cookie list */
  నిర్మాణం Curl_llist_కణుపు getnode; /* for getlist */
  అక్షరం *name;                     /* <this> = value */
  అక్షరం *విలువ;                    /* name = <this> */
  అక్షరం *మార్గం;                     /* canonical path */
  అక్షరం *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  గుర్తులేని పూర్ణాంకం creationtime;      /* time when the cookie was written */
  బిట్టు(tailmatch);                 /* tail-match the domain name */
  బిట్టు(secure);                    /* the 'secure' keyword was used */
  బిట్టు(livecookie);                /* updated from server, not a stored file */
  బిట్టు(httponly);                  /* the httponly directive is present */
  బిట్టు(ఉపసర్గ_secure);             /* secure prefix is set */
  బిట్టు(ఉపసర్గ_host);               /* host prefix is set */
};

/*
 * Available cookie prefixes, as defined in
 * draft-ietf-httpbis-rfc6265bis-02
 */
#నిర్వచించు COOKIE_ఉపసర్గ_SECURE (1 << 0)
#నిర్వచించు COOKIE_ఉపసర్గ_HOST   (1 << 1)

#నిర్వచించు COOKIE_HASH_కొలత 63

నిర్మాణం CookieInfo {
  /* linked lists of cookies we know of */
  నిర్మాణం Curl_llist cookielist[COOKIE_HASH_కొలత];
  curl_off_t తదుపరి_expiration; /* the next time at which expiration happens */
  గుర్తులేని పూర్ణాంకం numcookies;    /* number of cookies in the "jar" */
  గుర్తులేని పూర్ణాంకం lastct;        /* last creation-time used in the jar */
  బిట్టు(running);               /* state info, for cookie adding information */
  బిట్టు(newsession); /* new session, discard session cookies on load */
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
#నిర్వచించు MAX_COOKIE_పంక్తి 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#నిర్వచించు MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#నిర్వచించు MAX_సమితి_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#నిర్వచించు MAX_COOKIE_శీర్షిక_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#నిర్వచించు MAX_COOKIE_SEND_AMOUNT 150

నిర్మాణం Curl_easy;
నిర్మాణం connectdata;

bool Curl_secure_context(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#నిర్వచించు COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#నిర్వచించు COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#నిర్వచించు COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#నిర్వచించు COOKIE_NOPSL      (1<<3) /* skip PSL check */
#నిర్వచించు COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_కూడు(నిర్మాణం Curl_easy *దత్తాంశం,
                         నిర్మాణం CookieInfo *ci,
                         స్థిరరాశి అక్షరం *lineptr,
                         స్థిరరాశి అక్షరం *domain,
                         స్థిరరాశి అక్షరం *మార్గం,
                         స్థిరరాశి పూర్ణాంకం flags) WARN_వాడని_ఫలితం;
CURLcode Curl_cookie_getlist(నిర్మాణం Curl_easy *దత్తాంశం,
                             bool *okay, స్థిరరాశి అక్షరం *host,
                             నిర్మాణం Curl_llist *list) WARN_వాడని_ఫలితం;
శూన్య Curl_cookie_clearall(నిర్మాణం CookieInfo *ci);
శూన్య Curl_cookie_clearsess(నిర్మాణం CookieInfo *ci);

#ఒకవేళ defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#నిర్వచించు Curl_cookie_list(x)      ఖాళీ
#నిర్వచించు Curl_cookie_loadfiles(x, y) CURLE_OK
#నిర్వచించు Curl_cookie_init()       ఖాళీ
#నిర్వచించు Curl_cookie_నిరంతరం(x)       Curl_nop_stmt
#నిర్వచించు Curl_cookie_cleanup(x)   Curl_nop_stmt
#నిర్వచించు Curl_flush_cookies(x, y) Curl_nop_stmt
#లేకపోతే
శూన్య Curl_flush_cookies(నిర్మాణం Curl_easy *దత్తాంశం, bool cleanup);
శూన్య Curl_cookie_cleanup(నిర్మాణం CookieInfo *ci);
నిర్మాణం CookieInfo *Curl_cookie_init(శూన్య);
నిర్మాణం curl_slist *Curl_cookie_list(నిర్మాణం Curl_easy *దత్తాంశం);
CURLcode Curl_cookie_loadfiles(నిర్మాణం Curl_easy *దత్తాంశం,
                               పూర్ణాంకం flags) WARN_వాడని_ఫలితం;
శూన్య Curl_cookie_నిరంతరం(నిర్మాణం Curl_easy *దత్తాంశం);
#అయితే_ముగింపు

#అయితే_ముగింపు /* HEADER_CURL_COOKIE_H */
