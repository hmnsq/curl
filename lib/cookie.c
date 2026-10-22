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
#include "curl_setup.h"

#ако !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#include "urldata.h"
#include "cookie.h"
#include "psl.h"
#include "curl_trc.h"
#include "transfer.h"
#include "slist.h"
#include "curl_share.h"
#include "strcase.h"
#include "curl_fopen.h"
#include "curl_get_line.h"
#include "curl_memrchr.h"
#include "parsedate.h"
#include "curlx/strdup.h"
#include "curlx/dynbuf.h"
#include "llist.h"
#include "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
static void cap_expires(time_t now, struct Cookie *co)
{
  ако(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    ако(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

static void freecookie(struct Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->value);
  ако(maintoo)
    curlx_free(co);
}

static bool cookie_tailmatch(const char *cookie_domain,
                             const size_t cookie_domain_len,
                             const char *hostname)
{
  size_t hostname_len = strlen(hostname);

  ако(hostname_len < cookie_domain_len)
    врати FALSE;

  ако(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    врати FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  ако(hostname_len == cookie_domain_len)
    врати TRUE;
  ако('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    врати TRUE;
  врати FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
static bool pathmatch(const char *cookie_path, const char *uri_path)
{
  size_t cookie_path_len;
  size_t uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = strlen(cookie_path);
  ако(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    врати TRUE;
  }

  /* #-fragments are already cut off! */
  ако(strlen(uri_path) == 0 || uri_path[0] != '/')
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

  ако(uri_path_len < cookie_path_len)
    goto pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  ако(strncmp(cookie_path, uri_path, cookie_path_len))
    goto pathmatched;

  /* The cookie-path and the uri-path are identical. */
  ако(cookie_path_len == uri_path_len) {
    ret = TRUE;
    goto pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  ако(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    goto pathmatched;
  }

pathmatched:
  врати ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
static const char *get_top_domain(const char * const domain, size_t *outlen)
{
  size_t len = 0;
  const char *first = NULL, *last;

  ако(domain) {
    len = strlen(domain);
    last = memrchr(domain, '.', len);
    ако(last) {
      first = memrchr(domain, '.', (last - domain));
      ако(first)
        len -= (++first - domain);
    }
  }

  ако(outlen)
    *outlen = len;

  врати first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#ако defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
static size_t cookie_hash_domain(const char *domain, const size_t len)
{
  const char *end = domain + len;
  size_t h = 5381;

  док(domain < end) {
    size_t j = (size_t)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  врати (h % COOKIE_HASH_SIZE);
}

#ако defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
static size_t cookiehash(const char * const domain)
{
  const char *top;
  size_t len;

  ако(!domain || Curl_host_is_ipnum(domain))
    врати 0;

  top = get_top_domain(domain, &len);
  врати cookie_hash_domain(top, len);
}

/*
 * cookie path sanitize
 */
static char *sanitize_cookie_path(const char *cookie_path, size_t len)
{
  /* some sites send path attribute within '"'. */
  ако(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    ако(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  ако(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    врати curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  ако(len > 1 && cookie_path[len - 1] == '/')
    len--;

  врати curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
static CURLcode strstore(char **str, const char *newstr, size_t len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  ако(!*str)
    врати CURLE_OUT_OF_MEMORY;
  врати CURLE_OK;
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
static void remove_expired(struct CookieInfo *ci)
{
  struct Cookie *co;
  curl_off_t now = (curl_off_t)time(NULL);
  unsigned цео i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  ако(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    врати;
  инако
    ci->next_expiration = CURL_OFF_T_MAX;

  за(i = 0; i < COOKIE_HASH_SIZE; i++) {
    struct Curl_llist_node *n;
    struct Curl_llist_node *e = NULL;

    за(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_node_elem(n);
      e = Curl_node_next(n);
      ако(co->expires) {
        ако(co->expires < now) {
          Curl_node_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        инако ако(co->expires < ci->next_expiration)
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
static bool bad_domain(const char *domain, size_t len)
{
  ако((len == 9) && curl_strnequal(domain, "localhost", 9))
    врати FALSE;
  инако {
    /* there must be a dot present, but that dot must not be a trailing dot */
    const char *dot = memchr(domain, '.', len);
    ако(dot) {
      size_t i = dot - domain;
      ако((len - i) > 1)
        /* the dot is not the last byte */
        врати FALSE;
    }
  }
  врати TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
static bool invalid_octets(const char *ptr, size_t len)
{
  const unsigned char *p = (const unsigned char *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  док(len && *p) {
    ако((*p < 0x20) || (*p == 0x7f))
      врати TRUE;
    p++;
    len--;
  }
  врати FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_VALUE  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

static CURLcode storecookie(struct Cookie *co, const struct Curl_str *cp,
                            const char *path, const char *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  ако(!result)
    result = strstore(&co->value, curlx_str(&cp[COOKIE_VALUE]),
                      curlx_strlen(&cp[COOKIE_VALUE]));
  ако(!result) {
    size_t plen = 0;
    ако(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    инако ако(path) {
      /* No path was given in the header line, set the default */
      const char *endslash = strrchr(path, '/');
      ако(endslash)
        plen = endslash - path + 1; /* include end slash */
      инако
        plen = strlen(path);
    }

    ако(path) {
      co->path = sanitize_cookie_path(path, plen);
      ако(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  ако(!result) {
    ако(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    инако ако(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      ако(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  врати result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
static bool parse_first_pair(struct Curl_easy *data, struct Cookie *co,
                             struct Curl_str *cookie,
                             struct Curl_str *name,
                             struct Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  ако(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    врати FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  ако((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    врати FALSE;
  }

  /* Check if we have a reserved prefix set. */
  ако(!strncmp("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  инако ако(!strncmp("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_VALUE] = *val;
  врати TRUE;
}

static bool parse_domain(struct Curl_easy *data, struct Cookie *co,
                         struct Curl_str *cookie_domain,
                         struct Curl_str *val,
                         const char **domainp)
{
  bool is_ip;
  const char *domain = *domainp;
  const char *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  ако('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  ако(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  ако(!domain ||
     (is_ip &&
      !strncmp(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == strlen(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    ако(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  инако {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    врати FALSE;
  }
  врати TRUE;
}

static void parse_maxage(struct Cookie *co, struct Curl_str *val,
                         time_t *nowp)
{
  цео rc;
  const char *maxage = curlx_str(val);
  ако(*maxage == '\"')
    maxage++;
  rc = curlx_str_number(&maxage, &co->expires, CURL_OFF_T_MAX);
  ако(!*nowp)
    *nowp = time(NULL);
  switch(rc) {
  case STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    break;
  default:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    break;
  case STRE_OK:
    ако(!co->expires)
      co->expires = 1; /* expire now */
    инако ако(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    инако
      co->expires += *nowp;
    break;
  }
  cap_expires(*nowp, co);
}

static void parse_expires(struct Cookie *co, struct Curl_str *val,
                          time_t *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  ако(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    char dbuf[MAX_DATE_LENGTH + 1];
    time_t date = 0;
    memcpy(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    ако(!Curl_getdate_capped(dbuf, &date)) {
      ако(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    инако
      co->expires = 0;
    ако(!*nowp)
      *nowp = time(NULL);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
static CURLcode
parse_cookie_header(struct Curl_easy *data,
                    struct Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    const char *ptr, /* the header */
                    const char *domain, /* default domain */
                    /* full path used when this cookie is set */
                    const char *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  time_t now = 0;
  size_t linelength = strlen(ptr);
  CURLcode result = CURLE_OK;
  struct Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  ако(linelength > MAX_COOKIE_LINE)
    /* discard overly long lines at once */
    врати CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  memset(cookie, 0, sizeof(cookie));
  do {
    struct Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    цео rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    ако(!rc || (rc == STRE_SHORT)) {
      struct Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      ако(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        врати CURLE_OK;
      }

      ако(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        ако(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        ако(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          врати CURLE_OK;
        }
      }
      инако
        curlx_str_init(&val);

      ако(!curlx_strlen(&cookie[COOKIE_NAME])) {
        ако(!parse_first_pair(data, co, cookie, &name, &val, sep))
          врати CURLE_OK;
      }

      /* whether there's separator or not */
      инако ако(curlx_str_casecompare(&name, "secure")) {
        ако(secure_origin)
          co->secure = TRUE;
        инако {
          infof(data, "skipped cookie because not 'secure'");
          врати CURLE_OK;
        }
      }
      инако ако(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      инако ако(sep) {
        /* only if a separating equals sign is present */
        ако(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        инако ако(curlx_str_casecompare(&name, "domain")) {
          ако(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            врати CURLE_OK;
        }
        инако ако(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        инако ако(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } док(!curlx_str_single(&ptr, ';'));

  ако(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    ако(!result)
      *okay = TRUE;
  }
  врати result;
}

static CURLcode parse_netscape(struct Cookie *co,
                               const struct CookieInfo *ci,
                               bool *okay,
                               const char *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  const char *ptr, *next;
  цео fields;
  size_t len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  ако(!strncmp(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  ако(lineptr[0] == '#')
    /* do not even try the comments */
    врати CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  за(next = lineptr; next; fields++) {
    ptr = next;
    len = strcspn(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : NULL);
    switch(fields) {
    case 0:
      ако(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      ако(!co->domain)
        врати CURLE_OUT_OF_MEMORY;
      break;
    case 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      break;
    case 2:
      /* The file format allows the path field to remain not filled in */
      ако(strncmp("TRUE", ptr, len) && strncmp("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        ако(!co->path)
          врати CURLE_OUT_OF_MEMORY;
        break;
      }
      инако {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        ако(!co->path)
          врати CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    case 3:
      co->secure = FALSE;
      ако(curl_strnequal(ptr, "TRUE", len)) {
        ако(secure_origin || ci->running)
          co->secure = TRUE;
        инако
          врати CURLE_OK;
      }
      break;
    case 4:
      ако(curlx_str_number(&ptr, &co->expires, CURL_OFF_T_MAX))
        врати CURLE_OK;
      break;
    case 5:
      co->name = curlx_memdup0(ptr, len);
      ако(!co->name)
        врати CURLE_OUT_OF_MEMORY;
      инако {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        ако(!strncmp("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        инако ако(!strncmp("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      break;
    case 6:
      co->value = curlx_memdup0(ptr, len);
      ако(!co->value)
        врати CURLE_OUT_OF_MEMORY;
      break;
    }
  }
  ако(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->value = curlx_strdup("");
    ако(!co->value)
      врати CURLE_OUT_OF_MEMORY;
    инако
      fields++;
  }

  ако(fields != 7)
    /* we did not find the sufficient number of fields */
    врати CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  ако(invalid_octets(co->name, strlen(co->name)) ||
     invalid_octets(co->value, strlen(co->value)))
    врати CURLE_OK;

  *okay = TRUE;
  врати CURLE_OK;
}

static bool is_public_suffix(struct Curl_easy *data,
                             struct Cookie *co,
                             const char *domain)
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
  ако(!co->domain || Curl_host_is_ipnum(co->domain))
    врати FALSE;

  инако {
    bool acceptable = FALSE;
    char lcase[256];
    char lcookie[256];
    size_t dlen = domain ? strlen(domain) : 0;
    size_t clen = strlen(co->domain);

    /* trim trailing dots */
    ако(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    ако(clen && (co->domain[clen - 1] == '.'))
      clen--;

    ако((dlen < sizeof(lcase)) && (clen < sizeof(lcookie))) {
      const psl_ctx_t *psl = Curl_psl_use(data);
      ако(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        ако(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          ако(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        инако {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          ако(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      инако
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    ако(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      врати TRUE;
    }
  }
#инако
  (void)data;
  (void)co;
  (void)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  врати FALSE;
}

/* returns TRUE when replaced */
static bool replace_existing(struct Curl_easy *data,
                             struct Cookie *co,
                             const struct CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  struct Curl_llist_node *replace_n = NULL;
  struct Curl_llist_node *n;
  size_t myhash = cookiehash(co->domain);
  за(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_node_next(n)) {
    struct Cookie *clist = Curl_node_elem(n);
    ако(!strcmp(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      ако(clist->domain && co->domain) {
        ако(cookie_tailmatch(clist->domain, strlen(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, strlen(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      инако ако(!clist->domain && !co->domain)
        matching_domains = TRUE;

      ако(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        size_t cllen;
        const char *sep = NULL;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        ако(clist->path[0])
          sep = strchr(clist->path + 1, '/');
        ако(sep)
          cllen = sep - clist->path;
        инако
          cllen = strlen(clist->path);

        ако(!strncmp(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          врати FALSE;
        }
      }
    }

    ако(!replace_n && !strcmp(clist->name, co->name)) {
      /* the names are identical */

      ако(clist->domain && co->domain) {
        ако(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      инако ако(!clist->domain && !co->domain)
        replace_old = TRUE;

      ако(replace_old) {
        /* the domains were identical */

        ако(clist->path && co->path &&
           strcmp(clist->path, co->path))
          replace_old = FALSE;
        инако ако(!clist->path != !co->path)
          replace_old = FALSE;
      }

      ако(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        врати FALSE;
      }
      ако(replace_old)
        replace_n = n;
    }
  }
  ако(replace_n) {
    struct Cookie *repl = Curl_node_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_node_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  врати TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(struct Curl_easy *data,
                         struct CookieInfo *ci,
                         const char *lineptr, /* first character of the line */
                         const char *domain,  /* default domain */
                         const char *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         const цео flags)
{
  struct Cookie comem;
  struct Cookie *co;
  size_t myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  ако(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    врати CURLE_OK; /* silently ignore */

  co = &comem;
  memset(co, 0, sizeof(comem));

  ако(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  инако
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  ако(result || !okay)
    goto fail;

  ако(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    goto fail;

  ако(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    goto fail;

  ако(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    ако(co->secure && co->path && !strcmp(co->path, "/") && !co->tailmatch)
      ;
    инако
      goto fail;
  }

  ако(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    goto fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  ако(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  ако(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    goto fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, sizeof(comem));
  ако(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    goto fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->node);

  ако(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->value,
          co->domain, co->path, co->expires);

  ако(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  ако(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  ако(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  врати result;
fail:
  freecookie(co, FALSE);
  врати result;
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
struct CookieInfo *Curl_cookie_init(void)
{
  цео i;
  struct CookieInfo *ci = curlx_calloc(1, sizeof(struct CookieInfo));
  ако(!ci)
    врати NULL;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  за(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], NULL);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  врати ci;
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
static CURLcode cookie_load(struct Curl_easy *data, const char *file,
                            struct CookieInfo *ci, цео flags)
{
  FILE *handle = NULL;
  CURLcode result = CURLE_OK;
  FILE *fp = NULL;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  ако(file && *file) {
    ако(!strcmp(file, "-"))
      fp = stdin;
    инако {
      fp = curlx_fopen(file, "rb");
      ако(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      инако {
        curlx_struct_stat stat;
        ако((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = NULL;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        инако
          handle = fp;
      }
    }
  }

  ако(fp) {
    struct dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    do {
      result = Curl_get_line(&buf, fp, &eof);
      ако(!result) {
        const char *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        ако(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, NULL, NULL,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        ако(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } док(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    ако(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  врати result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(struct Curl_easy *data,
                               цео flags)
{
  CURLcode result = CURLE_OK;
  struct curl_slist *list = data->state.cookielist;
  ако(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    ако(!data->cookies)
      data->cookies = Curl_cookie_init();
    ако(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    инако {
      data->state.cookie_engine = TRUE;
      док(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        ако(result)
          break;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  врати result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
static цео cookie_sort(const void *p1, const void *p2)
{
  const struct Cookie *c1 = *(const struct Cookie * const *)p1;
  const struct Cookie *c2 = *(const struct Cookie * const *)p2;
  size_t l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? strlen(c1->path) : 0;
  l2 = c2->path ? strlen(c2->path) : 0;

  ако(l1 != l2)
    врати (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? strlen(c1->domain) : 0;
  l2 = c2->domain ? strlen(c2->domain) : 0;

  ако(l1 != l2)
    врати (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? strlen(c1->name) : 0;
  l2 = c2->name ? strlen(c2->name) : 0;

  ако(l1 != l2)
    врати (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  врати (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
static цео cookie_sort_ct(const void *p1, const void *p2)
{
  const struct Cookie *c1 = *(const struct Cookie * const *)p1;
  const struct Cookie *c2 = *(const struct Cookie * const *)p2;

  врати (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(struct Curl_easy *data, const char *host)
{
  врати Curl_xfer_is_secure(data) ||
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
CURLcode Curl_cookie_getlist(struct Curl_easy *data,
                             bool *okay,
                             const char *host,
                             struct Curl_llist *list)
{
  size_t matches = 0;
  const bool is_ip = Curl_host_is_ipnum(host);
  const size_t myhash = cookiehash(host);
  struct Curl_llist_node *n;
  const bool secure = Curl_secure_context(data, host);
  struct CookieInfo *ci = data->cookies;
  const char *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, NULL);

  ако(!ci || !Curl_llist_count(&ci->cookielist[myhash]))
    врати CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  за(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_node_next(n)) {
    struct Cookie *co = Curl_node_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    ако(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      ако(!co->domain ||
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
        ако(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          ако(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            break;
          }
        }
      }
    }
  }

  ако(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    struct Cookie **array;
    size_t i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(sizeof(struct Cookie *) * matches);
    ако(!array) {
      result = CURLE_OUT_OF_MEMORY;
      goto fail;
    }

    n = Curl_llist_head(list);

    за(i = 0; n; n = Curl_node_next(n))
      array[i++] = Curl_node_elem(n);

    /* now sort the cookie pointers in path length order */
    qsort(array, matches, sizeof(struct Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, NULL);

    за(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  врати CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, NULL);
  врати result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
void Curl_cookie_clearall(struct CookieInfo *ci)
{
  ако(ci) {
    unsigned цео i;
    за(i = 0; i < COOKIE_HASH_SIZE; i++) {
      struct Curl_llist_node *n;
      за(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        struct Cookie *c = Curl_node_elem(n);
        struct Curl_llist_node *e = Curl_node_next(n);
        Curl_node_remove(n);
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
void Curl_cookie_clearsess(struct CookieInfo *ci)
{
  unsigned цео i;

  ако(!ci)
    врати;

  за(i = 0; i < COOKIE_HASH_SIZE; i++) {
    struct Curl_llist_node *n = Curl_llist_head(&ci->cookielist[i]);
    struct Curl_llist_node *e = NULL;

    за(; n; n = e) {
      struct Cookie *curr = Curl_node_elem(n);
      e = Curl_node_next(n); /* in case the node is removed, get it early */
      ако(!curr->expires) {
        Curl_node_remove(n);
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
void Curl_cookie_cleanup(struct CookieInfo *ci)
{
  ако(ci) {
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
static char *get_netscape_format(const struct Cookie *co)
{
  врати curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /* value */
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
    co->value ? co->value : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
static CURLcode cookie_output(struct Curl_easy *data,
                              struct CookieInfo *ci,
                              const char *filename)
{
  FILE *out = NULL;
  bool use_stdout = FALSE;
  char *tempstore = NULL;
  CURLcode result = CURLE_OK;

  ако(!ci)
    /* no cookie engine alive */
    врати CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  ако(!strcmp("-", filename)) {
    /* use stdout */
    out = stdout;
    use_stdout = TRUE;
  }
  инако {
    result = Curl_fopen(data, filename, &out, &tempstore);
    ако(result)
      goto error;
  }

  fputs("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  ако(ci->numcookies) {
    unsigned цео i;
    size_t nvalid = 0;
    struct Cookie **array;
    struct Curl_llist_node *n;

    array = curlx_calloc(1, sizeof(struct Cookie *) * ci->numcookies);
    ако(!array) {
      result = CURLE_OUT_OF_MEMORY;
      goto error;
    }

    /* only sort the cookies with a domain property */
    за(i = 0; i < COOKIE_HASH_SIZE; i++) {
      за(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_node_next(n)) {
        struct Cookie *co = Curl_node_elem(n);
        ако(!co->domain)
          continue;
        array[nvalid++] = co;
      }
    }

    qsort(array, nvalid, sizeof(struct Cookie *), cookie_sort_ct);

    за(i = 0; i < nvalid; i++) {
      char *format_ptr = get_netscape_format(array[i]);
      ако(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        goto error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  ако(!use_stdout) {
    curlx_fclose(out);
    out = NULL;
    ако(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      goto error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  врати CURLE_OK;

error:
  ако(out && !use_stdout)
    curlx_fclose(out);
  ако(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  врати result;
}

static struct curl_slist *cookie_list(const struct Curl_easy *data)
{
  struct curl_slist *list = NULL;
  struct curl_slist *beg;
  struct curl_slist **tail = &list;
  unsigned цео i;
  struct Curl_llist_node *n;

  ако(!data->cookies || (data->cookies->numcookies == 0))
    врати NULL;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  за(i = 0; i < COOKIE_HASH_SIZE; i++) {
    за(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_node_next(n)) {
      struct Cookie *c = Curl_node_elem(n);
      char *line;
      ако(!c->domain)
        continue;
      line = get_netscape_format(c);
      ако(!line) {
        curl_slist_free_all(list);
        врати NULL;
      }
      beg = Curl_slist_append_nodup(NULL, line);
      ако(!beg) {
        curlx_free(line);
        curl_slist_free_all(list);
        врати NULL;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  врати list;
}

struct curl_slist *Curl_cookie_list(struct Curl_easy *data)
{
  struct curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  врати list;
}

void Curl_flush_cookies(struct Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  ако(data->cookies) {
    const char *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    ако(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      ако(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    ако(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = NULL;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

void Curl_cookie_run(struct Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  ако(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
