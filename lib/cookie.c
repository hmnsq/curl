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

#यदि !defined(CURL_DISABLE_HTTP) && !defined(CURL_DISABLE_COOKIES)

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "cookie.h"
#अन्तर्भाव "psl.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "slist.h"
#अन्तर्भाव "curl_share.h"
#अन्तर्भाव "strcase.h"
#अन्तर्भाव "curl_fopen.h"
#अन्तर्भाव "curl_get_line.h"
#अन्तर्भाव "curl_memrchr.h"
#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curlx/strdup.h"
#अन्तर्भाव "curlx/dynbuf.h"
#अन्तर्भाव "llist.h"
#अन्तर्भाव "curlx/strparse.h"

/* number of seconds in 400 days */
#define COOKIES_MAXAGE (400 * 24 * 3600)

/* Make sure cookies never expire further away in time than 400 days into the
   future. (from RFC6265bis draft-19)

   For the sake of easier testing, align the capped time to an even 60 second
   boundary. */
स्थिर शून्य cap_expires(समय_प्रकार now, संरचना Cookie *co)
{
  यदि(co->expires && (TIME_T_MAX - COOKIES_MAXAGE - 30) > now) {
    timediff_t cap = now + COOKIES_MAXAGE;
    यदि(co->expires > cap) {
      cap += 30;
      co->expires = (cap / 60) * 60;
    }
  }
}

स्थिर शून्य freecookie(संरचना Cookie *co, bool maintoo)
{
  curlx_free(co->domain);
  curlx_free(co->path);
  curlx_free(co->name);
  curlx_free(co->मान);
  यदि(maintoo)
    curlx_free(co);
}

स्थिर bool cookie_tailmatch(अचर वर्ण *cookie_domain,
                             अचर माप_प्रकार cookie_domain_len,
                             अचर वर्ण *hostname)
{
  माप_प्रकार hostname_len = म_माप(hostname);

  यदि(hostname_len < cookie_domain_len)
    फर्काऊ FALSE;

  यदि(!curl_strnequal(cookie_domain,
                     hostname + hostname_len - cookie_domain_len,
                     cookie_domain_len))
    फर्काऊ FALSE;

  /*
   * A lead char of cookie_domain is not '.'.
   * RFC6265 4.1.2.3. The Domain Attribute says:
   * For example, if the value of the Domain attribute is
   * "example.com", the user agent will include the cookie in the Cookie
   * header when making HTTP requests to example.com, www.example.com, and
   * www.corp.example.com.
   */
  यदि(hostname_len == cookie_domain_len)
    फर्काऊ TRUE;
  यदि('.' == *(hostname + hostname_len - cookie_domain_len - 1))
    फर्काऊ TRUE;
  फर्काऊ FALSE;
}

/*
 * matching cookie path and URL path
 * RFC6265 5.1.4 Paths and Path-Match
 */
स्थिर bool pathmatch(अचर वर्ण *cookie_path, अचर वर्ण *uri_path)
{
  माप_प्रकार cookie_path_len;
  माप_प्रकार uri_path_len;
  bool ret = FALSE;

  /* cookie_path must not have last '/' separator. ex: /sample */
  cookie_path_len = म_माप(cookie_path);
  यदि(cookie_path_len == 1) {
    /* cookie_path must be '/' */
    फर्काऊ TRUE;
  }

  /* #-fragments are already cut off! */
  यदि(म_माप(uri_path) == 0 || uri_path[0] != '/')
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

  uri_path_len = म_माप(uri_path);

  यदि(uri_path_len < cookie_path_len)
    गच्छ pathmatched;

  /* not using checkprefix() because matching should be case-sensitive */
  यदि(म_भेदन(cookie_path, uri_path, cookie_path_len))
    गच्छ pathmatched;

  /* The cookie-path and the uri-path are identical. */
  यदि(cookie_path_len == uri_path_len) {
    ret = TRUE;
    गच्छ pathmatched;
  }

  /* here, cookie_path_len < uri_path_len */
  यदि(uri_path[cookie_path_len] == '/') {
    ret = TRUE;
    गच्छ pathmatched;
  }

pathmatched:
  फर्काऊ ret;
}

/*
 * Return the top-level domain, for optimal hashing.
 */
स्थिर अचर वर्ण *get_शीर्ष_domain(अचर वर्ण * अचर domain, माप_प्रकार *outlen)
{
  माप_प्रकार len = 0;
  अचर वर्ण *first = रिक्त, *last;

  यदि(domain) {
    len = म_माप(domain);
    last = memrchr(domain, '.', len);
    यदि(last) {
      first = memrchr(domain, '.', (last - domain));
      यदि(first)
        len -= (++first - domain);
    }
  }

  यदि(outlen)
    *outlen = len;

  फर्काऊ first ? first : domain;
}

/* Avoid C1001, an "internal error" with MSVC14 */
#यदि defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", off)
#endif

/*
 * A case-insensitive hash for the cookie domains.
 */
स्थिर माप_प्रकार cookie_hash_domain(अचर वर्ण *domain, अचर माप_प्रकार len)
{
  अचर वर्ण *end = domain + len;
  माप_प्रकार h = 5381;

  जबसम्म(domain < end) {
    माप_प्रकार j = (माप_प्रकार)Curl_raw_toupper(*domain++);
    h += h << 5;
    h ^= j;
  }

  फर्काऊ (h % COOKIE_HASH_SIZE);
}

#यदि defined(_MSC_VER) && (_MSC_VER == 1900)
#pragma optimize("", on)
#endif

/*
 * Hash this domain.
 */
स्थिर माप_प्रकार cookiehash(अचर वर्ण * अचर domain)
{
  अचर वर्ण *शीर्ष;
  माप_प्रकार len;

  यदि(!domain || Curl_host_is_ipnum(domain))
    फर्काऊ 0;

  शीर्ष = get_शीर्ष_domain(domain, &len);
  फर्काऊ cookie_hash_domain(शीर्ष, len);
}

/*
 * cookie path sanitize
 */
स्थिर वर्ण *sanitize_cookie_path(अचर वर्ण *cookie_path, माप_प्रकार len)
{
  /* some sites send path attribute within '"'. */
  यदि(len && (cookie_path[0] == '\"')) {
    cookie_path++;
    len--;

    यदि(len && (cookie_path[len - 1] == '\"'))
      len--;
  }

  /* RFC6265 5.2.4 The Path Attribute */
  यदि(!len || (cookie_path[0] != '/'))
    /* Let cookie-path be the default-path. */
    फर्काऊ curlx_strdup("/");

  /* remove trailing slash when path is non-empty */
  /* convert /hoge/ to /hoge */
  यदि(len > 1 && cookie_path[len - 1] == '/')
    len--;

  फर्काऊ curlx_memdup0(cookie_path, len);
}

