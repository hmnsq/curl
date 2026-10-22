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

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "urlapi-int.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"
#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_pton.h"
#ଅନ୍ତର୍ଭାଵ "curlx/inet_ntop.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strdup.h"
#ଅନ୍ତର୍ଭାଵ "idn.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curl_memrchr.h"

#ifdef _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#define STARTS_WITH_DRIVE_PREFIX(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#endif

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#define STARTS_WITH_URL_DRIVE_PREFIX(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#define MAX_SCHEME_LEN 40
#define MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ଯଦି !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

ସ୍ଥିର ଶୂନ୍ୟ free_urlhandle(ସଂରଚନା Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->path);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *find_host_sep(ଅଚଳ ଅକ୍ଷର *url)
{
  /* Find the start of the hostname */
  ଅଚଳ ଅକ୍ଷର *sep = strstr(url, "//");
  ଯଦି(!sep)
    sep = url;
  ଅନ୍ୟଥା
    sep += 2;

  /* Find first / or ? */
  ଯେତେବେଳେ(*sep && *sep != '/' && *sep != '?')
    sep++;

  ଫେରନ୍ତୁ sep;
}

/* convert CURLcode to CURLUcode */
#define cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_OUT_OF_MEMORY)

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
UNITTEST CURLUcode urlencode_str(ସଂରଚନା dynbuf *o, ଅଚଳ ଅକ୍ଷର *url,
                                 size_t len, bool relative,
                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା query);
