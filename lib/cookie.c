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

#ಯದಿ !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "cookie.h"
#ಅನ್ತರ್ಭಾವ "psl.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "slist.h"
#ಅನ್ತರ್ಭಾವ "curl_share.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "curl_fopen.h"
#ಅನ್ತರ್ಭಾವ "curl_get_line.h"
#ಅನ್ತರ್ಭಾವ "curl_memrchr.h"
#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "curlx/dynbuf.h"
#ಅನ್ತರ್ಭಾವ "llist.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ಸ್ಥಿರ ಶೂನ್ಯ cap_expires(time_t now, ಸಂರಚನಾ Cookie *co)
{
  ಯದಿ(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    ಯದಿ(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ freecookie(ಸಂರಚನಾ Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->ಮೌಲ್ಯ);
  ಯದಿ(maintoo)
    curlx_free(co);
}

ಸ್ಥಿರ bool cookie_tailmatch(ಧ್ರುವ ವರ್ಣ *cookie_domain,
                             ಧ್ರುವ size_t cookie_domain_len,
                             ಧ್ರುವ ವರ್ಣ *hostname)
{
  size_t hostname_len = strlen(hostname);

  ಯದಿ(hostname_len < cookie_domain_len)
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಯದಿ(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  ಯದಿ(hostname_len == cookie_domain_len)
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಯದಿ('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    ಪ್ರತ್ಯಾಗಮ TRUE;
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ಸ್ಥಿರ bool pathmatch(ಧ್ರುವ ವರ್ಣ *cookie_path, ಧ್ರುವ ವರ್ಣ *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  ಯದಿ(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }

  /* #-fragments are already cut off! */
  ಯದಿ(strlen(uri_path) == 0 || uri_path[0] != '/')
    uri_path = "/";

  /*
   * here, RFC6265 5.1.4 says
   *  4. Output the characters of the uri-path from the first character up
   *     to, but not including, the right-most %x2F ("/").
   *  but URL path /hoge?fuga=xxx means /hoge/index.cgi?fuga=xxx in some site
   *  without redirect.
   *  Ignore this algorithm because /hoge is uri path for this case
   *  (uri path is not /).
   */

  uri_path_len = strlen(uri_path);

  ಯದಿ(uri_path_len < cookie_path_len)
    ಗಚ್ಛ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  ಯದಿ(strncmp(cookie_path, uri_path, cookie_path_len))
    ಗಚ್ಛ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  ಯದಿ(cookie_path_len == uri_path_len) {
    ret = TRUE;
    ಗಚ್ಛ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  ಯದಿ(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    ಗಚ್ಛ pathmatched;
  }

pathmatched:
  ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *get_ತುದಿ_domain(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ domain, size_t *outlen)
{
  size_t len = 0;
  ಧ್ರುವ ವರ್ಣ *first = ರಿಕ್ತ, *last;

  ಯದಿ(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    ಯದಿ(last) {
      first = memrchr(domain, '.', (last - domain));
      ಯದಿ(first)
        len -= (++first - domain);
    }
  }

  ಯದಿ(outlen)
    *outlen = len;

  ಪ್ರತ್ಯಾಗಮ first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#ಯದಿ defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
ಸ್ಥಿರ size_t cookie_hash_domain(ಧ್ರುವ ವರ್ಣ *domain, ಧ್ರುವ size_t len)
{
  ಧ್ರುವ ವರ್ಣ *end = domain + len;
  size_t h = 5381;

  ಯಾವತ್(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  ಪ್ರತ್ಯಾಗಮ (h % COOKIE_HASH_SIZE);
}

#ಯದಿ defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
ಸ್ಥಿರ size_t cookiehash(ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ domain)
{
  ಧ್ರುವ ವರ್ಣ *ತುದಿ;
  size_t len;

  ಯದಿ(!domain || Curl_host_is_ipnum(domain))
    ಪ್ರತ್ಯಾಗಮ 0;

  ತುದಿ = get_ತುದಿ_domain(domain, &len);
  ಪ್ರತ್ಯಾಗಮ cookie_hash_domain(ತುದಿ, len);
}

/*
 * cookie path sanitize
 */
ಸ್ಥಿರ ವರ್ಣ *sanitize_cookie_path(ಧ್ರುವ ವರ್ಣ *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  ಯದಿ(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    ಯದಿ(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  ಯದಿ(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    ಪ್ರತ್ಯಾಗಮ curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  ಯದಿ(len > 1 && cookie_path[len - 1] == '/')
    len--;

  ಪ್ರತ್ಯಾಗಮ curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ಸ್ಥಿರ CURLcode strstore(ವರ್ಣ **str, ಧ್ರುವ ವರ್ಣ *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  ಯದಿ(!*str)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

/*
 * remove_expired
 *
 * Remove expired cookies from the hash by inspecting the expires timestamp on
 * each cookie in the hash, freeing and deleting any where the timestamp is in
 * the past. If the cookiejar has recorded the next timestamp at which one or
 * more cookies expire, then processing will exit early in case this timestamp
 * is in the future.
 */
ಸ್ಥಿರ ಶೂನ್ಯ remove_expired(ಸಂರಚನಾ CookieInfo *ci)
{
  ಸಂರಚನಾ Cookie *co;
  curl_off_t now = (curl_off_t)time(ರಿಕ್ತ);
  unsigned ಪೂರ್ಣಾಙ್ಕ i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  ಯದಿ(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    ಪ್ರತ್ಯಾಗಮ;
  ಅನ್ಯಥಾ
    ci->next_expiration = CURL_OFF_T_MAX;

  ಪ್ರತಿ(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e = ರಿಕ್ತ;

    ಪ್ರತಿ(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_ಗಂಟು_elem(n);
      e = Curl_ಗಂಟು_next(n);
      ಯದಿ(co->expires) {
        ಯದಿ(co->expires < now) {
          Curl_ಗಂಟು_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        ಅನ್ಯಥಾ ಯದಿ(co->expires < ci->next_expiration)
          /*
           * If this cookie has an expiration timestamp earlier than what we
           * have seen so far then record it for the next round of expirations.
           */
          ci->next_expiration = co->expires;
      }
    }
  }
}

#ifndef USE_LIBPSL
/* Make sure domain contains a dot or is localhost. */
ಸ್ಥಿರ bool bad_domain(ಧ್ರುವ ವರ್ಣ *domain, size_t len)
{
  ಯದಿ((len == 9) && curl_strnequal(domain, "localhost", 9))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಅನ್ಯಥಾ {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ಧ್ರುವ ವರ್ಣ *dot = memchr(domain, '.', len);
    ಯದಿ(dot) {
      size_t i = dot - domain;
      ಯದಿ((len - i) > 1)
        /* the dot is not the last byte */
        ಪ್ರತ್ಯಾಗಮ FALSE;
    }
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ಸ್ಥಿರ bool invalid_octets(ಧ್ರುವ ವರ್ಣ *ptr, size_t len)
{
  ಧ್ರುವ unsigned ವರ್ಣ *p = (ಧ್ರುವ unsigned ವರ್ಣ *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  ಯಾವತ್(len && *p) {
    ಯದಿ((*p < 0x20) || (*p == 0x7f))
      ಪ್ರತ್ಯಾಗಮ TRUE;
    p++;
    len--;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_ಮೌಲ್ಯ  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

ಸ್ಥಿರ CURLcode storecookie(ಸಂರಚನಾ Cookie *co, ಧ್ರುವ ಸಂರಚನಾ Curl_str *cp,
                            ಧ್ರುವ ವರ್ಣ *path, ಧ್ರುವ ವರ್ಣ *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  ಯದಿ(!result)
    result = strstore(&co->ಮೌಲ್ಯ, curlx_str(&cp[COOKIE_ಮೌಲ್ಯ]),
                      curlx_strlen(&cp[COOKIE_ಮೌಲ್ಯ]));
  ಯದಿ(!result) {
    size_t plen = 0;
    ಯದಿ(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    ಅನ್ಯಥಾ ಯದಿ(path) {
      /* No path was given in the header line, set the default */
      ಧ್ರುವ ವರ್ಣ *endslash = strrchr(path, '/');
      ಯದಿ(endslash)
        plen = endslash - path + 1; /* include end slash */
      ಅನ್ಯಥಾ
        plen = strlen(path);
    }

    ಯದಿ(path) {
      co->path = sanitize_cookie_path(path, plen);
      ಯದಿ(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  ಯದಿ(!result) {
    ಯದಿ(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    ಅನ್ಯಥಾ ಯದಿ(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      ಯದಿ(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ಸ್ಥಿರ bool parse_first_pair(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Cookie *co,
                             ಸಂರಚನಾ Curl_str *cookie,
                             ಸಂರಚನಾ Curl_str *name,
                             ಸಂರಚನಾ Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  ಯದಿ(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  ಯದಿ((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }

  /* Check if we have a reserved prefix set. */
  ಯದಿ(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  ಅನ್ಯಥಾ ಯದಿ(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_ಮೌಲ್ಯ] = *val;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ bool parse_domain(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ Cookie *co,
                         ಸಂರಚನಾ Curl_str *cookie_domain,
                         ಸಂರಚನಾ Curl_str *val,
                         ಧ್ರುವ ವರ್ಣ **domainp)
{
  bool is_ip;
  ಧ್ರುವ ವರ್ಣ *domain = *domainp;
  ಧ್ರುವ ವರ್ಣ *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  ಯದಿ('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  ಯದಿ(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  ಯದಿ(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    ಯದಿ(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  ಅನ್ಯಥಾ {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

ಸ್ಥಿರ ಶೂನ್ಯ parse_maxage(ಸಂರಚನಾ Cookie *co, ಸಂರಚನಾ Curl_str *val,
                         time_t *nowp)
{
  ಪೂರ್ಣಾಙ್ಕ rc;
  ಧ್ರುವ ವರ್ಣ *maxage = curlx_str(val);
  ಯದಿ(*maxage == '\"')
    maxage++;
  rc = curlx_str_ಸಂಖ್ಯೆ(&maxage, &co->expires, CURL_OFF_T_MAX);
  ಯದಿ(!*nowp)
    *nowp = time(ರಿಕ್ತ);
  ವಿಕಲ್ಪ(rc) {
  ಪಕ್ಷ STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    ಭಙ್ಗ;
  ಪಕ್ಷ STRE_OK:
    ಯದಿ(!co->expires)
      co->expires = 1; /* expire now */
    ಅನ್ಯಥಾ ಯದಿ(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    ಅನ್ಯಥಾ
      co->expires += *nowp;
    ಭಙ್ಗ;
  }
  cap_expires(*nowp, co);
}

ಸ್ಥಿರ ಶೂನ್ಯ parse_expires(ಸಂರಚನಾ Cookie *co, ಸಂರಚನಾ Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  ಯದಿ(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    ವರ್ಣ dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    ಯದಿ(!Curl_getdate_capped(dbuf, &date)) {
      ಯದಿ(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    ಅನ್ಯಥಾ
      co->expires = 0;
    ಯದಿ(!*nowp)
      *nowp = time(ರಿಕ್ತ);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ಸ್ಥಿರ CURLcode
parse_cookie_header(ಸಂರಚನಾ Curl_easy *data,
                    ಸಂರಚನಾ Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ಧ್ರುವ ವರ್ಣ *ptr, /* the header */
                    ಧ್ರುವ ವರ್ಣ *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ಧ್ರುವ ವರ್ಣ *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  ಯದಿ(linelength > MAX_COOKIE_ಸಾಲು)
    /* discard overly long lines at once */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, ಪರಿಮಾಣ(cookie));
  ಕುರು {
    ಸಂರಚನಾ Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    ಪೂರ್ಣಾಙ್ಕ rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    ಯದಿ(!rc || (rc == STRE_SHORT)) {
      ಸಂರಚನಾ Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      ಯದಿ(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }

      ಯದಿ(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        ಯದಿ(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        ಯದಿ(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
      }
      ಅನ್ಯಥಾ
        curlx_str_init(&val);

      ಯದಿ(!curlx_strlen(&cookie[COOKIE_NAME])) {
        ಯದಿ(!parse_first_pair(data, co, cookie, &name, &val, sep))
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }

      /* whether there's separator or not */
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "secure")) {
        ಯದಿ(secure_origin)
          co->secure = TRUE;
        ಅನ್ಯಥಾ {
          infof(data, "skipped cookie because not 'secure'");
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
      }
      ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      ಅನ್ಯಥಾ ಯದಿ(sep) {
        /* only if a separating equals sign is present */
        ಯದಿ(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "domain")) {
          ಯದಿ(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            ಪ್ರತ್ಯಾಗಮ CURLE_OK;
        }
        ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        ಅನ್ಯಥಾ ಯದಿ(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } ಯಾವತ್(!curlx_str_single(&ptr, ';'));

  ಯದಿ(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    ಯದಿ(!result)
      *okay = TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode parse_netscape(ಸಂರಚನಾ Cookie *co,
                               ಧ್ರುವ ಸಂರಚನಾ CookieInfo *ci,
                               bool *okay,
                               ಧ್ರುವ ವರ್ಣ *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ಧ್ರುವ ವರ್ಣ *ptr, *next;
  ಪೂರ್ಣಾಙ್ಕ fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  ಯದಿ(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  ಯದಿ(lineptr[0] == '#')
    /* do not even try the comments */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  ಪ್ರತಿ(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : ರಿಕ್ತ);
    ವಿಕಲ್ಪ(fields) {
    ಪಕ್ಷ 0:
      ಯದಿ(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      ಯದಿ(!co->domain)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಭಙ್ಗ;
    ಪಕ್ಷ 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      ಭಙ್ಗ;
    ಪಕ್ಷ 2:
      /* The file format allows the path field to remain not filled in */
      ಯದಿ(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        ಯದಿ(!co->path)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
        ಭಙ್ಗ;
      }
      ಅನ್ಯಥಾ {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        ಯದಿ(!co->path)
          ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    ಪಕ್ಷ 3:
      co->secure = FALSE;
      ಯದಿ(curl_strnequal(ptr, "TRUE", len)) {
        ಯದಿ(secure_origin || ci->running)
          co->secure = TRUE;
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ 4:
      ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&ptr, &co->expires, CURL_OFF_T_MAX))
        ಪ್ರತ್ಯಾಗಮ CURLE_OK;
      ಭಙ್ಗ;
    ಪಕ್ಷ 5:
      co->name = curlx_memdup0(ptr, len);
      ಯದಿ(!co->name)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಅನ್ಯಥಾ {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        ಯದಿ(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        ಅನ್ಯಥಾ ಯದಿ(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ 6:
      co->ಮೌಲ್ಯ = curlx_memdup0(ptr, len);
      ಯದಿ(!co->ಮೌಲ್ಯ)
        ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
      ಭಙ್ಗ;
    }
  }
  ಯದಿ(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->ಮೌಲ್ಯ = curlx_strdup("");
    ಯದಿ(!co->ಮೌಲ್ಯ)
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ
      fields++;
  }

  ಯದಿ(fields != 7)
    /* we did not find the sufficient number of fields */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  ಯದಿ(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->ಮೌಲ್ಯ, strlen(co->ಮೌಲ್ಯ)))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  *okay = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ bool is_public_suffix(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Cookie *co,
                             ಧ್ರುವ ವರ್ಣ *domain)
{
#ifdef USE_LIBPSL
  /*
   * Check if the domain is a Public Suffix and if yes, ignore the cookie.
   * 'domain' is NULL when the cookie is loaded from file or
   * CURLOPT_COOKIELIST.
   */
  DEBUGASSERT(data);
  DEBUGASSERT(co);
  DEBUGF(infof(data, "PSL check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain ? co->domain : "[blank]",
               domain ? domain : "[file]"));
  ಯದಿ(!co->domain || Curl_host_is_ipnum(co->domain))
    ಪ್ರತ್ಯಾಗಮ FALSE;

  ಅನ್ಯಥಾ {
    bool acceptable = FALSE;
    ವರ್ಣ lcase[256];
    ವರ್ಣ lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    ಯದಿ(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    ಯದಿ(clen && (co->domain[clen - 1] == '.'))
      clen--;

    ಯದಿ((dlen < ಪರಿಮಾಣ(lcase)) && (clen < ಪರಿಮಾಣ(lcookie))) {
      ಧ್ರುವ psl_ctx_t *psl = Curl_psl_use(data);
      ಯದಿ(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        ಯದಿ(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          ಯದಿ(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        ಅನ್ಯಥಾ {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          ಯದಿ(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      ಅನ್ಯಥಾ
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    ಯದಿ(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      ಪ್ರತ್ಯಾಗಮ TRUE;
    }
  }
#ಅನ್ಯಥಾ
  (ಶೂನ್ಯ)data;
  (ಶೂನ್ಯ)co;
  (ಶೂನ್ಯ)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/* returns TRUE when replaced */
ಸ್ಥಿರ bool replace_existing(ಸಂರಚನಾ Curl_easy *data,
                             ಸಂರಚನಾ Cookie *co,
                             ಧ್ರುವ ಸಂರಚನಾ CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *replace_n = ರಿಕ್ತ;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
  size_t myhash = cookiehash(co->domain);
  ಪ್ರತಿ(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ಗಂಟು_next(n)) {
    ಸಂರಚನಾ Cookie *clist = Curl_ಗಂಟು_elem(n);
    ಯದಿ(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      ಯದಿ(clist->domain && co->domain) {
        ಯದಿ(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(!clist->domain && !co->domain)
        matching_domains = TRUE;

      ಯದಿ(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ಧ್ರುವ ವರ್ಣ *sep = ರಿಕ್ತ;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        ಯದಿ(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        ಯದಿ(sep)
          cllen = sep - clist->path;
        ಅನ್ಯಥಾ
          cllen = strlen(clist->path);

        ಯದಿ(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          ಪ್ರತ್ಯಾಗಮ FALSE;
        }
      }
    }

    ಯದಿ(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      ಯದಿ(clist->domain && co->domain) {
        ಯದಿ(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      ಅನ್ಯಥಾ ಯದಿ(!clist->domain && !co->domain)
        replace_old = TRUE;

      ಯದಿ(replace_old) {
        /* the domains were identical */

        ಯದಿ(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        ಅನ್ಯಥಾ ಯದಿ(!clist->path != !co->path)
          replace_old = FALSE;
      }

      ಯದಿ(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        ಪ್ರತ್ಯಾಗಮ FALSE;
      }
      ಯದಿ(replace_old)
        replace_n = n;
    }
  }
  ಯದಿ(replace_n) {
    ಸಂರಚನಾ Cookie *repl = Curl_ಗಂಟು_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ಗಂಟು_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(ಸಂರಚನಾ Curl_easy *data,
                         ಸಂರಚನಾ CookieInfo *ci,
                         ಧ್ರುವ ವರ್ಣ *lineptr, /* first character of the line */
                         ಧ್ರುವ ವರ್ಣ *domain,  /* default domain */
                         ಧ್ರುವ ವರ್ಣ *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಸಂರಚನಾ Cookie comem;
  ಸಂರಚನಾ Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  ಯದಿ(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    ಪ್ರತ್ಯಾಗಮ CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, ಪರಿಮಾಣ(comem));

  ಯದಿ(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  ಅನ್ಯಥಾ
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  ಯದಿ(result || !okay)
    ಗಚ್ಛ fail;

  ಯದಿ(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    ಗಚ್ಛ fail;

  ಯದಿ(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    ಗಚ್ಛ fail;

  ಯದಿ(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    ಯದಿ(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    ಅನ್ಯಥಾ
      ಗಚ್ಛ fail;
  }

  ಯದಿ(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    ಗಚ್ಛ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  ಯದಿ(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  ಯದಿ(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    ಗಚ್ಛ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, ಪರಿಮಾಣ(comem));
  ಯದಿ(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    ಗಚ್ಛ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ಗಂಟು);

  ಯದಿ(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->ಮೌಲ್ಯ,
          co->domain, co->path, co->expires);

  ಯದಿ(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  ಯದಿ(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  ಯದಿ(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  ಪ್ರತ್ಯಾಗಮ result;
fail:
  freecookie(co, FALSE);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Curl_cookie_init()
 *
 * Inits a cookie struct to read data from a local file. This is always
 * called before any cookies are set. File may be NULL in which case only the
 * struct is initialized. Is file is "-" then STDIN is read.
 *
 * If 'newsession' is TRUE, discard all "session cookies" on read from file.
 *
 * Note that 'data' might be called as NULL pointer. If data is NULL, 'file'
 * will be ignored.
 *
 * Returns NULL on out of memory.
 */
ಸಂರಚನಾ CookieInfo *Curl_cookie_init(ಶೂನ್ಯ)
{
  ಪೂರ್ಣಾಙ್ಕ i;
  ಸಂರಚನಾ CookieInfo *ci = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ CookieInfo));
  ಯದಿ(!ci)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  ಪ್ರತಿ(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], ರಿಕ್ತ);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  ಪ್ರತ್ಯಾಗಮ ci;
}

/*
 * cookie_load()
 *
 * Reads cookies from a local file. This is always called before any cookies
 * are set. If file is "-" then STDIN is read.
 *
 * If 'flags' has the COOKIE_NOSESSION bit set, discard all "session cookies"
 * read from file.
 */
ಸ್ಥಿರ CURLcode cookie_load(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *file,
                            ಸಂರಚನಾ CookieInfo *ci, ಪೂರ್ಣಾಙ್ಕ flags)
{
  FILE *handle = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;
  FILE *fp = ರಿಕ್ತ;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  ಯದಿ(file && *file) {
    ಯದಿ(!strcmp(file, "-"))
      fp = stdin;
    ಅನ್ಯಥಾ {
      fp = curlx_fopen(file, "rb");
      ಯದಿ(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      ಅನ್ಯಥಾ {
        curlx_struct_stat stat;
        ಯದಿ((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = ರಿಕ್ತ;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        ಅನ್ಯಥಾ
          handle = fp;
      }
    }
  }

  ಯದಿ(fp) {
    ಸಂರಚನಾ dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    ಕುರು {
      result = Curl_get_ಸಾಲು(&buf, fp, &eof);
      ಯದಿ(!result) {
        ಧ್ರುವ ವರ್ಣ *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        ಯದಿ(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, ರಿಕ್ತ, ರಿಕ್ತ,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        ಯದಿ(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } ಯಾವತ್(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    ಯದಿ(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ಸಂರಚನಾ Curl_easy *data,
                               ಪೂರ್ಣಾಙ್ಕ flags)
{
  CURLcode result = CURLE_OK;
  ಸಂರಚನಾ curl_slist *list = data->state.cookielist;
  ಯದಿ(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ಯದಿ(!data->cookies)
      data->cookies = Curl_cookie_init();
    ಯದಿ(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    ಅನ್ಯಥಾ {
      data->state.cookie_engine = TRUE;
      ಯಾವತ್(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        ಯದಿ(result)
          ಭಙ್ಗ;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cookie_sort(ಧ್ರುವ ಶೂನ್ಯ *p1, ಧ್ರುವ ಶೂನ್ಯ *p2)
{
  ಧ್ರುವ ಸಂರಚನಾ Cookie *c1 = *(ಧ್ರುವ ಸಂರಚನಾ Cookie * ಧ್ರುವ *)p1;
  ಧ್ರುವ ಸಂರಚನಾ Cookie *c2 = *(ಧ್ರುವ ಸಂರಚನಾ Cookie * ಧ್ರುವ *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  ಯದಿ(l1 != l2)
    ಪ್ರತ್ಯಾಗಮ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  ಯದಿ(l1 != l2)
    ಪ್ರತ್ಯಾಗಮ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  ಯದಿ(l1 != l2)
    ಪ್ರತ್ಯಾಗಮ (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  ಪ್ರತ್ಯಾಗಮ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ cookie_sort_ct(ಧ್ರುವ ಶೂನ್ಯ *p1, ಧ್ರುವ ಶೂನ್ಯ *p2)
{
  ಧ್ರುವ ಸಂರಚನಾ Cookie *c1 = *(ಧ್ರುವ ಸಂರಚನಾ Cookie * ಧ್ರುವ *)p1;
  ಧ್ರುವ ಸಂರಚನಾ Cookie *c2 = *(ಧ್ರುವ ಸಂರಚನಾ Cookie * ಧ್ರುವ *)p2;

  ಪ್ರತ್ಯಾಗಮ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *host)
{
  ಪ್ರತ್ಯಾಗಮ Curl_xfer_is_secure(data) ||
    curl_strequal("localhost", host) ||
    !strcmp(host, "127.0.0.1") ||
    !strcmp(host, "::1");
}

/*
 * Curl_cookie_getlist
 *
 * For a given host and path, return a linked list of cookies that the client
 * should send to the server if used now.
 *
 * It shall only return cookies that have not expired.
 *
 * 'okay' is TRUE when there is a list returned.
 */
CURLcode Curl_cookie_getlist(ಸಂರಚನಾ Curl_easy *data,
                             bool *okay,
                             ಧ್ರುವ ವರ್ಣ *host,
                             ಸಂರಚನಾ Curl_llist *list)
{
  size_t matches = 0;
  ಧ್ರುವ bool is_ip = Curl_host_is_ipnum(host);
  ಧ್ರುವ size_t myhash = cookiehash(host);
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
  ಧ್ರುವ bool secure = Curl_secure_context(data, host);
  ಸಂರಚನಾ CookieInfo *ci = data->cookies;
  ಧ್ರುವ ವರ್ಣ *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ರಿಕ್ತ);

  ಯದಿ(!ci || !Curl_llist_ಎಣಿಕೆ(&ci->cookielist[myhash]))
    ಪ್ರತ್ಯಾಗಮ CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  ಪ್ರತಿ(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ಗಂಟು_next(n)) {
    ಸಂರಚನಾ Cookie *co = Curl_ಗಂಟು_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    ಯದಿ(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      ಯದಿ(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, strlen(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        ಯದಿ(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          ಯದಿ(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            ಭಙ್ಗ;
          }
        }
      }
    }
  }

  ಯದಿ(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    ಸಂರಚನಾ Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(ಪರಿಮಾಣ(ಸಂರಚನಾ Cookie *) * matches);
    ಯದಿ(!array) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ fail;
    }

    n = Curl_llist_head(list);

    ಪ್ರತಿ(i = 0; n; n = Curl_ಗಂಟು_next(n))
      array[i++] = Curl_ಗಂಟು_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, ಪರಿಮಾಣ(ಸಂರಚನಾ Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ರಿಕ್ತ);

    ಪ್ರತಿ(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ರಿಕ್ತ);
  ಪ್ರತ್ಯಾಗಮ result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
ಶೂನ್ಯ Curl_cookie_clearall(ಸಂರಚನಾ CookieInfo *ci)
{
  ಯದಿ(ci) {
    unsigned ಪೂರ್ಣಾಙ್ಕ i;
    ಪ್ರತಿ(i = 0; i < COOKIE_HASH_SIZE; i++) {
      ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;
      ಪ್ರತಿ(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        ಸಂರಚನಾ Cookie *c = Curl_ಗಂಟು_elem(n);
        ಸಂರಚನಾ Curl_llist_ಗಂಟು *e = Curl_ಗಂಟು_next(n);
        Curl_ಗಂಟು_remove(n);
        freecookie(c, TRUE);
        n = e;
      }
    }
    ci->numcookies = 0;
  }
}

/*
 * Curl_cookie_clearsess
 *
 * Free all session cookies in the cookies list.
 */
ಶೂನ್ಯ Curl_cookie_clearsess(ಸಂರಚನಾ CookieInfo *ci)
{
  unsigned ಪೂರ್ಣಾಙ್ಕ i;

  ಯದಿ(!ci)
    ಪ್ರತ್ಯಾಗಮ;

  ಪ್ರತಿ(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n = Curl_llist_head(&ci->cookielist[i]);
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *e = ರಿಕ್ತ;

    ಪ್ರತಿ(; n; n = e) {
      ಸಂರಚನಾ Cookie *curr = Curl_ಗಂಟು_elem(n);
      e = Curl_ಗಂಟು_next(n); /* in case the node is removed, get it early */
      ಯದಿ(!curr->expires) {
        Curl_ಗಂಟು_remove(n);
        freecookie(curr, TRUE);
        ci->numcookies--;
      }
    }
  }
}

/*
 * Curl_cookie_cleanup()
 *
 * Free a "cookie object" previous created with Curl_cookie_init().
 */
ಶೂನ್ಯ Curl_cookie_cleanup(ಸಂರಚನಾ CookieInfo *ci)
{
  ಯದಿ(ci) {
    Curl_cookie_clearall(ci);
    curlx_free(ci); /* free the base struct as well */
  }
}

/*
 * get_netscape_format()
 *
 * Formats a string for Netscape output file, w/o a newline at the end.
 * Function returns a char * to a formatted line. The caller is responsible
 * for freeing the returned pointer.
 */
ಸ್ಥಿರ ವರ್ಣ *get_netscape_format(ಧ್ರುವ ಸಂರಚನಾ Cookie *co)
{
  ಪ್ರತ್ಯಾಗಮ curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*ಮೌಲ್ಯ*/
    co->httponly ? "#HttpOnly_" : "",
    /*
     * Make sure all domains are prefixed with a dot if they allow
     * tailmatching. This is Mozilla-style.
     */
    (co->tailmatch && co->domain && co->domain[0] != '.') ? "." : "",
    co->domain ? co->domain : "unknown",
    co->tailmatch ? "TRUE" : "FALSE",
    co->path ? co->path : "/",
    co->secure ? "TRUE" : "FALSE",
    co->expires,
    co->name,
    co->ಮೌಲ್ಯ ? co->ಮೌಲ್ಯ : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ಸ್ಥಿರ CURLcode cookie_output(ಸಂರಚನಾ Curl_easy *data,
                              ಸಂರಚನಾ CookieInfo *ci,
                              ಧ್ರುವ ವರ್ಣ *filename)
{
  FILE *out = ರಿಕ್ತ;
  bool use_stdout = FALSE;
  ವರ್ಣ *tempstore = ರಿಕ್ತ;
  CURLcode result = CURLE_OK;

  ಯದಿ(!ci)
    /* no cookie engine alive */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  ಯದಿ(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  ಅನ್ಯಥಾ {
    result = Curl_fopen(data, filename, &out, &tempstore);
    ಯದಿ(result)
      ಗಚ್ಛ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  ಯದಿ(ci->numcookies) {
    unsigned ಪೂರ್ಣಾಙ್ಕ i;
    size_t nvalid = 0;
    ಸಂರಚನಾ Cookie **array;
    ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;

    array = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Cookie *) * ci->numcookies);
    ಯದಿ(!array) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ error;
    }

    /* only sort the cookies with a domain property */
    ಪ್ರತಿ(i = 0; i < COOKIE_HASH_SIZE; i++) {
      ಪ್ರತಿ(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_ಗಂಟು_next(n)) {
        ಸಂರಚನಾ Cookie *co = Curl_ಗಂಟು_elem(n);
        ಯದಿ(!co->domain)
          ಅನುವರ್ತ;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, ಪರಿಮಾಣ(ಸಂರಚನಾ Cookie *), cookie_sort_ct);

    ಪ್ರತಿ(i = 0; i < nvalid; i++) {
      ವರ್ಣ *format_ptr = get_netscape_format(array[i]);
      ಯದಿ(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        ಗಚ್ಛ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  ಯದಿ(!use_stdout) {
    curlx_fclose(out);
    out = ರಿಕ್ತ;
    ಯದಿ(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      ಗಚ್ಛ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;

error:
  ಯದಿ(out && !use_stdout)
    curlx_fclose(out);
  ಯದಿ(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ ಸಂರಚನಾ curl_slist *cookie_list(ಧ್ರುವ ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ curl_slist *list = ರಿಕ್ತ;
  ಸಂರಚನಾ curl_slist *beg;
  ಸಂರಚನಾ curl_slist **tail = &list;
  unsigned ಪೂರ್ಣಾಙ್ಕ i;
  ಸಂರಚನಾ Curl_llist_ಗಂಟು *n;

  ಯದಿ(!data->cookies || (data->cookies->numcookies == 0))
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  ಪ್ರತಿ(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ಪ್ರತಿ(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_ಗಂಟು_next(n)) {
      ಸಂರಚನಾ Cookie *c = Curl_ಗಂಟು_elem(n);
      ವರ್ಣ *ಸಾಲು;
      ಯದಿ(!c->domain)
        ಅನುವರ್ತ;
      ಸಾಲು = get_netscape_format(c);
      ಯದಿ(!ಸಾಲು) {
        curl_slist_free_all(list);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
      beg = Curl_slist_append_nodup(ರಿಕ್ತ, ಸಾಲು);
      ಯದಿ(!beg) {
        curlx_free(ಸಾಲು);
        curl_slist_free_all(list);
        ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  ಪ್ರತ್ಯಾಗಮ list;
}

ಸಂರಚನಾ curl_slist *Curl_cookie_list(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  ಪ್ರತ್ಯಾಗಮ list;
}

ಶೂನ್ಯ Curl_flush_cookies(ಸಂರಚನಾ Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  ಯದಿ(data->cookies) {
    ಧ್ರುವ ವರ್ಣ *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    ಯದಿ(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      ಯದಿ(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    ಯದಿ(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = ರಿಕ್ತ;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

ಶೂನ್ಯ Curl_cookie_run(ಸಂರಚನಾ Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  ಯದಿ(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
