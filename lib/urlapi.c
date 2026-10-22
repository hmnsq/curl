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

#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urlapi-int.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_pton.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/inet_ntop.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strdup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "idn.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_memrchr.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARTS_WITH_DRIVE_ਅਗੇਤਰ(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#ਜੇ_ਅੰਤ

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ STARTS_WITH_URL_DRIVE_ਅਗੇਤਰ(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_SCHEME_LEN 40
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ਜੇ !defined(USE_IPV6) && !defined(AF_INET6)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ AF_INET6 (AF_INET + 1)
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_SCHEME "https"

ਸਥਿਰ ਖਾਲੀ free_urlhandle(ਬਣਤਰ Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->ਰਸਤਾ);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *ਲੱਭੋ_host_sep(ਅਚਲ ਅੱਖਰ *url)
{
  /* Find the start of the hostname */
  ਅਚਲ ਅੱਖਰ *sep = ਲੜੀ_ਖੋਜ(url, "//");
  ਜੇ(!sep)
    sep = url;
  ਨਹੀਂਤਾਂ
    sep += 2;

  /* Find first / or ? */
  ਜਦੋਂ(*sep && *sep != '/' && *sep != '?')
    sep++;

  ਵਾਪਸ sep;
}

/* convert CURLcode to CURLUcode */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)

/* urlencode_str() writes data into an output dynbuf and URL-encodes the
 * spaces in the source URL accordingly.
 *
 * This function re-encodes the string, meaning that it leaves already encoded
 * bytes as-is and works by encoding only what *has* to be encoded - unless it
 * has to uppercase the hex to normalize.
 *
 * Illegal percent-encoding sequences are left as-is.
 *
 * URL encoding should be skipped for hostnames, otherwise IDN resolution
 * will fail.
 *
 * 'query' tells if it is a query part or not, or if it is allowed to
 * "transition" into a query part with a question mark.
 *
 * @unittest 1675
 */
UNITTEST CURLUcode urlencode_str(ਬਣਤਰ dynbuf *o, ਅਚਲ ਅੱਖਰ *url,
                                 size_t len, bool relative,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ query);
