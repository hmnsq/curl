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
#만약 !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

static void free_urlhandle(struct Curl_URL *u)
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
static const char *find_host_sep(const char *url)
{
  /* Find the start of the hostname */
  const char *sep = strstr(url, "//");
  만약(!sep)
    sep = url;
  아니면
    sep += 2;

  /* Find first / or ? */
  동안(*sep && *sep != '/' && *sep != '?')
    sep++;

  반환 sep;
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
UNITTEST CURLUcode urlencode_str(struct dynbuf *o, const char *url,
                                 size_t len, bool relative,
                                 unsigned 정수 query);
UNITTEST CURLUcode urlencode_str(struct dynbuf *o, const char *url,
                                 size_t len, bool relative,
                                 unsigned 정수 query)
{
  /* we must add this with whitespace-replacing */
  const unsigned char *iptr;
  const unsigned char *host_sep = (const unsigned char *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  만약(!relative) {
    size_t n;
    host_sep = (const unsigned char *)find_host_sep(url);

    /* output the first piece as-is */
    n = (const char *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  위해(iptr = host_sep; len && !result;) {
    만약(*iptr == ' ') {
      만약(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      아니면
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    아니면 만약((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned char out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    아니면 만약(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned char hex = (unsigned char)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned char out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    아니면 {
      const unsigned char *start = iptr;
      동안(len) {
        만약(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          break;
        만약(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          break;
        만약(*iptr == '?') {
          만약(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            break;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (const char *)start, (size_t)(iptr - start));
    }
  }

  만약(result)
    반환 cc2cu(result);
  반환 CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(const char *url, char *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (void)buflen; /* only used in debug-builds */
  만약(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  만약(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    반환 0;
#endif
  만약(ISALPHA(url[0])) {
    만약(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    위해(i = 1; i < MAX_SCHEME_LEN; ++i) {
      char s = url[i];
      만약(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        만약(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      아니면 {
        break;
      }
    }
  }
  만약(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    만약(buf)
      buf[i] = 0;
    반환 len;
  }
  만약(buf)
    buf[0] = 0;
  반환 0;
}

/* scan for byte values <= 31, 127 and maybe space */
static bool badoctets(const char *input, size_t n, 정수 flags)
{
  const uint8_t *p = (const unsigned char *)input;
  const uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  동안(n--) {
    만약(*p <= control || *p == 127)
      반환 TRUE;
    p++;
  }
  반환 FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(struct Curl_URL *u,
                                        const char *login,
                                        size_t len,
                                        unsigned 정수 flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(struct Curl_URL *u,
                                        const char *login,
                                        size_t len,
                                        unsigned 정수 flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  char *userp = NULL;
  char *passwdp = NULL;
  char *optionsp = NULL;
  const struct Curl_scheme *h = NULL;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  const char *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  만약(!ptr)
    goto out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  만약(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : NULL);
  만약(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    goto out;
  }

  만약(userp) {
    만약(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      goto out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  만약(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  만약(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  만약(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  아니면 만약(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  아니면 만약(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_INPUT;

  userp = passwdp = optionsp = NULL;

  만약(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    반환 CURLUE_OK;
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

  반환 ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(struct Curl_URL *u, struct dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(struct Curl_URL *u, struct dynbuf *host,
                              bool has_scheme)
{
  const char *portptr;
  const char *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  만약(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    만약(!portptr)
      반환 CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    만약(*portptr) {
      만약(*portptr != ':')
        반환 CURLUE_BAD_PORT_NUMBER;
    }
    아니면
      portptr = NULL;
  }
  아니면
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  만약(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    정수 rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    만약(!*portptr)
      반환 has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_NUMBER;
    만약(*portptr == '\\')
      반환 CURLUE_BACKSLASH;
    rc = curlx_str_number(&portptr, &port, 0xffff);
    만약(rc)
      반환 CURLUE_BAD_PORT_NUMBER;
    아니면 만약(*portptr == '\\')
      반환 CURLUE_BACKSLASH;
    아니면 만약(*portptr)
      반환 CURLUE_BAD_PORT_NUMBER;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  반환 CURLUE_OK;
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
UNITTEST CURLUcode ipv6_parse(struct Curl_URL *u, char *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(struct Curl_URL *u, char *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  만약(hlen < 4) /* '[::]' is the shortest possible valid string */
    반환 CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  만약(hlen != len) {
    hlen = len;
    만약(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      char zoneid[MAX_ZONEID_LEN];
      정수 i = 0;
      char *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      만약(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      동안(!invalid_host_char[(unsigned char)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      만약(!i || (']' != *h))
        반환 CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      만약(!u->zoneid)
        반환 CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    아니면
      반환 CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    char dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    만약(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      반환 CURLUE_BAD_IPV6;
    만약(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  반환 CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
static CURLUcode hostname_check(char *hostname, size_t hlen)
{
  size_t i;
  위해(i = 0; i < hlen; i++) {
    만약(invalid_host_char[(unsigned char)hostname[i]])
      반환 CURLUE_BAD_HOSTNAME;
  }
  만약((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    반환 CURLUE_BAD_HOSTNAME;
  아니면 만약((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    반환 CURLUE_BAD_HOSTNAME;
  반환 CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
static CURLUcode hostname_check6(struct Curl_URL *u, char *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  만약(!hlen)
    반환 CURLUE_NO_HOST;
  아니면 만약(hostname[0] == '[')
    반환 ipv6_parse(u, hostname, hlen);

  반환 hostname_check(hostname, hlen);
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
UNITTEST 정수 ipv4_normalize(struct dynbuf *host);
UNITTEST 정수 ipv4_normalize(struct dynbuf *host)
{
  bool done = FALSE;
  정수 n = 0;
  const char *c = curlx_dyn_ptr(host);
  unsigned 정수 parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  만약(!ISDIGIT(*c))
    반환 HOST_NAME;

  동안(!done) {
    정수 rc;
    curl_off_t l;
    만약(*c == '0') {
      만약((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        만약(rc)
          반환 HOST_NAME;
      }
      아니면
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    아니면
      rc = curlx_str_number(&c, &l, UINT_MAX);

    만약(rc) {
      만약(!n || (rc != STRE_NO_NUM) || *c)
        반환 HOST_NAME;
      n--;
    }
    아니면
      parts[n] = (unsigned 정수)l;

    switch(*c) {
    case '.':
      만약(n == 3) {
        만약(c[1])
          /* something follows this dot */
          반환 HOST_NAME;
        done = TRUE;
      }
      아니면 {
        n++;
        c++;
      }
      break;

    case '\0':
      done = TRUE;
      break;

    default:
      반환 HOST_NAME;
    }
  }

  switch(n) {
  case 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    break;
  case 1: /* a.b -- 8.24 bits */
    만약((parts[0] > 0xff) || (parts[1] > 0xffffff))
      반환 HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    break;
  case 2: /* a.b.c -- 8.8.16 bits */
    만약((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      반환 HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    break;
  case 3: /* a.b.c.d -- 8.8.8.8 bits */
    만약((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      반환 HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    break;
  }
  만약(result)
    반환 HOST_ERROR;
  반환 HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
static CURLUcode urldecode_host(struct dynbuf *host)
{
  const char *per;
  const char *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  만약(!per)
    /* nothing to decode */
    반환 CURLUE_OK;
  아니면 {
    /* encoded */
    size_t dlen;
    char *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    만약(result)
      반환 CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    만약(result)
      반환 cc2cu(result);
  }

  반환 CURLUE_OK;
}

static CURLUcode parse_authority(struct Curl_URL *u,
                                 const char *auth, size_t authlen,
                                 unsigned 정수 flags,
                                 struct dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  만약(uc)
    반환 uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  만약(result) {
    uc = cc2cu(result);
    반환 uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  만약(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  만약(!uc)
    uc = urldecode_host(host);
  만약(uc)
    ;
  아니면 만약(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  아니면 {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    정수 type = ipv4_normalize(host);

    만약(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    아니면 만약(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  반환 uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, const char *authority)
{
  CURLUcode ures;
  struct dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  만약(ures)
    curlx_dyn_free(&host);
  아니면 {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  반환 ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

static bool is_dot(const char **str, size_t *clen)
{
  const char *p = *str;
  만약(*p == '.') {
    (*str)++;
    (*clen)--;
    반환 TRUE;
  }
  아니면 만약((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    반환 TRUE;
  }
  반환 FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
static bool needs_dedotdot(const char *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  만약(pn < 2)
    반환 FALSE;
  만약(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    반환 FALSE;
  동안(pn) {
    만약(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      만약(!pn || ISSLASH(*p))
        반환 TRUE;
      /* "../" or ".." before end of string */
      아니면 만약(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        반환 TRUE;
    }
    아니면 {
      p++;
      pn--;
    }
  }
  반환 FALSE;
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
UNITTEST 정수 dedotdotify(const char *input, size_t clen, char **outp);
UNITTEST 정수 dedotdotify(const char *input, size_t clen, char **outp)
{
  struct dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  const char *dinput = input;
  size_t dlen = clen;

  *outp = NULL;
  만약(!needs_dedotdot(input, clen))
    반환 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  만약(is_dot(&dinput, &dlen)) {
    만약(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      input = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    아니면 만약(is_dot(&dinput, &dlen)) {
      만약(!dlen)
        /* .. [end] */
        goto end;
      아니면 만약(ISSLASH(*dinput)) {
        /* ../ */
        input = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  동안(clen && !result) { /* until end of path content */
    만약(ISSLASH(*input)) {
      const char *p = &input[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      만약(is_dot(&p, &blen)) {
        만약(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          break;
        }
        아니면 만약(ISSLASH(*p)) { /* /./ */
          input = p;
          clen = blen;
          continue;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        아니면 만약(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          만약(len) {
            const char *ptr = curlx_dyn_ptr(&out);
            const char *last = memrchr(ptr, '/', len);
            만약(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          만약(blen) { /* /../ */
            input = p;
            clen = blen;
            continue;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          break;
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
  만약(!result) {
    만약(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    아니면 {
      *outp = curlx_strdup("");
      만약(!*outp)
        반환 1;
    }
  }
  반환 result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(const char *url, size_t urllen, CURLU *u,
                              const char **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(const char *url, size_t urllen, CURLU *u,
                              const char **pathp, size_t *pathlenp)
{
  const char *path;
  size_t pathlen;

  *pathp = NULL;
  *pathlenp = 0;
  만약(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    반환 CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  만약(path[0] != '/')
    반환 CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  만약(path[1] == '/') {
    /* swallow the two slashes */
    const char *ptr = &path[2];

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
    만약(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      만약(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      아니면
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        반환 CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#만약 !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  만약(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    반환 CURLUE_BAD_FILE_URL;
  }
#아니면
  /* If the path starts with a slash and a drive letter, ditch the slash */
  만약('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  만약(!u->scheme)
    반환 CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  반환 CURLUE_OK;
}

static CURLUcode parse_scheme(const char *url, CURLU *u, char *schemebuf,
                              size_t schemelen, unsigned 정수 flags,
                              const char **hostpp)
{
  /* clear path */
  const char *schemep = NULL;

  만약(schemelen) {
    정수 num_slashes = 0;
    const char *p = &url[schemelen + 1];
    만약(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      반환 CURLUE_UNSUPPORTED_SCHEME;

    만약(!ISSLASH(*p))
      /* less than one */
      반환 CURLUE_BAD_SLASHES;
    만약((flags & CURLU_NO_AUTHORITY)) {
      동안(ISSLASH(*p) && (num_slashes < 2)) {
        p++;
        num_slashes++;
      }
    }
    아니면 {
      동안(ISSLASH(*p) && (num_slashes < 4)) {
        p++;
        num_slashes++;
      }
      만약(num_slashes > 3)
        반환 CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  아니면 {
    /* no scheme! */

    만약(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      반환 CURLUE_BAD_SCHEME;

    만약(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  만약(schemep) {
    u->scheme = curlx_strdup(schemep);
    만약(!u->scheme)
      반환 CURLUE_OUT_OF_MEMORY;
  }
  반환 CURLUE_OK;
}

static CURLUcode guess_scheme(CURLU *u, struct dynbuf *host)
{
  const char *hostname = curlx_dyn_ptr(host);
  const char *schemep = NULL;
  /* legacy curl-style guess based on hostname */
  만약(checkprefix("ftp.", hostname))
    schemep = "ftp";
  아니면 만약(checkprefix("dict.", hostname))
    schemep = "dict";
  아니면 만약(checkprefix("ldap.", hostname))
    schemep = "ldap";
  아니면 만약(checkprefix("imap.", hostname))
    schemep = "imap";
  아니면 만약(checkprefix("smtp.", hostname))
    schemep = "smtp";
  아니면 만약(checkprefix("pop3.", hostname))
    schemep = "pop3";
  아니면
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  만약(!u->scheme)
    반환 CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  반환 CURLUE_OK;
}

static CURLUcode handle_fragment(CURLU *u, const char *fragment,
                                 size_t fraglen, unsigned 정수 flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  만약(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    만약(flags & CURLU_URLENCODE) {
      struct dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      만약(ures)
        반환 ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    아니면 {
      만약(badoctets(fragment, fraglen, flags))
        반환 CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      만약(!u->fragment)
        반환 CURLUE_OUT_OF_MEMORY;
    }
  }
  반환 CURLUE_OK;
}

static CURLUcode handle_query(CURLU *u, const char *query,
                              size_t qlen, unsigned 정수 flags)
{
  u->query_present = TRUE;
  만약(qlen > 1) {
    만약(flags & CURLU_URLENCODE) {
      struct dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      만약(ures)
        반환 ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    아니면 {
      만약(badoctets(query, qlen, flags))
        반환 CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      만약(!u->query)
        반환 CURLUE_OUT_OF_MEMORY;
    }
  }
  아니면 {
    /* single byte query */
    u->query = curlx_strdup("");
    만약(!u->query)
      반환 CURLUE_OUT_OF_MEMORY;
  }
  반환 CURLUE_OK;
}

static CURLUcode handle_path(CURLU *u, const char *path,
                             size_t pathlen, unsigned 정수 flags,
                             bool is_file)
{
  CURLUcode ures;
  만약(pathlen && (flags & CURLU_URLENCODE)) {
    struct dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    만약(ures)
      반환 ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  만약(pathlen >= (size_t)(1 + !is_file)) {
    만약(badoctets(path, pathlen, flags))
      반환 CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    만약(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      만약(!u->path)
        반환 CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    아니면 만약(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    만약(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      char *dedot;
      정수 err = dedotdotify(path, pathlen, &dedot);
      만약(err)
        반환 CURLUE_OUT_OF_MEMORY;
      만약(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  반환 CURLUE_OK;
}

static CURLUcode parseurl(const char *url, CURLU *u, unsigned 정수 flags)
{
  const char *path;
  size_t pathlen;
  char schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  struct dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  만약(urllen > CURL_MAX_INPUT_LENGTH)
    반환 CURLUE_MALFORMED_INPUT;

  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, sizeof(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  만약(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  아니면 {
    const char *hostp = NULL;
    const char *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    만약(ures)
      goto fail;

    /* find the end of the hostname + port number */
    p = hostp;
    동안(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    만약(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      만약(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    아니면 만약(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      만약(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    아니면
      ures = CURLUE_NO_HOST;
  }
  만약(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    const char *fragment = memchr(path, '#', pathlen);
    만약(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  만약(!ures) {
    const char *query = memchr(path, '?', pathlen);
    만약(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  만약(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  만약(!ures) {
    u->host = curlx_dyn_ptr(&host);
    반환 CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  반환 ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
static CURLUcode parseurl_and_replace(const char *url, CURLU *u,
                                      unsigned 정수 flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, sizeof(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  만약(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  반환 ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
static CURLUcode redirect_url(const char *base, const char *relurl,
                              CURLU *u, unsigned 정수 flags)
{
  struct dynbuf urlbuf;
  bool host_changed = FALSE;
  const char *useurl = relurl;
  const char *cutoff = NULL;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  const char *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  const char *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  만약(!base)
    반환 CURLUE_MALFORMED_INPUT; /* should never happen */

  /* handle different relative URL types */
  switch(relurl[0]) {
  case '/':
    만약(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    아니면
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    break;

  case '#':
    /* fragment-only change */
    만약(u->fragment_present)
      cutoff = strchr(protsep, '#');
    break;

  default:
    /* path or query-only change */
    만약(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    아니면 만약(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    만약(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      만약(cutoff)
        cutoff++; /* truncate after last slash */
    }
    break;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_INPUT_LENGTH);

  만약(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  아니면
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  반환 uc;
}

/*
 */
CURLU *curl_url(void)
{
  반환 curlx_calloc(1, sizeof(struct Curl_URL));
}

void curl_url_cleanup(CURLU *u)
{
  만약(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  do {                                          \
    만약((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      만약(!(dest)->name)                         \
        goto fail;                              \
    }                                           \
  } 동안(0)

CURLU *curl_url_dup(const CURLU *in)
{
  struct Curl_URL *u;

  만약(!in)
    반환 NULL;

  u = curlx_calloc(1, sizeof(struct Curl_URL));
  만약(u) {
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
  반환 u;
fail:
  curl_url_cleanup(u);
  반환 NULL;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#아니면
static CURLUcode host_decode(const char *host, char **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  만약(result)
    반환 (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  반환 CURLUE_OK;
}

static CURLUcode host_encode(const char *host, char **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  만약(result)
    반환 (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  반환 CURLUE_OK;
}
#endif

static CURLUcode urlget_format(const CURLU *u, CURLUPart what,
                               const char *ptr, char **partp,
                               bool plusdecode, unsigned 정수 flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  char *part = curlx_memdup0(ptr, partlen);
  *partp = NULL;
  만약(!part)
    반환 CURLUE_OUT_OF_MEMORY;
  만약(plusdecode) {
    /* convert + to space */
    char *plus = part;
    size_t i = 0;
    위해(i = 0; i < partlen; ++plus, i++) {
      만약(*plus == '+')
        *plus = ' ';
    }
  }
  만약(urldecode) {
    char *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    만약(result)
      반환 CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  만약(urlencode) {
    struct dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    만약(uc)
      반환 uc;
    part = curlx_dyn_ptr(&enc);
  }
  아니면 만약(punycode) {
    만약(!Curl_is_ASCII_name(u->host)) {
      char *punyversion = NULL;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      만약(uc)
        반환 uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      만약(uc) {
        curlx_free(punyversion);
        반환 uc;
      }
      part = punyversion;
    }
  }
  아니면 만약(depunyfy && Curl_is_ASCII_name(u->host)) {
    char *unpunified = NULL;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    만약(uc)
      반환 uc;
    part = unpunified;
  }
  *partp = part;
  반환 CURLUE_OK;
}

static CURLUcode file_url(const CURLU *u, char **part,
                          const char *fragmentsep,
                          const char *querysep)
{
  char *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  만약(!url)
    반환 CURLUE_OUT_OF_MEMORY;

  *part = url;
  반환 CURLUE_OK;
}

static CURLUcode urlget_url(const CURLU *u, char **part, unsigned 정수 flags)
{
  char *url;
  char *allochost = NULL;
  const char *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  const char *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  char portbuf[7];
  만약(curl_strequal("file", u->scheme))
    반환 file_url(u, part, fragmentsep, querysep);
  아니면 만약(!u->host)
    반환 CURLUE_NO_HOST;
  아니면 {
    const char *scheme;
    char *options = u->options;
    char *port = NULL;
    const struct Curl_scheme *h = NULL;
    char schemebuf[MAX_SCHEME_LEN + 5];
    만약(u->scheme)
      scheme = u->scheme;
    아니면 만약(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    아니면
      반환 CURLUE_NO_SCHEME;

    만약(u->port_present) {
      curl_msnprintf(portbuf, sizeof(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    만약(h) {
      만약(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, sizeof(portbuf), "%u", h->defport);
        port = portbuf;
      }
      아니면 만약(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = NULL;
      }

      만약(!(h->flags & PROTOPT_URLOPTIONS))
        options = NULL;
    }

    만약(u->host[0] == '[') {
      만약(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        struct dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
        만약(curlx_dyn_addf(&enc, "%.*s%%25%s]", (정수)hostlen - 1, u->host,
                          u->zoneid))
          반환 CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    아니면 만약(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(NULL, u->host, 0);
      만약(!allochost)
        반환 CURLUE_OUT_OF_MEMORY;
    }
    아니면 만약(flags & CURLU_PUNYCODE) {
      만약(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        만약(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        만약(ret) {
          curlx_safefree(allochost);
          반환 ret;
        }
      }
    }
    아니면 만약(flags & CURLU_PUNY2IDN) {
      만약(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        만약(ret)
          반환 ret;
      }
    }

    만약(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, sizeof(schemebuf), "%s://", scheme);
    아니면
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
  만약(!url)
    반환 CURLUE_OUT_OF_MEMORY;
  *part = url;
  반환 CURLUE_OK;
}

CURLUcode curl_url_get(const CURLU *u, CURLUPart what,
                       char **part, unsigned 정수 flags)
{
  const char *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  char portbuf[7];
  bool plusdecode = FALSE;
  만약(!u)
    반환 CURLUE_BAD_HANDLE;
  만약(!part)
    반환 CURLUE_BAD_PARTPOINTER;
  *part = NULL;

  switch(what) {
  case CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    만약((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      반환 CURLUE_NO_SCHEME;
    break;
  case CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    break;
  case CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    break;
  case CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    break;
  case CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    break;
  case CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    break;
  case CURLUPART_PORT:
    ptr = NULL;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    만약(u->port_present) {
      const struct Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : NULL;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      만약(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = NULL;
      }
      아니면 {
        curl_msnprintf(portbuf, sizeof(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    아니면 만약((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      const struct Curl_scheme *h = Curl_get_scheme(u->scheme);
      만약(h) {
        curl_msnprintf(portbuf, sizeof(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    break;
  case CURLUPART_PATH:
    ptr = u->path;
    만약(!ptr)
      ptr = "/";
    break;
  case CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    만약(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = NULL;
    break;
  case CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    만약(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    break;
  case CURLUPART_URL:
    반환 urlget_url(u, part, flags);
  default:
    ptr = NULL;
    break;
  }
  만약(ptr)
    반환 urlget_format(u, what, ptr, part, plusdecode, flags);

  반환 ifmissing;
}

static CURLUcode set_url_scheme(CURLU *u, const char *scheme,
                                unsigned 정수 flags)
{
  size_t plen = strlen(scheme);
  const struct Curl_scheme *h = NULL;
  만약((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    반환 CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  만약(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    반환 CURLUE_UNSUPPORTED_SCHEME;
  만약(!h) {
    const char *s = scheme;
    만약(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      동안(--plen) {
        만약(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        아니면
          반환 CURLUE_BAD_SCHEME;
      }
    }
    아니면
      반환 CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  반환 CURLUE_OK;
}

static CURLUcode set_url_port(CURLU *u, const char *provided_port)
{
  curl_off_t port;
  만약(!ISDIGIT(provided_port[0]))
    /* not a number */
    반환 CURLUE_BAD_PORT_NUMBER;
  만약(curlx_str_number(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    반환 CURLUE_BAD_PORT_NUMBER;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  반환 CURLUE_OK;
}

static CURLUcode set_url(CURLU *u, const char *url, size_t part_size,
                         unsigned 정수 flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  char *oldurl = NULL;

  만약(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    만약(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      반환 CURLUE_OK;
    }
    만약(uc == CURLUE_OUT_OF_MEMORY)
      반환 uc;
    반환 CURLUE_MALFORMED_INPUT;
  }

  /* if the new URL is absolute replace the existing with the new. */
  만약(Curl_is_absolute_url(url, NULL, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    반환 parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  만약(uc == CURLUE_OUT_OF_MEMORY)
    반환 uc;
  아니면 만약(uc)
    반환 parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  반환 uc;
}

static CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  switch(what) {
  case CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, sizeof(struct Curl_URL));
    break;
  case CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    break;
  case CURLUPART_USER:
    curlx_safefree(u->user);
    break;
  case CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    break;
  case CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    break;
  case CURLUPART_HOST:
    curlx_safefree(u->host);
    break;
  case CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    break;
  case CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    break;
  case CURLUPART_PATH:
    curlx_safefree(u->path);
    break;
  case CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    break;
  case CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    break;
  default:
    반환 CURLUE_UNKNOWN_PART;
  }
  반환 CURLUE_OK;
}

static bool allowed_in_path(unsigned char x)
{
  switch(x) {
  case '!':
  case '$':
  case '&':
  case '\'':
  case '(':
  case ')':
  case '{':
  case '}':
  case '[':
  case ']':
  case '*':
  case '+':
  case ',':
  case ';':
  case '=':
  case ':':
  case '@':
  case '/':
    반환 TRUE;
  }
  반환 FALSE;
}

static CURLUcode url_encode_part(struct dynbuf *encp,
                                 const char *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  const unsigned char *i;

  위해(i = (const unsigned char *)part; *i; i++) {
    CURLcode result;
    만약((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    아니면 만약(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      만약((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    아니면 {
      unsigned char out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    만약(result)
      반환 cc2cu(result);
  }
  반환 CURLUE_OK;
}

static CURLUcode url_uppercasehex_part(struct dynbuf *encp,
                                       const char *part)
{
  char *p;
  CURLcode result = curlx_dyn_add(encp, part);
  만약(result)
    반환 cc2cu(result);
  p = curlx_dyn_ptr(encp);
  동안(*p) {
    /* make sure percent encoded are upper case */
    만약((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    아니면
      p++;
  }
  반환 CURLUE_OK;
}

static CURLUcode url_append_query(CURLU *u, struct dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  만약(querylen) {
    struct dynbuf qbuf;
    CURLcode result;
    const char *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_INPUT_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    만약(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    만약(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    만약(result)
      goto nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    반환 CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    반환 cc2cu(result);
  }
  아니면 {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  반환 CURLUE_OK;
}

static CURLUcode url_sethost(CURLU *u, struct dynbuf *encp,
                             bool urlencode,
                             unsigned 정수 flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  char *newp = curlx_dyn_ptr(encp);
  만약(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  아니면 만약(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    char *decoded = NULL;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    만약(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    아니면 {
      struct dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_INPUT_LENGTH);
      만약(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      아니면 만약(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        만약(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  아니면 만약(hostname_check6(u, newp, n))
    bad = TRUE;
  아니면 만약(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  만약(bad) {
    curlx_dyn_free(encp);
    반환 CURLUE_BAD_HOSTNAME;
  }
  반환 CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       const char *part, unsigned 정수 flags)
{
  char **storep = NULL;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  만약(!u)
    반환 CURLUE_BAD_HANDLE;
  만약(!part)
    /* setting a part to NULL clears it */
    반환 urlset_clear(u, what);

  nalloc = strlen(part);
  만약(nalloc > CURL_MAX_INPUT_LENGTH)
    /* excessive input length */
    반환 CURLUE_MALFORMED_INPUT;

  switch(what) {
  case CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    만약(status)
      반환 status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    break;
  }
  case CURLUPART_USER:
    storep = &u->user;
    break;
  case CURLUPART_PASSWORD:
    storep = &u->password;
    break;
  case CURLUPART_OPTIONS:
    storep = &u->options;
    break;
  case CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    break;
  case CURLUPART_ZONEID:
    storep = &u->zoneid;
    break;
  case CURLUPART_PORT:
    반환 set_url_port(u, part);
  case CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    break;
  case CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    break;
  case CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    break;
  case CURLUPART_URL:
    반환 set_url(u, part, nalloc, flags);
  default:
    반환 CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    const char *newp = NULL;
    struct dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    만약(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      만약(result)
        반환 cc2cu(result);
    }
    만약(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    아니면
      status = url_uppercasehex_part(&enc, part);
    만약(!status) {
      newp = curlx_dyn_ptr(&enc);

      만약(appendquery && newp)
        반환 url_append_query(u, &enc);
      아니면 만약(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    만약(status)
      반환 status;

    만약(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (char *)CURL_UNCONST(newp);
  }
  반환 CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  const struct Curl_scheme *s = NULL;

  /* base must be an absolute URL */
  만약(!base->scheme || !base->host)
    반환 FALSE;
  만약(href->scheme && !curl_strequal(base->scheme, href->scheme))
    반환 FALSE;
  만약(href->host) {
    만약(!curl_strequal(base->host, href->host))
      반환 FALSE;

    만약(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      만약(!s) /* Cannot match default port for unknown scheme */
        반환 FALSE;
      /* to match, the present one must be the default port */
      만약((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        반환 FALSE;
    }
    아니면 만약(base->portnum != href->portnum) /* both present or missing */
      반환 FALSE;

    만약(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      반환 FALSE;
  }
  아니면 만약(href->port_present) /* no host in href, then there must be no port */
    반환 FALSE;
  반환 TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  만약(u->port_present) {
    *pport = u->portnum;
    반환 CURLUE_OK;
  }
  아니면 만약(u->scheme) {
    const struct Curl_scheme *s = Curl_get_scheme(u->scheme);
    만약(s && s->defport) {
      *pport = s->defport;
      반환 CURLUE_OK;
    }
  }
  *pport = 0;
  반환 CURLUE_NO_PORT;
}
