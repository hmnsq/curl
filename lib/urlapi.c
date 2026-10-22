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
#якщо !defined(USE_IPV6) && !defined(AF_INET6)
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
  якщо(!sep)
    sep = url;
  інакше
    sep += 2;

  /* Find first / or ? */
  поки(*sep && *sep != '/' && *sep != '?')
    sep++;

  повернути sep;
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
                                 unsigned ціле query);
UNITTEST CURLUcode urlencode_str(struct dynbuf *o, const char *url,
                                 size_t len, bool relative,
                                 unsigned ціле query)
{
  /* we must add this with whitespace-replacing */
  const unsigned char *iptr;
  const unsigned char *host_sep = (const unsigned char *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  якщо(!relative) {
    size_t n;
    host_sep = (const unsigned char *)find_host_sep(url);

    /* output the first piece as-is */
    n = (const char *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  для(iptr = host_sep; len && !result;) {
    якщо(*iptr == ' ') {
      якщо(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      інакше
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    інакше якщо((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned char out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    інакше якщо(*iptr == '%' && (len >= 3) &&
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
    інакше {
      const unsigned char *start = iptr;
      поки(len) {
        якщо(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          break;
        якщо(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          break;
        якщо(*iptr == '?') {
          якщо(query == QUERY_NOT_YET) {
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

  якщо(result)
    повернути cc2cu(result);
  повернути CURLUE_OK;
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
  якщо(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  якщо(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    повернути 0;
#endif
  якщо(ISALPHA(url[0])) {
    якщо(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    для(i = 1; i < MAX_SCHEME_LEN; ++i) {
      char s = url[i];
      якщо(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        якщо(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      інакше {
        break;
      }
    }
  }
  якщо(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    якщо(buf)
      buf[i] = 0;
    повернути len;
  }
  якщо(buf)
    buf[0] = 0;
  повернути 0;
}

/* scan for byte values <= 31, 127 and maybe space */
static bool badoctets(const char *input, size_t n, ціле flags)
{
  const uint8_t *p = (const unsigned char *)input;
  const uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  поки(n--) {
    якщо(*p <= control || *p == 127)
      повернути TRUE;
    p++;
  }
  повернути FALSE;
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
                                        unsigned ціле flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(struct Curl_URL *u,
                                        const char *login,
                                        size_t len,
                                        unsigned ціле flags,
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
  якщо(!ptr)
    goto out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  якщо(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : NULL);
  якщо(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    goto out;
  }

  якщо(userp) {
    якщо(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      goto out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  якщо(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  якщо(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  якщо(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  інакше якщо(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  інакше якщо(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_INPUT;

  userp = passwdp = optionsp = NULL;

  якщо(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    повернути CURLUE_OK;
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

  повернути ures;
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
  якщо(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    якщо(!portptr)
      повернути CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    якщо(*portptr) {
      якщо(*portptr != ':')
        повернути CURLUE_BAD_PORT_NUMBER;
    }
    інакше
      portptr = NULL;
  }
  інакше
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  якщо(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    ціле rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    якщо(!*portptr)
      повернути has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_NUMBER;
    якщо(*portptr == '\\')
      повернути CURLUE_BACKSLASH;
    rc = curlx_str_number(&portptr, &port, 0xffff);
    якщо(rc)
      повернути CURLUE_BAD_PORT_NUMBER;
    інакше якщо(*portptr == '\\')
      повернути CURLUE_BACKSLASH;
    інакше якщо(*portptr)
      повернути CURLUE_BAD_PORT_NUMBER;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  повернути CURLUE_OK;
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
  якщо(hlen < 4) /* '[::]' is the shortest possible valid string */
    повернути CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  якщо(hlen != len) {
    hlen = len;
    якщо(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      char zoneid[MAX_ZONEID_LEN];
      ціле i = 0;
      char *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      якщо(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      поки(!invalid_host_char[(unsigned char)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      якщо(!i || (']' != *h))
        повернути CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      якщо(!u->zoneid)
        повернути CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    інакше
      повернути CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    char dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    якщо(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      повернути CURLUE_BAD_IPV6;
    якщо(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  повернути CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
static CURLUcode hostname_check(char *hostname, size_t hlen)
{
  size_t i;
  для(i = 0; i < hlen; i++) {
    якщо(invalid_host_char[(unsigned char)hostname[i]])
      повернути CURLUE_BAD_HOSTNAME;
  }
  якщо((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    повернути CURLUE_BAD_HOSTNAME;
  інакше якщо((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    повернути CURLUE_BAD_HOSTNAME;
  повернути CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
static CURLUcode hostname_check6(struct Curl_URL *u, char *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  якщо(!hlen)
    повернути CURLUE_NO_HOST;
  інакше якщо(hostname[0] == '[')
    повернути ipv6_parse(u, hostname, hlen);

  повернути hostname_check(hostname, hlen);
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
UNITTEST ціле ipv4_normalize(struct dynbuf *host);
UNITTEST ціле ipv4_normalize(struct dynbuf *host)
{
  bool done = FALSE;
  ціле n = 0;
  const char *c = curlx_dyn_ptr(host);
  unsigned ціле parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  якщо(!ISDIGIT(*c))
    повернути HOST_NAME;

  поки(!done) {
    ціле rc;
    curl_off_t l;
    якщо(*c == '0') {
      якщо((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        якщо(rc)
          повернути HOST_NAME;
      }
      інакше
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    інакше
      rc = curlx_str_number(&c, &l, UINT_MAX);

    якщо(rc) {
      якщо(!n || (rc != STRE_NO_NUM) || *c)
        повернути HOST_NAME;
      n--;
    }
    інакше
      parts[n] = (unsigned ціле)l;

    switch(*c) {
    case '.':
      якщо(n == 3) {
        якщо(c[1])
          /* something follows this dot */
          повернути HOST_NAME;
        done = TRUE;
      }
      інакше {
        n++;
        c++;
      }
      break;

    case '\0':
      done = TRUE;
      break;

    default:
      повернути HOST_NAME;
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
    якщо((parts[0] > 0xff) || (parts[1] > 0xffffff))
      повернути HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    break;
  case 2: /* a.b.c -- 8.8.16 bits */
    якщо((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      повернути HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    break;
  case 3: /* a.b.c.d -- 8.8.8.8 bits */
    якщо((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      повернути HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    break;
  }
  якщо(result)
    повернути HOST_ERROR;
  повернути HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
static CURLUcode urldecode_host(struct dynbuf *host)
{
  const char *per;
  const char *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  якщо(!per)
    /* nothing to decode */
    повернути CURLUE_OK;
  інакше {
    /* encoded */
    size_t dlen;
    char *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    якщо(result)
      повернути CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    якщо(result)
      повернути cc2cu(result);
  }

  повернути CURLUE_OK;
}

static CURLUcode parse_authority(struct Curl_URL *u,
                                 const char *auth, size_t authlen,
                                 unsigned ціле flags,
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
  якщо(uc)
    повернути uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  якщо(result) {
    uc = cc2cu(result);
    повернути uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  якщо(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  якщо(!uc)
    uc = urldecode_host(host);
  якщо(uc)
    ;
  інакше якщо(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  інакше {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    ціле type = ipv4_normalize(host);

    якщо(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    інакше якщо(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  повернути uc;
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
  якщо(ures)
    curlx_dyn_free(&host);
  інакше {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  повернути ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

static bool is_dot(const char **str, size_t *clen)
{
  const char *p = *str;
  якщо(*p == '.') {
    (*str)++;
    (*clen)--;
    повернути TRUE;
  }
  інакше якщо((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    повернути TRUE;
  }
  повернути FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
static bool needs_dedotdot(const char *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  якщо(pn < 2)
    повернути FALSE;
  якщо(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    повернути FALSE;
  поки(pn) {
    якщо(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      якщо(!pn || ISSLASH(*p))
        повернути TRUE;
      /* "../" or ".." before end of string */
      інакше якщо(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        повернути TRUE;
    }
    інакше {
      p++;
      pn--;
    }
  }
  повернути FALSE;
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
UNITTEST ціле dedotdotify(const char *input, size_t clen, char **outp);
UNITTEST ціле dedotdotify(const char *input, size_t clen, char **outp)
{
  struct dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  const char *dinput = input;
  size_t dlen = clen;

  *outp = NULL;
  якщо(!needs_dedotdot(input, clen))
    повернути 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  якщо(is_dot(&dinput, &dlen)) {
    якщо(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      input = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    інакше якщо(is_dot(&dinput, &dlen)) {
      якщо(!dlen)
        /* .. [end] */
        goto end;
      інакше якщо(ISSLASH(*dinput)) {
        /* ../ */
        input = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  поки(clen && !result) { /* until end of path content */
    якщо(ISSLASH(*input)) {
      const char *p = &input[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      якщо(is_dot(&p, &blen)) {
        якщо(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          break;
        }
        інакше якщо(ISSLASH(*p)) { /* /./ */
          input = p;
          clen = blen;
          continue;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        інакше якщо(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          якщо(len) {
            const char *ptr = curlx_dyn_ptr(&out);
            const char *last = memrchr(ptr, '/', len);
            якщо(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          якщо(blen) { /* /../ */
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
  якщо(!result) {
    якщо(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    інакше {
      *outp = curlx_strdup("");
      якщо(!*outp)
        повернути 1;
    }
  }
  повернути result ? 1 : 0; /* success */
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
  якщо(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    повернути CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  якщо(path[0] != '/')
    повернути CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  якщо(path[1] == '/') {
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
    якщо(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      якщо(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      інакше
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        повернути CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#якщо !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  якщо(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    повернути CURLUE_BAD_FILE_URL;
  }
#інакше
  /* If the path starts with a slash and a drive letter, ditch the slash */
  якщо('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  якщо(!u->scheme)
    повернути CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  повернути CURLUE_OK;
}

static CURLUcode parse_scheme(const char *url, CURLU *u, char *schemebuf,
                              size_t schemelen, unsigned ціле flags,
                              const char **hostpp)
{
  /* clear path */
  const char *schemep = NULL;

  якщо(schemelen) {
    ціле num_slashes = 0;
    const char *p = &url[schemelen + 1];
    якщо(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      повернути CURLUE_UNSUPPORTED_SCHEME;

    якщо(!ISSLASH(*p))
      /* less than one */
      повернути CURLUE_BAD_SLASHES;
    якщо((flags & CURLU_NO_AUTHORITY)) {
      поки(ISSLASH(*p) && (num_slashes < 2)) {
        p++;
        num_slashes++;
      }
    }
    інакше {
      поки(ISSLASH(*p) && (num_slashes < 4)) {
        p++;
        num_slashes++;
      }
      якщо(num_slashes > 3)
        повернути CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  інакше {
    /* no scheme! */

    якщо(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      повернути CURLUE_BAD_SCHEME;

    якщо(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  якщо(schemep) {
    u->scheme = curlx_strdup(schemep);
    якщо(!u->scheme)
      повернути CURLUE_OUT_OF_MEMORY;
  }
  повернути CURLUE_OK;
}

static CURLUcode guess_scheme(CURLU *u, struct dynbuf *host)
{
  const char *hostname = curlx_dyn_ptr(host);
  const char *schemep = NULL;
  /* legacy curl-style guess based on hostname */
  якщо(checkprefix("ftp.", hostname))
    schemep = "ftp";
  інакше якщо(checkprefix("dict.", hostname))
    schemep = "dict";
  інакше якщо(checkprefix("ldap.", hostname))
    schemep = "ldap";
  інакше якщо(checkprefix("imap.", hostname))
    schemep = "imap";
  інакше якщо(checkprefix("smtp.", hostname))
    schemep = "smtp";
  інакше якщо(checkprefix("pop3.", hostname))
    schemep = "pop3";
  інакше
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  якщо(!u->scheme)
    повернути CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  повернути CURLUE_OK;
}

static CURLUcode handle_fragment(CURLU *u, const char *fragment,
                                 size_t fraglen, unsigned ціле flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  якщо(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    якщо(flags & CURLU_URLENCODE) {
      struct dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      якщо(ures)
        повернути ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    інакше {
      якщо(badoctets(fragment, fraglen, flags))
        повернути CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      якщо(!u->fragment)
        повернути CURLUE_OUT_OF_MEMORY;
    }
  }
  повернути CURLUE_OK;
}

static CURLUcode handle_query(CURLU *u, const char *query,
                              size_t qlen, unsigned ціле flags)
{
  u->query_present = TRUE;
  якщо(qlen > 1) {
    якщо(flags & CURLU_URLENCODE) {
      struct dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      якщо(ures)
        повернути ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    інакше {
      якщо(badoctets(query, qlen, flags))
        повернути CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      якщо(!u->query)
        повернути CURLUE_OUT_OF_MEMORY;
    }
  }
  інакше {
    /* single byte query */
    u->query = curlx_strdup("");
    якщо(!u->query)
      повернути CURLUE_OUT_OF_MEMORY;
  }
  повернути CURLUE_OK;
}

static CURLUcode handle_path(CURLU *u, const char *path,
                             size_t pathlen, unsigned ціле flags,
                             bool is_file)
{
  CURLUcode ures;
  якщо(pathlen && (flags & CURLU_URLENCODE)) {
    struct dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    якщо(ures)
      повернути ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  якщо(pathlen >= (size_t)(1 + !is_file)) {
    якщо(badoctets(path, pathlen, flags))
      повернути CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    якщо(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      якщо(!u->path)
        повернути CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    інакше якщо(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    якщо(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      char *dedot;
      ціле err = dedotdotify(path, pathlen, &dedot);
      якщо(err)
        повернути CURLUE_OUT_OF_MEMORY;
      якщо(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  повернути CURLUE_OK;
}

static CURLUcode parseurl(const char *url, CURLU *u, unsigned ціле flags)
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
  якщо(urllen > CURL_MAX_INPUT_LENGTH)
    повернути CURLUE_MALFORMED_INPUT;

  curlx_dyn_init(&host, CURL_MAX_INPUT_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, sizeof(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  якщо(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  інакше {
    const char *hostp = NULL;
    const char *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    якщо(ures)
      goto fail;

    /* find the end of the hostname + port number */
    p = hostp;
    поки(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    якщо(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      якщо(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    інакше якщо(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      якщо(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    інакше
      ures = CURLUE_NO_HOST;
  }
  якщо(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    const char *fragment = memchr(path, '#', pathlen);
    якщо(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  якщо(!ures) {
    const char *query = memchr(path, '?', pathlen);
    якщо(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  якщо(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  якщо(!ures) {
    u->host = curlx_dyn_ptr(&host);
    повернути CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  повернути ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
static CURLUcode parseurl_and_replace(const char *url, CURLU *u,
                                      unsigned ціле flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, sizeof(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  якщо(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  повернути ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
static CURLUcode redirect_url(const char *base, const char *relurl,
                              CURLU *u, unsigned ціле flags)
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
  якщо(!base)
    повернути CURLUE_MALFORMED_INPUT; /* should never happen */

  /* handle different relative URL types */
  switch(relurl[0]) {
  case '/':
    якщо(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    інакше
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    break;

  case '#':
    /* fragment-only change */
    якщо(u->fragment_present)
      cutoff = strchr(protsep, '#');
    break;

  default:
    /* path or query-only change */
    якщо(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    інакше якщо(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    якщо(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      якщо(cutoff)
        cutoff++; /* truncate after last slash */
    }
    break;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_INPUT_LENGTH);

  якщо(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  інакше
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  повернути uc;
}

/*
 */
CURLU *curl_url(void)
{
  повернути curlx_calloc(1, sizeof(struct Curl_URL));
}

void curl_url_cleanup(CURLU *u)
{
  якщо(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  do {                                          \
    якщо((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      якщо(!(dest)->name)                         \
        goto fail;                              \
    }                                           \
  } поки(0)

CURLU *curl_url_dup(const CURLU *in)
{
  struct Curl_URL *u;

  якщо(!in)
    повернути NULL;

  u = curlx_calloc(1, sizeof(struct Curl_URL));
  якщо(u) {
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
  повернути u;
fail:
  curl_url_cleanup(u);
  повернути NULL;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#інакше
static CURLUcode host_decode(const char *host, char **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  якщо(result)
    повернути (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  повернути CURLUE_OK;
}

static CURLUcode host_encode(const char *host, char **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  якщо(result)
    повернути (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  повернути CURLUE_OK;
}
#endif

static CURLUcode urlget_format(const CURLU *u, CURLUPart what,
                               const char *ptr, char **partp,
                               bool plusdecode, unsigned ціле flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  char *part = curlx_memdup0(ptr, partlen);
  *partp = NULL;
  якщо(!part)
    повернути CURLUE_OUT_OF_MEMORY;
  якщо(plusdecode) {
    /* convert + to space */
    char *plus = part;
    size_t i = 0;
    для(i = 0; i < partlen; ++plus, i++) {
      якщо(*plus == '+')
        *plus = ' ';
    }
  }
  якщо(urldecode) {
    char *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    якщо(result)
      повернути CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  якщо(urlencode) {
    struct dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    якщо(uc)
      повернути uc;
    part = curlx_dyn_ptr(&enc);
  }
  інакше якщо(punycode) {
    якщо(!Curl_is_ASCII_name(u->host)) {
      char *punyversion = NULL;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      якщо(uc)
        повернути uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      якщо(uc) {
        curlx_free(punyversion);
        повернути uc;
      }
      part = punyversion;
    }
  }
  інакше якщо(depunyfy && Curl_is_ASCII_name(u->host)) {
    char *unpunified = NULL;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    якщо(uc)
      повернути uc;
    part = unpunified;
  }
  *partp = part;
  повернути CURLUE_OK;
}

static CURLUcode file_url(const CURLU *u, char **part,
                          const char *fragmentsep,
                          const char *querysep)
{
  char *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  якщо(!url)
    повернути CURLUE_OUT_OF_MEMORY;

  *part = url;
  повернути CURLUE_OK;
}

static CURLUcode urlget_url(const CURLU *u, char **part, unsigned ціле flags)
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
  якщо(curl_strequal("file", u->scheme))
    повернути file_url(u, part, fragmentsep, querysep);
  інакше якщо(!u->host)
    повернути CURLUE_NO_HOST;
  інакше {
    const char *scheme;
    char *options = u->options;
    char *port = NULL;
    const struct Curl_scheme *h = NULL;
    char schemebuf[MAX_SCHEME_LEN + 5];
    якщо(u->scheme)
      scheme = u->scheme;
    інакше якщо(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    інакше
      повернути CURLUE_NO_SCHEME;

    якщо(u->port_present) {
      curl_msnprintf(portbuf, sizeof(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    якщо(h) {
      якщо(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, sizeof(portbuf), "%u", h->defport);
        port = portbuf;
      }
      інакше якщо(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = NULL;
      }

      якщо(!(h->flags & PROTOPT_URLOPTIONS))
        options = NULL;
    }

    якщо(u->host[0] == '[') {
      якщо(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        struct dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_INPUT_LENGTH);
        якщо(curlx_dyn_addf(&enc, "%.*s%%25%s]", (ціле)hostlen - 1, u->host,
                          u->zoneid))
          повернути CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    інакше якщо(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(NULL, u->host, 0);
      якщо(!allochost)
        повернути CURLUE_OUT_OF_MEMORY;
    }
    інакше якщо(flags & CURLU_PUNYCODE) {
      якщо(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        якщо(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        якщо(ret) {
          curlx_safefree(allochost);
          повернути ret;
        }
      }
    }
    інакше якщо(flags & CURLU_PUNY2IDN) {
      якщо(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        якщо(ret)
          повернути ret;
      }
    }

    якщо(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, sizeof(schemebuf), "%s://", scheme);
    інакше
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
  якщо(!url)
    повернути CURLUE_OUT_OF_MEMORY;
  *part = url;
  повернути CURLUE_OK;
}

CURLUcode curl_url_get(const CURLU *u, CURLUPart what,
                       char **part, unsigned ціле flags)
{
  const char *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  char portbuf[7];
  bool plusdecode = FALSE;
  якщо(!u)
    повернути CURLUE_BAD_HANDLE;
  якщо(!part)
    повернути CURLUE_BAD_PARTPOINTER;
  *part = NULL;

  switch(what) {
  case CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    якщо((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      повернути CURLUE_NO_SCHEME;
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
    якщо(u->port_present) {
      const struct Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : NULL;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      якщо(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = NULL;
      }
      інакше {
        curl_msnprintf(portbuf, sizeof(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    інакше якщо((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      const struct Curl_scheme *h = Curl_get_scheme(u->scheme);
      якщо(h) {
        curl_msnprintf(portbuf, sizeof(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    break;
  case CURLUPART_PATH:
    ptr = u->path;
    якщо(!ptr)
      ptr = "/";
    break;
  case CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    якщо(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = NULL;
    break;
  case CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    якщо(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    break;
  case CURLUPART_URL:
    повернути urlget_url(u, part, flags);
  default:
    ptr = NULL;
    break;
  }
  якщо(ptr)
    повернути urlget_format(u, what, ptr, part, plusdecode, flags);

  повернути ifmissing;
}

static CURLUcode set_url_scheme(CURLU *u, const char *scheme,
                                unsigned ціле flags)
{
  size_t plen = strlen(scheme);
  const struct Curl_scheme *h = NULL;
  якщо((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    повернути CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  якщо(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    повернути CURLUE_UNSUPPORTED_SCHEME;
  якщо(!h) {
    const char *s = scheme;
    якщо(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      поки(--plen) {
        якщо(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        інакше
          повернути CURLUE_BAD_SCHEME;
      }
    }
    інакше
      повернути CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  повернути CURLUE_OK;
}

static CURLUcode set_url_port(CURLU *u, const char *provided_port)
{
  curl_off_t port;
  якщо(!ISDIGIT(provided_port[0]))
    /* not a number */
    повернути CURLUE_BAD_PORT_NUMBER;
  якщо(curlx_str_number(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    повернути CURLUE_BAD_PORT_NUMBER;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  повернути CURLUE_OK;
}

static CURLUcode set_url(CURLU *u, const char *url, size_t part_size,
                         unsigned ціле flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  char *oldurl = NULL;

  якщо(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    якщо(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      повернути CURLUE_OK;
    }
    якщо(uc == CURLUE_OUT_OF_MEMORY)
      повернути uc;
    повернути CURLUE_MALFORMED_INPUT;
  }

  /* if the new URL is absolute replace the existing with the new. */
  якщо(Curl_is_absolute_url(url, NULL, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    повернути parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  якщо(uc == CURLUE_OUT_OF_MEMORY)
    повернути uc;
  інакше якщо(uc)
    повернути parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  повернути uc;
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
    повернути CURLUE_UNKNOWN_PART;
  }
  повернути CURLUE_OK;
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
    повернути TRUE;
  }
  повернути FALSE;
}

static CURLUcode url_encode_part(struct dynbuf *encp,
                                 const char *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  const unsigned char *i;

  для(i = (const unsigned char *)part; *i; i++) {
    CURLcode result;
    якщо((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    інакше якщо(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      якщо((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    інакше {
      unsigned char out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    якщо(result)
      повернути cc2cu(result);
  }
  повернути CURLUE_OK;
}

static CURLUcode url_uppercasehex_part(struct dynbuf *encp,
                                       const char *part)
{
  char *p;
  CURLcode result = curlx_dyn_add(encp, part);
  якщо(result)
    повернути cc2cu(result);
  p = curlx_dyn_ptr(encp);
  поки(*p) {
    /* make sure percent encoded are upper case */
    якщо((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    інакше
      p++;
  }
  повернути CURLUE_OK;
}

static CURLUcode url_append_query(CURLU *u, struct dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  якщо(querylen) {
    struct dynbuf qbuf;
    CURLcode result;
    const char *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_INPUT_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    якщо(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    якщо(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    якщо(result)
      goto nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    повернути CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    повернути cc2cu(result);
  }
  інакше {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  повернути CURLUE_OK;
}

static CURLUcode url_sethost(CURLU *u, struct dynbuf *encp,
                             bool urlencode,
                             unsigned ціле flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  char *newp = curlx_dyn_ptr(encp);
  якщо(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  інакше якщо(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    char *decoded = NULL;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    якщо(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    інакше {
      struct dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_INPUT_LENGTH);
      якщо(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      інакше якщо(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        якщо(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  інакше якщо(hostname_check6(u, newp, n))
    bad = TRUE;
  інакше якщо(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  якщо(bad) {
    curlx_dyn_free(encp);
    повернути CURLUE_BAD_HOSTNAME;
  }
  повернути CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       const char *part, unsigned ціле flags)
{
  char **storep = NULL;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  якщо(!u)
    повернути CURLUE_BAD_HANDLE;
  якщо(!part)
    /* setting a part to NULL clears it */
    повернути urlset_clear(u, what);

  nalloc = strlen(part);
  якщо(nalloc > CURL_MAX_INPUT_LENGTH)
    /* excessive input length */
    повернути CURLUE_MALFORMED_INPUT;

  switch(what) {
  case CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    якщо(status)
      повернути status;
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
    повернути set_url_port(u, part);
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
    повернути set_url(u, part, nalloc, flags);
  default:
    повернути CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    const char *newp = NULL;
    struct dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    якщо(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      якщо(result)
        повернути cc2cu(result);
    }
    якщо(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    інакше
      status = url_uppercasehex_part(&enc, part);
    якщо(!status) {
      newp = curlx_dyn_ptr(&enc);

      якщо(appendquery && newp)
        повернути url_append_query(u, &enc);
      інакше якщо(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    якщо(status)
      повернути status;

    якщо(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (char *)CURL_UNCONST(newp);
  }
  повернути CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  const struct Curl_scheme *s = NULL;

  /* base must be an absolute URL */
  якщо(!base->scheme || !base->host)
    повернути FALSE;
  якщо(href->scheme && !curl_strequal(base->scheme, href->scheme))
    повернути FALSE;
  якщо(href->host) {
    якщо(!curl_strequal(base->host, href->host))
      повернути FALSE;

    якщо(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      якщо(!s) /* Cannot match default port for unknown scheme */
        повернути FALSE;
      /* to match, the present one must be the default port */
      якщо((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        повернути FALSE;
    }
    інакше якщо(base->portnum != href->portnum) /* both present or missing */
      повернути FALSE;

    якщо(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      повернути FALSE;
  }
  інакше якщо(href->port_present) /* no host in href, then there must be no port */
    повернути FALSE;
  повернути TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  якщо(u->port_present) {
    *pport = u->portnum;
    повернути CURLUE_OK;
  }
  інакше якщо(u->scheme) {
    const struct Curl_scheme *s = Curl_get_scheme(u->scheme);
    якщо(s && s->defport) {
      *pport = s->defport;
      повернути CURLUE_OK;
    }
  }
  *pport = 0;
  повернути CURLUE_NO_PORT;
}
