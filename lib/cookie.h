#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_COOKIE_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_COOKIE_H
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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "llist.h"

ರಚನೆ Cookie {
  ರಚನೆ Curl_llist_ಗಂಟು ಗಂಟು;    /* for the main cookie list */
  ರಚನೆ Curl_llist_ಗಂಟು getnode; /* for getlist */
  ಅಕ್ಷರ *name;                     /* <this> = value */
  ಅಕ್ಷರ *ಮೌಲ್ಯ;                    /* name = <this> */
  ಅಕ್ಷರ *ಮಾರ್ಗ;                     /* canonical path */
  ಅಕ್ಷರ *domain;                   /* domain = <this> */
  curl_off_t expires;             /* expires = <this> */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ creationtime;      /* time when the cookie was written */
  ಬಿಟ್(tailmatch);                 /* tail-match the domain name */
  ಬಿಟ್(secure);                    /* the 'secure' keyword was used */
  ಬಿಟ್(livecookie);                /* updated from server, not a stored file */
  ಬಿಟ್(httponly);                  /* the httponly directive is present */
  ಬಿಟ್(ಪೂರ್ವಪ್ರತ್ಯಯ_secure);             /* secure prefix is set */
  ಬಿಟ್(ಪೂರ್ವಪ್ರತ್ಯಯ_host);               /* host prefix is set */
};

/*
 * Available cookie prefixes, as defined in
 * draft-ietf-httpbis-rfc6265bis-02
 */
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_ಪೂರ್ವಪ್ರತ್ಯಯ_SECURE (1 << 0)
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_ಪೂರ್ವಪ್ರತ್ಯಯ_HOST   (1 << 1)

#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_HASH_ಗಾತ್ರ 63

ರಚನೆ CookieInfo {
  /* linked lists of cookies we know of */
  ರಚನೆ Curl_llist cookielist[COOKIE_HASH_ಗಾತ್ರ];
  curl_off_t ಮುಂದಿನ_expiration; /* the next time at which expiration happens */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ numcookies;    /* number of cookies in the "jar" */
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ lastct;        /* last creation-time used in the jar */
  ಬಿಟ್(running);               /* state info, for cookie adding information */
  ಬಿಟ್(newsession); /* new session, discard session cookies on load */
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
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_COOKIE_ಸಾಲು 5000

/* Maximum length of an incoming cookie name or content we deal with. Longer
   cookies are ignored. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_NAME 4096

/* Maximum number of Set-Cookie: lines accepted in a single response. If more
   such header lines are received, they are ignored. This value must be less
   than 256 since an unsigned char is used to count. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ಗಣ_COOKIE_AMOUNT 50

/** Limits for OUTGOING cookies **/

/* Maximum size for an outgoing cookie line libcurl will use in an http
   request. This is the default maximum length used in some versions of Apache
   httpd. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_COOKIE_ಶೀರ್ಷಿಕೆ_LEN 8190

/* Maximum number of cookies libcurl will send in a single request, even if
   there might be more cookies that match. One reason to cap the number is to
   keep the maximum HTTP request within the maximum allowed size. */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_COOKIE_SEND_AMOUNT 150

ರಚನೆ Curl_easy;
ರಚನೆ connectdata;

bool Curl_secure_context(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host);

/*
 * Add a cookie to the internal list of cookies. The domain and path arguments
 * are only used if the COOKIE_HTTPHEADER bit is set in the flags.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_HTTPHEADER (1<<0) /* if HTTP header-style line */
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_NOEXPIRE   (1<<1) /* skip remove_expired() */
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_SECURE     (1<<2) /* connection is over secure origin */
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_NOPSL      (1<<3) /* skip PSL check */
#ವ್ಯಾಖ್ಯಾನಿಸು COOKIE_NOSESSION  (1<<6) /* drop session cookies */

CURLcode Curl_cookie_ಕೂಡಿಸು(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                         ರಚನೆ CookieInfo *ci,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *lineptr,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *domain,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                         ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ flags) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
CURLcode Curl_cookie_getlist(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                             bool *okay, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host,
                             ರಚನೆ Curl_llist *list) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
ಶೂನ್ಯ Curl_cookie_clearall(ರಚನೆ CookieInfo *ci);
ಶೂನ್ಯ Curl_cookie_clearsess(ರಚನೆ CookieInfo *ci);

#ಆದರೆ defined(CURL_DISABLE_HTTP) || defined(CURL_DISABLE_COOKIES)
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_cookie_list(x)      ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_cookie_loadfiles(x, y) CURLE_OK
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_cookie_init()       ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_cookie_ಸತತ(x)       Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_cookie_cleanup(x)   Curl_nop_stmt
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_flush_cookies(x, y) Curl_nop_stmt
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ Curl_flush_cookies(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool cleanup);
ಶೂನ್ಯ Curl_cookie_cleanup(ರಚನೆ CookieInfo *ci);
ರಚನೆ CookieInfo *Curl_cookie_init(ಶೂನ್ಯ);
ರಚನೆ curl_slist *Curl_cookie_list(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
CURLcode Curl_cookie_loadfiles(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                               ಪೂರ್ಣಾಂಕ flags) WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
ಶೂನ್ಯ Curl_cookie_ಸತತ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_COOKIE_H */
