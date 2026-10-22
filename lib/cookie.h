#अगर_अघोषित HEADER_CURL_COOKIE_H
#घोषणा HEADER_CURL_COOKIE_H
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
#समावेश "curl_setup.h"

#समावेश "llist.h"

काष्ठा Cookie {
  काष्ठा Curl_llist_गाँठ गाँठ;    /* for the main cookie list */
  काष्ठा Curl_llist_गाँठ getnode; /* for getlist */
  अक्षर *name;                     /* <this> = value */
  अक्षर *मूल्य;                    /* name = <this> */
  अक्षर *path;                     /* canonical path */
  अक्षर *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  अचिन्हित पूर्णांक creationtime;      /* time when the cookie was written */
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
#घोषणा COOKIE_PREFIX__SECURE (1 << 0)
#घोषणा COOKIE_PREFIX__HOST   (1 << 1)

#घोषणा COOKIE_HASH_आकार 63

काष्ठा CookieInfo {
  /* linked lists of cookies we know of */
  काष्ठा Curl_llist cookielist[COOKIE_HASH_आकार];
  curl_off_t अग्र_expiration; /* the next time at which expiration happens */
  अचिन्हित पूर्णांक numcookies;    /* number of cookies in the "jar" */
  अचिन्हित पूर्णांक lastct;        /* last creation-time used in the jar */
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
#घोषणा MAX_COOKIE_पंक्ति 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#घोषणा MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#घोषणा MAX_SET_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#घोषणा MAX_COOKIE_HEADER_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#घोषणा MAX_COOKIE_SEND_AMOUNT 150

काष्ठा Curl_easy;
काष्ठा connectdata;

bool Curl_secure_context(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#घोषणा COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#घोषणा COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#घोषणा COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#घोषणा COOKIE_NOPSL      (1<<3) /* skip PSL check */
#घोषणा COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_add(काष्ठा Curl_easy *तथ्य,
                         काष्ठा CookieInfo *ci,
                         स्थिर अक्षर *lineptr,
                         स्थिर अक्षर *domain,
                         स्थिर अक्षर *path,
                         स्थिर पूर्णांक flags) WARN_UNUSED_परिणाम;
CURLcode Curl_cookie_getlist(काष्ठा Curl_easy *तथ्य,
                             bool *okay, स्थिर अक्षर *host,
                             काष्ठा Curl_llist *list) WARN_UNUSED_परिणाम;
व्योम Curl_cookie_clearall(काष्ठा CookieInfo *ci);
व्योम Curl_cookie_clearsess(काष्ठा CookieInfo *ci);

#अगर defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#घोषणा Curl_cookie_list(x)      शून्य
#घोषणा Curl_cookie_loadfiles(x, y) CURLE_OK
#घोषणा Curl_cookie_init()       शून्य
#घोषणा Curl_cookie_run(x)       Curl_nop_stmt
#घोषणा Curl_cookie_cleanup(x)   Curl_nop_stmt
#घोषणा Curl_flush_cookies(x, y) Curl_nop_stmt
#अन्यथा
व्योम Curl_flush_cookies(काष्ठा Curl_easy *तथ्य, bool cleanup);
व्योम Curl_cookie_cleanup(काष्ठा CookieInfo *ci);
काष्ठा CookieInfo *Curl_cookie_init(व्योम);
काष्ठा curl_slist *Curl_cookie_list(काष्ठा Curl_easy *तथ्य);
CURLcode Curl_cookie_loadfiles(काष्ठा Curl_easy *तथ्य,
                               पूर्णांक flags) WARN_UNUSED_परिणाम;
व्योम Curl_cookie_run(काष्ठा Curl_easy *तथ्य);
#पूर्ण_अगर

#पूर्ण_अगर /* HEADER_CURL_COOKIE_H */