UNITTEST CURLUcode urlencode_str(ਬਣਤਰ dynbuf *o, ਅਚਲ ਅੱਖਰ *url,
                                 size_t len, bool relative,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ query)
{
  /* we must add this with whitespace-replacing */
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *iptr;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *host_sep = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)url;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  ਜੇ(!relative) {
    size_t n;
    host_sep = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਲੱਭੋ_host_sep(url);

    /* output the first piece as-is */
    n = (ਅਚਲ ਅੱਖਰ *)host_sep - url;
    ਨਤੀਜਾ = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  ਲਈ(iptr = host_sep; len && !ਨਤੀਜਾ;) {
    ਜੇ(*iptr == ' ') {
      ਜੇ(query != QUERY_YES)
        ਨਤੀਜਾ = curlx_dyn_addn(o, "%20", 3);
      ਨਹੀਂਤਾਂ
        ਨਤੀਜਾ = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    ਨਹੀਂਤਾਂ ਜੇ((*iptr < ' ') || (*iptr >= 0x7f)) {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਬਾਹਰਲਾ[3] = { '%' };
      Curl_hexbyte(&ਬਾਹਰਲਾ[1], *iptr);
      ਨਤੀਜਾ = curlx_dyn_addn(o, ਬਾਹਰਲਾ, 3);
      iptr++;
      len--;
    }
    ਨਹੀਂਤਾਂ ਜੇ(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ hex = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਬਾਹਰਲਾ[3] = { '%' };
      Curl_hexbyte(&ਬਾਹਰਲਾ[1], hex);
      ਨਤੀਜਾ = curlx_dyn_addn(o, ਬਾਹਰਲਾ, 3);
      iptr += 3;
      len -= 3;
    }
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸ਼ੁਰੂ = iptr;
      ਜਦੋਂ(len) {
        ਜੇ(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          ਰੋਕੋ;
        ਜੇ(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          ਰੋਕੋ;
        ਜੇ(*iptr == '?') {
          ਜੇ(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            ਰੋਕੋ;
          }
        }
        iptr++;
        len--;
      }
      ਨਤੀਜਾ = curlx_dyn_addn(o, (ਅਚਲ ਅੱਖਰ *)ਸ਼ੁਰੂ, (size_t)(iptr - ਸ਼ੁਰੂ));
    }
  }

  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ cc2cu(ਨਤੀਜਾ);
  ਵਾਪਸ CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ਅਚਲ ਅੱਖਰ *url, ਅੱਖਰ *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (ਖਾਲੀ)buflen; /* only used in debug-builds */
  ਜੇ(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _WIN32
  ਜੇ(guess_scheme && STARTS_WITH_DRIVE_ਅਗੇਤਰ(url))
    ਵਾਪਸ 0;
#ਜੇ_ਅੰਤ
  ਜੇ(ISALPHA(url[0])) {
    ਜੇ(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    ਲਈ(i = 1; i < MAX_SCHEME_LEN; ++i) {
      ਅੱਖਰ s = url[i];
      ਜੇ(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        ਜੇ(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      ਨਹੀਂਤਾਂ {
        ਰੋਕੋ;
      }
    }
  }
  ਜੇ(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    ਜੇ(buf)
      buf[i] = 0;
    ਵਾਪਸ len;
  }
  ਜੇ(buf)
    buf[0] = 0;
  ਵਾਪਸ 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ਸਥਿਰ bool badoctets(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, size_t n, ਪੂਰਨ flags)
{
  ਅਚਲ uint8_t *p = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਨਿਵੇਸ਼;
  ਅਚਲ uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  ਜਦੋਂ(n--) {
    ਜੇ(*p <= control || *p == 127)
      ਵਾਪਸ TRUE;
    p++;
  }
  ਵਾਪਸ FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ਬਣਤਰ Curl_URL *u,
                                        ਅਚਲ ਅੱਖਰ *login,
                                        size_t len,
                                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                        size_t *hostname_ਵਿੱਥ);
UNITTEST CURLUcode parse_hostname_login(ਬਣਤਰ Curl_URL *u,
                                        ਅਚਲ ਅੱਖਰ *login,
                                        size_t len,
                                        ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                        size_t *hostname_ਵਿੱਥ)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode ਨਤੀਜਾ;
  ਅੱਖਰ *userp = ਨਿਰਾ;
  ਅੱਖਰ *passwdp = ਨਿਰਾ;
  ਅੱਖਰ *optionsp = ਨਿਰਾ;
  ਅਚਲ ਬਣਤਰ Curl_scheme *h = ਨਿਰਾ;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ਅਚਲ ਅੱਖਰ *ptr;

  DEBUGASSERT(login);

  *hostname_ਵਿੱਥ = 0;
  ptr = memchr(login, '@', len);
  ਜੇ(!ptr)
    ਜਾਓ ਬਾਹਰਲਾ;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  ਜੇ(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  ਨਤੀਜਾ = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ਨਿਰਾ);
  ਜੇ(ਨਤੀਜਾ) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    ਜਾਓ ਬਾਹਰਲਾ;
  }

  ਜੇ(userp) {
    ਜੇ(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      ਜਾਓ ਬਾਹਰਲਾ;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  ਜੇ(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  ਜੇ(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  ਜੇ(userp && badoctets(userp, ਲੜੀ_ਲੰਬਾਈ(userp), flags))
    ures = CURLUE_BAD_USER;
  ਨਹੀਂਤਾਂ ਜੇ(passwdp && badoctets(passwdp, ਲੜੀ_ਲੰਬਾਈ(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  ਨਹੀਂਤਾਂ ਜੇ(optionsp && badoctets(optionsp, ਲੜੀ_ਲੰਬਾਈ(optionsp), flags))
    ures = CURLUE_MALFORMED_ਨਿਵੇਸ਼;

  userp = passwdp = optionsp = ਨਿਰਾ;

  ਜੇ(!ures) {
    /* the hostname starts at this offset */
    *hostname_ਵਿੱਥ = ptr - login;
    ਵਾਪਸ CURLUE_OK;
  }

ਬਾਹਰਲਾ:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  ਵਾਪਸ ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_ਪੋਰਟ(ਬਣਤਰ Curl_URL *u, ਬਣਤਰ dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_ਪੋਰਟ(ਬਣਤਰ Curl_URL *u, ਬਣਤਰ dynbuf *host,
                              bool has_scheme)
{
  ਅਚਲ ਅੱਖਰ *portptr;
  ਅਚਲ ਅੱਖਰ *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->ਪੋਰਟ_present = FALSE;
  ਜੇ(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    ਜੇ(!portptr)
      ਵਾਪਸ CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    ਜੇ(*portptr) {
      ਜੇ(*portptr != ':')
        ਵਾਪਸ CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ;
    }
    ਨਹੀਂਤਾਂ
      portptr = ਨਿਰਾ;
  }
  ਨਹੀਂਤਾਂ
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  ਜੇ(portptr) {
    curl_off_t ਪੋਰਟ;
    size_t keep = portptr - hostname;
    ਪੂਰਨ rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    ਜੇ(!*portptr)
      ਵਾਪਸ has_scheme ? CURLUE_OK : CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ;
    ਜੇ(*portptr == '\\')
      ਵਾਪਸ CURLUE_BACKSLASH;
    rc = curlx_str_ਸੰਖਿਆ(&portptr, &ਪੋਰਟ, 0xffff);
    ਜੇ(rc)
      ਵਾਪਸ CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ;
    ਨਹੀਂਤਾਂ ਜੇ(*portptr == '\\')
      ਵਾਪਸ CURLUE_BACKSLASH;
    ਨਹੀਂਤਾਂ ਜੇ(*portptr)
      ਵਾਪਸ CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ;

    u->portnum = (uint16_t)ਪੋਰਟ;
    u->ਪੋਰਟ_present = TRUE;
  }

  ਵਾਪਸ CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ਸਥਿਰ ਅਚਲ bool invalid_host_char[256] = {
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x00-0x0F */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, /* 0x10-0x1F */
  1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, /* 0x20-0x2F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, /* 0x30-0x3F */
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x40-0x4F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, /* 0x50-0x5F */
  1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, /* 0x60-0x6F */
  0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1  /* 0x70-0x7F */
};

/* This function assumes 'hostname' now starts with [. It trims 'hostname' in
 * place and it sets u->zoneid if present.
 *
 * @unittest 1675
 */
UNITTEST CURLUcode ipv6_parse(ਬਣਤਰ Curl_URL *u, ਅੱਖਰ *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ਬਣਤਰ Curl_URL *u, ਅੱਖਰ *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  ਜੇ(hlen < 4) /* '[::]' is the shortest possible valid string */
    ਵਾਪਸ CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  ਜੇ(hlen != len) {
    hlen = len;
    ਜੇ(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      ਅੱਖਰ zoneid[MAX_ZONEID_LEN];
      ਪੂਰਨ i = 0;
      ਅੱਖਰ *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      ਜੇ(!ਲੜੀ_ਤੁਲਨਾ(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      ਜਦੋਂ(!invalid_host_char[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      ਜੇ(!i || (']' != *h))
        ਵਾਪਸ CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      ਜੇ(!u->zoneid)
        ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    ਅੱਖਰ dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    ਜੇ(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      ਵਾਪਸ CURLUE_BAD_IPV6;
    ਜੇ(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = ਲੜੀ_ਲੰਬਾਈ(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  ਵਾਪਸ CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ਸਥਿਰ CURLUcode hostname_check(ਅੱਖਰ *hostname, size_t hlen)
{
  size_t i;
  ਲਈ(i = 0; i < hlen; i++) {
    ਜੇ(invalid_host_char[(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)hostname[i]])
      ਵਾਪਸ CURLUE_BAD_HOSTNAME;
  }
  ਜੇ((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    ਵਾਪਸ CURLUE_BAD_HOSTNAME;
  ਨਹੀਂਤਾਂ ਜੇ((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    ਵਾਪਸ CURLUE_BAD_HOSTNAME;
  ਵਾਪਸ CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ਸਥਿਰ CURLUcode hostname_check6(ਬਣਤਰ Curl_URL *u, ਅੱਖਰ *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  ਜੇ(!hlen)
    ਵਾਪਸ CURLUE_NO_HOST;
  ਨਹੀਂਤਾਂ ਜੇ(hostname[0] == '[')
    ਵਾਪਸ ipv6_parse(u, hostname, hlen);

  ਵਾਪਸ hostname_check(hostname, hlen);
}

/*
 * Handle partial IPv4 numerical addresses and different bases, like
 * '16843009', '0x7f', '0x7f.1' '0177.1.1.1' etc.
 *
 * If the given input string is syntactically wrong IPv4 or any part for
 * example is too big, this function returns HOST_NAME.
 *
 * Output the "normalized" version of that input string in plain quad decimal
 * integers.
 *
 * A single dot following the numerical address is accepted and "swallowed" as
 * if it was never there.
 *
 * Returns the host type.
 *
 * @unittest 1675
 */
UNITTEST ਪੂਰਨ ipv4_normalize(ਬਣਤਰ dynbuf *host);
UNITTEST ਪੂਰਨ ipv4_normalize(ਬਣਤਰ dynbuf *host)
{
  bool ਪੂਰਾ = FALSE;
  ਪੂਰਨ n = 0;
  ਅਚਲ ਅੱਖਰ *c = curlx_dyn_ptr(host);
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ parts[4] = { 0, 0, 0, 0 };
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!ISDIGIT(*c))
    ਵਾਪਸ HOST_NAME;

  ਜਦੋਂ(!ਪੂਰਾ) {
    ਪੂਰਨ rc;
    curl_off_t l;
    ਜੇ(*c == '0') {
      ਜੇ((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        ਜੇ(rc)
          ਵਾਪਸ HOST_NAME;
      }
      ਨਹੀਂਤਾਂ
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    ਨਹੀਂਤਾਂ
      rc = curlx_str_ਸੰਖਿਆ(&c, &l, UINT_MAX);

    ਜੇ(rc) {
      ਜੇ(!n || (rc != STRE_NO_ਅੰਕ) || *c)
        ਵਾਪਸ HOST_NAME;
      n--;
    }
    ਨਹੀਂਤਾਂ
      parts[n] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)l;

    ਚੋਣ(*c) {
    ਹਾਲਤ '.':
      ਜੇ(n == 3) {
        ਜੇ(c[1])
          /* something follows this dot */
          ਵਾਪਸ HOST_NAME;
        ਪੂਰਾ = TRUE;
      }
      ਨਹੀਂਤਾਂ {
        n++;
        c++;
      }
      ਰੋਕੋ;

    ਹਾਲਤ '\0':
      ਪੂਰਾ = TRUE;
      ਰੋਕੋ;

    ਮੂਲ:
      ਵਾਪਸ HOST_NAME;
    }
  }

  ਚੋਣ(n) {
  ਹਾਲਤ 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    ਨਤੀਜਾ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    ਰੋਕੋ;
  ਹਾਲਤ 1: /* a.b -- 8.24 bits */
    ਜੇ((parts[0] > 0xff) || (parts[1] > 0xffffff))
      ਵਾਪਸ HOST_NAME;
    curlx_dyn_reset(host);
    ਨਤੀਜਾ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    ਰੋਕੋ;
  ਹਾਲਤ 2: /* a.b.c -- 8.8.16 bits */
    ਜੇ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      ਵਾਪਸ HOST_NAME;
    curlx_dyn_reset(host);
    ਨਤੀਜਾ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    ਰੋਕੋ;
  ਹਾਲਤ 3: /* a.b.c.d -- 8.8.8.8 bits */
    ਜੇ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      ਵਾਪਸ HOST_NAME;
    curlx_dyn_reset(host);
    ਨਤੀਜਾ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    ਰੋਕੋ;
  }
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ HOST_ERROR;
  ਵਾਪਸ HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ਸਥਿਰ CURLUcode urldecode_host(ਬਣਤਰ dynbuf *host)
{
  ਅਚਲ ਅੱਖਰ *ਪ੍ਰਤੀ;
  ਅਚਲ ਅੱਖਰ *hostname = curlx_dyn_ptr(host);
  ਪ੍ਰਤੀ = memchr(hostname, '%', curlx_dyn_len(host));
  ਜੇ(!ਪ੍ਰਤੀ)
    /* nothing to decode */
    ਵਾਪਸ CURLUE_OK;
  ਨਹੀਂਤਾਂ {
    /* encoded */
    size_t dlen;
    ਅੱਖਰ *decoded;
    CURLcode ਨਤੀਜਾ = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    ਨਤੀਜਾ = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ cc2cu(ਨਤੀਜਾ);
  }

  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode parse_authority(ਬਣਤਰ Curl_URL *u,
                                 ਅਚਲ ਅੱਖਰ *auth, size_t authlen,
                                 ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                                 ਬਣਤਰ dynbuf *host,
                                 bool has_scheme)
{
  size_t ਵਿੱਥ;
  CURLUcode uc;
  CURLcode ਨਤੀਜਾ;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &ਵਿੱਥ);
  ਜੇ(uc)
    ਵਾਪਸ uc;

  ਨਤੀਜਾ = curlx_dyn_addn(host, auth + ਵਿੱਥ, authlen - ਵਿੱਥ);
  ਜੇ(ਨਤੀਜਾ) {
    uc = cc2cu(ਨਤੀਜਾ);
    ਵਾਪਸ uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_ਪੋਰਟ(u, host, has_scheme);

  ਜੇ(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  ਜੇ(!uc)
    uc = urldecode_host(host);
  ਜੇ(uc)
    ;
  ਨਹੀਂਤਾਂ ਜੇ(auth[ਵਿੱਥ] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  ਨਹੀਂਤਾਂ {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    ਪੂਰਨ type = ipv4_normalize(host);

    ਜੇ(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    ਨਹੀਂਤਾਂ ਜੇ(type == HOST_ERROR)
      uc = CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }

  ਵਾਪਸ uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_ਸੈੱਟ_authority(CURLU *u, ਅਚਲ ਅੱਖਰ *authority)
{
  CURLUcode ures;
  ਬਣਤਰ dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

  ures = parse_authority(u, authority, ਲੜੀ_ਲੰਬਾਈ(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  ਜੇ(ures)
    curlx_dyn_free(&host);
  ਨਹੀਂਤਾਂ {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  ਵਾਪਸ ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ਸਥਿਰ bool is_dot(ਅਚਲ ਅੱਖਰ **str, size_t *clen)
{
  ਅਚਲ ਅੱਖਰ *p = *str;
  ਜੇ(*p == '.') {
    (*str)++;
    (*clen)--;
    ਵਾਪਸ TRUE;
  }
  ਨਹੀਂਤਾਂ ਜੇ((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ਸਥਿਰ bool needs_dedotdot(ਅਚਲ ਅੱਖਰ *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  ਜੇ(pn < 2)
    ਵਾਪਸ FALSE;
  ਜੇ(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    ਵਾਪਸ FALSE;
  ਜਦੋਂ(pn) {
    ਜੇ(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      ਜੇ(!pn || ISSLASH(*p))
        ਵਾਪਸ TRUE;
      /* "../" or ".." before end of string */
      ਨਹੀਂਤਾਂ ਜੇ(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        ਵਾਪਸ TRUE;
    }
    ਨਹੀਂਤਾਂ {
      p++;
      pn--;
    }
  }
  ਵਾਪਸ FALSE;
}

/*
 * dedotdotify()
 *
 * This function gets a null-terminated path with dot and dotdot sequences
 * passed in and strips them off according to the rules in RFC 3986 section
 * 5.2.4.
 *
 * The function handles a path. It should not contain the query nor fragment.
 *
 * RETURNS
 *
 * Zero for success and 'out' set to an allocated string (or NULL if there's
 * nothing to do).
 *
 * @unittest 1395
 */
UNITTEST ਪੂਰਨ dedotdotify(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, size_t clen, ਅੱਖਰ **outp);
UNITTEST ਪੂਰਨ dedotdotify(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼, size_t clen, ਅੱਖਰ **outp)
{
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* variables for leading dot checks */
  ਅਚਲ ਅੱਖਰ *dinput = ਨਿਵੇਸ਼;
  size_t dlen = clen;

  *outp = ਨਿਰਾ;
  ਜੇ(!needs_dedotdot(ਨਿਵੇਸ਼, clen))
    ਵਾਪਸ 0;

  curlx_dyn_init(&ਬਾਹਰਲਾ, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  ਜੇ(is_dot(&dinput, &dlen)) {
    ਜੇ(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      ਨਿਵੇਸ਼ = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    ਨਹੀਂਤਾਂ ਜੇ(is_dot(&dinput, &dlen)) {
      ਜੇ(!dlen)
        /* .. [end] */
        ਜਾਓ ਸਿਰਾ;
      ਨਹੀਂਤਾਂ ਜੇ(ISSLASH(*dinput)) {
        /* ../ */
        ਨਿਵੇਸ਼ = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  ਜਦੋਂ(clen && !ਨਤੀਜਾ) { /* until end of path content */
    ਜੇ(ISSLASH(*ਨਿਵੇਸ਼)) {
      ਅਚਲ ਅੱਖਰ *p = &ਨਿਵੇਸ਼[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      ਜੇ(is_dot(&p, &blen)) {
        ਜੇ(!blen) { /* /. */
          ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, "/", 1);
          ਰੋਕੋ;
        }
        ਨਹੀਂਤਾਂ ਜੇ(ISSLASH(*p)) { /* /./ */
          ਨਿਵੇਸ਼ = p;
          clen = blen;
          ਜਾਰੀ;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        ਨਹੀਂਤਾਂ ਜੇ(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&ਬਾਹਰਲਾ);
          ਜੇ(len) {
            ਅਚਲ ਅੱਖਰ *ptr = curlx_dyn_ptr(&ਬਾਹਰਲਾ);
            ਅਚਲ ਅੱਖਰ *ਆਖਰੀ = memrchr(ptr, '/', len);
            ਜੇ(ਆਖਰੀ)
              /* trim the output at the slash */
              curlx_dyn_setlen(&ਬਾਹਰਲਾ, ਆਖਰੀ - ptr);
          }

          ਜੇ(blen) { /* /../ */
            ਨਿਵੇਸ਼ = p;
            clen = blen;
            ਜਾਰੀ;
          }
          ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, "/", 1);
          ਰੋਕੋ;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, ਨਿਵੇਸ਼, 1);
    ਨਿਵੇਸ਼++;
    clen--;
  }
ਸਿਰਾ:
  ਜੇ(!ਨਤੀਜਾ) {
    ਜੇ(curlx_dyn_len(&ਬਾਹਰਲਾ))
      *outp = curlx_dyn_ptr(&ਬਾਹਰਲਾ);
    ਨਹੀਂਤਾਂ {
      *outp = curlx_strdup("");
      ਜੇ(!*outp)
        ਵਾਪਸ 1;
    }
  }
  ਵਾਪਸ ਨਤੀਜਾ ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ਅਚਲ ਅੱਖਰ *url, size_t urllen, CURLU *u,
                              ਅਚਲ ਅੱਖਰ **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ਅਚਲ ਅੱਖਰ *url, size_t urllen, CURLU *u,
                              ਅਚਲ ਅੱਖਰ **pathp, size_t *pathlenp)
{
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;
  size_t pathlen;

  *pathp = ਨਿਰਾ;
  *pathlenp = 0;
  ਜੇ(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    ਵਾਪਸ CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  ਰਸਤਾ = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  ਜੇ(ਰਸਤਾ[0] != '/')
    ਵਾਪਸ CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  ਜੇ(ਰਸਤਾ[1] == '/') {
    /* swallow the two slashes */
    ਅਚਲ ਅੱਖਰ *ptr = &ਰਸਤਾ[2];

    /*
     * According to RFC 8089, a file: URL can be reliably dereferenced if:
     *
     *  o it has no/blank hostname, or
     *
     *  o the hostname matches "localhost" (case-insensitively), or
     *
     *  o the hostname is a FQDN that resolves to this machine, or
     *
     * For brevity, we only consider URLs with empty, "localhost", or
     * "127.0.0.1" hostnames as local, otherwise as an UNC String.
     *
     * Additionally, there is an exception for URLs with a Windows drive
     * letter in the authority (which was accidentally omitted from RFC 8089
     * Appendix E, but believe me, it was meant to be there. --MK)
     */
    ਜੇ(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_ਅਗੇਤਰ(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      ਜੇ(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      ਨਹੀਂਤਾਂ
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        ਵਾਪਸ CURLUE_BAD_FILE_URL;
    }

    ਰਸਤਾ = ptr;
    pathlen = urllen - (ptr - url);
  }

#ਜੇ !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  ਜੇ(('/' == ਰਸਤਾ[0] && STARTS_WITH_URL_DRIVE_ਅਗੇਤਰ(&ਰਸਤਾ[1])) ||
     STARTS_WITH_URL_DRIVE_ਅਗੇਤਰ(ਰਸਤਾ)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    ਵਾਪਸ CURLUE_BAD_FILE_URL;
  }
#ਨਹੀਂਤਾਂ
  /* If the path starts with a slash and a drive letter, ditch the slash */
  ਜੇ('/' == ਰਸਤਾ[0] && STARTS_WITH_URL_DRIVE_ਅਗੇਤਰ(&ਰਸਤਾ[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    ਰਸਤਾ++;
    pathlen--;
  }
#ਜੇ_ਅੰਤ
  u->scheme = curlx_strdup("file");
  ਜੇ(!u->scheme)
    ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  *pathp = ਰਸਤਾ;
  *pathlenp = pathlen;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode parse_scheme(ਅਚਲ ਅੱਖਰ *url, CURLU *u, ਅੱਖਰ *schemebuf,
                              size_t schemelen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                              ਅਚਲ ਅੱਖਰ **hostpp)
{
  /* clear path */
  ਅਚਲ ਅੱਖਰ *schemep = ਨਿਰਾ;

  ਜੇ(schemelen) {
    ਪੂਰਨ ਅੰਕ_slashes = 0;
    ਅਚਲ ਅੱਖਰ *p = &url[schemelen + 1];
    ਜੇ(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      ਵਾਪਸ CURLUE_UNSUPPORTED_SCHEME;

    ਜੇ(!ISSLASH(*p))
      /* less than one */
      ਵਾਪਸ CURLUE_BAD_SLASHES;
    ਜੇ((flags & CURLU_NO_AUTHORITY)) {
      ਜਦੋਂ(ISSLASH(*p) && (ਅੰਕ_slashes < 2)) {
        p++;
        ਅੰਕ_slashes++;
      }
    }
    ਨਹੀਂਤਾਂ {
      ਜਦੋਂ(ISSLASH(*p) && (ਅੰਕ_slashes < 4)) {
        p++;
        ਅੰਕ_slashes++;
      }
      ਜੇ(ਅੰਕ_slashes > 3)
        ਵਾਪਸ CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  ਨਹੀਂਤਾਂ {
    /* no scheme! */

    ਜੇ(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      ਵਾਪਸ CURLUE_BAD_SCHEME;

    ਜੇ(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  ਜੇ(schemep) {
    u->scheme = curlx_strdup(schemep);
    ਜੇ(!u->scheme)
      ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode guess_scheme(CURLU *u, ਬਣਤਰ dynbuf *host)
{
  ਅਚਲ ਅੱਖਰ *hostname = curlx_dyn_ptr(host);
  ਅਚਲ ਅੱਖਰ *schemep = ਨਿਰਾ;
  /* legacy curl-style guess based on hostname */
  ਜੇ(checkprefix("ftp.", hostname))
    schemep = "ftp";
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("dict.", hostname))
    schemep = "dict";
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("ldap.", hostname))
    schemep = "ldap";
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("imap.", hostname))
    schemep = "imap";
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("smtp.", hostname))
    schemep = "smtp";
  ਨਹੀਂਤਾਂ ਜੇ(checkprefix("pop3.", hostname))
    schemep = "pop3";
  ਨਹੀਂਤਾਂ
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  ਜੇ(!u->scheme)
    ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  u->guessed_scheme = TRUE;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode handle_fragment(CURLU *u, ਅਚਲ ਅੱਖਰ *fragment,
                                 size_t fraglen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  ਜੇ(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    ਜੇ(flags & CURLU_URLENCODE) {
      ਬਣਤਰ dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      ਜੇ(ures)
        ਵਾਪਸ ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(badoctets(fragment, fraglen, flags))
        ਵਾਪਸ CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      ਜੇ(!u->fragment)
        ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode handle_query(CURLU *u, ਅਚਲ ਅੱਖਰ *query,
                              size_t qlen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  u->query_present = TRUE;
  ਜੇ(qlen > 1) {
    ਜੇ(flags & CURLU_URLENCODE) {
      ਬਣਤਰ dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      ਜੇ(ures)
        ਵਾਪਸ ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    ਨਹੀਂਤਾਂ {
      ਜੇ(badoctets(query, qlen, flags))
        ਵਾਪਸ CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      ਜੇ(!u->query)
        ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
  }
  ਨਹੀਂਤਾਂ {
    /* single byte query */
    u->query = curlx_strdup("");
    ਜੇ(!u->query)
      ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode handle_ਰਸਤਾ(CURLU *u, ਅਚਲ ਅੱਖਰ *ਰਸਤਾ,
                             size_t pathlen, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags,
                             bool is_file)
{
  CURLUcode ures;
  ਜੇ(pathlen && (flags & CURLU_URLENCODE)) {
    ਬਣਤਰ dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
    ures = urlencode_str(&enc, ਰਸਤਾ, pathlen, TRUE, QUERY_NO);
    ਜੇ(ures)
      ਵਾਪਸ ures;
    pathlen = curlx_dyn_len(&enc);
    ਰਸਤਾ = u->ਰਸਤਾ = curlx_dyn_ptr(&enc);
  }

  ਜੇ(pathlen >= (size_t)(1 + !is_file)) {
    ਜੇ(badoctets(ਰਸਤਾ, pathlen, flags))
      ਵਾਪਸ CURLUE_BAD_ਰਸਤਾ;

    /* paths for file:// scheme can be one byte, others need to be two */
    ਜੇ(!u->ਰਸਤਾ) {
      u->ਰਸਤਾ = curlx_memdup0(ਰਸਤਾ, pathlen);
      ਜੇ(!u->ਰਸਤਾ)
        ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਰਸਤਾ = u->ਰਸਤਾ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->ਰਸਤਾ[pathlen] = 0;

    ਜੇ(!(flags & CURLU_ਰਸਤਾ_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      ਅੱਖਰ *dedot;
      ਪੂਰਨ err = dedotdotify(ਰਸਤਾ, pathlen, &dedot);
      ਜੇ(err)
        ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜੇ(dedot) {
        curlx_free(u->ਰਸਤਾ);
        u->ਰਸਤਾ = dedot;
      }
    }
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode parseurl(ਅਚਲ ਅੱਖਰ *url, CURLU *u, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  ਅਚਲ ਅੱਖਰ *ਰਸਤਾ;
  size_t pathlen;
  ਅੱਖਰ schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ਬਣਤਰ dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = ਲੜੀ_ਲੰਬਾਈ(url);
  ਜੇ(urllen > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
    ਵਾਪਸ CURLUE_MALFORMED_ਨਿਵੇਸ਼;

  curlx_dyn_init(&host, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

  schemelen = Curl_is_absolute_url(url, schemebuf, ਆਕਾਰ(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  ਜੇ(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &ਰਸਤਾ, &pathlen);
  }
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਅੱਖਰ *hostp = ਨਿਰਾ;
    ਅਚਲ ਅੱਖਰ *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    ਜੇ(ures)
      ਜਾਓ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    ਜਦੋਂ(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    ਰਸਤਾ = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (ਰਸਤਾ - url);
    ਜੇ(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      ਜੇ(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    ਨਹੀਂਤਾਂ ਜੇ(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      ਜੇ(curlx_dyn_ਜੋੜੋ(&host, ""))
        ures = CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਹੀਂਤਾਂ
      ures = CURLUE_NO_HOST;
  }
  ਜੇ(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ਅਚਲ ਅੱਖਰ *fragment = memchr(ਰਸਤਾ, '#', pathlen);
    ਜੇ(fragment) {
      size_t fraglen = pathlen - (fragment - ਰਸਤਾ);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  ਜੇ(!ures) {
    ਅਚਲ ਅੱਖਰ *query = memchr(ਰਸਤਾ, '?', pathlen);
    ਜੇ(query) {
      size_t qlen = pathlen - (query - ਰਸਤਾ);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  ਜੇ(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_ਰਸਤਾ(u, ਰਸਤਾ, pathlen, flags, is_file);
  ਜੇ(!ures) {
    u->host = curlx_dyn_ptr(&host);
    ਵਾਪਸ CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  ਵਾਪਸ ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ਸਥਿਰ CURLUcode parseurl_and_replace(ਅਚਲ ਅੱਖਰ *url, CURLU *u,
                                      ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, ਆਕਾਰ(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  ਜੇ(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  ਵਾਪਸ ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ਸਥਿਰ CURLUcode redirect_url(ਅਚਲ ਅੱਖਰ *ਅਧਾਰ, ਅਚਲ ਅੱਖਰ *relurl,
                              CURLU *u, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  ਬਣਤਰ dynbuf urlbuf;
  bool host_ਬਦਲਿਆ = FALSE;
  ਅਚਲ ਅੱਖਰ *useurl = relurl;
  ਅਚਲ ਅੱਖਰ *cutoff = ਨਿਰਾ;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ਅਚਲ ਅੱਖਰ *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ਅਚਲ ਅੱਖਰ *protsep = ਅਧਾਰ + ਲੜੀ_ਲੰਬਾਈ(scheme) + 3;
  DEBUGASSERT(ਅਧਾਰ && relurl && u); /* all set here */
  ਜੇ(!ਅਧਾਰ)
    ਵਾਪਸ CURLUE_MALFORMED_ਨਿਵੇਸ਼; /* should never happen */

  /* handle different relative URL types */
  ਚੋਣ(relurl[0]) {
  ਹਾਲਤ '/':
    ਜੇ(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_ਬਦਲਿਆ = TRUE;
    }
    ਨਹੀਂਤਾਂ
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    ਰੋਕੋ;

  ਹਾਲਤ '#':
    /* fragment-only change */
    ਜੇ(u->fragment_present)
      cutoff = strchr(protsep, '#');
    ਰੋਕੋ;

  ਮੂਲ:
    /* path or query-only change */
    ਜੇ(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    ਨਹੀਂਤਾਂ ਜੇ(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    ਜੇ(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : ਲੜੀ_ਲੰਬਾਈ(protsep));
      ਜੇ(cutoff)
        cutoff++; /* truncate after last slash */
    }
    ਰੋਕੋ;
  }

  prelen = cutoff ? (size_t)(cutoff - ਅਧਾਰ) : ਲੜੀ_ਲੰਬਾਈ(ਅਧਾਰ);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

  ਜੇ(!curlx_dyn_addn(&urlbuf, ਅਧਾਰ, prelen) &&
     !urlencode_str(&urlbuf, useurl, ਲੜੀ_ਲੰਬਾਈ(useurl), !host_ਬਦਲਿਆ,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_ਰਸਤਾ_AS_IS);
  }
  ਨਹੀਂਤਾਂ
    uc = CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  curlx_dyn_free(&urlbuf);
  ਵਾਪਸ uc;
}

/*
 */
CURLU *curl_url(ਖਾਲੀ)
{
  ਵਾਪਸ curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_URL));
}

ਖਾਲੀ curl_url_cleanup(CURLU *u)
{
  ਜੇ(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DUP(dest, src, name)                    \
  ਕਰੋ {                                          \
    ਜੇ((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      ਜੇ(!(dest)->name)                         \
        ਜਾਓ fail;                              \
    }                                           \
  } ਜਦੋਂ(0)

CURLU *curl_url_dup(ਅਚਲ CURLU *ਵਿੱਚ)
{
  ਬਣਤਰ Curl_URL *u;

  ਜੇ(!ਵਿੱਚ)
    ਵਾਪਸ ਨਿਰਾ;

  u = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ Curl_URL));
  ਜੇ(u) {
    DUP(u, ਵਿੱਚ, scheme);
    DUP(u, ਵਿੱਚ, user);
    DUP(u, ਵਿੱਚ, password);
    DUP(u, ਵਿੱਚ, options);
    DUP(u, ਵਿੱਚ, host);
    DUP(u, ਵਿੱਚ, ਰਸਤਾ);
    DUP(u, ਵਿੱਚ, query);
    DUP(u, ਵਿੱਚ, fragment);
    DUP(u, ਵਿੱਚ, zoneid);
    u->portnum = ਵਿੱਚ->portnum;
    u->ਪੋਰਟ_present = ਵਿੱਚ->ਪੋਰਟ_present;
    u->fragment_present = ਵਿੱਚ->fragment_present;
    u->query_present = ਵਿੱਚ->query_present;
    u->guessed_scheme = ਵਿੱਚ->guessed_scheme;
  }
  ਵਾਪਸ u;
fail:
  curl_url_cleanup(u);
  ਵਾਪਸ ਨਿਰਾ;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_IDN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ host_decode(x, y) CURLUE_LACKS_IDN
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ host_encode(x, y) CURLUE_LACKS_IDN
#ਨਹੀਂਤਾਂ
ਸਥਿਰ CURLUcode host_decode(ਅਚਲ ਅੱਖਰ *host, ਅੱਖਰ **allochost)
{
  CURLcode ਨਤੀਜਾ = Curl_idn_decode(host, allochost);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ (ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) ?
      CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : CURLUE_BAD_HOSTNAME;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode host_encode(ਅਚਲ ਅੱਖਰ *host, ਅੱਖਰ **allochost)
{
  CURLcode ਨਤੀਜਾ = Curl_idn_encode(host, allochost);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ (ਨਤੀਜਾ == CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ) ?
      CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ : CURLUE_BAD_HOSTNAME;
  ਵਾਪਸ CURLUE_OK;
}
#ਜੇ_ਅੰਤ

ਸਥਿਰ CURLUcode urlget_format(ਅਚਲ CURLU *u, CURLUPart what,
                               ਅਚਲ ਅੱਖਰ *ptr, ਅੱਖਰ **partp,
                               bool plusdecode, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = ਲੜੀ_ਲੰਬਾਈ(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  ਅੱਖਰ *part = curlx_memdup0(ptr, partlen);
  *partp = ਨਿਰਾ;
  ਜੇ(!part)
    ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  ਜੇ(plusdecode) {
    /* convert + to space */
    ਅੱਖਰ *plus = part;
    size_t i = 0;
    ਲਈ(i = 0; i < partlen; ++plus, i++) {
      ਜੇ(*plus == '+')
        *plus = ' ';
    }
  }
  ਜੇ(urldecode) {
    ਅੱਖਰ *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode ਨਤੀਜਾ = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  ਜੇ(urlencode) {
    ਬਣਤਰ dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    ਜੇ(uc)
      ਵਾਪਸ uc;
    part = curlx_dyn_ptr(&enc);
  }
  ਨਹੀਂਤਾਂ ਜੇ(punycode) {
    ਜੇ(!Curl_is_ASCII_name(u->host)) {
      ਅੱਖਰ *punyversion = ਨਿਰਾ;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      ਜੇ(uc)
        ਵਾਪਸ uc;
      uc = hostname_check(punyversion, ਲੜੀ_ਲੰਬਾਈ(punyversion));
      ਜੇ(uc) {
        curlx_free(punyversion);
        ਵਾਪਸ uc;
      }
      part = punyversion;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(depunyfy && Curl_is_ASCII_name(u->host)) {
    ਅੱਖਰ *unpunified = ਨਿਰਾ;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    ਜੇ(uc)
      ਵਾਪਸ uc;
    part = unpunified;
  }
  *partp = part;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode file_url(ਅਚਲ CURLU *u, ਅੱਖਰ **part,
                          ਅਚਲ ਅੱਖਰ *fragmentsep,
                          ਅਚਲ ਅੱਖਰ *querysep)
{
  ਅੱਖਰ *url = curl_maprintf("file://%s%s%s%s%s",
                            u->ਰਸਤਾ, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  ਜੇ(!url)
    ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  *part = url;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode urlget_url(ਅਚਲ CURLU *u, ਅੱਖਰ **part, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  ਅੱਖਰ *url;
  ਅੱਖਰ *allochost = ਨਿਰਾ;
  ਅਚਲ ਅੱਖਰ *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ)) ?
    "#" : "";
  ਅਚਲ ਅੱਖਰ *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ)) ?
    "?" : "";
  ਅੱਖਰ portbuf[7];
  ਜੇ(curl_strequal("file", u->scheme))
    ਵਾਪਸ file_url(u, part, fragmentsep, querysep);
  ਨਹੀਂਤਾਂ ਜੇ(!u->host)
    ਵਾਪਸ CURLUE_NO_HOST;
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਅੱਖਰ *scheme;
    ਅੱਖਰ *options = u->options;
    ਅੱਖਰ *ਪੋਰਟ = ਨਿਰਾ;
    ਅਚਲ ਬਣਤਰ Curl_scheme *h = ਨਿਰਾ;
    ਅੱਖਰ schemebuf[MAX_SCHEME_LEN + 5];
    ਜੇ(u->scheme)
      scheme = u->scheme;
    ਨਹੀਂਤਾਂ ਜੇ(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLUE_NO_SCHEME;

    ਜੇ(u->ਪੋਰਟ_present) {
      curl_msnprintf(portbuf, ਆਕਾਰ(portbuf), "%u", u->portnum);
      ਪੋਰਟ = portbuf;
    }

    h = Curl_get_scheme(scheme);
    ਜੇ(h) {
      ਜੇ(!u->ਪੋਰਟ_present && (flags & CURLU_DEFAULT_ਪੋਰਟ)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, ਆਕਾਰ(portbuf), "%u", h->defport);
        ਪੋਰਟ = portbuf;
      }
      ਨਹੀਂਤਾਂ ਜੇ(u->ਪੋਰਟ_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_ਪੋਰਟ)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        ਪੋਰਟ = ਨਿਰਾ;
      }

      ਜੇ(!(h->flags & PROTOPT_URLOPTIONS))
        options = ਨਿਰਾ;
    }

    ਜੇ(u->host[0] == '[') {
      ਜੇ(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ਬਣਤਰ dynbuf enc;
        size_t hostlen = ਲੜੀ_ਲੰਬਾਈ(u->host);
        curlx_dyn_init(&enc, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
        ਜੇ(curlx_dyn_addf(&enc, "%.*s%%25%s]", (ਪੂਰਨ)hostlen - 1, u->host,
                          u->zoneid))
          ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ਨਿਰਾ, u->host, 0);
      ਜੇ(!allochost)
        ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(flags & CURLU_PUNYCODE) {
      ਜੇ(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        ਜੇ(!ret)
          ret = hostname_check(allochost, ਲੜੀ_ਲੰਬਾਈ(allochost));
        ਜੇ(ret) {
          curlx_safefree(allochost);
          ਵਾਪਸ ret;
        }
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(flags & CURLU_PUNY2IDN) {
      ਜੇ(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        ਜੇ(ret)
          ਵਾਪਸ ret;
      }
    }

    ਜੇ(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, ਆਕਾਰ(schemebuf), "%s://", scheme);
    ਨਹੀਂਤਾਂ
      schemebuf[0] = 0;

    url = curl_maprintf("%s%s%s%s%s%s%s%s%s%s%s%s%s%s%s",
                        schemebuf,
                        u->user ? u->user : "",
                        u->password ? ":" : "",
                        u->password ? u->password : "",
                        options ? ";" : "",
                        options ? options : "",
                        (u->user || u->password || options) ? "@" : "",
                        allochost ? allochost : u->host,
                        ਪੋਰਟ ? ":" : "",
                        ਪੋਰਟ ? ਪੋਰਟ : "",
                        u->ਰਸਤਾ ? u->ਰਸਤਾ : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  ਜੇ(!url)
    ਵਾਪਸ CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
  *part = url;
  ਵਾਪਸ CURLUE_OK;
}

CURLUcode curl_url_get(ਅਚਲ CURLU *u, CURLUPart what,
                       ਅੱਖਰ **part, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  ਅਚਲ ਅੱਖਰ *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  ਅੱਖਰ portbuf[7];
  bool plusdecode = FALSE;
  ਜੇ(!u)
    ਵਾਪਸ CURLUE_BAD_HANDLE;
  ਜੇ(!part)
    ਵਾਪਸ CURLUE_BAD_PARTPOINTER;
  *part = ਨਿਰਾ;

  ਚੋਣ(what) {
  ਹਾਲਤ CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    ਜੇ((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      ਵਾਪਸ CURLUE_NO_SCHEME;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ਪੋਰਟ:
    ptr = ਨਿਰਾ;
    ifmissing = CURLUE_NO_ਪੋਰਟ;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    ਜੇ(u->ਪੋਰਟ_present) {
      ਅਚਲ ਬਣਤਰ Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ਨਿਰਾ;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      ਜੇ(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_ਪੋਰਟ)) {
        ptr = ਨਿਰਾ;
      }
      ਨਹੀਂਤਾਂ {
        curl_msnprintf(portbuf, ਆਕਾਰ(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ((flags & CURLU_DEFAULT_ਪੋਰਟ) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ਅਚਲ ਬਣਤਰ Curl_scheme *h = Curl_get_scheme(u->scheme);
      ਜੇ(h) {
        curl_msnprintf(portbuf, ਆਕਾਰ(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ਰਸਤਾ:
    ptr = u->ਰਸਤਾ;
    ਜੇ(!ptr)
      ptr = "/";
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    ਜੇ(ptr && !ptr[0] && !(flags & CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ))
      /* there was a blank query and the user does not ask for it */
      ptr = ਨਿਰਾ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    ਜੇ(!ptr && u->fragment_present && flags & CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_URL:
    ਵਾਪਸ urlget_url(u, part, flags);
  ਮੂਲ:
    ptr = ਨਿਰਾ;
    ਰੋਕੋ;
  }
  ਜੇ(ptr)
    ਵਾਪਸ urlget_format(u, what, ptr, part, plusdecode, flags);

  ਵਾਪਸ ifmissing;
}

ਸਥਿਰ CURLUcode ਸੈੱਟ_url_scheme(CURLU *u, ਅਚਲ ਅੱਖਰ *scheme,
                                ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  size_t plen = ਲੜੀ_ਲੰਬਾਈ(scheme);
  ਅਚਲ ਬਣਤਰ Curl_scheme *h = ਨਿਰਾ;
  ਜੇ((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    ਵਾਪਸ CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  ਜੇ(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->ਲੜੀਵਾਰ))
    ਵਾਪਸ CURLUE_UNSUPPORTED_SCHEME;
  ਜੇ(!h) {
    ਅਚਲ ਅੱਖਰ *s = scheme;
    ਜੇ(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      ਜਦੋਂ(--plen) {
        ਜੇ(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        ਨਹੀਂਤਾਂ
          ਵਾਪਸ CURLUE_BAD_SCHEME;
      }
    }
    ਨਹੀਂਤਾਂ
      ਵਾਪਸ CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode ਸੈੱਟ_url_ਪੋਰਟ(CURLU *u, ਅਚਲ ਅੱਖਰ *provided_ਪੋਰਟ)
{
  curl_off_t ਪੋਰਟ;
  ਜੇ(!ISDIGIT(provided_ਪੋਰਟ[0]))
    /* not a number */
    ਵਾਪਸ CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ;
  ਜੇ(curlx_str_ਸੰਖਿਆ(&provided_ਪੋਰਟ, &ਪੋਰਟ, 0xffff) || *provided_ਪੋਰਟ)
    /* weirdly provided number, not good! */
    ਵਾਪਸ CURLUE_BAD_ਪੋਰਟ_ਸੰਖਿਆ;
  u->portnum = (uint16_t)ਪੋਰਟ;
  u->ਪੋਰਟ_present = TRUE;
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode ਸੈੱਟ_url(CURLU *u, ਅਚਲ ਅੱਖਰ *url, size_t part_ਮਾਪ,
                         ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  ਅੱਖਰ *oldurl = ਨਿਰਾ;

  ਜੇ(!part_ਮਾਪ) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    ਜੇ(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      ਵਾਪਸ CURLUE_OK;
    }
    ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
      ਵਾਪਸ uc;
    ਵਾਪਸ CURLUE_MALFORMED_ਨਿਵੇਸ਼;
  }

  /* if the new URL is absolute replace the existing with the new. */
  ਜੇ(Curl_is_absolute_url(url, ਨਿਰਾ, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    ਵਾਪਸ parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_ਖਾਲੀ_ਥਾਵਾਂ);
  ਜੇ(uc == CURLUE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ)
    ਵਾਪਸ uc;
  ਨਹੀਂਤਾਂ ਜੇ(uc)
    ਵਾਪਸ parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  ਵਾਪਸ uc;
}

ਸਥਿਰ CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  ਚੋਣ(what) {
  ਹਾਲਤ CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, ਆਕਾਰ(ਬਣਤਰ Curl_URL));
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_USER:
    curlx_safefree(u->user);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_HOST:
    curlx_safefree(u->host);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ਪੋਰਟ:
    u->portnum = 0;
    u->ਪੋਰਟ_present = FALSE;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ਰਸਤਾ:
    curlx_safefree(u->ਰਸਤਾ);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    ਰੋਕੋ;
  ਮੂਲ:
    ਵਾਪਸ CURLUE_UNKNOWN_PART;
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ bool allowed_ਵਿੱਚ_ਰਸਤਾ(ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ x)
{
  ਚੋਣ(x) {
  ਹਾਲਤ '!':
  ਹਾਲਤ '$':
  ਹਾਲਤ '&':
  ਹਾਲਤ '\'':
  ਹਾਲਤ '(':
  ਹਾਲਤ ')':
  ਹਾਲਤ '{':
  ਹਾਲਤ '}':
  ਹਾਲਤ '[':
  ਹਾਲਤ ']':
  ਹਾਲਤ '*':
  ਹਾਲਤ '+':
  ਹਾਲਤ ',':
  ਹਾਲਤ ';':
  ਹਾਲਤ '=':
  ਹਾਲਤ ':':
  ਹਾਲਤ '@':
  ਹਾਲਤ '/':
    ਵਾਪਸ TRUE;
  }
  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLUcode url_encode_part(ਬਣਤਰ dynbuf *encp,
                                 ਅਚਲ ਅੱਖਰ *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *i;

  ਲਈ(i = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)part; *i; i++) {
    CURLcode ਨਤੀਜਾ;
    ਜੇ((*i == ' ') && plusencode)
      ਨਤੀਜਾ = curlx_dyn_addn(encp, "+", 1);
    ਨਹੀਂਤਾਂ ਜੇ(ISUNRESERVED(*i) ||
            (pathmode && allowed_ਵਿੱਚ_ਰਸਤਾ(*i)) ||
            ((*i == '=') && equalsencode)) {
      ਜੇ((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      ਨਤੀਜਾ = curlx_dyn_addn(encp, i, 1);
    }
    ਨਹੀਂਤਾਂ {
      ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਬਾਹਰਲਾ[3] = { '%' };
      Curl_hexbyte(&ਬਾਹਰਲਾ[1], *i);
      ਨਤੀਜਾ = curlx_dyn_addn(encp, ਬਾਹਰਲਾ, 3);
    }
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ cc2cu(ਨਤੀਜਾ);
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode url_uppercasehex_part(ਬਣਤਰ dynbuf *encp,
                                       ਅਚਲ ਅੱਖਰ *part)
{
  ਅੱਖਰ *p;
  CURLcode ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(encp, part);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ cc2cu(ਨਤੀਜਾ);
  p = curlx_dyn_ptr(encp);
  ਜਦੋਂ(*p) {
    /* make sure percent encoded are upper case */
    ਜੇ((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    ਨਹੀਂਤਾਂ
      p++;
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode url_append_query(CURLU *u, ਬਣਤਰ dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? ਲੜੀ_ਲੰਬਾਈ(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  ਜੇ(querylen) {
    ਬਣਤਰ dynbuf qbuf;
    CURLcode ਨਤੀਜਾ;
    ਅਚਲ ਅੱਖਰ *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);

    /* add original query */
    ਨਤੀਜਾ = curlx_dyn_addn(&qbuf, u->query, querylen);
    ਜੇ(!ਨਤੀਜਾ && addamperand)
      /* add ampersand */
      ਨਤੀਜਾ = curlx_dyn_addn(&qbuf, "&", 1);
    ਜੇ(!ਨਤੀਜਾ)
      /* add new query part */
      ਨਤੀਜਾ = curlx_dyn_ਜੋੜੋ(&qbuf, newp);
    ਜੇ(ਨਤੀਜਾ)
      ਜਾਓ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    ਵਾਪਸ CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    ਵਾਪਸ cc2cu(ਨਤੀਜਾ);
  }
  ਨਹੀਂਤਾਂ {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  ਵਾਪਸ CURLUE_OK;
}

ਸਥਿਰ CURLUcode url_sethost(CURLU *u, ਬਣਤਰ dynbuf *encp,
                             bool urlencode,
                             ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  ਅੱਖਰ *newp = curlx_dyn_ptr(encp);
  ਜੇ(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  ਨਹੀਂਤਾਂ ਜੇ(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    ਅੱਖਰ *decoded = ਨਿਰਾ;
    CURLcode ਨਤੀਜਾ = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    ਜੇ(ਨਤੀਜਾ || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    ਨਹੀਂਤਾਂ {
      ਬਣਤਰ dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ);
      ਜੇ(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      ਨਹੀਂਤਾਂ ਜੇ(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        ਜੇ(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  ਨਹੀਂਤਾਂ ਜੇ(hostname_check6(u, newp, n))
    bad = TRUE;
  ਨਹੀਂਤਾਂ ਜੇ(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  ਜੇ(bad) {
    curlx_dyn_free(encp);
    ਵਾਪਸ CURLUE_BAD_HOSTNAME;
  }
  ਵਾਪਸ CURLUE_OK;
}

CURLUcode curl_url_ਸੈੱਟ(CURLU *u, CURLUPart what,
                       ਅਚਲ ਅੱਖਰ *part, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ flags)
{
  ਅੱਖਰ **storep = ਨਿਰਾ;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  ਜੇ(!u)
    ਵਾਪਸ CURLUE_BAD_HANDLE;
  ਜੇ(!part)
    /* setting a part to NULL clears it */
    ਵਾਪਸ urlset_clear(u, what);

  nalloc = ਲੜੀ_ਲੰਬਾਈ(part);
  ਜੇ(nalloc > CURL_MAX_ਨਿਵੇਸ਼_ਲੰਮਾਈ)
    /* excessive input length */
    ਵਾਪਸ CURLUE_MALFORMED_ਨਿਵੇਸ਼;

  ਚੋਣ(what) {
  ਹਾਲਤ CURLUPART_SCHEME: {
    CURLUcode ਸਥਿਤੀ = ਸੈੱਟ_url_scheme(u, part, flags);
    ਜੇ(ਸਥਿਤੀ)
      ਵਾਪਸ ਸਥਿਤੀ;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    ਰੋਕੋ;
  }
  ਹਾਲਤ CURLUPART_USER:
    storep = &u->user;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_PASSWORD:
    storep = &u->password;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_OPTIONS:
    storep = &u->options;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ZONEID:
    storep = &u->zoneid;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_ਪੋਰਟ:
    ਵਾਪਸ ਸੈੱਟ_url_ਪੋਰਟ(u, part);
  ਹਾਲਤ CURLUPART_ਰਸਤਾ:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->ਰਸਤਾ;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    ਰੋਕੋ;
  ਹਾਲਤ CURLUPART_URL:
    ਵਾਪਸ ਸੈੱਟ_url(u, part, nalloc, flags);
  ਮੂਲ:
    ਵਾਪਸ CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ਅਚਲ ਅੱਖਰ *newp = ਨਿਰਾ;
    ਬਣਤਰ dynbuf enc;
    CURLUcode ਸਥਿਤੀ;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    ਜੇ(leadingslash && (part[0] != '/')) {
      CURLcode ਨਤੀਜਾ = curlx_dyn_addn(&enc, "/", 1);
      ਜੇ(ਨਤੀਜਾ)
        ਵਾਪਸ cc2cu(ਨਤੀਜਾ);
    }
    ਜੇ(urlencode)
      ਸਥਿਤੀ = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    ਨਹੀਂਤਾਂ
      ਸਥਿਤੀ = url_uppercasehex_part(&enc, part);
    ਜੇ(!ਸਥਿਤੀ) {
      newp = curlx_dyn_ptr(&enc);

      ਜੇ(appendquery && newp)
        ਵਾਪਸ url_append_query(u, &enc);
      ਨਹੀਂਤਾਂ ਜੇ(what == CURLUPART_HOST)
        ਸਥਿਤੀ = url_sethost(u, &enc, urlencode, flags);
    }
    ਜੇ(ਸਥਿਤੀ)
      ਵਾਪਸ ਸਥਿਤੀ;

    ਜੇ(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (ਅੱਖਰ *)CURL_UNCONST(newp);
  }
  ਵਾਪਸ CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *ਅਧਾਰ, CURLU *href)
{
  ਅਚਲ ਬਣਤਰ Curl_scheme *s = ਨਿਰਾ;

  /* base must be an absolute URL */
  ਜੇ(!ਅਧਾਰ->scheme || !ਅਧਾਰ->host)
    ਵਾਪਸ FALSE;
  ਜੇ(href->scheme && !curl_strequal(ਅਧਾਰ->scheme, href->scheme))
    ਵਾਪਸ FALSE;
  ਜੇ(href->host) {
    ਜੇ(!curl_strequal(ਅਧਾਰ->host, href->host))
      ਵਾਪਸ FALSE;

    ਜੇ(ਅਧਾਰ->ਪੋਰਟ_present != href->ਪੋਰਟ_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(ਅਧਾਰ->scheme);
      ਜੇ(!s) /* Cannot match default port for unknown scheme */
        ਵਾਪਸ FALSE;
      /* to match, the present one must be the default port */
      ਜੇ((ਅਧਾਰ->ਪੋਰਟ_present && (ਅਧਾਰ->portnum != s->defport)) ||
         (href->ਪੋਰਟ_present && (href->portnum != s->defport)))
        ਵਾਪਸ FALSE;
    }
    ਨਹੀਂਤਾਂ ਜੇ(ਅਧਾਰ->portnum != href->portnum) /* both present or missing */
      ਵਾਪਸ FALSE;

    ਜੇ(!curl_strequal(ਅਧਾਰ->zoneid ? ਅਧਾਰ->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      ਵਾਪਸ FALSE;
  }
  ਨਹੀਂਤਾਂ ਜੇ(href->ਪੋਰਟ_present) /* no host in href, then there must be no port */
    ਵਾਪਸ FALSE;
  ਵਾਪਸ TRUE;
}

CURLUcode Curl_url_get_ਪੋਰਟ(CURLU *u, uint16_t *pport)
{
  ਜੇ(u->ਪੋਰਟ_present) {
    *pport = u->portnum;
    ਵਾਪਸ CURLUE_OK;
  }
  ਨਹੀਂਤਾਂ ਜੇ(u->scheme) {
    ਅਚਲ ਬਣਤਰ Curl_scheme *s = Curl_get_scheme(u->scheme);
    ਜੇ(s && s->defport) {
      *pport = s->defport;
      ਵਾਪਸ CURLUE_OK;
    }
  }
  *pport = 0;
  ਵਾਪਸ CURLUE_NO_ਪੋਰਟ;
}
