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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଯଦି !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "cookie.h"
#ଅନ୍ତର୍ଭାଵ "psl.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "slist.h"
#ଅନ୍ତର୍ଭାଵ "curl_share.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "curl_fopen.h"
#ଅନ୍ତର୍ଭାଵ "curl_get_line.h"
#ଅନ୍ତର୍ଭାଵ "curl_memrchr.h"
#ଅନ୍ତର୍ଭାଵ "parsedate.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "curlx/dynbuf.h"
#ଅନ୍ତର୍ଭାଵ "llist.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
ସ୍ଥିର ଶୂନ୍ୟ cap_expires(time_t now, ସଂରଚନା Cookie *co)
{
  ଯଦି(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    ଯଦି(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

ସ୍ଥିର ଶୂନ୍ୟ freecookie(ସଂରଚନା Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->ମୂଲ୍ୟ);
  ଯଦି(maintoo)
    curlx_free(co);
}

ସ୍ଥିର bool cookie_tailmatch(ଅଚଳ ଅକ୍ଷର *cookie_domain,
                             ଅଚଳ size_t cookie_domain_len,
                             ଅଚଳ ଅକ୍ଷର *hostname)
{
  size_t hostname_len = strlen(hostname);

  ଯଦି(hostname_len < cookie_domain_len)
    ଫେରନ୍ତୁ FALSE;

  ଯଦି(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    ଫେରନ୍ତୁ FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  ଯଦି(hostname_len == cookie_domain_len)
    ଫେରନ୍ତୁ TRUE;
  ଯଦି('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    ଫେରନ୍ତୁ TRUE;
  ଫେରନ୍ତୁ FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
ସ୍ଥିର bool pathmatch(ଅଚଳ ଅକ୍ଷର *cookie_path, ଅଚଳ ଅକ୍ଷର *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  ଯଦି(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    ଫେରନ୍ତୁ TRUE;
  }

  /* #-fragments are already cut off! */
  ଯଦି(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  ଯଦି(uri_path_len < cookie_path_len)
    ଯାଅ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  ଯଦି(strncmp(cookie_path, uri_path, cookie_path_len))
    ଯାଅ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  ଯଦି(cookie_path_len == uri_path_len) {
    ret = TRUE;
    ଯାଅ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  ଯଦି(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    ଯାଅ pathmatched;
  }

pathmatched:
  ଫେରନ୍ତୁ ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *get_ଶୀର୍ଷ_domain(ଅଚଳ ଅକ୍ଷର * ଅଚଳ domain, size_t *outlen)
{
  size_t len = 0;
  ଅଚଳ ଅକ୍ଷର *first = ଖାଲି, *last;

  ଯଦି(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    ଯଦି(last) {
      first = memrchr(domain, '.', (last - domain));
      ଯଦି(first)
        len -= (++first - domain);
    }
  }

  ଯଦି(outlen)
    *outlen = len;

  ଫେରନ୍ତୁ first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#ଯଦି defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
ସ୍ଥିର size_t cookie_hash_domain(ଅଚଳ ଅକ୍ଷର *domain, ଅଚଳ size_t len)
{
  ଅଚଳ ଅକ୍ଷର *end = domain + len;
  size_t h = 5381;

  ଯେତେବେଳେ(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  ଫେରନ୍ତୁ (h % COOKIE_HASH_SIZE);
}

#ଯଦି defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
ସ୍ଥିର size_t cookiehash(ଅଚଳ ଅକ୍ଷର * ଅଚଳ domain)
{
  ଅଚଳ ଅକ୍ଷର *ଶୀର୍ଷ;
  size_t len;

  ଯଦି(!domain || Curl_host_is_ipnum(domain))
    ଫେରନ୍ତୁ 0;

  ଶୀର୍ଷ = get_ଶୀର୍ଷ_domain(domain, &len);
  ଫେରନ୍ତୁ cookie_hash_domain(ଶୀର୍ଷ, len);
}

/*
 * cookie path sanitize
 */
ସ୍ଥିର ଅକ୍ଷର *sanitize_cookie_path(ଅଚଳ ଅକ୍ଷର *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  ଯଦି(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    ଯଦି(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  ଯଦି(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    ଫେରନ୍ତୁ curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  ଯଦି(len > 1 && cookie_path[len - 1] == '/')
    len--;

  ଫେରନ୍ତୁ curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
ସ୍ଥିର CURLcode strstore(ଅକ୍ଷର **str, ଅଚଳ ଅକ୍ଷର *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  ଯଦି(!*str)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
  ଫେରନ୍ତୁ CURLE_OK;
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
ସ୍ଥିର ଶୂନ୍ୟ remove_expired(ସଂରଚନା CookieInfo *ci)
{
  ସଂରଚନା Cookie *co;
  curl_off_t now = (curl_off_t)time(ଖାଲି);
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  ଯଦି(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    ଫେରନ୍ତୁ;
  ଅନ୍ୟଥା
    ci->next_expiration = CURL_OFF_T_MAX;

  ପାଇଁ(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e = ଖାଲି;

    ପାଇଁ(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_ଗଣ୍ଠି_elem(n);
      e = Curl_ଗଣ୍ଠି_next(n);
      ଯଦି(co->expires) {
        ଯଦି(co->expires < now) {
          Curl_ଗଣ୍ଠି_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        ଅନ୍ୟଥା ଯଦି(co->expires < ci->next_expiration)
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
ସ୍ଥିର bool bad_domain(ଅଚଳ ଅକ୍ଷର *domain, size_t len)
{
  ଯଦି((len == 9) && curl_strnequal(domain, "localhost", 9))
    ଫେରନ୍ତୁ FALSE;
  ଅନ୍ୟଥା {
    /* there must be a dot present, but that dot must not be a trailing dot */
    ଅଚଳ ଅକ୍ଷର *dot = memchr(domain, '.', len);
    ଯଦି(dot) {
      size_t i = dot - domain;
      ଯଦି((len - i) > 1)
        /* the dot is not the last byte */
        ଫେରନ୍ତୁ FALSE;
    }
  }
  ଫେରନ୍ତୁ TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
ସ୍ଥିର bool invalid_octets(ଅଚଳ ଅକ୍ଷର *ptr, size_t len)
{
  ଅଚଳ unsigned ଅକ୍ଷର *p = (ଅଚଳ unsigned ଅକ୍ଷର *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  ଯେତେବେଳେ(len && *p) {
    ଯଦି((*p < 0x20) || (*p == 0x7f))
      ଫେରନ୍ତୁ TRUE;
    p++;
    len--;
  }
  ଫେରନ୍ତୁ FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_ମୂଲ୍ୟ  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

ସ୍ଥିର CURLcode storecookie(ସଂରଚନା Cookie *co, ଅଚଳ ସଂରଚନା Curl_str *cp,
                            ଅଚଳ ଅକ୍ଷର *path, ଅଚଳ ଅକ୍ଷର *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  ଯଦି(!result)
    result = strstore(&co->ମୂଲ୍ୟ, curlx_str(&cp[COOKIE_ମୂଲ୍ୟ]),
                      curlx_strlen(&cp[COOKIE_ମୂଲ୍ୟ]));
  ଯଦି(!result) {
    size_t plen = 0;
    ଯଦି(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    ଅନ୍ୟଥା ଯଦି(path) {
      /* No path was given in the header line, set the default */
      ଅଚଳ ଅକ୍ଷର *endslash = strrchr(path, '/');
      ଯଦି(endslash)
        plen = endslash - path + 1; /* include end slash */
      ଅନ୍ୟଥା
        plen = strlen(path);
    }

    ଯଦି(path) {
      co->path = sanitize_cookie_path(path, plen);
      ଯଦି(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  ଯଦି(!result) {
    ଯଦି(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    ଅନ୍ୟଥା ଯଦି(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      ଯଦି(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  ଫେରନ୍ତୁ result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
ସ୍ଥିର bool parse_first_pair(ସଂରଚନା Curl_easy *data, ସଂରଚନା Cookie *co,
                             ସଂରଚନା Curl_str *cookie,
                             ସଂରଚନା Curl_str *name,
                             ସଂରଚନା Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  ଯଦି(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    ଫେରନ୍ତୁ FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  ଯଦି((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    ଫେରନ୍ତୁ FALSE;
  }

  /* Check if we have a reserved prefix set. */
  ଯଦି(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  ଅନ୍ୟଥା ଯଦି(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_ମୂଲ୍ୟ] = *val;
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର bool parse_domain(ସଂରଚନା Curl_easy *data, ସଂରଚନା Cookie *co,
                         ସଂରଚନା Curl_str *cookie_domain,
                         ସଂରଚନା Curl_str *val,
                         ଅଚଳ ଅକ୍ଷର **domainp)
{
  bool is_ip;
  ଅଚଳ ଅକ୍ଷର *domain = *domainp;
  ଅଚଳ ଅକ୍ଷର *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  ଯଦି('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  ଯଦି(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  ଯଦି(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    ଯଦି(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  ଅନ୍ୟଥା {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    ଫେରନ୍ତୁ FALSE;
  }
  ଫେରନ୍ତୁ TRUE;
}

ସ୍ଥିର ଶୂନ୍ୟ parse_maxage(ସଂରଚନା Cookie *co, ସଂରଚନା Curl_str *val,
                         time_t *nowp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  ଅଚଳ ଅକ୍ଷର *maxage = curlx_str(val);
  ଯଦି(*maxage == '\"')
    maxage++;
  rc = curlx_str_ସଂଖ୍ୟା(&maxage, &co->expires, CURL_OFF_T_MAX);
  ଯଦି(!*nowp)
    *nowp = time(ଖାଲି);
  ବାଛ(rc) {
  ସ୍ଥିତି STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି STRE_OK:
    ଯଦି(!co->expires)
      co->expires = 1; /* expire now */
    ଅନ୍ୟଥା ଯଦି(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    ଅନ୍ୟଥା
      co->expires += *nowp;
    ଭାଙ୍ଗ;
  }
  cap_expires(*nowp, co);
}

ସ୍ଥିର ଶୂନ୍ୟ parse_expires(ସଂରଚନା Cookie *co, ସଂରଚନା Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  ଯଦି(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    ଅକ୍ଷର dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    ଯଦି(!Curl_getdate_capped(dbuf, &date)) {
      ଯଦି(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    ଅନ୍ୟଥା
      co->expires = 0;
    ଯଦି(!*nowp)
      *nowp = time(ଖାଲି);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
ସ୍ଥିର CURLcode
parse_cookie_header(ସଂରଚନା Curl_easy *data,
                    ସଂରଚନା Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    ଅଚଳ ଅକ୍ଷର *ptr, /* the header */
                    ଅଚଳ ଅକ୍ଷର *domain, /* default domain */
                    /* full path used when this cookie is set */
                    ଅଚଳ ଅକ୍ଷର *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  ସଂରଚନା Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  ଯଦି(linelength > MAX_COOKIE_ଧାଡ଼ି)
    /* discard overly long lines at once */
    ଫେରନ୍ତୁ CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, ଆକାର(cookie));
  କର {
    ସଂରଚନା Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    ଯଦି(!rc || (rc == STRE_SHORT)) {
      ସଂରଚନା Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      ଯଦି(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        ଫେରନ୍ତୁ CURLE_OK;
      }

      ଯଦି(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        ଯଦି(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        ଯଦି(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          ଫେରନ୍ତୁ CURLE_OK;
        }
      }
      ଅନ୍ୟଥା
        curlx_str_init(&val);

      ଯଦି(!curlx_strlen(&cookie[COOKIE_NAME])) {
        ଯଦି(!parse_first_pair(data, co, cookie, &name, &val, sep))
          ଫେରନ୍ତୁ CURLE_OK;
      }

      /* whether there's separator or not */
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "secure")) {
        ଯଦି(secure_origin)
          co->secure = TRUE;
        ଅନ୍ୟଥା {
          infof(data, "skipped cookie because not 'secure'");
          ଫେରନ୍ତୁ CURLE_OK;
        }
      }
      ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      ଅନ୍ୟଥା ଯଦି(sep) {
        /* only if a separating equals sign is present */
        ଯଦି(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "domain")) {
          ଯଦି(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            ଫେରନ୍ତୁ CURLE_OK;
        }
        ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        ଅନ୍ୟଥା ଯଦି(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } ଯେତେବେଳେ(!curlx_str_single(&ptr, ';'));

  ଯଦି(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    ଯଦି(!result)
      *okay = TRUE;
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode parse_netscape(ସଂରଚନା Cookie *co,
                               ଅଚଳ ସଂରଚନା CookieInfo *ci,
                               bool *okay,
                               ଅଚଳ ଅକ୍ଷର *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  ଅଚଳ ଅକ୍ଷର *ptr, *next;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  ଯଦି(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  ଯଦି(lineptr[0] == '#')
    /* do not even try the comments */
    ଫେରନ୍ତୁ CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  ପାଇଁ(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : ଖାଲି);
    ବାଛ(fields) {
    ସ୍ଥିତି 0:
      ଯଦି(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      ଯଦି(!co->domain)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 2:
      /* The file format allows the path field to remain not filled in */
      ଯଦି(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        ଯଦି(!co->path)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
        ଭାଙ୍ଗ;
      }
      ଅନ୍ୟଥା {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        ଯଦି(!co->path)
          ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    ସ୍ଥିତି 3:
      co->secure = FALSE;
      ଯଦି(curl_strnequal(ptr, "TRUE", len)) {
        ଯଦି(secure_origin || ci->running)
          co->secure = TRUE;
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ CURLE_OK;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 4:
      ଯଦି(curlx_str_ସଂଖ୍ୟା(&ptr, &co->expires, CURL_OFF_T_MAX))
        ଫେରନ୍ତୁ CURLE_OK;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 5:
      co->name = curlx_memdup0(ptr, len);
      ଯଦି(!co->name)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଅନ୍ୟଥା {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        ଯଦି(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        ଅନ୍ୟଥା ଯଦି(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 6:
      co->ମୂଲ୍ୟ = curlx_memdup0(ptr, len);
      ଯଦି(!co->ମୂଲ୍ୟ)
        ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
      ଭାଙ୍ଗ;
    }
  }
  ଯଦି(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->ମୂଲ୍ୟ = curlx_strdup("");
    ଯଦି(!co->ମୂଲ୍ୟ)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା
      fields++;
  }

  ଯଦି(fields != 7)
    /* we did not find the sufficient number of fields */
    ଫେରନ୍ତୁ CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  ଯଦି(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->ମୂଲ୍ୟ, strlen(co->ମୂଲ୍ୟ)))
    ଫେରନ୍ତୁ CURLE_OK;

  *okay = TRUE;
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର bool is_public_suffix(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Cookie *co,
                             ଅଚଳ ଅକ୍ଷର *domain)
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
  ଯଦି(!co->domain || Curl_host_is_ipnum(co->domain))
    ଫେରନ୍ତୁ FALSE;

  ଅନ୍ୟଥା {
    bool acceptable = FALSE;
    ଅକ୍ଷର lcase[256];
    ଅକ୍ଷର lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    ଯଦି(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    ଯଦି(clen && (co->domain[clen - 1] == '.'))
      clen--;

    ଯଦି((dlen < ଆକାର(lcase)) && (clen < ଆକାର(lcookie))) {
      ଅଚଳ psl_ctx_t *psl = Curl_psl_use(data);
      ଯଦି(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        ଯଦି(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          ଯଦି(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        ଅନ୍ୟଥା {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          ଯଦି(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      ଅନ୍ୟଥା
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    ଯଦି(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      ଫେରନ୍ତୁ TRUE;
    }
  }
#ଅନ୍ୟଥା
  (ଶୂନ୍ୟ)data;
  (ଶୂନ୍ୟ)co;
  (ଶୂନ୍ୟ)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  ଫେରନ୍ତୁ FALSE;
}

/* returns TRUE when replaced */
ସ୍ଥିର bool replace_existing(ସଂରଚନା Curl_easy *data,
                             ସଂରଚନା Cookie *co,
                             ଅଚଳ ସଂରଚନା CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *replace_n = ଖାଲି;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
  size_t myhash = cookiehash(co->domain);
  ପାଇଁ(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ଗଣ୍ଠି_next(n)) {
    ସଂରଚନା Cookie *clist = Curl_ଗଣ୍ଠି_elem(n);
    ଯଦି(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      ଯଦି(clist->domain && co->domain) {
        ଯଦି(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(!clist->domain && !co->domain)
        matching_domains = TRUE;

      ଯଦି(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        ଅଚଳ ଅକ୍ଷର *sep = ଖାଲି;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        ଯଦି(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        ଯଦି(sep)
          cllen = sep - clist->path;
        ଅନ୍ୟଥା
          cllen = strlen(clist->path);

        ଯଦି(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          ଫେରନ୍ତୁ FALSE;
        }
      }
    }

    ଯଦି(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      ଯଦି(clist->domain && co->domain) {
        ଯଦି(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      ଅନ୍ୟଥା ଯଦି(!clist->domain && !co->domain)
        replace_old = TRUE;

      ଯଦି(replace_old) {
        /* the domains were identical */

        ଯଦି(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        ଅନ୍ୟଥା ଯଦି(!clist->path != !co->path)
          replace_old = FALSE;
      }

      ଯଦି(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        ଫେରନ୍ତୁ FALSE;
      }
      ଯଦି(replace_old)
        replace_n = n;
    }
  }
  ଯଦି(replace_n) {
    ସଂରଚନା Cookie *repl = Curl_ଗଣ୍ଠି_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_ଗଣ୍ଠି_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  ଫେରନ୍ତୁ TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(ସଂରଚନା Curl_easy *data,
                         ସଂରଚନା CookieInfo *ci,
                         ଅଚଳ ଅକ୍ଷର *lineptr, /* first character of the line */
                         ଅଚଳ ଅକ୍ଷର *domain,  /* default domain */
                         ଅଚଳ ଅକ୍ଷର *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ସଂରଚନା Cookie comem;
  ସଂରଚନା Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  ଯଦି(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    ଫେରନ୍ତୁ CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, ଆକାର(comem));

  ଯଦି(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  ଅନ୍ୟଥା
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  ଯଦି(result || !okay)
    ଯାଅ fail;

  ଯଦି(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    ଯାଅ fail;

  ଯଦି(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    ଯାଅ fail;

  ଯଦି(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    ଯଦି(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    ଅନ୍ୟଥା
      ଯାଅ fail;
  }

  ଯଦି(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    ଯାଅ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  ଯଦି(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  ଯଦି(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    ଯାଅ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, ଆକାର(comem));
  ଯଦି(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    ଯାଅ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->ଗଣ୍ଠି);

  ଯଦି(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->ମୂଲ୍ୟ,
          co->domain, co->path, co->expires);

  ଯଦି(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  ଯଦି(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  ଯଦି(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  ଫେରନ୍ତୁ result;
fail:
  freecookie(co, FALSE);
  ଫେରନ୍ତୁ result;
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
ସଂରଚନା CookieInfo *Curl_cookie_init(ଶୂନ୍ୟ)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ସଂରଚନା CookieInfo *ci = curlx_calloc(1, ଆକାର(ସଂରଚନା CookieInfo));
  ଯଦି(!ci)
    ଫେରନ୍ତୁ ଖାଲି;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  ପାଇଁ(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], ଖାଲି);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  ଫେରନ୍ତୁ ci;
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
ସ୍ଥିର CURLcode cookie_load(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *file,
                            ସଂରଚନା CookieInfo *ci, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  FILE *handle = ଖାଲି;
  CURLcode result = CURLE_OK;
  FILE *fp = ଖାଲି;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  ଯଦି(file && *file) {
    ଯଦି(!strcmp(file, "-"))
      fp = stdin;
    ଅନ୍ୟଥା {
      fp = curlx_fopen(file, "rb");
      ଯଦି(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      ଅନ୍ୟଥା {
        curlx_struct_stat stat;
        ଯଦି((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = ଖାଲି;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        ଅନ୍ୟଥା
          handle = fp;
      }
    }
  }

  ଯଦି(fp) {
    ସଂରଚନା dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    କର {
      result = Curl_get_ଧାଡ଼ି(&buf, fp, &eof);
      ଯଦି(!result) {
        ଅଚଳ ଅକ୍ଷର *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        ଯଦି(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, ଖାଲି, ଖାଲି,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        ଯଦି(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } ଯେତେବେଳେ(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    ଯଦି(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  ଫେରନ୍ତୁ result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(ସଂରଚନା Curl_easy *data,
                               ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  CURLcode result = CURLE_OK;
  ସଂରଚନା curl_slist *list = data->state.cookielist;
  ଯଦି(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ଯଦି(!data->cookies)
      data->cookies = Curl_cookie_init();
    ଯଦି(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    ଅନ୍ୟଥା {
      data->state.cookie_engine = TRUE;
      ଯେତେବେଳେ(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        ଯଦି(result)
          ଭାଙ୍ଗ;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  ଫେରନ୍ତୁ result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cookie_sort(ଅଚଳ ଶୂନ୍ୟ *p1, ଅଚଳ ଶୂନ୍ୟ *p2)
{
  ଅଚଳ ସଂରଚନା Cookie *c1 = *(ଅଚଳ ସଂରଚନା Cookie * ଅଚଳ *)p1;
  ଅଚଳ ସଂରଚନା Cookie *c2 = *(ଅଚଳ ସଂରଚନା Cookie * ଅଚଳ *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  ଯଦି(l1 != l2)
    ଫେରନ୍ତୁ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  ଯଦି(l1 != l2)
    ଫେରନ୍ତୁ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  ଯଦି(l1 != l2)
    ଫେରନ୍ତୁ (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  ଫେରନ୍ତୁ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା cookie_sort_ct(ଅଚଳ ଶୂନ୍ୟ *p1, ଅଚଳ ଶୂନ୍ୟ *p2)
{
  ଅଚଳ ସଂରଚନା Cookie *c1 = *(ଅଚଳ ସଂରଚନା Cookie * ଅଚଳ *)p1;
  ଅଚଳ ସଂରଚନା Cookie *c2 = *(ଅଚଳ ସଂରଚନା Cookie * ଅଚଳ *)p2;

  ଫେରନ୍ତୁ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *host)
{
  ଫେରନ୍ତୁ Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(ସଂରଚନା Curl_easy *data,
                             bool *okay,
                             ଅଚଳ ଅକ୍ଷର *host,
                             ସଂରଚନା Curl_llist *list)
{
  size_t matches = 0;
  ଅଚଳ bool is_ip = Curl_host_is_ipnum(host);
  ଅଚଳ size_t myhash = cookiehash(host);
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
  ଅଚଳ bool secure = Curl_secure_context(data, host);
  ସଂରଚନା CookieInfo *ci = data->cookies;
  ଅଚଳ ଅକ୍ଷର *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, ଖାଲି);

  ଯଦି(!ci || !Curl_llist_ଗଣନା(&ci->cookielist[myhash]))
    ଫେରନ୍ତୁ CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  ପାଇଁ(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_ଗଣ୍ଠି_next(n)) {
    ସଂରଚନା Cookie *co = Curl_ଗଣ୍ଠି_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    ଯଦି(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      ଯଦି(!co->domain ||
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
        ଯଦି(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          ଯଦି(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            ଭାଙ୍ଗ;
          }
        }
      }
    }
  }

  ଯଦି(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    ସଂରଚନା Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(ଆକାର(ସଂରଚନା Cookie *) * matches);
    ଯଦି(!array) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ fail;
    }

    n = Curl_llist_head(list);

    ପାଇଁ(i = 0; n; n = Curl_ଗଣ୍ଠି_next(n))
      array[i++] = Curl_ଗଣ୍ଠି_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, ଆକାର(ସଂରଚନା Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, ଖାଲି);

    ପାଇଁ(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  ଫେରନ୍ତୁ CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, ଖାଲି);
  ଫେରନ୍ତୁ result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
ଶୂନ୍ୟ Curl_cookie_clearall(ସଂରଚନା CookieInfo *ci)
{
  ଯଦି(ci) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    ପାଇଁ(i = 0; i < COOKIE_HASH_SIZE; i++) {
      ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;
      ପାଇଁ(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        ସଂରଚନା Cookie *c = Curl_ଗଣ୍ଠି_elem(n);
        ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e = Curl_ଗଣ୍ଠି_next(n);
        Curl_ଗଣ୍ଠି_remove(n);
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
ଶୂନ୍ୟ Curl_cookie_clearsess(ସଂରଚନା CookieInfo *ci)
{
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ଯଦି(!ci)
    ଫେରନ୍ତୁ;

  ପାଇଁ(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n = Curl_llist_head(&ci->cookielist[i]);
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *e = ଖାଲି;

    ପାଇଁ(; n; n = e) {
      ସଂରଚନା Cookie *curr = Curl_ଗଣ୍ଠି_elem(n);
      e = Curl_ଗଣ୍ଠି_next(n); /* in case the node is removed, get it early */
      ଯଦି(!curr->expires) {
        Curl_ଗଣ୍ଠି_remove(n);
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
ଶୂନ୍ୟ Curl_cookie_cleanup(ସଂରଚନା CookieInfo *ci)
{
  ଯଦି(ci) {
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
ସ୍ଥିର ଅକ୍ଷର *get_netscape_format(ଅଚଳ ସଂରଚନା Cookie *co)
{
  ଫେରନ୍ତୁ curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*ମୂଲ୍ୟ*/
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
    co->ମୂଲ୍ୟ ? co->ମୂଲ୍ୟ : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
ସ୍ଥିର CURLcode cookie_output(ସଂରଚନା Curl_easy *data,
                              ସଂରଚନା CookieInfo *ci,
                              ଅଚଳ ଅକ୍ଷର *filename)
{
  FILE *out = ଖାଲି;
  bool use_stdout = FALSE;
  ଅକ୍ଷର *tempstore = ଖାଲି;
  CURLcode result = CURLE_OK;

  ଯଦି(!ci)
    /* no cookie engine alive */
    ଫେରନ୍ତୁ CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  ଯଦି(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  ଅନ୍ୟଥା {
    result = Curl_fopen(data, filename, &out, &tempstore);
    ଯଦି(result)
      ଯାଅ error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  ଯଦି(ci->numcookies) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
    size_t nvalid = 0;
    ସଂରଚନା Cookie **array;
    ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;

    array = curlx_calloc(1, ଆକାର(ସଂରଚନା Cookie *) * ci->numcookies);
    ଯଦି(!array) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ error;
    }

    /* only sort the cookies with a domain property */
    ପାଇଁ(i = 0; i < COOKIE_HASH_SIZE; i++) {
      ପାଇଁ(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_ଗଣ୍ଠି_next(n)) {
        ସଂରଚନା Cookie *co = Curl_ଗଣ୍ଠି_elem(n);
        ଯଦି(!co->domain)
          ଜାରି;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, ଆକାର(ସଂରଚନା Cookie *), cookie_sort_ct);

    ପାଇଁ(i = 0; i < nvalid; i++) {
      ଅକ୍ଷର *format_ptr = get_netscape_format(array[i]);
      ଯଦି(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        ଯାଅ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  ଯଦି(!use_stdout) {
    curlx_fclose(out);
    out = ଖାଲି;
    ଯଦି(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      ଯାଅ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  ଫେରନ୍ତୁ CURLE_OK;

error:
  ଯଦି(out && !use_stdout)
    curlx_fclose(out);
  ଯଦି(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର ସଂରଚନା curl_slist *cookie_list(ଅଚଳ ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା curl_slist *list = ଖାଲି;
  ସଂରଚନା curl_slist *beg;
  ସଂରଚନା curl_slist **tail = &list;
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;
  ସଂରଚନା Curl_llist_ଗଣ୍ଠି *n;

  ଯଦି(!data->cookies || (data->cookies->numcookies == 0))
    ଫେରନ୍ତୁ ଖାଲି;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  ପାଇଁ(i = 0; i < COOKIE_HASH_SIZE; i++) {
    ପାଇଁ(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_ଗଣ୍ଠି_next(n)) {
      ସଂରଚନା Cookie *c = Curl_ଗଣ୍ଠି_elem(n);
      ଅକ୍ଷର *ଧାଡ଼ି;
      ଯଦି(!c->domain)
        ଜାରି;
      ଧାଡ଼ି = get_netscape_format(c);
      ଯଦି(!ଧାଡ଼ି) {
        curl_slist_free_all(list);
        ଫେରନ୍ତୁ ଖାଲି;
      }
      beg = Curl_slist_append_nodup(ଖାଲି, ଧାଡ଼ି);
      ଯଦି(!beg) {
        curlx_free(ଧାଡ଼ି);
        curl_slist_free_all(list);
        ଫେରନ୍ତୁ ଖାଲି;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  ଫେରନ୍ତୁ list;
}

ସଂରଚନା curl_slist *Curl_cookie_list(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  ଫେରନ୍ତୁ list;
}

ଶୂନ୍ୟ Curl_flush_cookies(ସଂରଚନା Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  ଯଦି(data->cookies) {
    ଅଚଳ ଅକ୍ଷର *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    ଯଦି(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      ଯଦି(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    ଯଦି(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = ଖାଲି;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

ଶୂନ୍ୟ Curl_cookie_run(ସଂରଚନା Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  ଯଦି(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