UNITTEST CURLUcode urlencode_str(ସଂରଚନା dynbuf *o, ଅଚଳ ଅକ୍ଷର *url,
                                 size_t len, bool relative,
                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା query)
{
  /* we must add this with whitespace-replacing */
  ଅଚଳ unsigned ଅକ୍ଷର *iptr;
  ଅଚଳ unsigned ଅକ୍ଷର *host_sep = (ଅଚଳ unsigned ଅକ୍ଷର *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  ଯଦି(!relative) {
    size_t n;
    host_sep = (ଅଚଳ unsigned ଅକ୍ଷର *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ଅଚଳ ଅକ୍ଷର *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  ପାଇଁ(iptr = host_sep; len && !result;) {
    ଯଦି(*iptr == ' ') {
      ଯଦି(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      ଅନ୍ୟଥା
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    ଅନ୍ୟଥା ଯଦି((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned ଅକ୍ଷର out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    ଅନ୍ୟଥା ଯଦି(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned ଅକ୍ଷର hex = (unsigned ଅକ୍ଷର)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned ଅକ୍ଷର out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    ଅନ୍ୟଥା {
      ଅଚଳ unsigned ଅକ୍ଷର *start = iptr;
      ଯେତେବେଳେ(len) {
        ଯଦି(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          ଭାଙ୍ଗ;
        ଯଦି(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          ଭାଙ୍ଗ;
        ଯଦି(*iptr == '?') {
          ଯଦି(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            ଭାଙ୍ଗ;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ଅଚଳ ଅକ୍ଷର *)start, (size_t)(iptr - start));
    }
  }

  ଯଦି(result)
    ଫେରନ୍ତୁ cc2cu(result);
  ଫେରନ୍ତୁ CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ଅଚଳ ଅକ୍ଷର *url, ଅକ୍ଷର *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (ଶୂନ୍ୟ)buflen; /* only used in debug-builds */
  ଯଦି(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  ଯଦି(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    ଫେରନ୍ତୁ 0;
#endif
  ଯଦି(ISALPHA(url[0])) {
    ଯଦି(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    ପାଇଁ(i = 1; i < MAX_SCHEME_LEN; ++i) {
      ଅକ୍ଷର s = url[i];
      ଯଦି(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        ଯଦି(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      ଅନ୍ୟଥା {
        ଭାଙ୍ଗ;
      }
    }
  }
  ଯଦି(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    ଯଦି(buf)
      buf[i] = 0;
    ଫେରନ୍ତୁ len;
  }
  ଯଦି(buf)
    buf[0] = 0;
  ଫେରନ୍ତୁ 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ସ୍ଥିର bool badoctets(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, size_t n, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ଅଚଳ uint8_t *p = (ଅଚଳ unsigned ଅକ୍ଷର *)ନିବେଶ;
  ଅଚଳ uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  ଯେତେବେଳେ(n--) {
    ଯଦି(*p <= control || *p == 127)
      ଫେରନ୍ତୁ TRUE;
    p++;
  }
  ଫେରନ୍ତୁ FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ସଂରଚନା Curl_URL *u,
                                        ଅଚଳ ଅକ୍ଷର *login,
                                        size_t len,
                                        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(ସଂରଚନା Curl_URL *u,
                                        ଅଚଳ ଅକ୍ଷର *login,
                                        size_t len,
                                        unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  ଅକ୍ଷର *userp = ଖାଲି;
  ଅକ୍ଷର *passwdp = ଖାଲି;
  ଅକ୍ଷର *optionsp = ଖାଲି;
  ଅଚଳ ସଂରଚନା Curl_scheme *h = ଖାଲି;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ଅଚଳ ଅକ୍ଷର *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  ଯଦି(!ptr)
    ଯାଅ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  ଯଦି(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ଖାଲି);
  ଯଦି(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    ଯାଅ out;
  }

  ଯଦି(userp) {
    ଯଦି(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      ଯାଅ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  ଯଦି(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  ଯଦି(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  ଯଦି(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  ଅନ୍ୟଥା ଯଦି(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  ଅନ୍ୟଥା ଯଦି(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_ନିବେଶ;

  userp = passwdp = optionsp = ଖାଲି;

  ଯଦି(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    ଫେରନ୍ତୁ CURLUE_OK;
  }

out:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  ଫେରନ୍ତୁ ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(ସଂରଚନା Curl_URL *u, ସଂରଚନା dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(ସଂରଚନା Curl_URL *u, ସଂରଚନା dynbuf *host,
                              bool has_scheme)
{
  ଅଚଳ ଅକ୍ଷର *portptr;
  ଅଚଳ ଅକ୍ଷର *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  ଯଦି(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    ଯଦି(!portptr)
      ଫେରନ୍ତୁ CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    ଯଦି(*portptr) {
      ଯଦି(*portptr != ':')
        ଫେରନ୍ତୁ CURLUE_BAD_PORT_ସଂଖ୍ୟା;
    }
    ଅନ୍ୟଥା
      portptr = ଖାଲି;
  }
  ଅନ୍ୟଥା
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  ଯଦି(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    ଯଦି(!*portptr)
      ଫେରନ୍ତୁ has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_ସଂଖ୍ୟା;
    ଯଦି(*portptr == '\\')
      ଫେରନ୍ତୁ CURLUE_BACKSLASH;
    rc = curlx_str_ସଂଖ୍ୟା(&portptr, &port, 0xffff);
    ଯଦି(rc)
      ଫେରନ୍ତୁ CURLUE_BAD_PORT_ସଂଖ୍ୟା;
    ଅନ୍ୟଥା ଯଦି(*portptr == '\\')
      ଫେରନ୍ତୁ CURLUE_BACKSLASH;
    ଅନ୍ୟଥା ଯଦି(*portptr)
      ଫେରନ୍ତୁ CURLUE_BAD_PORT_ସଂଖ୍ୟା;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  ଫେରନ୍ତୁ CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ସ୍ଥିର ଅଚଳ bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(ସଂରଚନା Curl_URL *u, ଅକ୍ଷର *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ସଂରଚନା Curl_URL *u, ଅକ୍ଷର *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  ଯଦି(hlen < 4) /* '[::]' is the shortest possible valid string */
    ଫେରନ୍ତୁ CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  ଯଦି(hlen != len) {
    hlen = len;
    ଯଦି(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      ଅକ୍ଷର zoneid[MAX_ZONEID_LEN];
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
      ଅକ୍ଷର *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      ଯଦି(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      ଯେତେବେଳେ(!invalid_host_char[(unsigned ଅକ୍ଷର)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      ଯଦି(!i || (']' != *h))
        ଫେରନ୍ତୁ CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      ଯଦି(!u->zoneid)
        ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    ଅକ୍ଷର dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    ଯଦି(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      ଫେରନ୍ତୁ CURLUE_BAD_IPV6;
    ଯଦି(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ସ୍ଥିର CURLUcode hostname_check(ଅକ୍ଷର *hostname, size_t hlen)
{
  size_t i;
  ପାଇଁ(i = 0; i < hlen; i++) {
    ଯଦି(invalid_host_char[(unsigned ଅକ୍ଷର)hostname[i]])
      ଫେରନ୍ତୁ CURLUE_BAD_HOSTNAME;
  }
  ଯଦି((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    ଫେରନ୍ତୁ CURLUE_BAD_HOSTNAME;
  ଅନ୍ୟଥା ଯଦି((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    ଫେରନ୍ତୁ CURLUE_BAD_HOSTNAME;
  ଫେରନ୍ତୁ CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ସ୍ଥିର CURLUcode hostname_check6(ସଂରଚନା Curl_URL *u, ଅକ୍ଷର *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  ଯଦି(!hlen)
    ଫେରନ୍ତୁ CURLUE_NO_HOST;
  ଅନ୍ୟଥା ଯଦି(hostname[0] == '[')
    ଫେରନ୍ତୁ ipv6_parse(u, hostname, hlen);

  ଫେରନ୍ତୁ hostname_check(hostname, hlen);
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
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା ipv4_normalize(ସଂରଚନା dynbuf *host);
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା ipv4_normalize(ସଂରଚନା dynbuf *host)
{
  bool done = FALSE;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = 0;
  ଅଚଳ ଅକ୍ଷର *c = curlx_dyn_ptr(host);
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  ଯଦି(!ISDIGIT(*c))
    ଫେରନ୍ତୁ HOST_NAME;

  ଯେତେବେଳେ(!done) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
    curl_off_t l;
    ଯଦି(*c == '0') {
      ଯଦି((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        ଯଦି(rc)
          ଫେରନ୍ତୁ HOST_NAME;
      }
      ଅନ୍ୟଥା
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    ଅନ୍ୟଥା
      rc = curlx_str_ସଂଖ୍ୟା(&c, &l, UINT_MAX);

    ଯଦି(rc) {
      ଯଦି(!n || (rc != STRE_NO_ଅଙ୍କ) || *c)
        ଫେରନ୍ତୁ HOST_NAME;
      n--;
    }
    ଅନ୍ୟଥା
      parts[n] = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)l;

    ବାଛ(*c) {
    ସ୍ଥିତି '.':
      ଯଦି(n == 3) {
        ଯଦି(c[1])
          /* something follows this dot */
          ଫେରନ୍ତୁ HOST_NAME;
        done = TRUE;
      }
      ଅନ୍ୟଥା {
        n++;
        c++;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି '\0':
      done = TRUE;
      ଭାଙ୍ଗ;

    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ଫେରନ୍ତୁ HOST_NAME;
    }
  }

  ବାଛ(n) {
  ସ୍ଥିତି 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 1: /* a.b -- 8.24 bits */
    ଯଦି((parts[0] > 0xff) || (parts[1] > 0xffffff))
      ଫେରନ୍ତୁ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 2: /* a.b.c -- 8.8.16 bits */
    ଯଦି((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      ଫେରନ୍ତୁ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 3: /* a.b.c.d -- 8.8.8.8 bits */
    ଯଦି((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      ଫେରନ୍ତୁ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    ଭାଙ୍ଗ;
  }
  ଯଦି(result)
    ଫେରନ୍ତୁ HOST_ERROR;
  ଫେରନ୍ତୁ HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ସ୍ଥିର CURLUcode urldecode_host(ସଂରଚନା dynbuf *host)
{
  ଅଚଳ ଅକ୍ଷର *per;
  ଅଚଳ ଅକ୍ଷର *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  ଯଦି(!per)
    /* nothing to decode */
    ଫେରନ୍ତୁ CURLUE_OK;
  ଅନ୍ୟଥା {
    /* encoded */
    size_t dlen;
    ଅକ୍ଷର *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    ଯଦି(result)
      ଫେରନ୍ତୁ CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    ଯଦି(result)
      ଫେରନ୍ତୁ cc2cu(result);
  }

  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode parse_authority(ସଂରଚନା Curl_URL *u,
                                 ଅଚଳ ଅକ୍ଷର *auth, size_t authlen,
                                 unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                                 ସଂରଚନା dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  ଯଦି(uc)
    ଫେରନ୍ତୁ uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  ଯଦି(result) {
    uc = cc2cu(result);
    ଫେରନ୍ତୁ uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  ଯଦି(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  ଯଦି(!uc)
    uc = urldecode_host(host);
  ଯଦି(uc)
    ;
  ଅନ୍ୟଥା ଯଦି(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  ଅନ୍ୟଥା {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା type = ipv4_normalize(host);

    ଯଦି(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    ଅନ୍ୟଥା ଯଦି(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  ଫେରନ୍ତୁ uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ଅଚଳ ଅକ୍ଷର *authority)
{
  CURLUcode ures;
  ସଂରଚନା dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_ନିବେଶ_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  ଯଦି(ures)
    curlx_dyn_free(&host);
  ଅନ୍ୟଥା {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  ଫେରନ୍ତୁ ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ସ୍ଥିର bool is_dot(ଅଚଳ ଅକ୍ଷର **str, size_t *clen)
{
  ଅଚଳ ଅକ୍ଷର *p = *str;
  ଯଦି(*p == '.') {
    (*str)++;
    (*clen)--;
    ଫେରନ୍ତୁ TRUE;
  }
  ଅନ୍ୟଥା ଯଦି((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ସ୍ଥିର bool needs_dedotdot(ଅଚଳ ଅକ୍ଷର *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  ଯଦି(pn < 2)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    ଫେରନ୍ତୁ FALSE;
  ଯେତେବେଳେ(pn) {
    ଯଦି(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      ଯଦି(!pn || ISSLASH(*p))
        ଫେରନ୍ତୁ TRUE;
      /* "../" or ".." before end of string */
      ଅନ୍ୟଥା ଯଦି(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        ଫେରନ୍ତୁ TRUE;
    }
    ଅନ୍ୟଥା {
      p++;
      pn--;
    }
  }
  ଫେରନ୍ତୁ FALSE;
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
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା dedotdotify(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, size_t clen, ଅକ୍ଷର **outp);
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା dedotdotify(ଅଚଳ ଅକ୍ଷର *ନିବେଶ, size_t clen, ଅକ୍ଷର **outp)
{
  ସଂରଚନା dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ଅଚଳ ଅକ୍ଷର *dinput = ନିବେଶ;
  size_t dlen = clen;

  *outp = ଖାଲି;
  ଯଦି(!needs_dedotdot(ନିବେଶ, clen))
    ଫେରନ୍ତୁ 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  ଯଦି(is_dot(&dinput, &dlen)) {
    ଯଦି(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      ନିବେଶ = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    ଅନ୍ୟଥା ଯଦି(is_dot(&dinput, &dlen)) {
      ଯଦି(!dlen)
        /* .. [end] */
        ଯାଅ end;
      ଅନ୍ୟଥା ଯଦି(ISSLASH(*dinput)) {
        /* ../ */
        ନିବେଶ = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  ଯେତେବେଳେ(clen && !result) { /* until end of path content */
    ଯଦି(ISSLASH(*ନିବେଶ)) {
      ଅଚଳ ଅକ୍ଷର *p = &ନିବେଶ[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      ଯଦି(is_dot(&p, &blen)) {
        ଯଦି(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          ଭାଙ୍ଗ;
        }
        ଅନ୍ୟଥା ଯଦି(ISSLASH(*p)) { /* /./ */
          ନିବେଶ = p;
          clen = blen;
          ଜାରି;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        ଅନ୍ୟଥା ଯଦି(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          ଯଦି(len) {
            ଅଚଳ ଅକ୍ଷର *ptr = curlx_dyn_ptr(&out);
            ଅଚଳ ଅକ୍ଷର *last = memrchr(ptr, '/', len);
            ଯଦି(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          ଯଦି(blen) { /* /../ */
            ନିବେଶ = p;
            clen = blen;
            ଜାରି;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          ଭାଙ୍ଗ;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, ନିବେଶ, 1);
    ନିବେଶ++;
    clen--;
  }
end:
  ଯଦି(!result) {
    ଯଦି(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    ଅନ୍ୟଥା {
      *outp = curlx_strdup("");
      ଯଦି(!*outp)
        ଫେରନ୍ତୁ 1;
    }
  }
  ଫେରନ୍ତୁ result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ଅଚଳ ଅକ୍ଷର *url, size_t urllen, CURLU *u,
                              ଅଚଳ ଅକ୍ଷର **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ଅଚଳ ଅକ୍ଷର *url, size_t urllen, CURLU *u,
                              ଅଚଳ ଅକ୍ଷର **pathp, size_t *pathlenp)
{
  ଅଚଳ ଅକ୍ଷର *path;
  size_t pathlen;

  *pathp = ଖାଲି;
  *pathlenp = 0;
  ଯଦି(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    ଫେରନ୍ତୁ CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  ଯଦି(path[0] != '/')
    ଫେରନ୍ତୁ CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  ଯଦି(path[1] == '/') {
    /* swallow the two slashes */
    ଅଚଳ ଅକ୍ଷର *ptr = &path[2];

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
    ଯଦି(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      ଯଦି(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      ଅନ୍ୟଥା
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        ଫେରନ୍ତୁ CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#ଯଦି !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  ଯଦି(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    ଫେରନ୍ତୁ CURLUE_BAD_FILE_URL;
  }
#ଅନ୍ୟଥା
  /* If the path starts with a slash and a drive letter, ditch the slash */
  ଯଦି('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  ଯଦି(!u->scheme)
    ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode parse_scheme(ଅଚଳ ଅକ୍ଷର *url, CURLU *u, ଅକ୍ଷର *schemebuf,
                              size_t schemelen, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                              ଅଚଳ ଅକ୍ଷର **hostpp)
{
  /* clear path */
  ଅଚଳ ଅକ୍ଷର *schemep = ଖାଲି;

  ଯଦି(schemelen) {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_slashes = 0;
    ଅଚଳ ଅକ୍ଷର *p = &url[schemelen + 1];
    ଯଦି(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      ଫେରନ୍ତୁ CURLUE_UNSUPPORTED_SCHEME;

    ଯଦି(!ISSLASH(*p))
      /* less than one */
      ଫେରନ୍ତୁ CURLUE_BAD_SLASHES;
    ଯଦି((flags & CURLU_NO_AUTHORITY)) {
      ଯେତେବେଳେ(ISSLASH(*p) && (ଅଙ୍କ_slashes < 2)) {
        p++;
        ଅଙ୍କ_slashes++;
      }
    }
    ଅନ୍ୟଥା {
      ଯେତେବେଳେ(ISSLASH(*p) && (ଅଙ୍କ_slashes < 4)) {
        p++;
        ଅଙ୍କ_slashes++;
      }
      ଯଦି(ଅଙ୍କ_slashes > 3)
        ଫେରନ୍ତୁ CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  ଅନ୍ୟଥା {
    /* no scheme! */

    ଯଦି(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      ଫେରନ୍ତୁ CURLUE_BAD_SCHEME;

    ଯଦି(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  ଯଦି(schemep) {
    u->scheme = curlx_strdup(schemep);
    ଯଦି(!u->scheme)
      ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode guess_scheme(CURLU *u, ସଂରଚନା dynbuf *host)
{
  ଅଚଳ ଅକ୍ଷର *hostname = curlx_dyn_ptr(host);
  ଅଚଳ ଅକ୍ଷର *schemep = ଖାଲି;
  /* legacy curl-style guess based on hostname */
  ଯଦି(checkprefix("ftp.", hostname))
    schemep = "ftp";
  ଅନ୍ୟଥା ଯଦି(checkprefix("dict.", hostname))
    schemep = "dict";
  ଅନ୍ୟଥା ଯଦି(checkprefix("ldap.", hostname))
    schemep = "ldap";
  ଅନ୍ୟଥା ଯଦି(checkprefix("imap.", hostname))
    schemep = "imap";
  ଅନ୍ୟଥା ଯଦି(checkprefix("smtp.", hostname))
    schemep = "smtp";
  ଅନ୍ୟଥା ଯଦି(checkprefix("pop3.", hostname))
    schemep = "pop3";
  ଅନ୍ୟଥା
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  ଯଦି(!u->scheme)
    ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode handle_fragment(CURLU *u, ଅଚଳ ଅକ୍ଷର *fragment,
                                 size_t fraglen, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  ଯଦି(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    ଯଦି(flags & CURLU_URLENCODE) {
      ସଂରଚନା dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_ନିବେଶ_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      ଯଦି(ures)
        ଫେରନ୍ତୁ ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    ଅନ୍ୟଥା {
      ଯଦି(badoctets(fragment, fraglen, flags))
        ଫେରନ୍ତୁ CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      ଯଦି(!u->fragment)
        ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
    }
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode handle_query(CURLU *u, ଅଚଳ ଅକ୍ଷର *query,
                              size_t qlen, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  u->query_present = TRUE;
  ଯଦି(qlen > 1) {
    ଯଦି(flags & CURLU_URLENCODE) {
      ସଂରଚନା dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_ନିବେଶ_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      ଯଦି(ures)
        ଫେରନ୍ତୁ ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    ଅନ୍ୟଥା {
      ଯଦି(badoctets(query, qlen, flags))
        ଫେରନ୍ତୁ CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      ଯଦି(!u->query)
        ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
    }
  }
  ଅନ୍ୟଥା {
    /* single byte query */
    u->query = curlx_strdup("");
    ଯଦି(!u->query)
      ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode handle_path(CURLU *u, ଅଚଳ ଅକ୍ଷର *path,
                             size_t pathlen, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags,
                             bool is_file)
{
  CURLUcode ures;
  ଯଦି(pathlen && (flags & CURLU_URLENCODE)) {
    ସଂରଚନା dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ନିବେଶ_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    ଯଦି(ures)
      ଫେରନ୍ତୁ ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  ଯଦି(pathlen >= (size_t)(1 + !is_file)) {
    ଯଦି(badoctets(path, pathlen, flags))
      ଫେରନ୍ତୁ CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    ଯଦି(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      ଯଦି(!u->path)
        ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    ଅନ୍ୟଥା ଯଦି(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    ଯଦି(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      ଅକ୍ଷର *dedot;
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା err = dedotdotify(path, pathlen, &dedot);
      ଯଦି(err)
        ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
      ଯଦି(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode parseurl(ଅଚଳ ଅକ୍ଷର *url, CURLU *u, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ଅଚଳ ଅକ୍ଷର *path;
  size_t pathlen;
  ଅକ୍ଷର schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ସଂରଚନା dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  ଯଦି(urllen > CURL_MAX_ନିବେଶ_LENGTH)
    ଫେରନ୍ତୁ CURLUE_MALFORMED_ନିବେଶ;

  curlx_dyn_init(&host, CURL_MAX_ନିବେଶ_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, ଆକାର(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  ଯଦି(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  ଅନ୍ୟଥା {
    ଅଚଳ ଅକ୍ଷର *hostp = ଖାଲି;
    ଅଚଳ ଅକ୍ଷର *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    ଯଦି(ures)
      ଯାଅ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    ଯେତେବେଳେ(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    ଯଦି(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      ଯଦି(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    ଅନ୍ୟଥା ଯଦି(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      ଯଦି(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    ଅନ୍ୟଥା
      ures = CURLUE_NO_HOST;
  }
  ଯଦି(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ଅଚଳ ଅକ୍ଷର *fragment = memchr(path, '#', pathlen);
    ଯଦି(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  ଯଦି(!ures) {
    ଅଚଳ ଅକ୍ଷର *query = memchr(path, '?', pathlen);
    ଯଦି(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  ଯଦି(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  ଯଦି(!ures) {
    u->host = curlx_dyn_ptr(&host);
    ଫେରନ୍ତୁ CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  ଫେରନ୍ତୁ ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ସ୍ଥିର CURLUcode parseurl_and_replace(ଅଚଳ ଅକ୍ଷର *url, CURLU *u,
                                      unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, ଆକାର(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  ଯଦି(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  ଫେରନ୍ତୁ ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ସ୍ଥିର CURLUcode redirect_url(ଅଚଳ ଅକ୍ଷର *base, ଅଚଳ ଅକ୍ଷର *relurl,
                              CURLU *u, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ସଂରଚନା dynbuf urlbuf;
  bool host_changed = FALSE;
  ଅଚଳ ଅକ୍ଷର *useurl = relurl;
  ଅଚଳ ଅକ୍ଷର *cutoff = ଖାଲି;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ଅଚଳ ଅକ୍ଷର *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ଅଚଳ ଅକ୍ଷର *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  ଯଦି(!base)
    ଫେରନ୍ତୁ CURLUE_MALFORMED_ନିବେଶ; /* should never happen */

  /* handle different relative URL types */
  ବାଛ(relurl[0]) {
  ସ୍ଥିତି '/':
    ଯଦି(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    ଅନ୍ୟଥା
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    ଭାଙ୍ଗ;

  ସ୍ଥିତି '#':
    /* fragment-only change */
    ଯଦି(u->fragment_present)
      cutoff = strchr(protsep, '#');
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    /* path or query-only change */
    ଯଦି(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    ଅନ୍ୟଥା ଯଦି(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    ଯଦି(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      ଯଦି(cutoff)
        cutoff++; /* truncate after last slash */
    }
    ଭାଙ୍ଗ;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_ନିବେଶ_LENGTH);

  ଯଦି(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  ଅନ୍ୟଥା
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  ଫେରନ୍ତୁ uc;
}

/*
 */
CURLU *curl_url(ଶୂନ୍ୟ)
{
  ଫେରନ୍ତୁ curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_URL));
}

ଶୂନ୍ୟ curl_url_cleanup(CURLU *u)
{
  ଯଦି(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  କର {                                          \
    ଯଦି((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      ଯଦି(!(dest)->name)                         \
        ଯାଅ fail;                              \
    }                                           \
  } ଯେତେବେଳେ(0)

CURLU *curl_url_dup(ଅଚଳ CURLU *in)
{
  ସଂରଚନା Curl_URL *u;

  ଯଦି(!in)
    ଫେରନ୍ତୁ ଖାଲି;

  u = curlx_calloc(1, ଆକାର(ସଂରଚନା Curl_URL));
  ଯଦି(u) {
    DUP(u, in, scheme);
    DUP(u, in, user);
    DUP(u, in, password);
    DUP(u, in, options);
    DUP(u, in, host);
    DUP(u, in, path);
    DUP(u, in, query);
    DUP(u, in, fragment);
    DUP(u, in, zoneid);
    u->portnum = in->portnum;
    u->port_present = in->port_present;
    u->fragment_present = in->fragment_present;
    u->query_present = in->query_present;
    u->guessed_scheme = in->guessed_scheme;
  }
  ଫେରନ୍ତୁ u;
fail:
  curl_url_cleanup(u);
  ଫେରନ୍ତୁ ଖାଲି;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#ଅନ୍ୟଥା
ସ୍ଥିର CURLUcode host_decode(ଅଚଳ ଅକ୍ଷର *host, ଅକ୍ଷର **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  ଯଦି(result)
    ଫେରନ୍ତୁ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode host_encode(ଅଚଳ ଅକ୍ଷର *host, ଅକ୍ଷର **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  ଯଦି(result)
    ଫେରନ୍ତୁ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  ଫେରନ୍ତୁ CURLUE_OK;
}
#endif

ସ୍ଥିର CURLUcode urlget_format(ଅଚଳ CURLU *u, CURLUPart what,
                               ଅଚଳ ଅକ୍ଷର *ptr, ଅକ୍ଷର **partp,
                               bool plusdecode, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  ଅକ୍ଷର *part = curlx_memdup0(ptr, partlen);
  *partp = ଖାଲି;
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
  ଯଦି(plusdecode) {
    /* convert + to space */
    ଅକ୍ଷର *plus = part;
    size_t i = 0;
    ପାଇଁ(i = 0; i < partlen; ++plus, i++) {
      ଯଦି(*plus == '+')
        *plus = ' ';
    }
  }
  ଯଦି(urldecode) {
    ଅକ୍ଷର *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    ଯଦି(result)
      ଫେରନ୍ତୁ CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  ଯଦି(urlencode) {
    ସଂରଚନା dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ନିବେଶ_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    ଯଦି(uc)
      ଫେରନ୍ତୁ uc;
    part = curlx_dyn_ptr(&enc);
  }
  ଅନ୍ୟଥା ଯଦି(punycode) {
    ଯଦି(!Curl_is_ASCII_name(u->host)) {
      ଅକ୍ଷର *punyversion = ଖାଲି;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      ଯଦି(uc)
        ଫେରନ୍ତୁ uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      ଯଦି(uc) {
        curlx_free(punyversion);
        ଫେରନ୍ତୁ uc;
      }
      part = punyversion;
    }
  }
  ଅନ୍ୟଥା ଯଦି(depunyfy && Curl_is_ASCII_name(u->host)) {
    ଅକ୍ଷର *unpunified = ଖାଲି;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    ଯଦି(uc)
      ଫେରନ୍ତୁ uc;
    part = unpunified;
  }
  *partp = part;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode file_url(ଅଚଳ CURLU *u, ଅକ୍ଷର **part,
                          ଅଚଳ ଅକ୍ଷର *fragmentsep,
                          ଅଚଳ ଅକ୍ଷର *querysep)
{
  ଅକ୍ଷର *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  ଯଦି(!url)
    ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;

  *part = url;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode urlget_url(ଅଚଳ CURLU *u, ଅକ୍ଷର **part, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ଅକ୍ଷର *url;
  ଅକ୍ଷର *allochost = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  ଅଚଳ ଅକ୍ଷର *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  ଅକ୍ଷର portbuf[7];
  ଯଦି(curl_strequal("file", u->scheme))
    ଫେରନ୍ତୁ file_url(u, part, fragmentsep, querysep);
  ଅନ୍ୟଥା ଯଦି(!u->host)
    ଫେରନ୍ତୁ CURLUE_NO_HOST;
  ଅନ୍ୟଥା {
    ଅଚଳ ଅକ୍ଷର *scheme;
    ଅକ୍ଷର *options = u->options;
    ଅକ୍ଷର *port = ଖାଲି;
    ଅଚଳ ସଂରଚନା Curl_scheme *h = ଖାଲି;
    ଅକ୍ଷର schemebuf[MAX_SCHEME_LEN + 5];
    ଯଦି(u->scheme)
      scheme = u->scheme;
    ଅନ୍ୟଥା ଯଦି(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLUE_NO_SCHEME;

    ଯଦି(u->port_present) {
      curl_msnprintf(portbuf, ଆକାର(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    ଯଦି(h) {
      ଯଦି(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, ଆକାର(portbuf), "%u", h->defport);
        port = portbuf;
      }
      ଅନ୍ୟଥା ଯଦି(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = ଖାଲି;
      }

      ଯଦି(!(h->flags & PROTOPT_URLOPTIONS))
        options = ଖାଲି;
    }

    ଯଦି(u->host[0] == '[') {
      ଯଦି(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ସଂରଚନା dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_ନିବେଶ_LENGTH);
        ଯଦି(curlx_dyn_addf(&enc, "%.*s%%25%s]", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)hostlen - 1, u->host,
                          u->zoneid))
          ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    ଅନ୍ୟଥା ଯଦି(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ଖାଲି, u->host, 0);
      ଯଦି(!allochost)
        ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
    }
    ଅନ୍ୟଥା ଯଦି(flags & CURLU_PUNYCODE) {
      ଯଦି(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        ଯଦି(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        ଯଦି(ret) {
          curlx_safefree(allochost);
          ଫେରନ୍ତୁ ret;
        }
      }
    }
    ଅନ୍ୟଥା ଯଦି(flags & CURLU_PUNY2IDN) {
      ଯଦି(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        ଯଦି(ret)
          ଫେରନ୍ତୁ ret;
      }
    }

    ଯଦି(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, ଆକାର(schemebuf), "%s://", scheme);
    ଅନ୍ୟଥା
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
                        port ? ":" : "",
                        port ? port : "",
                        u->path ? u->path : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  ଯଦି(!url)
    ଫେରନ୍ତୁ CURLUE_OUT_OF_MEMORY;
  *part = url;
  ଫେରନ୍ତୁ CURLUE_OK;
}

CURLUcode curl_url_get(ଅଚଳ CURLU *u, CURLUPart what,
                       ଅକ୍ଷର **part, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ଅଚଳ ଅକ୍ଷର *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  ଅକ୍ଷର portbuf[7];
  bool plusdecode = FALSE;
  ଯଦି(!u)
    ଫେରନ୍ତୁ CURLUE_BAD_HANDLE;
  ଯଦି(!part)
    ଫେରନ୍ତୁ CURLUE_BAD_PARTPOINTER;
  *part = ଖାଲି;

  ବାଛ(what) {
  ସ୍ଥିତି CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    ଯଦି((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      ଫେରନ୍ତୁ CURLUE_NO_SCHEME;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PORT:
    ptr = ଖାଲି;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    ଯଦି(u->port_present) {
      ଅଚଳ ସଂରଚନା Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ଖାଲି;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      ଯଦି(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = ଖାଲି;
      }
      ଅନ୍ୟଥା {
        curl_msnprintf(portbuf, ଆକାର(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    ଅନ୍ୟଥା ଯଦି((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ଅଚଳ ସଂରଚନା Curl_scheme *h = Curl_get_scheme(u->scheme);
      ଯଦି(h) {
        curl_msnprintf(portbuf, ଆକାର(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PATH:
    ptr = u->path;
    ଯଦି(!ptr)
      ptr = "/";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    ଯଦି(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = ଖାଲି;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    ଯଦି(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_URL:
    ଫେରନ୍ତୁ urlget_url(u, part, flags);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ptr = ଖାଲି;
    ଭାଙ୍ଗ;
  }
  ଯଦି(ptr)
    ଫେରନ୍ତୁ urlget_format(u, what, ptr, part, plusdecode, flags);

  ଫେରନ୍ତୁ ifmissing;
}

ସ୍ଥିର CURLUcode set_url_scheme(CURLU *u, ଅଚଳ ଅକ୍ଷର *scheme,
                                unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  size_t plen = strlen(scheme);
  ଅଚଳ ସଂରଚନା Curl_scheme *h = ଖାଲି;
  ଯଦି((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    ଫେରନ୍ତୁ CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  ଯଦି(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    ଫେରନ୍ତୁ CURLUE_UNSUPPORTED_SCHEME;
  ଯଦି(!h) {
    ଅଚଳ ଅକ୍ଷର *s = scheme;
    ଯଦି(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      ଯେତେବେଳେ(--plen) {
        ଯଦି(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        ଅନ୍ୟଥା
          ଫେରନ୍ତୁ CURLUE_BAD_SCHEME;
      }
    }
    ଅନ୍ୟଥା
      ଫେରନ୍ତୁ CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode set_url_port(CURLU *u, ଅଚଳ ଅକ୍ଷର *provided_port)
{
  curl_off_t port;
  ଯଦି(!ISDIGIT(provided_port[0]))
    /* not a number */
    ଫେରନ୍ତୁ CURLUE_BAD_PORT_ସଂଖ୍ୟା;
  ଯଦି(curlx_str_ସଂଖ୍ୟା(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    ଫେରନ୍ତୁ CURLUE_BAD_PORT_ସଂଖ୍ୟା;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode set_url(CURLU *u, ଅଚଳ ଅକ୍ଷର *url, size_t part_size,
                         unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  ଅକ୍ଷର *oldurl = ଖାଲି;

  ଯଦି(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    ଯଦି(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      ଫେରନ୍ତୁ CURLUE_OK;
    }
    ଯଦି(uc == CURLUE_OUT_OF_MEMORY)
      ଫେରନ୍ତୁ uc;
    ଫେରନ୍ତୁ CURLUE_MALFORMED_ନିବେଶ;
  }

  /* if the new URL is absolute replace the existing with the new. */
  ଯଦି(Curl_is_absolute_url(url, ଖାଲି, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    ଫେରନ୍ତୁ parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  ଯଦି(uc == CURLUE_OUT_OF_MEMORY)
    ଫେରନ୍ତୁ uc;
  ଅନ୍ୟଥା ଯଦି(uc)
    ଫେରନ୍ତୁ parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  ଫେରନ୍ତୁ uc;
}

ସ୍ଥିର CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  ବାଛ(what) {
  ସ୍ଥିତି CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, ଆକାର(ସଂରଚନା Curl_URL));
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_USER:
    curlx_safefree(u->user);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_HOST:
    curlx_safefree(u->host);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PATH:
    curlx_safefree(u->path);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLUE_UNKNOWN_PART;
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର bool allowed_in_path(unsigned ଅକ୍ଷର x)
{
  ବାଛ(x) {
  ସ୍ଥିତି '!':
  ସ୍ଥିତି '$':
  ସ୍ଥିତି '&':
  ସ୍ଥିତି '\'':
  ସ୍ଥିତି '(':
  ସ୍ଥିତି ')':
  ସ୍ଥିତି '{':
  ସ୍ଥିତି '}':
  ସ୍ଥିତି '[':
  ସ୍ଥିତି ']':
  ସ୍ଥିତି '*':
  ସ୍ଥିତି '+':
  ସ୍ଥିତି ',':
  ସ୍ଥିତି ';':
  ସ୍ଥିତି '=':
  ସ୍ଥିତି ':':
  ସ୍ଥିତି '@':
  ସ୍ଥିତି '/':
    ଫେରନ୍ତୁ TRUE;
  }
  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLUcode url_encode_part(ସଂରଚନା dynbuf *encp,
                                 ଅଚଳ ଅକ୍ଷର *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ଅଚଳ unsigned ଅକ୍ଷର *i;

  ପାଇଁ(i = (ଅଚଳ unsigned ଅକ୍ଷର *)part; *i; i++) {
    CURLcode result;
    ଯଦି((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    ଅନ୍ୟଥା ଯଦି(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      ଯଦି((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    ଅନ୍ୟଥା {
      unsigned ଅକ୍ଷର out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    ଯଦି(result)
      ଫେରନ୍ତୁ cc2cu(result);
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode url_uppercasehex_part(ସଂରଚନା dynbuf *encp,
                                       ଅଚଳ ଅକ୍ଷର *part)
{
  ଅକ୍ଷର *p;
  CURLcode result = curlx_dyn_add(encp, part);
  ଯଦି(result)
    ଫେରନ୍ତୁ cc2cu(result);
  p = curlx_dyn_ptr(encp);
  ଯେତେବେଳେ(*p) {
    /* make sure percent encoded are upper case */
    ଯଦି((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    ଅନ୍ୟଥା
      p++;
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode url_append_query(CURLU *u, ସଂରଚନା dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  ଯଦି(querylen) {
    ସଂରଚନା dynbuf qbuf;
    CURLcode result;
    ଅଚଳ ଅକ୍ଷର *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_ନିବେଶ_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    ଯଦି(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    ଯଦି(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    ଯଦି(result)
      ଯାଅ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    ଫେରନ୍ତୁ CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    ଫେରନ୍ତୁ cc2cu(result);
  }
  ଅନ୍ୟଥା {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

ସ୍ଥିର CURLUcode url_sethost(CURLU *u, ସଂରଚନା dynbuf *encp,
                             bool urlencode,
                             unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  ଅକ୍ଷର *newp = curlx_dyn_ptr(encp);
  ଯଦି(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  ଅନ୍ୟଥା ଯଦି(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    ଅକ୍ଷର *decoded = ଖାଲି;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    ଯଦି(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    ଅନ୍ୟଥା {
      ସଂରଚନା dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_ନିବେଶ_LENGTH);
      ଯଦି(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      ଅନ୍ୟଥା ଯଦି(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        ଯଦି(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  ଅନ୍ୟଥା ଯଦି(hostname_check6(u, newp, n))
    bad = TRUE;
  ଅନ୍ୟଥା ଯଦି(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  ଯଦି(bad) {
    curlx_dyn_free(encp);
    ଫେରନ୍ତୁ CURLUE_BAD_HOSTNAME;
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ଅଚଳ ଅକ୍ଷର *part, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
  ଅକ୍ଷର **storep = ଖାଲି;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  ଯଦି(!u)
    ଫେରନ୍ତୁ CURLUE_BAD_HANDLE;
  ଯଦି(!part)
    /* setting a part to NULL clears it */
    ଫେରନ୍ତୁ urlset_clear(u, what);

  nalloc = strlen(part);
  ଯଦି(nalloc > CURL_MAX_ନିବେଶ_LENGTH)
    /* excessive input length */
    ଫେରନ୍ତୁ CURLUE_MALFORMED_ନିବେଶ;

  ବାଛ(what) {
  ସ୍ଥିତି CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    ଯଦି(status)
      ଫେରନ୍ତୁ status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    ଭାଙ୍ଗ;
  }
  ସ୍ଥିତି CURLUPART_USER:
    storep = &u->user;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PASSWORD:
    storep = &u->password;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_OPTIONS:
    storep = &u->options;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_ZONEID:
    storep = &u->zoneid;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_PORT:
    ଫେରନ୍ତୁ set_url_port(u, part);
  ସ୍ଥିତି CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLUPART_URL:
    ଫେରନ୍ତୁ set_url(u, part, nalloc, flags);
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ଅଚଳ ଅକ୍ଷର *newp = ଖାଲି;
    ସଂରଚନା dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    ଯଦି(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      ଯଦି(result)
        ଫେରନ୍ତୁ cc2cu(result);
    }
    ଯଦି(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    ଅନ୍ୟଥା
      status = url_uppercasehex_part(&enc, part);
    ଯଦି(!status) {
      newp = curlx_dyn_ptr(&enc);

      ଯଦି(appendquery && newp)
        ଫେରନ୍ତୁ url_append_query(u, &enc);
      ଅନ୍ୟଥା ଯଦି(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    ଯଦି(status)
      ଫେରନ୍ତୁ status;

    ଯଦି(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (ଅକ୍ଷର *)CURL_UNCONST(newp);
  }
  ଫେରନ୍ତୁ CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ଅଚଳ ସଂରଚନା Curl_scheme *s = ଖାଲି;

  /* base must be an absolute URL */
  ଯଦି(!base->scheme || !base->host)
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(href->scheme && !curl_strequal(base->scheme, href->scheme))
    ଫେରନ୍ତୁ FALSE;
  ଯଦି(href->host) {
    ଯଦି(!curl_strequal(base->host, href->host))
      ଫେରନ୍ତୁ FALSE;

    ଯଦି(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      ଯଦି(!s) /* Cannot match default port for unknown scheme */
        ଫେରନ୍ତୁ FALSE;
      /* to match, the present one must be the default port */
      ଯଦି((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        ଫେରନ୍ତୁ FALSE;
    }
    ଅନ୍ୟଥା ଯଦି(base->portnum != href->portnum) /* both present or missing */
      ଫେରନ୍ତୁ FALSE;

    ଯଦି(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      ଫେରନ୍ତୁ FALSE;
  }
  ଅନ୍ୟଥା ଯଦି(href->port_present) /* no host in href, then there must be no port */
    ଫେରନ୍ତୁ FALSE;
  ଫେରନ୍ତୁ TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  ଯଦି(u->port_present) {
    *pport = u->portnum;
    ଫେରନ୍ତୁ CURLUE_OK;
  }
  ଅନ୍ୟଥା ଯଦି(u->scheme) {
    ଅଚଳ ସଂରଚନା Curl_scheme *s = Curl_get_scheme(u->scheme);
    ଯଦି(s && s->defport) {
      *pport = s->defport;
      ଫେରନ୍ତୁ CURLUE_OK;
    }
  }
  *pport = 0;
  ଫେରନ୍ତୁ CURLUE_NO_PORT;
}