/*
 * strstore
 *
 * A thin wrapper around curlx_memdup0().
 */
स्थिर CURLcode strstore(वर्ण **str, अचर वर्ण *newstr, माप_प्रकार len)
{
  DEBUGASSERT(str);
  *str = curlx_memdup0(newstr, len);
  यदि(!*str)
    फर्काऊ CURLE_OUT_OF_MEMORY;
  फर्काऊ CURLE_OK;
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
स्थिर शून्य remove_expired(संरचना CookieInfo *ci)
{
  संरचना Cookie *co;
  curl_off_t now = (curl_off_t)समय(रिक्त);
  अचिन्हित पूर्णाङ्क i;

  /*
   * If the earliest expiration timestamp in the jar is in the future we can
   * skip scanning the whole jar and instead exit early as there will not be
   * any cookies to evict. If we need to evict, reset the next_expiration
   * counter in order to track the next one. In case the recorded first
   * expiration is the max offset, then perform the safe fallback of checking
   * all cookies.
   */
  यदि(now < ci->next_expiration &&
     ci->next_expiration != CURL_OFF_T_MAX)
    फर्काऊ;
  अन्यथा
    ci->next_expiration = CURL_OFF_T_MAX;

  लागि(i = 0; i < COOKIE_HASH_SIZE; i++) {
    संरचना Curl_llist_गाँठो *n;
    संरचना Curl_llist_गाँठो *e = रिक्त;

    लागि(n = Curl_llist_head(&ci->cookielist[i]); n; n = e) {
      co = Curl_गाँठो_elem(n);
      e = Curl_गाँठो_next(n);
      यदि(co->expires) {
        यदि(co->expires < now) {
          Curl_गाँठो_remove(n);
          freecookie(co, TRUE);
          ci->numcookies--;
        }
        अन्यथा यदि(co->expires < ci->next_expiration)
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
स्थिर bool bad_domain(अचर वर्ण *domain, माप_प्रकार len)
{
  यदि((len == 9) && curl_strnequal(domain, "localhost", 9))
    फर्काऊ FALSE;
  अन्यथा {
    /* there must be a dot present, but that dot must not be a trailing dot */
    अचर वर्ण *dot = स_प्रथम(domain, '.', len);
    यदि(dot) {
      माप_प्रकार i = dot - domain;
      यदि((len - i) > 1)
        /* the dot is not the last byte */
        फर्काऊ FALSE;
    }
  }
  फर्काऊ TRUE;
}
#endif

/* RFC 6265 section 4.1.1 says a server should accept this range:

   cookie-octet    = %x21 / %x23-2B / %x2D-3A / %x3C-5B / %x5D-7E

   Yet, Firefox and Chrome as of June 2022 accept space, comma and
   double-quotes fine. The prime reason for filtering out control bytes is that
   some HTTP servers return 400 for requests that contain such.
 */
स्थिर bool invalid_octets(अचर वर्ण *ptr, माप_प्रकार len)
{
  अचर अचिन्हित वर्ण *p = (अचर अचिन्हित वर्ण *)ptr;
  /* Reject all bytes \x01 - \x1f + \x7f */
  जबसम्म(len && *p) {
    यदि((*p < 0x20) || (*p == 0x7f))
      फर्काऊ TRUE;
    p++;
    len--;
  }
  फर्काऊ FALSE;
}

/* The maximum length we accept a date string for the 'expire' keyword. The
   standard date formats are within the 30 bytes range. This adds an extra
   margin to make sure it realistically works with what is used out there. */
#define MAX_DATE_LENGTH 80

#define COOKIE_NAME   0
#define COOKIE_मान  1
#define COOKIE_DOMAIN 2
#define COOKIE_PATH   3

#define COOKIE_PIECES 4 /* the list above */

स्थिर CURLcode storecookie(संरचना Cookie *co, अचर संरचना Curl_str *cp,
                            अचर वर्ण *path, अचर वर्ण *domain)
{
  CURLcode result;
  result = strstore(&co->name, curlx_str(&cp[COOKIE_NAME]),
                    curlx_strlen(&cp[COOKIE_NAME]));
  यदि(!result)
    result = strstore(&co->मान, curlx_str(&cp[COOKIE_मान]),
                      curlx_strlen(&cp[COOKIE_मान]));
  यदि(!result) {
    माप_प्रकार plen = 0;
    यदि(curlx_strlen(&cp[COOKIE_PATH])) {
      path = curlx_str(&cp[COOKIE_PATH]);
      plen = curlx_strlen(&cp[COOKIE_PATH]);
    }
    अन्यथा यदि(path) {
      /* No path was given in the header line, set the default */
      अचर वर्ण *endslash = म_खोजप(path, '/');
      यदि(endslash)
        plen = endslash - path + 1; /* include end slash */
      अन्यथा
        plen = म_माप(path);
    }

    यदि(path) {
      co->path = sanitize_cookie_path(path, plen);
      यदि(!co->path)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  यदि(!result) {
    यदि(curlx_strlen(&cp[COOKIE_DOMAIN]))
      result = strstore(&co->domain, curlx_str(&cp[COOKIE_DOMAIN]),
                        curlx_strlen(&cp[COOKIE_DOMAIN]));
    अन्यथा यदि(domain) {
      /* no domain was given in the header line, set the default */
      co->domain = curlx_strdup(domain);
      यदि(!co->domain)
        result = CURLE_OUT_OF_MEMORY;
    }
  }
  फर्काऊ result;
}

/*
 * Parse the first name/value pair of the cookie header, which is the actual
 * cookie name and value.
 */
स्थिर bool parse_first_pair(संरचना Curl_easy *data, संरचना Cookie *co,
                             संरचना Curl_str *cookie,
                             संरचना Curl_str *name,
                             संरचना Curl_str *val,
                             bool sep)
{
  /* The first name/value pair is the actual cookie name */
  यदि(!sep || !curlx_strlen(name)) {
    infof(data, "invalid cookie, dropped");
    फर्काऊ FALSE;
  }

  /*
   * Check for too long individual name or contents. Chrome and Firefox
   * support 4095 or 4096 bytes combo
   */
  यदि((curlx_strlen(name) + curlx_strlen(val)) > MAX_NAME) {
    infof(data, "oversized cookie dropped, name/val %zu + %zu bytes",
          curlx_strlen(name), curlx_strlen(val));
    फर्काऊ FALSE;
  }

  /* Check if we have a reserved prefix set. */
  यदि(!म_भेदन("__Secure-", curlx_str(name), 9))
    co->prefix_secure = TRUE;
  अन्यथा यदि(!म_भेदन("__Host-", curlx_str(name), 7))
    co->prefix_host = TRUE;

  cookie[COOKIE_NAME] = *name;
  cookie[COOKIE_मान] = *val;
  फर्काऊ TRUE;
}

स्थिर bool parse_domain(संरचना Curl_easy *data, संरचना Cookie *co,
                         संरचना Curl_str *cookie_domain,
                         संरचना Curl_str *val,
                         अचर वर्ण **domainp)
{
  bool is_ip;
  अचर वर्ण *domain = *domainp;
  अचर वर्ण *v = curlx_str(val);
  /*
   * Now, we make sure that our host is within the given domain, or
   * the given domain is not valid and thus cannot be set.
   */

  यदि('.' == *v)
    curlx_str_nudge(val, 1);

#ifndef USE_LIBPSL
  /*
   * Without PSL we do not know when the incoming cookie is set on a
   * TLD or otherwise "protected" suffix. To reduce risk, we require a
   * dot OR the exact hostname being "localhost".
   */
  यदि(bad_domain(curlx_str(val), curlx_strlen(val))) {
    *domainp = ":";
    domain = ":";
  }
#endif

  is_ip = Curl_host_is_ipnum(domain ? domain : curlx_str(val));

  यदि(!domain ||
     (is_ip &&
      !म_भेदन(curlx_str(val), domain, curlx_strlen(val)) &&
      (curlx_strlen(val) == म_माप(domain))) ||
     (!is_ip && cookie_tailmatch(curlx_str(val),
                                  curlx_strlen(val), domain))) {
    *cookie_domain = *val;
    यदि(!is_ip)
      co->tailmatch = TRUE; /* we always do that if the domain name was
                               given */
  }
  अन्यथा {
    /*
     * We did not get a tailmatch and then the attempted set domain is
     * not a domain to which the current host belongs. Mark as bad.
     */
    infof(data, "skipped cookie with bad tailmatch domain: %s",
          curlx_str(val));
    फर्काऊ FALSE;
  }
  फर्काऊ TRUE;
}

स्थिर शून्य parse_maxage(संरचना Cookie *co, संरचना Curl_str *val,
                         समय_प्रकार *nowp)
{
  पूर्णाङ्क rc;
  अचर वर्ण *maxage = curlx_str(val);
  यदि(*maxage == '\"')
    maxage++;
  rc = curlx_str_सङ्ख्या(&maxage, &co->expires, CURL_OFF_T_MAX);
  यदि(!*nowp)
    *nowp = समय(रिक्त);
  विकल्प(rc) {
  पक्ष STRE_OVERFLOW:
    /* overflow, used max value */
    co->expires = CURL_OFF_T_MAX;
    रोक;
  स्वभाव:
    /* negative or otherwise bad, expire */
    co->expires = 1;
    रोक;
  पक्ष STRE_OK:
    यदि(!co->expires)
      co->expires = 1; /* expire now */
    अन्यथा यदि(CURL_OFF_T_MAX - *nowp < co->expires)
      /* would overflow */
      co->expires = CURL_OFF_T_MAX;
    अन्यथा
      co->expires += *nowp;
    रोक;
  }
  cap_expires(*nowp, co);
}

स्थिर शून्य parse_expires(संरचना Cookie *co, संरचना Curl_str *val,
                          समय_प्रकार *nowp)
{
  /*
   * Let max-age have priority.
   *
   * If the date cannot get parsed for whatever reason, the cookie
   * will be treated as a session cookie
   */
  यदि(!co->expires && (curlx_strlen(val) < MAX_DATE_LENGTH)) {
    वर्ण dbuf[MAX_DATE_LENGTH + 1];
    समय_प्रकार date = 0;
    स_नकल(dbuf, curlx_str(val), curlx_strlen(val));
    dbuf[curlx_strlen(val)] = 0;
    यदि(!Curl_getdate_capped(dbuf, &date)) {
      यदि(!date)
        date++;
      co->expires = (curl_off_t)date;
    }
    अन्यथा
      co->expires = 0;
    यदि(!*nowp)
      *nowp = समय(रिक्त);
    cap_expires(*nowp, co);
  }
}

/* this function returns errors on OOM etc, not for cookie format problems */
स्थिर CURLcode
parse_cookie_header(संरचना Curl_easy *data,
                    संरचना Cookie *co,
                    bool *okay, /* if the cookie was fine */
                    अचर वर्ण *ptr, /* the header */
                    अचर वर्ण *domain, /* default domain */
                    /* full path used when this cookie is set */
                    अचर वर्ण *path,
                    bool secure_origin)
{
  /* This line was read off an HTTP-header */
  समय_प्रकार now = 0;
  माप_प्रकार linelength = म_माप(ptr);
  CURLcode result = CURLE_OK;
  संरचना Curl_str cookie[COOKIE_PIECES];
  *okay = FALSE;
  यदि(linelength > MAX_COOKIE_पङ्क्ति)
    /* discard overly long lines at once */
    फर्काऊ CURLE_OK;

  /* memset instead of initializer because gcc 4.8.1 is silly */
  स_रखो(cookie, 0, परिमाण(cookie));
  गर {
    संरचना Curl_str name;

    /* we have a <name>=<value> pair or a stand-alone word here */
    पूर्णाङ्क rc = curlx_str_cspn(&ptr, &name, ";\r\n=");
    यदि(!rc || (rc == STRE_SHORT)) {
      संरचना Curl_str val;
      bool sep = FALSE;
      curlx_str_trimblanks(&name);

      यदि(invalid_octets(curlx_str(&name), curlx_strlen(&name))) {
        infof(data, "invalid octets in name, cookie dropped");
        फर्काऊ CURLE_OK;
      }

      यदि(!curlx_str_single(&ptr, '=')) {
        sep = TRUE; /* a '=' was used */
        यदि(!curlx_str_cspn(&ptr, &val, ";\r\n"))
          curlx_str_trimblanks(&val);

        यदि(invalid_octets(curlx_str(&val), curlx_strlen(&val))) {
          infof(data, "invalid octets in value, cookie dropped");
          फर्काऊ CURLE_OK;
        }
      }
      अन्यथा
        curlx_str_init(&val);

      यदि(!curlx_strlen(&cookie[COOKIE_NAME])) {
        यदि(!parse_first_pair(data, co, cookie, &name, &val, sep))
          फर्काऊ CURLE_OK;
      }

      /* whether there's separator or not */
      अन्यथा यदि(curlx_str_casecompare(&name, "secure")) {
        यदि(secure_origin)
          co->secure = TRUE;
        अन्यथा {
          infof(data, "skipped cookie because not 'secure'");
          फर्काऊ CURLE_OK;
        }
      }
      अन्यथा यदि(curlx_str_casecompare(&name, "httponly"))
        co->httponly = TRUE;
      अन्यथा यदि(sep) {
        /* only if a separating equals sign is present */
        यदि(curlx_str_casecompare(&name, "path"))
          cookie[COOKIE_PATH] = val;
        अन्यथा यदि(curlx_str_casecompare(&name, "domain")) {
          यदि(curlx_strlen(&val) &&
             !parse_domain(data, co, &cookie[COOKIE_DOMAIN], &val, &domain))
            फर्काऊ CURLE_OK;
        }
        अन्यथा यदि(curlx_str_casecompare(&name, "max-age") && curlx_strlen(&val))
          parse_maxage(co, &val, &now);
        अन्यथा यदि(curlx_str_casecompare(&name, "expires") && curlx_strlen(&val))
          parse_expires(co, &val, &now);
      }
    }
  } जबसम्म(!curlx_str_single(&ptr, ';'));

  यदि(curlx_strlen(&cookie[COOKIE_NAME])) {
    /* the header was fine, now store the data */
    result = storecookie(co, &cookie[0], path, domain);
    यदि(!result)
      *okay = TRUE;
  }
  फर्काऊ result;
}

स्थिर CURLcode parse_netscape(संरचना Cookie *co,
                               अचर संरचना CookieInfo *ci,
                               bool *okay,
                               अचर वर्ण *lineptr,
                               bool secure_origin)
{
  /*
   * This line is NOT an HTTP header style line, we do offer support for
   * reading the odd netscape cookies-file format here
   */
  अचर वर्ण *ptr, *next;
  पूर्णाङ्क fields;
  माप_प्रकार len;
  *okay = FALSE;

  /*
   * In 2008, Internet Explorer introduced HTTP-only cookies to prevent XSS
   * attacks. Cookies marked httpOnly are not accessible to JavaScript. In
   * Firefox's cookie files, they are prefixed #HttpOnly_ and the rest
   * remains as usual, so we skip 10 characters of the line.
   */
  यदि(!म_भेदन(lineptr, "#HttpOnly_", 10)) {
    lineptr += 10;
    co->httponly = TRUE;
  }

  यदि(lineptr[0] == '#')
    /* do not even try the comments */
    फर्काऊ CURLE_OK;

  /*
   * Now loop through the fields and init the struct we already have
   * allocated
   */
  fields = 0;
  लागि(next = lineptr; next; fields++) {
    ptr = next;
    len = म_खोज(ptr, "\t\r\n");
    next = (ptr[len] == '\t' ? &ptr[len + 1] : रिक्त);
    विकल्प(fields) {
    पक्ष 0:
      यदि(ptr[0] == '.') { /* skip preceding dots */
        ptr++;
        len--;
      }
      co->domain = curlx_memdup0(ptr, len);
      यदि(!co->domain)
        फर्काऊ CURLE_OUT_OF_MEMORY;
      रोक;
    पक्ष 1:
      /*
       * flag: A TRUE/FALSE value indicating if all machines within a given
       * domain can access the variable. Set TRUE when the cookie says
       * .example.com and to false when the domain is complete www.example.com
       */
      co->tailmatch = !!curl_strnequal(ptr, "TRUE", len);
      रोक;
    पक्ष 2:
      /* The file format allows the path field to remain not filled in */
      यदि(म_भेदन("TRUE", ptr, len) && म_भेदन("FALSE", ptr, len)) {
        /* only if the path does not look like a boolean option! */
        co->path = sanitize_cookie_path(ptr, len);
        यदि(!co->path)
          फर्काऊ CURLE_OUT_OF_MEMORY;
        रोक;
      }
      अन्यथा {
        /* this does not look like a path, make one up! */
        co->path = curlx_strdup("/");
        यदि(!co->path)
          फर्काऊ CURLE_OUT_OF_MEMORY;
      }
      fields++; /* add a field and fall down to secure */
      FALLTHROUGH();
    पक्ष 3:
      co->secure = FALSE;
      यदि(curl_strnequal(ptr, "TRUE", len)) {
        यदि(secure_origin || ci->running)
          co->secure = TRUE;
        अन्यथा
          फर्काऊ CURLE_OK;
      }
      रोक;
    पक्ष 4:
      यदि(curlx_str_सङ्ख्या(&ptr, &co->expires, CURL_OFF_T_MAX))
        फर्काऊ CURLE_OK;
      रोक;
    पक्ष 5:
      co->name = curlx_memdup0(ptr, len);
      यदि(!co->name)
        फर्काऊ CURLE_OUT_OF_MEMORY;
      अन्यथा {
        /* For Netscape file format cookies we check prefix on the name.
           These prefixes are matched case sensitively, same as on the
           header path and as the 6265bis document specifies. */
        यदि(!म_भेदन("__Secure-", co->name, 9))
          co->prefix_secure = TRUE;
        अन्यथा यदि(!म_भेदन("__Host-", co->name, 7))
          co->prefix_host = TRUE;
      }
      रोक;
    पक्ष 6:
      co->मान = curlx_memdup0(ptr, len);
      यदि(!co->मान)
        फर्काऊ CURLE_OUT_OF_MEMORY;
      रोक;
    }
  }
  यदि(fields == 6) {
    /* we got a cookie with blank contents, fix it */
    co->मान = curlx_strdup("");
    यदि(!co->मान)
      फर्काऊ CURLE_OUT_OF_MEMORY;
    अन्यथा
      fields++;
  }

  यदि(fields != 7)
    /* we did not find the sufficient number of fields */
    फर्काऊ CURLE_OK;

  /* Reject control octets in the name or value, matching the filtering done
     for cookies set over HTTP. A cookie loaded from a file is later sent in
     request headers, so the same bytes that make a server reject a request
     must not slip in through the file. */
  यदि(invalid_octets(co->name, म_माप(co->name)) ||
     invalid_octets(co->मान, म_माप(co->मान)))
    फर्काऊ CURLE_OK;

  *okay = TRUE;
  फर्काऊ CURLE_OK;
}

स्थिर bool is_public_suffix(संरचना Curl_easy *data,
                             संरचना Cookie *co,
                             अचर वर्ण *domain)
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
  यदि(!co->domain || Curl_host_is_ipnum(co->domain))
    फर्काऊ FALSE;

  अन्यथा {
    bool acceptable = FALSE;
    वर्ण lcase[256];
    वर्ण lcookie[256];
    माप_प्रकार dlen = domain ? म_माप(domain) : 0;
    माप_प्रकार clen = म_माप(co->domain);

    /* trim trailing dots */
    यदि(dlen && (domain[dlen - 1] == '.'))
      dlen--;
    यदि(clen && (co->domain[clen - 1] == '.'))
      clen--;

    यदि((dlen < परिमाण(lcase)) && (clen < परिमाण(lcookie))) {
      अचर psl_ctx_t *psl = Curl_psl_use(data);
      यदि(psl) {
        /* the PSL check requires lowercase domain name and pattern */
        Curl_strntolower(lcookie, co->domain, clen);
        lcookie[clen] = 0;
        यदि(domain) {
          Curl_strntolower(lcase, domain, dlen);
          lcase[dlen] = 0;
          acceptable = psl_is_cookie_domain_acceptable(psl, lcase, lcookie);

          /* if the cookie is acceptable, but is set for a PSL domain, then it
             cannot be tailmatching */
          यदि(acceptable && co->tailmatch &&
             curl_strequal(co->domain, domain) &&
             psl_is_public_suffix(psl, lcookie))
            co->tailmatch = FALSE;
        }
        अन्यथा {
          bool suffix = psl_is_public_suffix(psl, lcookie);
          acceptable = TRUE;
          यदि(suffix)
            /* switch off tailmatching when set for a PSL domain */
            co->tailmatch = FALSE;
        }
        Curl_psl_release(data);
      }
      अन्यथा
        infof(data, "libpsl problem, rejecting cookie for safety");
    }

    यदि(!acceptable) {
      infof(data, "cookie '%s' dropped, domain '%s' must not "
            "set cookies for '%s'", co->name,
            domain ? domain : "[file]", co->domain);
      फर्काऊ TRUE;
    }
  }
#अन्यथा
  (शून्य)data;
  (शून्य)co;
  (शून्य)domain;
  DEBUGF(infof(data, "NO PSL to check set-cookie '%s' for domain=%s in %s",
               co->name, co->domain, domain ? domain : "[file]"));
#endif
  फर्काऊ FALSE;
}

/* returns TRUE when replaced */
स्थिर bool replace_existing(संरचना Curl_easy *data,
                             संरचना Cookie *co,
                             अचर संरचना CookieInfo *ci,
                             bool secure,
                             bool *replacep)
{
  bool replace_old = FALSE;
  संरचना Curl_llist_गाँठो *replace_n = रिक्त;
  संरचना Curl_llist_गाँठो *n;
  माप_प्रकार myhash = cookiehash(co->domain);
  लागि(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_गाँठो_next(n)) {
    संरचना Cookie *clist = Curl_गाँठो_elem(n);
    यदि(!म_भेद(clist->name, co->name)) {
      /* the names are identical */
      bool matching_domains = FALSE;

      यदि(clist->domain && co->domain) {
        यदि(cookie_tailmatch(clist->domain, म_माप(clist->domain),
                            co->domain) ||
           cookie_tailmatch(co->domain, म_माप(co->domain), clist->domain))
          /* The existing one is a tail of the new or vice versa */
          matching_domains = TRUE;
      }
      अन्यथा यदि(!clist->domain && !co->domain)
        matching_domains = TRUE;

      यदि(matching_domains && /* the domains were identical */
         clist->path && co->path && /* both have paths */
         clist->secure && !co->secure && !secure) {
        माप_प्रकार cllen;
        अचर वर्ण *sep = रिक्त;

        /*
         * A non-secure cookie may not overlay an existing secure cookie.
         * For an existing cookie "a" with path "/login", refuse a new
         * cookie "a" with for example path "/login/en", while the path
         * "/loginhelper" is ok.
         */

        DEBUGASSERT(clist->path[0]);
        यदि(clist->path[0])
          sep = म_अक्षर(clist->path + 1, '/');
        यदि(sep)
          cllen = sep - clist->path;
        अन्यथा
          cllen = म_माप(clist->path);

        यदि(!म_भेदन(clist->path, co->path, cllen)) {
          infof(data, "cookie '%s' for domain '%s' dropped, would "
                "overlay an existing cookie", co->name, co->domain);
          फर्काऊ FALSE;
        }
      }
    }

    यदि(!replace_n && !म_भेद(clist->name, co->name)) {
      /* the names are identical */

      यदि(clist->domain && co->domain) {
        यदि(curl_strequal(clist->domain, co->domain) &&
           (clist->tailmatch == co->tailmatch))
          /* The domains are identical */
          replace_old = TRUE;
      }
      अन्यथा यदि(!clist->domain && !co->domain)
        replace_old = TRUE;

      यदि(replace_old) {
        /* the domains were identical */

        यदि(clist->path && co->path &&
           म_भेद(clist->path, co->path))
          replace_old = FALSE;
        अन्यथा यदि(!clist->path != !co->path)
          replace_old = FALSE;
      }

      यदि(replace_old && !co->livecookie && clist->livecookie) {
        /*
         * Both cookies matched fine, except that the already present cookie
         * is "live", which means it was set from a header, while the new one
         * was read from a file and thus is not "live". "live" cookies are
         * preferred so the new cookie is freed.
         */
        फर्काऊ FALSE;
      }
      यदि(replace_old)
        replace_n = n;
    }
  }
  यदि(replace_n) {
    संरचना Cookie *repl = Curl_गाँठो_elem(replace_n);

    /* when replacing, creationtime is kept from old */
    co->creationtime = repl->creationtime;

    /* unlink the old */
    Curl_गाँठो_remove(replace_n);

    /* free the old cookie */
    freecookie(repl, TRUE);
  }
  *replacep = replace_old;
  फर्काऊ TRUE;
}

/*
 * Curl_cookie_add
 *
 * Add a single cookie line to the cookie keeping object. Be aware that
 * sometimes we get an IP-only hostname, and that might also be a numerical
 * IPv6 address.
 *
 */
CURLcode Curl_cookie_add(संरचना Curl_easy *data,
                         संरचना CookieInfo *ci,
                         अचर वर्ण *lineptr, /* first character of the line */
                         अचर वर्ण *domain,  /* default domain */
                         अचर वर्ण *path,    /* full path used when this
                                                 cookie is set, used to get
                                                 default path for the cookie
                                                 unless set */
                         अचर पूर्णाङ्क flags)
{
  संरचना Cookie comem;
  संरचना Cookie *co;
  माप_प्रकार myhash;
  CURLcode result;
  bool replaces = FALSE;
  bool okay;

  DEBUGASSERT(data);
  DEBUGASSERT(MAX_SET_COOKIE_AMOUNT <= 255); /* counter is an unsigned char */
  यदि(data->req.setcookies >= MAX_SET_COOKIE_AMOUNT)
    फर्काऊ CURLE_OK; /* silently ignore */

  co = &comem;
  स_रखो(co, 0, परिमाण(comem));

  यदि(flags & COOKIE_HTTPHEADER)
    result = parse_cookie_header(data, co, &okay,
                                 lineptr, domain, path, flags & COOKIE_SECURE);
  अन्यथा
    result = parse_netscape(co, ci, &okay, lineptr, flags & COOKIE_SECURE);

  यदि(result || !okay)
    गच्छ fail;

  यदि(co->prefix_secure && !co->secure)
    /* The __Secure- prefix only requires that the cookie be set secure */
    गच्छ fail;

  यदि(!(flags & COOKIE_NOPSL) && is_public_suffix(data, co, domain))
    गच्छ fail;

  यदि(co->prefix_host) {
    /*
     * The __Host- prefix requires the cookie to be secure, have a "/" path
     * and not have a domain set.
     */
    यदि(co->secure && co->path && !म_भेद(co->path, "/") && !co->tailmatch)
      ;
    अन्यथा
      गच्छ fail;
  }

  यदि(!ci->running &&    /* read from a file */
     ci->newsession &&  /* clean session cookies */
     !co->expires)      /* this is a session cookie */
    गच्छ fail;

  co->livecookie = ci->running;
  co->creationtime = ++ci->lastct;

  यदि(!(flags & COOKIE_NOEXPIRE))
    remove_expired(ci);

  /*
   * Now we have parsed the incoming line, we must now check if this supersedes
   * an already existing cookie, which it may if the previous have the same
   * domain and path as this.
   */
  यदि(!replace_existing(data, co, ci, flags & COOKIE_SECURE, &replaces))
    गच्छ fail;

  /* clone the stack struct into heap */
  co = curlx_memdup(&comem, परिमाण(comem));
  यदि(!co) {
    co = &comem;
    result = CURLE_OUT_OF_MEMORY;
    गच्छ fail; /* bail out if we are this low on memory */
  }

  /* add this cookie to the list */
  myhash = cookiehash(co->domain);
  Curl_llist_append(&ci->cookielist[myhash], co, &co->गाँठो);

  यदि(ci->running)
    /* Only show this when NOT reading the cookies from a file */
    infof(data, "%s cookie %s=\"%s\" for domain %s, path %s, "
          "expire %" FMT_OFF_T,
          replaces ? "Replaced" : "Added", co->name, co->मान,
          co->domain, co->path, co->expires);

  यदि(!replaces)
    ci->numcookies++; /* one more cookie in the jar */

  /*
   * Now that we have added a new cookie to the jar, update the expiration
   * tracker in case it is the next one to expire.
   */
  यदि(co->expires && (co->expires < ci->next_expiration))
    ci->next_expiration = co->expires;

  यदि(flags & COOKIE_HTTPHEADER)
    data->req.setcookies++;

  फर्काऊ result;
fail:
  freecookie(co, FALSE);
  फर्काऊ result;
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
संरचना CookieInfo *Curl_cookie_init(शून्य)
{
  पूर्णाङ्क i;
  संरचना CookieInfo *ci = curlx_calloc(1, परिमाण(संरचना CookieInfo));
  यदि(!ci)
    फर्काऊ रिक्त;

  /* This does not use the destructor callback since we want to add
     and remove to lists while keeping the cookie struct intact */
  लागि(i = 0; i < COOKIE_HASH_SIZE; i++)
    Curl_llist_init(&ci->cookielist[i], रिक्त);
  /*
   * Initialize the next_expiration time to signal that we do not have enough
   * information yet.
   */
  ci->next_expiration = CURL_OFF_T_MAX;

  फर्काऊ ci;
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
स्थिर CURLcode cookie_load(संरचना Curl_easy *data, अचर वर्ण *file,
                            संरचना CookieInfo *ci, पूर्णाङ्क flags)
{
  खाता *handle = रिक्त;
  CURLcode result = CURLE_OK;
  खाता *fp = रिक्त;
  DEBUGASSERT(ci);
  DEBUGASSERT(data);
  DEBUGASSERT(file);

  ci->newsession = !!(flags & COOKIE_NOSESSION); /* new session? */
  ci->running = FALSE; /* this is not running, this is init */

  यदि(file && *file) {
    यदि(!म_भेद(file, "-"))
      fp = मानक_निवेश;
    अन्यथा {
      fp = curlx_fopen(file, "rb");
      यदि(!fp)
        infof(data, "WARNING: failed to open cookie file \"%s\"", file);
      अन्यथा {
        curlx_struct_stat stat;
        यदि((curlx_fstat(fileno(fp), &stat) != -1) && S_ISDIR(stat.st_mode)) {
          curlx_fclose(fp);
          fp = रिक्त;
          infof(data, "WARNING: cookie filename points to a directory: \"%s\"",
                file);
        }
        अन्यथा
          handle = fp;
      }
    }
  }

  यदि(fp) {
    संरचना dynbuf buf;
    bool eof = FALSE;
    /* get_netscape_format() uses curl_maprintf(), which returns at most
       DYN_APRINTF - 1 bytes. cookie_output() adds a newline (CRLF on Windows),
       and this dynbuf needs one more byte for its null-terminator. This is a
       cap, not an allocation. */
    curlx_dyn_init(&buf, DYN_APRINTF + 2);
    गर {
      result = Curl_get_पङ्क्ति(&buf, fp, &eof);
      यदि(!result) {
        अचर वर्ण *lineptr = curlx_dyn_ptr(&buf);
        bool headerline = FALSE;
        यदि(checkprefix("Set-Cookie:", lineptr)) {
          /* This is a cookie line, get it! */
          lineptr += 11;
          headerline = TRUE;
          curlx_str_passblanks(&lineptr);
        }

        result = Curl_cookie_add(data, ci, lineptr, रिक्त, रिक्त,
                                 (headerline ? COOKIE_HTTPHEADER : 0) |
                                 COOKIE_NOEXPIRE | COOKIE_SECURE |
                                 (flags & COOKIE_NOPSL));
        /* Ignore individual cookie problems unless we ran out of memory */
        यदि(result != CURLE_OUT_OF_MEMORY)
          result = CURLE_OK;
      }
    } जबसम्म(!result && !eof);
    curlx_dyn_free(&buf); /* free the line buffer */

    /*
     * Remove expired cookies from the hash. We must make sure to run this
     * after reading the file, and not on every cookie.
     */
    remove_expired(ci);

    यदि(handle)
      curlx_fclose(handle);
  }
  data->state.cookie_engine = TRUE;
  ci->running = TRUE; /* now, we are running */

  फर्काऊ result;
}

/*
 * Load cookies from all given cookie files (CURLOPT_COOKIEFILE).
 */
CURLcode Curl_cookie_loadfiles(संरचना Curl_easy *data,
                               पूर्णाङ्क flags)
{
  CURLcode result = CURLE_OK;
  संरचना curl_slist *list = data->state.cookielist;
  यदि(list) {
    Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
    यदि(!data->cookies)
      data->cookies = Curl_cookie_init();
    यदि(!data->cookies)
      result = CURLE_OUT_OF_MEMORY;
    अन्यथा {
      data->state.cookie_engine = TRUE;
      जबसम्म(list) {
        result = cookie_load(data, list->data, data->cookies, flags);
        यदि(result)
          रोक;
        list = list->next;
      }
    }
    Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  }
  फर्काऊ result;
}

/*
 * cookie_sort
 *
 * Helper function to sort cookies such that the longest path gets before the
 * shorter path. Path, domain and name lengths are considered in that order,
 * with the creationtime as the tiebreaker. The creationtime is guaranteed to
 * be unique per cookie, so we know we will get an ordering at that point.
 */
स्थिर पूर्णाङ्क cookie_sort(अचर शून्य *p1, अचर शून्य *p2)
{
  अचर संरचना Cookie *c1 = *(अचर संरचना Cookie * अचर *)p1;
  अचर संरचना Cookie *c2 = *(अचर संरचना Cookie * अचर *)p2;
  माप_प्रकार l1, l2;

  /* 1 - compare cookie path lengths */
  l1 = c1->path ? म_माप(c1->path) : 0;
  l2 = c2->path ? म_माप(c2->path) : 0;

  यदि(l1 != l2)
    फर्काऊ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 2 - compare cookie domain lengths */
  l1 = c1->domain ? म_माप(c1->domain) : 0;
  l2 = c2->domain ? म_माप(c2->domain) : 0;

  यदि(l1 != l2)
    फर्काऊ (l2 > l1) ? 1 : -1; /* avoid size_t <=> int conversions */

  /* 3 - compare cookie name lengths */
  l1 = c1->name ? म_माप(c1->name) : 0;
  l2 = c2->name ? म_माप(c2->name) : 0;

  यदि(l1 != l2)
    फर्काऊ (l2 > l1) ? 1 : -1;

  /* 4 - compare cookie creation time */
  फर्काऊ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

/*
 * cookie_sort_ct
 *
 * Helper function to sort cookies according to creation time.
 */
स्थिर पूर्णाङ्क cookie_sort_ct(अचर शून्य *p1, अचर शून्य *p2)
{
  अचर संरचना Cookie *c1 = *(अचर संरचना Cookie * अचर *)p1;
  अचर संरचना Cookie *c2 = *(अचर संरचना Cookie * अचर *)p2;

  फर्काऊ (c2->creationtime > c1->creationtime) ? 1 : -1;
}

bool Curl_secure_context(संरचना Curl_easy *data, अचर वर्ण *host)
{
  फर्काऊ Curl_xfer_is_secure(data) ||
    curl_strequal("localhost", host) ||
    !म_भेद(host, "127.0.0.1") ||
    !म_भेद(host, "::1");
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
CURLcode Curl_cookie_getlist(संरचना Curl_easy *data,
                             bool *okay,
                             अचर वर्ण *host,
                             संरचना Curl_llist *list)
{
  माप_प्रकार matches = 0;
  अचर bool is_ip = Curl_host_is_ipnum(host);
  अचर माप_प्रकार myhash = cookiehash(host);
  संरचना Curl_llist_गाँठो *n;
  अचर bool secure = Curl_secure_context(data, host);
  संरचना CookieInfo *ci = data->cookies;
  अचर वर्ण *path = data->state.up.path;
  CURLcode result = CURLE_OK;
  *okay = FALSE;

  Curl_llist_init(list, रिक्त);

  यदि(!ci || !Curl_llist_गन्ती(&ci->cookielist[myhash]))
    फर्काऊ CURLE_OK; /* no cookie struct or no cookies in the struct */

  /* at first, remove expired cookies */
  remove_expired(ci);

  लागि(n = Curl_llist_head(&ci->cookielist[myhash]); n; n = Curl_गाँठो_next(n)) {
    संरचना Cookie *co = Curl_गाँठो_elem(n);

    /* if the cookie requires we are secure we must only continue if we are! */
    यदि(co->secure ? secure : TRUE) {

      /* now check if the domain is correct */
      यदि(!co->domain ||
         (co->tailmatch && !is_ip &&
          cookie_tailmatch(co->domain, म_माप(co->domain), host)) ||
         ((!co->tailmatch || is_ip) && curl_strequal(host, co->domain))) {
        /*
         * the right part of the host matches the domain stuff in the
         * cookie data
         */

        /*
         * now check the left part of the path with the cookies path
         * requirement
         */
        यदि(!co->path || pathmatch(co->path, path)) {

          /*
           * This is a match and we add it to the return-linked-list
           */
          Curl_llist_append(list, co, &co->getnode);
          matches++;
          यदि(matches >= MAX_COOKIE_SEND_AMOUNT) {
            infof(data, "Included max number of cookies (%zu) in request!",
                  matches);
            रोक;
          }
        }
      }
    }
  }

  यदि(matches) {
    /*
     * Now we need to make sure that if there is a name appearing more than
     * once, the longest specified path version comes first. To make this the
     * swiftest way, we sort them all based on path length.
     */
    संरचना Cookie **array;
    माप_प्रकार i;

    /* alloc an array and store all cookie pointers */
    array = curlx_malloc(परिमाण(संरचना Cookie *) * matches);
    यदि(!array) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ fail;
    }

    n = Curl_llist_head(list);

    लागि(i = 0; n; n = Curl_गाँठो_next(n))
      array[i++] = Curl_गाँठो_elem(n);

    /* now sort the cookie pointers in path length order */
    क्विक(array, matches, परिमाण(संरचना Cookie *), cookie_sort);

    /* remake the linked list order according to the new order */
    Curl_llist_destroy(list, रिक्त);

    लागि(i = 0; i < matches; i++)
      Curl_llist_append(list, array[i], &array[i]->getnode);

    curlx_free(array); /* remove the temporary data again */
  }

  *okay = TRUE;
  फर्काऊ CURLE_OK; /* success */

fail:
  /* failure, clear up the allocated chain and return NULL */
  Curl_llist_destroy(list, रिक्त);
  फर्काऊ result; /* error */
}

/*
 * Curl_cookie_clearall
 *
 * Clear all existing cookies and reset the counter.
 */
शून्य Curl_cookie_clearall(संरचना CookieInfo *ci)
{
  यदि(ci) {
    अचिन्हित पूर्णाङ्क i;
    लागि(i = 0; i < COOKIE_HASH_SIZE; i++) {
      संरचना Curl_llist_गाँठो *n;
      लागि(n = Curl_llist_head(&ci->cookielist[i]); n;) {
        संरचना Cookie *c = Curl_गाँठो_elem(n);
        संरचना Curl_llist_गाँठो *e = Curl_गाँठो_next(n);
        Curl_गाँठो_remove(n);
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
शून्य Curl_cookie_clearsess(संरचना CookieInfo *ci)
{
  अचिन्हित पूर्णाङ्क i;

  यदि(!ci)
    फर्काऊ;

  लागि(i = 0; i < COOKIE_HASH_SIZE; i++) {
    संरचना Curl_llist_गाँठो *n = Curl_llist_head(&ci->cookielist[i]);
    संरचना Curl_llist_गाँठो *e = रिक्त;

    लागि(; n; n = e) {
      संरचना Cookie *curr = Curl_गाँठो_elem(n);
      e = Curl_गाँठो_next(n); /* in case the node is removed, get it early */
      यदि(!curr->expires) {
        Curl_गाँठो_remove(n);
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
शून्य Curl_cookie_cleanup(संरचना CookieInfo *ci)
{
  यदि(ci) {
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
स्थिर वर्ण *get_netscape_format(अचर संरचना Cookie *co)
{
  फर्काऊ curl_maprintf(
    "%s"               /* httponly preamble */
    "%s%s\t"           /* domain */
    "%s\t"             /* tailmatch */
    "%s\t"             /* path */
    "%s\t"             /* secure */
    "%" FMT_OFF_T "\t" /* expires */
    "%s\t"             /* name */
    "%s",              /*मान*/
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
    co->मान ? co->मान : "");
}

/*
 * cookie_output()
 *
 * Writes all internally known cookies to the specified file. Specify
 * "-" as filename to write to stdout.
 *
 * The function returns non-zero on write failure.
 */
स्थिर CURLcode cookie_output(संरचना Curl_easy *data,
                              संरचना CookieInfo *ci,
                              अचर वर्ण *filename)
{
  खाता *out = रिक्त;
  bool use_stdout = FALSE;
  वर्ण *tempstore = रिक्त;
  CURLcode result = CURLE_OK;

  यदि(!ci)
    /* no cookie engine alive */
    फर्काऊ CURLE_OK;

  /* at first, remove expired cookies */
  remove_expired(ci);

  यदि(!म_भेद("-", filename)) {
    /* use stdout */
    out = मानक_निकास;
    use_stdout = TRUE;
  }
  अन्यथा {
    result = Curl_fopen(data, filename, &out, &tempstore);
    यदि(result)
      गच्छ error;
  }

  ख_माला_दो("# Netscape HTTP Cookie File\n"
        "# https://curl.se/docs/http-cookies.html\n"
        "# This file was generated by libcurl! Edit at your own risk.\n\n",
        out);

  यदि(ci->numcookies) {
    अचिन्हित पूर्णाङ्क i;
    माप_प्रकार nvalid = 0;
    संरचना Cookie **array;
    संरचना Curl_llist_गाँठो *n;

    array = curlx_calloc(1, परिमाण(संरचना Cookie *) * ci->numcookies);
    यदि(!array) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ error;
    }

    /* only sort the cookies with a domain property */
    लागि(i = 0; i < COOKIE_HASH_SIZE; i++) {
      लागि(n = Curl_llist_head(&ci->cookielist[i]); n; n = Curl_गाँठो_next(n)) {
        संरचना Cookie *co = Curl_गाँठो_elem(n);
        यदि(!co->domain)
          जारी;
        array[nvalid++] = co;
      }
    }

    क्विक(array, nvalid, परिमाण(संरचना Cookie *), cookie_sort_ct);

    लागि(i = 0; i < nvalid; i++) {
      वर्ण *format_ptr = get_netscape_format(array[i]);
      यदि(!format_ptr) {
        curlx_free(array);
        result = CURLE_OUT_OF_MEMORY;
        गच्छ error;
      }
      curl_mfprintf(out, "%s\n", format_ptr);
      curlx_free(format_ptr);
    }

    curlx_free(array);
  }

  यदि(!use_stdout) {
    curlx_fclose(out);
    out = रिक्त;
    यदि(tempstore && curlx_rename(tempstore, filename)) {
      result = CURLE_WRITE_ERROR;
      गच्छ error;
    }
  }

  /*
   * If we reach here we have successfully written a cookie file so there is
   * no need to inspect the error, any error case should have jumped into the
   * error block below.
   */
  curlx_free(tempstore);
  फर्काऊ CURLE_OK;

error:
  यदि(out && !use_stdout)
    curlx_fclose(out);
  यदि(tempstore) {
    unlink(tempstore);
    curlx_free(tempstore);
  }
  फर्काऊ result;
}

स्थिर संरचना curl_slist *cookie_list(अचर संरचना Curl_easy *data)
{
  संरचना curl_slist *list = रिक्त;
  संरचना curl_slist *beg;
  संरचना curl_slist **tail = &list;
  अचिन्हित पूर्णाङ्क i;
  संरचना Curl_llist_गाँठो *n;

  यदि(!data->cookies || (data->cookies->numcookies == 0))
    फर्काऊ रिक्त;

  /* at first, remove expired cookies */
  remove_expired(data->cookies);

  लागि(i = 0; i < COOKIE_HASH_SIZE; i++) {
    लागि(n = Curl_llist_head(&data->cookies->cookielist[i]); n;
        n = Curl_गाँठो_next(n)) {
      संरचना Cookie *c = Curl_गाँठो_elem(n);
      वर्ण *पङ्क्ति;
      यदि(!c->domain)
        जारी;
      पङ्क्ति = get_netscape_format(c);
      यदि(!पङ्क्ति) {
        curl_slist_free_all(list);
        फर्काऊ रिक्त;
      }
      beg = Curl_slist_append_nodup(रिक्त, पङ्क्ति);
      यदि(!beg) {
        curlx_free(पङ्क्ति);
        curl_slist_free_all(list);
        फर्काऊ रिक्त;
      }
      *tail = beg;
      tail = &beg->next;
    }
  }

  फर्काऊ list;
}

संरचना curl_slist *Curl_cookie_list(संरचना Curl_easy *data)
{
  संरचना curl_slist *list;
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  list = cookie_list(data);
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
  फर्काऊ list;
}

शून्य Curl_flush_cookies(संरचना Curl_easy *data, bool cleanup)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  /* only save the cookie file if a transfer was started (cookies->running is
     set), as otherwise the cookies were not completely initialized and there
     might be cookie files that were not loaded so saving the file is the
     wrong thing. */
  यदि(data->cookies) {
    अचर वर्ण *cookiejar = CURL_EASY_STR(data, STRING_COOKIEJAR);
    यदि(cookiejar && data->cookies->running) {
      /* if we have a destination file for all the cookies to get dumped to */
      CURLcode result = cookie_output(data, data->cookies, cookiejar);
      यदि(result)
        infof(data, "WARNING: failed to save cookies in %s: %s",
              cookiejar, curl_easy_strerror(result));
    }

    यदि(cleanup && (!data->share || (data->cookies != data->share->cookies))) {
      Curl_cookie_cleanup(data->cookies);
      data->cookies = रिक्त;
    }
  }
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

शून्य Curl_cookie_run(संरचना Curl_easy *data)
{
  Curl_share_lock(data, CURL_LOCK_DATA_COOKIE, CURL_LOCK_ACCESS_SINGLE);
  यदि(data->cookies)
    data->cookies->running = TRUE;
  Curl_share_unlock(data, CURL_LOCK_DATA_COOKIE);
}

#endif /* CURL_DISABLE_HTTP || CURL_DISABLE_COOKIES */
