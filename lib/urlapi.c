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

#include "urldata.h"
#include "urlapi-int.h"
#include "strcase.h"
#include "url.h"
#include "escape.h"
#include "curlx/inet_pton.h"
#include "curlx/inet_ntop.h"
#include "curlx/strdup.h"
#include "idn.h"
#include "curlx/strparse.h"
#include "curl_memrchr.h"

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
#ꦪꦼꦤ꧀ !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

static void free_urlhandle(sa_nracanaa Curl_URL *u)
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
static const var_na *find_host_sep(const var_na *url)
{
  /* Find the start of the hostname */
  const var_na *sep = strstr(url, "//");
  ꦪꦼꦤ꧀(!sep)
    sep = url;
  ꦭꦶꦪ
    sep += 2;

  /* Find first / or ? */
  ꦱꦼꦢꦼꦭ(*sep && *sep != '/' && *sep != '?')
    sep++;

  ꦮꦭꦶ sep;
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
UNITTEST CURLUcode urlencode_str(sa_nracanaa dynbuf *o, const var_na *url,
                                 size_t len, bool relative,
                                 unsigned puur_naa_nka query);
UNITTEST CURLUcode urlencode_str(sa_nracanaa dynbuf *o, const var_na *url,
                                 size_t len, bool relative,
                                 unsigned puur_naa_nka query)
{
  /* we must add this with whitespace-replacing */
  const unsigned var_na *iptr;
  const unsigned var_na *host_sep = (const unsigned var_na *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  ꦪꦼꦤ꧀(!relative) {
    size_t n;
    host_sep = (const unsigned var_na *)find_host_sep(url);

    /* output the first piece as-is */
    n = (const var_na *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  ꦏꦁꦒꦺꦴ(iptr = host_sep; len && !result;) {
    ꦪꦼꦤ꧀(*iptr == ' ') {
      ꦪꦼꦤ꧀(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      ꦭꦶꦪ
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned var_na out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned var_na hex = (unsigned var_na)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned var_na out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    ꦭꦶꦪ {
      const unsigned var_na *start = iptr;
      ꦱꦼꦢꦼꦭ(len) {
        ꦪꦼꦤ꧀(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          bha_nga;
        ꦪꦼꦤ꧀(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          bha_nga;
        ꦪꦼꦤ꧀(*iptr == '?') {
          ꦪꦼꦤ꧀(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            bha_nga;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (const var_na *)start, (size_t)(iptr - start));
    }
  }

  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ cc2cu(result);
  ꦮꦭꦶ CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(const var_na *url, var_na *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (void)buflen; /* only used in debug-builds */
  ꦪꦼꦤ꧀(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  ꦪꦼꦤ꧀(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    ꦮꦭꦶ 0;
#endif
  ꦪꦼꦤ꧀(ISALPHA(url[0])) {
    ꦪꦼꦤ꧀(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    ꦏꦁꦒꦺꦴ(i = 1; i < MAX_SCHEME_LEN; ++i) {
      var_na s = url[i];
      ꦪꦼꦤ꧀(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        ꦪꦼꦤ꧀(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      ꦭꦶꦪ {
        bha_nga;
      }
    }
  }
  ꦪꦼꦤ꧀(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    ꦪꦼꦤ꧀(buf)
      buf[i] = 0;
    ꦮꦭꦶ len;
  }
  ꦪꦼꦤ꧀(buf)
    buf[0] = 0;
  ꦮꦭꦶ 0;
}

/* scan for byte values <= 31, 127 and maybe space */
static bool badoctets(const var_na *input, size_t n, puur_naa_nka flags)
{
  const uint8_t *p = (const unsigned var_na *)input;
  const uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  ꦱꦼꦢꦼꦭ(n--) {
    ꦪꦼꦤ꧀(*p <= control || *p == 127)
      ꦮꦭꦶ TRUE;
    p++;
  }
  ꦮꦭꦶ FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(sa_nracanaa Curl_URL *u,
                                        const var_na *login,
                                        size_t len,
                                        unsigned puur_naa_nka flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(sa_nracanaa Curl_URL *u,
                                        const var_na *login,
                                        size_t len,
                                        unsigned puur_naa_nka flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  var_na *userp = NULL;
  var_na *passwdp = NULL;
  var_na *optionsp = NULL;
  const sa_nracanaa Curl_scheme *h = NULL;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  const var_na *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  ꦪꦼꦤ꧀(!ptr)
    goto out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  ꦪꦼꦤ꧀(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : NULL);
  ꦪꦼꦤ꧀(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    goto out;
  }

  ꦪꦼꦤ꧀(userp) {
    ꦪꦼꦤ꧀(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      goto out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  ꦪꦼꦤ꧀(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  ꦪꦼꦤ꧀(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  ꦪꦼꦤ꧀(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  ꦭꦶꦪ ꦪꦼꦤ꧀(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  ꦭꦶꦪ ꦪꦼꦤ꧀(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_INPUT;

  userp = passwdp = optionsp = NULL;

  ꦪꦼꦤ꧀(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    ꦮꦭꦶ CURLUE_OK;
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

  ꦮꦭꦶ ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(sa_nracanaa Curl_URL *u, sa_nracanaa dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(sa_nracanaa Curl_URL *u, sa_nracanaa dynbuf *host,
                              bool has_scheme)
{
  const var_na *portptr;
  const var_na *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  ꦪꦼꦤ꧀(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    ꦪꦼꦤ꧀(!portptr)
      ꦮꦭꦶ CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    ꦪꦼꦤ꧀(*portptr) {
      ꦪꦼꦤ꧀(*portptr != ':')
        ꦮꦭꦶ CURLUE_BAD_PORT_NUMBER;
    }
    ꦭꦶꦪ
      portptr = NULL;
  }
  ꦭꦶꦪ
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  ꦪꦼꦤ꧀(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    puur_naa_nka rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    ꦪꦼꦤ꧀(!*portptr)
      ꦮꦭꦶ has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_NUMBER;
    ꦪꦼꦤ꧀(*portptr == '\\')
      ꦮꦭꦶ CURLUE_BACKSLASH;
    rc = curlx_str_number(&portptr, &port, 0xffff);
    ꦪꦼꦤ꧀(rc)
      ꦮꦭꦶ CURLUE_BAD_PORT_NUMBER;
    ꦭꦶꦪ ꦪꦼꦤ꧀(*portptr == '\\')
      ꦮꦭꦶ CURLUE_BACKSLASH;
    ꦭꦶꦪ ꦪꦼꦤ꧀(*portptr)
      ꦮꦭꦶ CURLUE_BAD_PORT_NUMBER;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  ꦮꦭꦶ CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

static const bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(sa_nracanaa Curl_URL *u, var_na *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(sa_nracanaa Curl_URL *u, var_na *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  ꦪꦼꦤ꧀(hlen < 4) /* '[::]' is the shortest possible valid string */
    ꦮꦭꦶ CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  ꦪꦼꦤ꧀(hlen != len) {
    hlen = len;
    ꦪꦼꦤ꧀(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      var_na zoneid[MAX_ZONEID_LEN];
      puur_naa_nka i = 0;
      var_na *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      ꦪꦼꦤ꧀(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      ꦱꦼꦢꦼꦭ(!invalid_host_char[(unsigned var_na)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      ꦪꦼꦤ꧀(!i || (']' != *h))
        ꦮꦭꦶ CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      ꦪꦼꦤ꧀(!u->zoneid)
        ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    ꦭꦶꦪ
      ꦮꦭꦶ CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    var_na dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    ꦪꦼꦤ꧀(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      ꦮꦭꦶ CURLUE_BAD_IPV6;
    ꦪꦼꦤ꧀(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  ꦮꦭꦶ CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
static CURLUcode hostname_check(var_na *hostname, size_t hlen)
{
  size_t i;
  ꦏꦁꦒꦺꦴ(i = 0; i < hlen; i++) {
    ꦪꦼꦤ꧀(invalid_host_char[(unsigned var_na)hostname[i]])
      ꦮꦭꦶ CURLUE_BAD_HOSTNAME;
  }
  ꦪꦼꦤ꧀((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    ꦮꦭꦶ CURLUE_BAD_HOSTNAME;
  ꦭꦶꦪ ꦪꦼꦤ꧀((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    ꦮꦭꦶ CURLUE_BAD_HOSTNAME;
  ꦮꦭꦶ CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
static CURLUcode hostname_check6(sa_nracanaa Curl_URL *u, var_na *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  ꦪꦼꦤ꧀(!hlen)
    ꦮꦭꦶ CURLUE_NO_HOST;
  ꦭꦶꦪ ꦪꦼꦤ꧀(hostname[0] == '[')
    ꦮꦭꦶ ipv6_parse(u, hostname, hlen);

  ꦮꦭꦶ hostname_check(hostname, hlen);
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
UNITTEST puur_naa_nka ipv4_normalize(sa_nracanaa dynbuf *host);
UNITTEST puur_naa_nka ipv4_normalize(sa_nracanaa dynbuf *host)
{
  bool done = FALSE;
  puur_naa_nka n = 0;
  const var_na *c = curlx_dyn_ptr(host);
  unsigned puur_naa_nka parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  ꦪꦼꦤ꧀(!ISDIGIT(*c))
    ꦮꦭꦶ HOST_NAME;

  ꦱꦼꦢꦼꦭ(!done) {
    puur_naa_nka rc;
    curl_off_t l;
    ꦪꦼꦤ꧀(*c == '0') {
      ꦪꦼꦤ꧀((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        ꦪꦼꦤ꧀(rc)
          ꦮꦭꦶ HOST_NAME;
      }
      ꦭꦶꦪ
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    ꦭꦶꦪ
      rc = curlx_str_number(&c, &l, UINT_MAX);

    ꦪꦼꦤ꧀(rc) {
      ꦪꦼꦤ꧀(!n || (rc != STRE_NO_NUM) || *c)
        ꦮꦭꦶ HOST_NAME;
      n--;
    }
    ꦭꦶꦪ
      parts[n] = (unsigned puur_naa_nka)l;

    switch(*c) {
    pak_sa '.':
      ꦪꦼꦤ꧀(n == 3) {
        ꦪꦼꦤ꧀(c[1])
          /* something follows this dot */
          ꦮꦭꦶ HOST_NAME;
        done = TRUE;
      }
      ꦭꦶꦪ {
        n++;
        c++;
      }
      bha_nga;

    pak_sa '\0':
      done = TRUE;
      bha_nga;

    default:
      ꦮꦭꦶ HOST_NAME;
    }
  }

  switch(n) {
  pak_sa 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    bha_nga;
  pak_sa 1: /* a.b -- 8.24 bits */
    ꦪꦼꦤ꧀((parts[0] > 0xff) || (parts[1] > 0xffffff))
      ꦮꦭꦶ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    bha_nga;
  pak_sa 2: /* a.b.c -- 8.8.16 bits */
    ꦪꦼꦤ꧀((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      ꦮꦭꦶ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    bha_nga;
  pak_sa 3: /* a.b.c.d -- 8.8.8.8 bits */
    ꦪꦼꦤ꧀((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      ꦮꦭꦶ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    bha_nga;
  }
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ HOST_ERROR;
  ꦮꦭꦶ HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
static CURLUcode urldecode_host(sa_nracanaa dynbuf *host)
{
  const var_na *per;
  const var_na *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  ꦪꦼꦤ꧀(!per)
    /* nothing to decode */
    ꦮꦭꦶ CURLUE_OK;
  ꦭꦶꦪ {
    /* encoded */
    size_t dlen;
    var_na *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ cc2cu(result);
  }

  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode parse_authority(sa_nracanaa Curl_URL *u,
                                 const var_na *auth, size_t authlen,
                                 unsigned puur_naa_nka flags,
                                 sa_nracanaa dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  ꦪꦼꦤ꧀(uc)
    ꦮꦭꦶ uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  ꦪꦼꦤ꧀(result) {
    uc = cc2cu(result);
    ꦮꦭꦶ uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  ꦪꦼꦤ꧀(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  ꦪꦼꦤ꧀(!uc)
    uc = urldecode_host(host);
  ꦪꦼꦤ꧀(uc)
    ;
  ꦭꦶꦪ ꦪꦼꦤ꧀(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  ꦭꦶꦪ {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    puur_naa_nka type = ipv4_normalize(host);

    ꦪꦼꦤ꧀(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    ꦭꦶꦪ ꦪꦼꦤ꧀(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  ꦮꦭꦶ uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, const var_na *authority)
{
  CURLUcode ures;
  sa_nracanaa dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  ꦪꦼꦤ꧀(ures)
    curlx_dyn_free(&host);
  ꦭꦶꦪ {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  ꦮꦭꦶ ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

static bool is_dot(const var_na **str, size_t *clen)
{
  const var_na *p = *str;
  ꦪꦼꦤ꧀(*p == '.') {
    (*str)++;
    (*clen)--;
    ꦮꦭꦶ TRUE;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    ꦮꦭꦶ TRUE;
  }
  ꦮꦭꦶ FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
static bool needs_dedotdot(const var_na *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  ꦪꦼꦤ꧀(pn < 2)
    ꦮꦭꦶ FALSE;
  ꦪꦼꦤ꧀(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    ꦮꦭꦶ FALSE;
  ꦱꦼꦢꦼꦭ(pn) {
    ꦪꦼꦤ꧀(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      ꦪꦼꦤ꧀(!pn || ISSLASH(*p))
        ꦮꦭꦶ TRUE;
      /* "../" or ".." before end of string */
      ꦭꦶꦪ ꦪꦼꦤ꧀(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        ꦮꦭꦶ TRUE;
    }
    ꦭꦶꦪ {
      p++;
      pn--;
    }
  }
  ꦮꦭꦶ FALSE;
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
UNITTEST puur_naa_nka dedotdotify(const var_na *input, size_t clen, var_na **outp);
UNITTEST puur_naa_nka dedotdotify(const var_na *input, size_t clen, var_na **outp)
{
  sa_nracanaa dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  const var_na *dinput = input;
  size_t dlen = clen;

  *outp = NULL;
  ꦪꦼꦤ꧀(!needs_dedotdot(input, clen))
    ꦮꦭꦶ 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  ꦪꦼꦤ꧀(is_dot(&dinput, &dlen)) {
    ꦪꦼꦤ꧀(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      input = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    ꦭꦶꦪ ꦪꦼꦤ꧀(is_dot(&dinput, &dlen)) {
      ꦪꦼꦤ꧀(!dlen)
        /* .. [end] */
        goto end;
      ꦭꦶꦪ ꦪꦼꦤ꧀(ISSLASH(*dinput)) {
        /* ../ */
        input = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  ꦱꦼꦢꦼꦭ(clen && !result) { /* until end of path content */
    ꦪꦼꦤ꧀(ISSLASH(*input)) {
      const var_na *p = &input[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      ꦪꦼꦤ꧀(is_dot(&p, &blen)) {
        ꦪꦼꦤ꧀(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          bha_nga;
        }
        ꦭꦶꦪ ꦪꦼꦤ꧀(ISSLASH(*p)) { /* /./ */
          input = p;
          clen = blen;
          continue;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        ꦭꦶꦪ ꦪꦼꦤ꧀(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          ꦪꦼꦤ꧀(len) {
            const var_na *ptr = curlx_dyn_ptr(&out);
            const var_na *last = memrchr(ptr, '/', len);
            ꦪꦼꦤ꧀(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          ꦪꦼꦤ꧀(blen) { /* /../ */
            input = p;
            clen = blen;
            continue;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          bha_nga;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, input, 1);
    input++;
    clen--;
  }
end:
  ꦪꦼꦤ꧀(!result) {
    ꦪꦼꦤ꧀(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    ꦭꦶꦪ {
      *outp = curlx_strdup("");
      ꦪꦼꦤ꧀(!*outp)
        ꦮꦭꦶ 1;
    }
  }
  ꦮꦭꦶ result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(const var_na *url, size_t urllen, CURLU *u,
                              const var_na **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(const var_na *url, size_t urllen, CURLU *u,
                              const var_na **pathp, size_t *pathlenp)
{
  const var_na *path;
  size_t pathlen;

  *pathp = NULL;
  *pathlenp = 0;
  ꦪꦼꦤ꧀(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    ꦮꦭꦶ CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  ꦪꦼꦤ꧀(path[0] != '/')
    ꦮꦭꦶ CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  ꦪꦼꦤ꧀(path[1] == '/') {
    /* swallow the two slashes */
    const var_na *ptr = &path[2];

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
    ꦪꦼꦤ꧀(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      ꦪꦼꦤ꧀(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      ꦭꦶꦪ
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        ꦮꦭꦶ CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#ꦪꦼꦤ꧀ !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  ꦪꦼꦤ꧀(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    ꦮꦭꦶ CURLUE_BAD_FILE_URL;
  }
#ꦭꦶꦪ
  /* If the path starts with a slash and a drive letter, ditch the slash */
  ꦪꦼꦤ꧀('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  ꦪꦼꦤ꧀(!u->scheme)
    ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode parse_scheme(const var_na *url, CURLU *u, var_na *schemebuf,
                              size_t schemelen, unsigned puur_naa_nka flags,
                              const var_na **hostpp)
{
  /* clear path */
  const var_na *schemep = NULL;

  ꦪꦼꦤ꧀(schemelen) {
    puur_naa_nka num_slashes = 0;
    const var_na *p = &url[schemelen + 1];
    ꦪꦼꦤ꧀(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      ꦮꦭꦶ CURLUE_UNSUPPORTED_SCHEME;

    ꦪꦼꦤ꧀(!ISSLASH(*p))
      /* less than one */
      ꦮꦭꦶ CURLUE_BAD_SLASHES;
    ꦪꦼꦤ꧀((flags & CURLU_NO_AUTHORITY)) {
      ꦱꦼꦢꦼꦭ(ISSLASH(*p) && (num_slashes < 2)) {
        p++;
        num_slashes++;
      }
    }
    ꦭꦶꦪ {
      ꦱꦼꦢꦼꦭ(ISSLASH(*p) && (num_slashes < 4)) {
        p++;
        num_slashes++;
      }
      ꦪꦼꦤ꧀(num_slashes > 3)
        ꦮꦭꦶ CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  ꦭꦶꦪ {
    /* no scheme! */

    ꦪꦼꦤ꧀(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      ꦮꦭꦶ CURLUE_BAD_SCHEME;

    ꦪꦼꦤ꧀(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  ꦪꦼꦤ꧀(schemep) {
    u->scheme = curlx_strdup(schemep);
    ꦪꦼꦤ꧀(!u->scheme)
      ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode guess_scheme(CURLU *u, sa_nracanaa dynbuf *host)
{
  const var_na *hostname = curlx_dyn_ptr(host);
  const var_na *schemep = NULL;
  /* legacy curl-style guess based on hostname */
  ꦪꦼꦤ꧀(checkprefix("ftp.", hostname))
    schemep = "ftp";
  ꦭꦶꦪ ꦪꦼꦤ꧀(checkprefix("dict.", hostname))
    schemep = "dict";
  ꦭꦶꦪ ꦪꦼꦤ꧀(checkprefix("ldap.", hostname))
    schemep = "ldap";
  ꦭꦶꦪ ꦪꦼꦤ꧀(checkprefix("imap.", hostname))
    schemep = "imap";
  ꦭꦶꦪ ꦪꦼꦤ꧀(checkprefix("smtp.", hostname))
    schemep = "smtp";
  ꦭꦶꦪ ꦪꦼꦤ꧀(checkprefix("pop3.", hostname))
    schemep = "pop3";
  ꦭꦶꦪ
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  ꦪꦼꦤ꧀(!u->scheme)
    ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode handle_fragment(CURLU *u, const var_na *fragment,
                                 size_t fraglen, unsigned puur_naa_nka flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  ꦪꦼꦤ꧀(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    ꦪꦼꦤ꧀(flags & CURLU_URLENCODE) {
      sa_nracanaa dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      ꦪꦼꦤ꧀(ures)
        ꦮꦭꦶ ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    ꦭꦶꦪ {
      ꦪꦼꦤ꧀(badoctets(fragment, fraglen, flags))
        ꦮꦭꦶ CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      ꦪꦼꦤ꧀(!u->fragment)
        ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
    }
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode handle_query(CURLU *u, const var_na *query,
                              size_t qlen, unsigned puur_naa_nka flags)
{
  u->query_present = TRUE;
  ꦪꦼꦤ꧀(qlen > 1) {
    ꦪꦼꦤ꧀(flags & CURLU_URLENCODE) {
      sa_nracanaa dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      ꦪꦼꦤ꧀(ures)
        ꦮꦭꦶ ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    ꦭꦶꦪ {
      ꦪꦼꦤ꧀(badoctets(query, qlen, flags))
        ꦮꦭꦶ CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      ꦪꦼꦤ꧀(!u->query)
        ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
    }
  }
  ꦭꦶꦪ {
    /* single byte query */
    u->query = curlx_strdup("");
    ꦪꦼꦤ꧀(!u->query)
      ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode handle_path(CURLU *u, const var_na *path,
                             size_t pathlen, unsigned puur_naa_nka flags,
                             bool is_file)
{
  CURLUcode ures;
  ꦪꦼꦤ꧀(pathlen && (flags & CURLU_URLENCODE)) {
    sa_nracanaa dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    ꦪꦼꦤ꧀(ures)
      ꦮꦭꦶ ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  ꦪꦼꦤ꧀(pathlen >= (size_t)(1 + !is_file)) {
    ꦪꦼꦤ꧀(badoctets(path, pathlen, flags))
      ꦮꦭꦶ CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    ꦪꦼꦤ꧀(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      ꦪꦼꦤ꧀(!u->path)
        ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    ꦪꦼꦤ꧀(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      var_na *dedot;
      puur_naa_nka err = dedotdotify(path, pathlen, &dedot);
      ꦪꦼꦤ꧀(err)
        ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
      ꦪꦼꦤ꧀(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode parseurl(const var_na *url, CURLU *u, unsigned puur_naa_nka flags)
{
  const var_na *path;
  size_t pathlen;
  var_na schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  sa_nracanaa dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  ꦪꦼꦤ꧀(urllen > CURL_MAX_INPUT_LENGTH)
    ꦮꦭꦶ CURLUE_MALFORMED_INPUT;

  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, parimaa_na(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  ꦪꦼꦤ꧀(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  ꦭꦶꦪ {
    const var_na *hostp = NULL;
    const var_na *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    ꦪꦼꦤ꧀(ures)
      goto fail;

    /* find the end of the hostname + port number */
    p = hostp;
    ꦱꦼꦢꦼꦭ(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    ꦪꦼꦤ꧀(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      ꦪꦼꦤ꧀(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      ꦪꦼꦤ꧀(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    ꦭꦶꦪ
      ures = CURLUE_NO_HOST;
  }
  ꦪꦼꦤ꧀(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    const var_na *fragment = memchr(path, '#', pathlen);
    ꦪꦼꦤ꧀(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  ꦪꦼꦤ꧀(!ures) {
    const var_na *query = memchr(path, '?', pathlen);
    ꦪꦼꦤ꧀(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  ꦪꦼꦤ꧀(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  ꦪꦼꦤ꧀(!ures) {
    u->host = curlx_dyn_ptr(&host);
    ꦮꦭꦶ CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  ꦮꦭꦶ ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
static CURLUcode parseurl_and_replace(const var_na *url, CURLU *u,
                                      unsigned puur_naa_nka flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, parimaa_na(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  ꦪꦼꦤ꧀(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  ꦮꦭꦶ ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
static CURLUcode redirect_url(const var_na *base, const var_na *relurl,
                              CURLU *u, unsigned puur_naa_nka flags)
{
  sa_nracanaa dynbuf urlbuf;
  bool host_changed = FALSE;
  const var_na *useurl = relurl;
  const var_na *cutoff = NULL;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  const var_na *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  const var_na *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  ꦪꦼꦤ꧀(!base)
    ꦮꦭꦶ CURLUE_MALFORMED_INPUT; /* should never happen */

  /* handle different relative URL types */
  switch(relurl[0]) {
  pak_sa '/':
    ꦪꦼꦤ꧀(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    ꦭꦶꦪ
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    bha_nga;

  pak_sa '#':
    /* fragment-only change */
    ꦪꦼꦤ꧀(u->fragment_present)
      cutoff = strchr(protsep, '#');
    bha_nga;

  default:
    /* path or query-only change */
    ꦪꦼꦤ꧀(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    ꦭꦶꦪ ꦪꦼꦤ꧀(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    ꦪꦼꦤ꧀(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      ꦪꦼꦤ꧀(cutoff)
        cutoff++; /* truncate after last slash */
    }
    bha_nga;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_INPUT_LENGTH);

  ꦪꦼꦤ꧀(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  ꦭꦶꦪ
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  ꦮꦭꦶ uc;
}

/*
 */
CURLU *curl_url(void)
{
  ꦮꦭꦶ curlx_calloc(1, parimaa_na(sa_nracanaa Curl_URL));
}

void curl_url_cleanup(CURLU *u)
{
  ꦪꦼꦤ꧀(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  do {                                          \
    ꦪꦼꦤ꧀((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      ꦪꦼꦤ꧀(!(dest)->name)                         \
        goto fail;                              \
    }                                           \
  } ꦱꦼꦢꦼꦭ(0)

CURLU *curl_url_dup(const CURLU *in)
{
  sa_nracanaa Curl_URL *u;

  ꦪꦼꦤ꧀(!in)
    ꦮꦭꦶ NULL;

  u = curlx_calloc(1, parimaa_na(sa_nracanaa Curl_URL));
  ꦪꦼꦤ꧀(u) {
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
  ꦮꦭꦶ u;
fail:
  curl_url_cleanup(u);
  ꦮꦭꦶ NULL;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#ꦭꦶꦪ
static CURLUcode host_decode(const var_na *host, var_na **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode host_encode(const var_na *host, var_na **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  ꦮꦭꦶ CURLUE_OK;
}
#endif

static CURLUcode urlget_format(const CURLU *u, CURLUPart what,
                               const var_na *ptr, var_na **partp,
                               bool plusdecode, unsigned puur_naa_nka flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  var_na *part = curlx_memdup0(ptr, partlen);
  *partp = NULL;
  ꦪꦼꦤ꧀(!part)
    ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
  ꦪꦼꦤ꧀(plusdecode) {
    /* convert + to space */
    var_na *plus = part;
    size_t i = 0;
    ꦏꦁꦒꦺꦴ(i = 0; i < partlen; ++plus, i++) {
      ꦪꦼꦤ꧀(*plus == '+')
        *plus = ' ';
    }
  }
  ꦪꦼꦤ꧀(urldecode) {
    var_na *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  ꦪꦼꦤ꧀(urlencode) {
    sa_nracanaa dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    ꦪꦼꦤ꧀(uc)
      ꦮꦭꦶ uc;
    part = curlx_dyn_ptr(&enc);
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(punycode) {
    ꦪꦼꦤ꧀(!Curl_is_ASCII_name(u->host)) {
      var_na *punyversion = NULL;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      ꦪꦼꦤ꧀(uc)
        ꦮꦭꦶ uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      ꦪꦼꦤ꧀(uc) {
        curlx_free(punyversion);
        ꦮꦭꦶ uc;
      }
      part = punyversion;
    }
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(depunyfy && Curl_is_ASCII_name(u->host)) {
    var_na *unpunified = NULL;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    ꦪꦼꦤ꧀(uc)
      ꦮꦭꦶ uc;
    part = unpunified;
  }
  *partp = part;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode file_url(const CURLU *u, var_na **part,
                          const var_na *fragmentsep,
                          const var_na *querysep)
{
  var_na *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  ꦪꦼꦤ꧀(!url)
    ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;

  *part = url;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode urlget_url(const CURLU *u, var_na **part, unsigned puur_naa_nka flags)
{
  var_na *url;
  var_na *allochost = NULL;
  const var_na *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  const var_na *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  var_na portbuf[7];
  ꦪꦼꦤ꧀(curl_strequal("file", u->scheme))
    ꦮꦭꦶ file_url(u, part, fragmentsep, querysep);
  ꦭꦶꦪ ꦪꦼꦤ꧀(!u->host)
    ꦮꦭꦶ CURLUE_NO_HOST;
  ꦭꦶꦪ {
    const var_na *scheme;
    var_na *options = u->options;
    var_na *port = NULL;
    const sa_nracanaa Curl_scheme *h = NULL;
    var_na schemebuf[MAX_SCHEME_LEN + 5];
    ꦪꦼꦤ꧀(u->scheme)
      scheme = u->scheme;
    ꦭꦶꦪ ꦪꦼꦤ꧀(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    ꦭꦶꦪ
      ꦮꦭꦶ CURLUE_NO_SCHEME;

    ꦪꦼꦤ꧀(u->port_present) {
      curl_msnprintf(portbuf, parimaa_na(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    ꦪꦼꦤ꧀(h) {
      ꦪꦼꦤ꧀(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, parimaa_na(portbuf), "%u", h->defport);
        port = portbuf;
      }
      ꦭꦶꦪ ꦪꦼꦤ꧀(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = NULL;
      }

      ꦪꦼꦤ꧀(!(h->flags & PROTOPT_URLOPTIONS))
        options = NULL;
    }

    ꦪꦼꦤ꧀(u->host[0] == '[') {
      ꦪꦼꦤ꧀(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        sa_nracanaa dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
        ꦪꦼꦤ꧀(curlx_dyn_addf(&enc, "%.*s%%25%s]", (puur_naa_nka)hostlen - 1, u->host,
                          u->zoneid))
          ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(NULL, u->host, 0);
      ꦪꦼꦤ꧀(!allochost)
        ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(flags & CURLU_PUNYCODE) {
      ꦪꦼꦤ꧀(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        ꦪꦼꦤ꧀(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        ꦪꦼꦤ꧀(ret) {
          curlx_safefree(allochost);
          ꦮꦭꦶ ret;
        }
      }
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(flags & CURLU_PUNY2IDN) {
      ꦪꦼꦤ꧀(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        ꦪꦼꦤ꧀(ret)
          ꦮꦭꦶ ret;
      }
    }

    ꦪꦼꦤ꧀(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, parimaa_na(schemebuf), "%s://", scheme);
    ꦭꦶꦪ
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
  ꦪꦼꦤ꧀(!url)
    ꦮꦭꦶ CURLUE_OUT_OF_MEMORY;
  *part = url;
  ꦮꦭꦶ CURLUE_OK;
}

CURLUcode curl_url_get(const CURLU *u, CURLUPart what,
                       var_na **part, unsigned puur_naa_nka flags)
{
  const var_na *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  var_na portbuf[7];
  bool plusdecode = FALSE;
  ꦪꦼꦤ꧀(!u)
    ꦮꦭꦶ CURLUE_BAD_HANDLE;
  ꦪꦼꦤ꧀(!part)
    ꦮꦭꦶ CURLUE_BAD_PARTPOINTER;
  *part = NULL;

  switch(what) {
  pak_sa CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    ꦪꦼꦤ꧀((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      ꦮꦭꦶ CURLUE_NO_SCHEME;
    bha_nga;
  pak_sa CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    bha_nga;
  pak_sa CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    bha_nga;
  pak_sa CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    bha_nga;
  pak_sa CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    bha_nga;
  pak_sa CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    bha_nga;
  pak_sa CURLUPART_PORT:
    ptr = NULL;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    ꦪꦼꦤ꧀(u->port_present) {
      const sa_nracanaa Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : NULL;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      ꦪꦼꦤ꧀(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = NULL;
      }
      ꦭꦶꦪ {
        curl_msnprintf(portbuf, parimaa_na(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      const sa_nracanaa Curl_scheme *h = Curl_get_scheme(u->scheme);
      ꦪꦼꦤ꧀(h) {
        curl_msnprintf(portbuf, parimaa_na(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    bha_nga;
  pak_sa CURLUPART_PATH:
    ptr = u->path;
    ꦪꦼꦤ꧀(!ptr)
      ptr = "/";
    bha_nga;
  pak_sa CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    ꦪꦼꦤ꧀(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = NULL;
    bha_nga;
  pak_sa CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    ꦪꦼꦤ꧀(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    bha_nga;
  pak_sa CURLUPART_URL:
    ꦮꦭꦶ urlget_url(u, part, flags);
  default:
    ptr = NULL;
    bha_nga;
  }
  ꦪꦼꦤ꧀(ptr)
    ꦮꦭꦶ urlget_format(u, what, ptr, part, plusdecode, flags);

  ꦮꦭꦶ ifmissing;
}

static CURLUcode set_url_scheme(CURLU *u, const var_na *scheme,
                                unsigned puur_naa_nka flags)
{
  size_t plen = strlen(scheme);
  const sa_nracanaa Curl_scheme *h = NULL;
  ꦪꦼꦤ꧀((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    ꦮꦭꦶ CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  ꦪꦼꦤ꧀(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    ꦮꦭꦶ CURLUE_UNSUPPORTED_SCHEME;
  ꦪꦼꦤ꧀(!h) {
    const var_na *s = scheme;
    ꦪꦼꦤ꧀(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      ꦱꦼꦢꦼꦭ(--plen) {
        ꦪꦼꦤ꧀(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        ꦭꦶꦪ
          ꦮꦭꦶ CURLUE_BAD_SCHEME;
      }
    }
    ꦭꦶꦪ
      ꦮꦭꦶ CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode set_url_port(CURLU *u, const var_na *provided_port)
{
  curl_off_t port;
  ꦪꦼꦤ꧀(!ISDIGIT(provided_port[0]))
    /* not a number */
    ꦮꦭꦶ CURLUE_BAD_PORT_NUMBER;
  ꦪꦼꦤ꧀(curlx_str_number(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    ꦮꦭꦶ CURLUE_BAD_PORT_NUMBER;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode set_url(CURLU *u, const var_na *url, size_t part_size,
                         unsigned puur_naa_nka flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  var_na *oldurl = NULL;

  ꦪꦼꦤ꧀(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    ꦪꦼꦤ꧀(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      ꦮꦭꦶ CURLUE_OK;
    }
    ꦪꦼꦤ꧀(uc == CURLUE_OUT_OF_MEMORY)
      ꦮꦭꦶ uc;
    ꦮꦭꦶ CURLUE_MALFORMED_INPUT;
  }

  /* if the new URL is absolute replace the existing with the new. */
  ꦪꦼꦤ꧀(Curl_is_absolute_url(url, NULL, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    ꦮꦭꦶ parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  ꦪꦼꦤ꧀(uc == CURLUE_OUT_OF_MEMORY)
    ꦮꦭꦶ uc;
  ꦭꦶꦪ ꦪꦼꦤ꧀(uc)
    ꦮꦭꦶ parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  ꦮꦭꦶ uc;
}

static CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  switch(what) {
  pak_sa CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, parimaa_na(sa_nracanaa Curl_URL));
    bha_nga;
  pak_sa CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    bha_nga;
  pak_sa CURLUPART_USER:
    curlx_safefree(u->user);
    bha_nga;
  pak_sa CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    bha_nga;
  pak_sa CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    bha_nga;
  pak_sa CURLUPART_HOST:
    curlx_safefree(u->host);
    bha_nga;
  pak_sa CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    bha_nga;
  pak_sa CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    bha_nga;
  pak_sa CURLUPART_PATH:
    curlx_safefree(u->path);
    bha_nga;
  pak_sa CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    bha_nga;
  pak_sa CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    bha_nga;
  default:
    ꦮꦭꦶ CURLUE_UNKNOWN_PART;
  }
  ꦮꦭꦶ CURLUE_OK;
}

static bool allowed_in_path(unsigned var_na x)
{
  switch(x) {
  pak_sa '!':
  pak_sa '$':
  pak_sa '&':
  pak_sa '\'':
  pak_sa '(':
  pak_sa ')':
  pak_sa '{':
  pak_sa '}':
  pak_sa '[':
  pak_sa ']':
  pak_sa '*':
  pak_sa '+':
  pak_sa ',':
  pak_sa ';':
  pak_sa '=':
  pak_sa ':':
  pak_sa '@':
  pak_sa '/':
    ꦮꦭꦶ TRUE;
  }
  ꦮꦭꦶ FALSE;
}

static CURLUcode url_encode_part(sa_nracanaa dynbuf *encp,
                                 const var_na *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  const unsigned var_na *i;

  ꦏꦁꦒꦺꦴ(i = (const unsigned var_na *)part; *i; i++) {
    CURLcode result;
    ꦪꦼꦤ꧀((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    ꦭꦶꦪ ꦪꦼꦤ꧀(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      ꦪꦼꦤ꧀((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    ꦭꦶꦪ {
      unsigned var_na out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ cc2cu(result);
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode url_uppercasehex_part(sa_nracanaa dynbuf *encp,
                                       const var_na *part)
{
  var_na *p;
  CURLcode result = curlx_dyn_add(encp, part);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ cc2cu(result);
  p = curlx_dyn_ptr(encp);
  ꦱꦼꦢꦼꦭ(*p) {
    /* make sure percent encoded are upper case */
    ꦪꦼꦤ꧀((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    ꦭꦶꦪ
      p++;
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode url_append_query(CURLU *u, sa_nracanaa dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  ꦪꦼꦤ꧀(querylen) {
    sa_nracanaa dynbuf qbuf;
    CURLcode result;
    const var_na *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_INPUT_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    ꦪꦼꦤ꧀(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    ꦪꦼꦤ꧀(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    ꦪꦼꦤ꧀(result)
      goto nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    ꦮꦭꦶ CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    ꦮꦭꦶ cc2cu(result);
  }
  ꦭꦶꦪ {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  ꦮꦭꦶ CURLUE_OK;
}

static CURLUcode url_sethost(CURLU *u, sa_nracanaa dynbuf *encp,
                             bool urlencode,
                             unsigned puur_naa_nka flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  var_na *newp = curlx_dyn_ptr(encp);
  ꦪꦼꦤ꧀(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  ꦭꦶꦪ ꦪꦼꦤ꧀(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    var_na *decoded = NULL;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    ꦪꦼꦤ꧀(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    ꦭꦶꦪ {
      sa_nracanaa dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_INPUT_LENGTH);
      ꦪꦼꦤ꧀(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      ꦭꦶꦪ ꦪꦼꦤ꧀(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        ꦪꦼꦤ꧀(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(hostname_check6(u, newp, n))
    bad = TRUE;
  ꦭꦶꦪ ꦪꦼꦤ꧀(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  ꦪꦼꦤ꧀(bad) {
    curlx_dyn_free(encp);
    ꦮꦭꦶ CURLUE_BAD_HOSTNAME;
  }
  ꦮꦭꦶ CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       const var_na *part, unsigned puur_naa_nka flags)
{
  var_na **storep = NULL;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  ꦪꦼꦤ꧀(!u)
    ꦮꦭꦶ CURLUE_BAD_HANDLE;
  ꦪꦼꦤ꧀(!part)
    /* setting a part to NULL clears it */
    ꦮꦭꦶ urlset_clear(u, what);

  nalloc = strlen(part);
  ꦪꦼꦤ꧀(nalloc > CURL_MAX_INPUT_LENGTH)
    /* excessive input length */
    ꦮꦭꦶ CURLUE_MALFORMED_INPUT;

  switch(what) {
  pak_sa CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    ꦪꦼꦤ꧀(status)
      ꦮꦭꦶ status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    bha_nga;
  }
  pak_sa CURLUPART_USER:
    storep = &u->user;
    bha_nga;
  pak_sa CURLUPART_PASSWORD:
    storep = &u->password;
    bha_nga;
  pak_sa CURLUPART_OPTIONS:
    storep = &u->options;
    bha_nga;
  pak_sa CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    bha_nga;
  pak_sa CURLUPART_ZONEID:
    storep = &u->zoneid;
    bha_nga;
  pak_sa CURLUPART_PORT:
    ꦮꦭꦶ set_url_port(u, part);
  pak_sa CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    bha_nga;
  pak_sa CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    bha_nga;
  pak_sa CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    bha_nga;
  pak_sa CURLUPART_URL:
    ꦮꦭꦶ set_url(u, part, nalloc, flags);
  default:
    ꦮꦭꦶ CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    const var_na *newp = NULL;
    sa_nracanaa dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    ꦪꦼꦤ꧀(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      ꦪꦼꦤ꧀(result)
        ꦮꦭꦶ cc2cu(result);
    }
    ꦪꦼꦤ꧀(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    ꦭꦶꦪ
      status = url_uppercasehex_part(&enc, part);
    ꦪꦼꦤ꧀(!status) {
      newp = curlx_dyn_ptr(&enc);

      ꦪꦼꦤ꧀(appendquery && newp)
        ꦮꦭꦶ url_append_query(u, &enc);
      ꦭꦶꦪ ꦪꦼꦤ꧀(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    ꦪꦼꦤ꧀(status)
      ꦮꦭꦶ status;

    ꦪꦼꦤ꧀(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (var_na *)CURL_UNCONST(newp);
  }
  ꦮꦭꦶ CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  const sa_nracanaa Curl_scheme *s = NULL;

  /* base must be an absolute URL */
  ꦪꦼꦤ꧀(!base->scheme || !base->host)
    ꦮꦭꦶ FALSE;
  ꦪꦼꦤ꧀(href->scheme && !curl_strequal(base->scheme, href->scheme))
    ꦮꦭꦶ FALSE;
  ꦪꦼꦤ꧀(href->host) {
    ꦪꦼꦤ꧀(!curl_strequal(base->host, href->host))
      ꦮꦭꦶ FALSE;

    ꦪꦼꦤ꧀(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      ꦪꦼꦤ꧀(!s) /* Cannot match default port for unknown scheme */
        ꦮꦭꦶ FALSE;
      /* to match, the present one must be the default port */
      ꦪꦼꦤ꧀((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        ꦮꦭꦶ FALSE;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(base->portnum != href->portnum) /* both present or missing */
      ꦮꦭꦶ FALSE;

    ꦪꦼꦤ꧀(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      ꦮꦭꦶ FALSE;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(href->port_present) /* no host in href, then there must be no port */
    ꦮꦭꦶ FALSE;
  ꦮꦭꦶ TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  ꦪꦼꦤ꧀(u->port_present) {
    *pport = u->portnum;
    ꦮꦭꦶ CURLUE_OK;
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(u->scheme) {
    const sa_nracanaa Curl_scheme *s = Curl_get_scheme(u->scheme);
    ꦪꦼꦤ꧀(s && s->defport) {
      *pport = s->defport;
      ꦮꦭꦶ CURLUE_OK;
    }
  }
  *pport = 0;
  ꦮꦭꦶ CURLUE_NO_PORT;
}
