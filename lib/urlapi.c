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

#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "urlapi-int.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"
#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "escape.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_pton.h"
#ಅನ್ತರ್ಭಾವ "curlx/inet_ntop.h"
#ಅನ್ತರ್ಭಾವ "curlx/strdup.h"
#ಅನ್ತರ್ಭಾವ "idn.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curl_memrchr.h"

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
#ಯದಿ !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

ಸ್ಥಿರ ಶೂನ್ಯ free_urlhandle(ಸಂರಚನಾ Curl_URL *u)
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
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *find_host_sep(ಧ್ರುವ ವರ್ಣ *url)
{
  /* Find the start of the hostname */
  ಧ್ರುವ ವರ್ಣ *sep = strstr(url, "//");
  ಯದಿ(!sep)
    sep = url;
  ಅನ್ಯಥಾ
    sep += 2;

  /* Find first / or ? */
  ಯಾವತ್(*sep && *sep != '/' && *sep != '?')
    sep++;

  ಪ್ರತ್ಯಾಗಮ sep;
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
UNITTEST CURLUcode urlencode_str(ಸಂರಚನಾ dynbuf *o, ಧ್ರುವ ವರ್ಣ *url,
                                 size_t len, bool relative,
                                 unsigned ಪೂರ್ಣಾಙ್ಕ query);
UNITTEST CURLUcode urlencode_str(ಸಂರಚನಾ dynbuf *o, ಧ್ರುವ ವರ್ಣ *url,
                                 size_t len, bool relative,
                                 unsigned ಪೂರ್ಣಾಙ್ಕ query)
{
  /* we must add this with whitespace-replacing */
  ಧ್ರುವ unsigned ವರ್ಣ *iptr;
  ಧ್ರುವ unsigned ವರ್ಣ *host_sep = (ಧ್ರುವ unsigned ವರ್ಣ *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  ಯದಿ(!relative) {
    size_t n;
    host_sep = (ಧ್ರುವ unsigned ವರ್ಣ *)find_host_sep(url);

    /* output the first piece as-is */
    n = (ಧ್ರುವ ವರ್ಣ *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  ಪ್ರತಿ(iptr = host_sep; len && !result;) {
    ಯದಿ(*iptr == ' ') {
      ಯದಿ(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      ಅನ್ಯಥಾ
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    ಅನ್ಯಥಾ ಯದಿ((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned ವರ್ಣ out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    ಅನ್ಯಥಾ ಯದಿ(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned ವರ್ಣ hex = (unsigned ವರ್ಣ)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned ವರ್ಣ out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    ಅನ್ಯಥಾ {
      ಧ್ರುವ unsigned ವರ್ಣ *start = iptr;
      ಯಾವತ್(len) {
        ಯದಿ(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          ಭಙ್ಗ;
        ಯದಿ(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          ಭಙ್ಗ;
        ಯದಿ(*iptr == '?') {
          ಯದಿ(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            ಭಙ್ಗ;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (ಧ್ರುವ ವರ್ಣ *)start, (size_t)(iptr - start));
    }
  }

  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ cc2cu(result);
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ಧ್ರುವ ವರ್ಣ *url, ವರ್ಣ *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (ಶೂನ್ಯ)buflen; /* only used in debug-builds */
  ಯದಿ(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  ಯದಿ(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    ಪ್ರತ್ಯಾಗಮ 0;
#endif
  ಯದಿ(ISALPHA(url[0])) {
    ಯದಿ(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    ಪ್ರತಿ(i = 1; i < MAX_SCHEME_LEN; ++i) {
      ವರ್ಣ s = url[i];
      ಯದಿ(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        ಯದಿ(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      ಅನ್ಯಥಾ {
        ಭಙ್ಗ;
      }
    }
  }
  ಯದಿ(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    ಯದಿ(buf)
      buf[i] = 0;
    ಪ್ರತ್ಯಾಗಮ len;
  }
  ಯದಿ(buf)
    buf[0] = 0;
  ಪ್ರತ್ಯಾಗಮ 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ಸ್ಥಿರ bool badoctets(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, size_t n, ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಧ್ರುವ uint8_t *p = (ಧ್ರುವ unsigned ವರ್ಣ *)ಒಳಹರಿವು;
  ಧ್ರುವ uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  ಯಾವತ್(n--) {
    ಯದಿ(*p <= control || *p == 127)
      ಪ್ರತ್ಯಾಗಮ TRUE;
    p++;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ಸಂರಚನಾ Curl_URL *u,
                                        ಧ್ರುವ ವರ್ಣ *login,
                                        size_t len,
                                        unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(ಸಂರಚನಾ Curl_URL *u,
                                        ಧ್ರುವ ವರ್ಣ *login,
                                        size_t len,
                                        unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  ವರ್ಣ *userp = ರಿಕ್ತ;
  ವರ್ಣ *passwdp = ರಿಕ್ತ;
  ವರ್ಣ *optionsp = ರಿಕ್ತ;
  ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *h = ರಿಕ್ತ;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ಧ್ರುವ ವರ್ಣ *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  ಯದಿ(!ptr)
    ಗಚ್ಛ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  ಯದಿ(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ರಿಕ್ತ);
  ಯದಿ(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    ಗಚ್ಛ out;
  }

  ಯದಿ(userp) {
    ಯದಿ(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      ಗಚ್ಛ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  ಯದಿ(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  ಯದಿ(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  ಯದಿ(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  ಅನ್ಯಥಾ ಯದಿ(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  ಅನ್ಯಥಾ ಯದಿ(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_ಒಳಹರಿವು;

  userp = passwdp = optionsp = ರಿಕ್ತ;

  ಯದಿ(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
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

  ಪ್ರತ್ಯಾಗಮ ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(ಸಂರಚನಾ Curl_URL *u, ಸಂರಚನಾ dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(ಸಂರಚನಾ Curl_URL *u, ಸಂರಚನಾ dynbuf *host,
                              bool has_scheme)
{
  ಧ್ರುವ ವರ್ಣ *portptr;
  ಧ್ರುವ ವರ್ಣ *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  ಯದಿ(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    ಯದಿ(!portptr)
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    ಯದಿ(*portptr) {
      ಯದಿ(*portptr != ':')
        ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PORT_ಸಂಖ್ಯೆ;
    }
    ಅನ್ಯಥಾ
      portptr = ರಿಕ್ತ;
  }
  ಅನ್ಯಥಾ
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  ಯದಿ(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    ಪೂರ್ಣಾಙ್ಕ rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    ಯದಿ(!*portptr)
      ಪ್ರತ್ಯಾಗಮ has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_ಸಂಖ್ಯೆ;
    ಯದಿ(*portptr == '\\')
      ಪ್ರತ್ಯಾಗಮ CURLUE_BACKSLASH;
    rc = curlx_str_ಸಂಖ್ಯೆ(&portptr, &port, 0xffff);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PORT_ಸಂಖ್ಯೆ;
    ಅನ್ಯಥಾ ಯದಿ(*portptr == '\\')
      ಪ್ರತ್ಯಾಗಮ CURLUE_BACKSLASH;
    ಅನ್ಯಥಾ ಯದಿ(*portptr)
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PORT_ಸಂಖ್ಯೆ;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ಸ್ಥಿರ ಧ್ರುವ bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(ಸಂರಚನಾ Curl_URL *u, ವರ್ಣ *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ಸಂರಚನಾ Curl_URL *u, ವರ್ಣ *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  ಯದಿ(hlen < 4) /* '[::]' is the shortest possible valid string */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  ಯದಿ(hlen != len) {
    hlen = len;
    ಯದಿ(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      ವರ್ಣ zoneid[MAX_ZONEID_LEN];
      ಪೂರ್ಣಾಙ್ಕ i = 0;
      ವರ್ಣ *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      ಯದಿ(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      ಯಾವತ್(!invalid_host_char[(unsigned ವರ್ಣ)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      ಯದಿ(!i || (']' != *h))
        ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      ಯದಿ(!u->zoneid)
        ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    ವರ್ಣ dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    ಯದಿ(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_IPV6;
    ಯದಿ(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ಸ್ಥಿರ CURLUcode hostname_check(ವರ್ಣ *hostname, size_t hlen)
{
  size_t i;
  ಪ್ರತಿ(i = 0; i < hlen; i++) {
    ಯದಿ(invalid_host_char[(unsigned ವರ್ಣ)hostname[i]])
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HOSTNAME;
  }
  ಯದಿ((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HOSTNAME;
  ಅನ್ಯಥಾ ಯದಿ((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HOSTNAME;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ಸ್ಥಿರ CURLUcode hostname_check6(ಸಂರಚನಾ Curl_URL *u, ವರ್ಣ *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  ಯದಿ(!hlen)
    ಪ್ರತ್ಯಾಗಮ CURLUE_NO_HOST;
  ಅನ್ಯಥಾ ಯದಿ(hostname[0] == '[')
    ಪ್ರತ್ಯಾಗಮ ipv6_parse(u, hostname, hlen);

  ಪ್ರತ್ಯಾಗಮ hostname_check(hostname, hlen);
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
UNITTEST ಪೂರ್ಣಾಙ್ಕ ipv4_normalize(ಸಂರಚನಾ dynbuf *host);
UNITTEST ಪೂರ್ಣಾಙ್ಕ ipv4_normalize(ಸಂರಚನಾ dynbuf *host)
{
  bool done = FALSE;
  ಪೂರ್ಣಾಙ್ಕ n = 0;
  ಧ್ರುವ ವರ್ಣ *c = curlx_dyn_ptr(host);
  unsigned ಪೂರ್ಣಾಙ್ಕ parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  ಯದಿ(!ISDIGIT(*c))
    ಪ್ರತ್ಯಾಗಮ HOST_NAME;

  ಯಾವತ್(!done) {
    ಪೂರ್ಣಾಙ್ಕ rc;
    curl_off_t l;
    ಯದಿ(*c == '0') {
      ಯದಿ((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        ಯದಿ(rc)
          ಪ್ರತ್ಯಾಗಮ HOST_NAME;
      }
      ಅನ್ಯಥಾ
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    ಅನ್ಯಥಾ
      rc = curlx_str_ಸಂಖ್ಯೆ(&c, &l, UINT_MAX);

    ಯದಿ(rc) {
      ಯದಿ(!n || (rc != STRE_NO_ಅಂಕೆ) || *c)
        ಪ್ರತ್ಯಾಗಮ HOST_NAME;
      n--;
    }
    ಅನ್ಯಥಾ
      parts[n] = (unsigned ಪೂರ್ಣಾಙ್ಕ)l;

    ವಿಕಲ್ಪ(*c) {
    ಪಕ್ಷ '.':
      ಯದಿ(n == 3) {
        ಯದಿ(c[1])
          /* something follows this dot */
          ಪ್ರತ್ಯಾಗಮ HOST_NAME;
        done = TRUE;
      }
      ಅನ್ಯಥಾ {
        n++;
        c++;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ '\0':
      done = TRUE;
      ಭಙ್ಗ;

    ಸ್ವಭಾವ:
      ಪ್ರತ್ಯಾಗಮ HOST_NAME;
    }
  }

  ವಿಕಲ್ಪ(n) {
  ಪಕ್ಷ 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    ಭಙ್ಗ;
  ಪಕ್ಷ 1: /* a.b -- 8.24 bits */
    ಯದಿ((parts[0] > 0xff) || (parts[1] > 0xffffff))
      ಪ್ರತ್ಯಾಗಮ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    ಭಙ್ಗ;
  ಪಕ್ಷ 2: /* a.b.c -- 8.8.16 bits */
    ಯದಿ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      ಪ್ರತ್ಯಾಗಮ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    ಭಙ್ಗ;
  ಪಕ್ಷ 3: /* a.b.c.d -- 8.8.8.8 bits */
    ಯದಿ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      ಪ್ರತ್ಯಾಗಮ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    ಭಙ್ಗ;
  }
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ HOST_ERROR;
  ಪ್ರತ್ಯಾಗಮ HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ಸ್ಥಿರ CURLUcode urldecode_host(ಸಂರಚನಾ dynbuf *host)
{
  ಧ್ರುವ ವರ್ಣ *per;
  ಧ್ರುವ ವರ್ಣ *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  ಯದಿ(!per)
    /* nothing to decode */
    ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
  ಅನ್ಯಥಾ {
    /* encoded */
    size_t dlen;
    ವರ್ಣ *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ cc2cu(result);
  }

  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode parse_authority(ಸಂರಚನಾ Curl_URL *u,
                                 ಧ್ರುವ ವರ್ಣ *auth, size_t authlen,
                                 unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                                 ಸಂರಚನಾ dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  ಯದಿ(uc)
    ಪ್ರತ್ಯಾಗಮ uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  ಯದಿ(result) {
    uc = cc2cu(result);
    ಪ್ರತ್ಯಾಗಮ uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  ಯದಿ(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  ಯದಿ(!uc)
    uc = urldecode_host(host);
  ಯದಿ(uc)
    ;
  ಅನ್ಯಥಾ ಯದಿ(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  ಅನ್ಯಥಾ {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    ಪೂರ್ಣಾಙ್ಕ type = ipv4_normalize(host);

    ಯದಿ(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    ಅನ್ಯಥಾ ಯದಿ(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  ಪ್ರತ್ಯಾಗಮ uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, ಧ್ರುವ ವರ್ಣ *authority)
{
  CURLUcode ures;
  ಸಂರಚನಾ dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_ಒಳಹರಿವು_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  ಯದಿ(ures)
    curlx_dyn_free(&host);
  ಅನ್ಯಥಾ {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  ಪ್ರತ್ಯಾಗಮ ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ಸ್ಥಿರ bool is_dot(ಧ್ರುವ ವರ್ಣ **str, size_t *clen)
{
  ಧ್ರುವ ವರ್ಣ *p = *str;
  ಯದಿ(*p == '.') {
    (*str)++;
    (*clen)--;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಅನ್ಯಥಾ ಯದಿ((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ಸ್ಥಿರ bool needs_dedotdot(ಧ್ರುವ ವರ್ಣ *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  ಯದಿ(pn < 2)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯಾವತ್(pn) {
    ಯದಿ(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      ಯದಿ(!pn || ISSLASH(*p))
        ಪ್ರತ್ಯಾಗಮ TRUE;
      /* "../" or ".." before end of string */
      ಅನ್ಯಥಾ ಯದಿ(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಅನ್ಯಥಾ {
      p++;
      pn--;
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
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
UNITTEST ಪೂರ್ಣಾಙ್ಕ dedotdotify(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, size_t clen, ವರ್ಣ **outp);
UNITTEST ಪೂರ್ಣಾಙ್ಕ dedotdotify(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು, size_t clen, ವರ್ಣ **outp)
{
  ಸಂರಚನಾ dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  ಧ್ರುವ ವರ್ಣ *dinput = ಒಳಹರಿವು;
  size_t dlen = clen;

  *outp = ರಿಕ್ತ;
  ಯದಿ(!needs_dedotdot(ಒಳಹರಿವು, clen))
    ಪ್ರತ್ಯಾಗಮ 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  ಯದಿ(is_dot(&dinput, &dlen)) {
    ಯದಿ(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      ಒಳಹರಿವು = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    ಅನ್ಯಥಾ ಯದಿ(is_dot(&dinput, &dlen)) {
      ಯದಿ(!dlen)
        /* .. [end] */
        ಗಚ್ಛ end;
      ಅನ್ಯಥಾ ಯದಿ(ISSLASH(*dinput)) {
        /* ../ */
        ಒಳಹರಿವು = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  ಯಾವತ್(clen && !result) { /* until end of path content */
    ಯದಿ(ISSLASH(*ಒಳಹರಿವು)) {
      ಧ್ರುವ ವರ್ಣ *p = &ಒಳಹರಿವು[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      ಯದಿ(is_dot(&p, &blen)) {
        ಯದಿ(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          ಭಙ್ಗ;
        }
        ಅನ್ಯಥಾ ಯದಿ(ISSLASH(*p)) { /* /./ */
          ಒಳಹರಿವು = p;
          clen = blen;
          ಅನುವರ್ತ;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        ಅನ್ಯಥಾ ಯದಿ(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          ಯದಿ(len) {
            ಧ್ರುವ ವರ್ಣ *ptr = curlx_dyn_ptr(&out);
            ಧ್ರುವ ವರ್ಣ *last = memrchr(ptr, '/', len);
            ಯದಿ(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          ಯದಿ(blen) { /* /../ */
            ಒಳಹರಿವು = p;
            clen = blen;
            ಅನುವರ್ತ;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          ಭಙ್ಗ;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, ಒಳಹರಿವು, 1);
    ಒಳಹರಿವು++;
    clen--;
  }
end:
  ಯದಿ(!result) {
    ಯದಿ(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    ಅನ್ಯಥಾ {
      *outp = curlx_strdup("");
      ಯದಿ(!*outp)
        ಪ್ರತ್ಯಾಗಮ 1;
    }
  }
  ಪ್ರತ್ಯಾಗಮ result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ಧ್ರುವ ವರ್ಣ *url, size_t urllen, CURLU *u,
                              ಧ್ರುವ ವರ್ಣ **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ಧ್ರುವ ವರ್ಣ *url, size_t urllen, CURLU *u,
                              ಧ್ರುವ ವರ್ಣ **pathp, size_t *pathlenp)
{
  ಧ್ರುವ ವರ್ಣ *path;
  size_t pathlen;

  *pathp = ರಿಕ್ತ;
  *pathlenp = 0;
  ಯದಿ(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  ಯದಿ(path[0] != '/')
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  ಯದಿ(path[1] == '/') {
    /* swallow the two slashes */
    ಧ್ರುವ ವರ್ಣ *ptr = &path[2];

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
    ಯದಿ(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      ಯದಿ(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      ಅನ್ಯಥಾ
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#ಯದಿ !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  ಯದಿ(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_FILE_URL;
  }
#ಅನ್ಯಥಾ
  /* If the path starts with a slash and a drive letter, ditch the slash */
  ಯದಿ('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  ಯದಿ(!u->scheme)
    ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode parse_scheme(ಧ್ರುವ ವರ್ಣ *url, CURLU *u, ವರ್ಣ *schemebuf,
                              size_t schemelen, unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                              ಧ್ರುವ ವರ್ಣ **hostpp)
{
  /* clear path */
  ಧ್ರುವ ವರ್ಣ *schemep = ರಿಕ್ತ;

  ಯದಿ(schemelen) {
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_slashes = 0;
    ಧ್ರುವ ವರ್ಣ *p = &url[schemelen + 1];
    ಯದಿ(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      ಪ್ರತ್ಯಾಗಮ CURLUE_UNSUPPORTED_SCHEME;

    ಯದಿ(!ISSLASH(*p))
      /* less than one */
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_SLASHES;
    ಯದಿ((flags & CURLU_NO_AUTHORITY)) {
      ಯಾವತ್(ISSLASH(*p) && (ಅಂಕೆ_slashes < 2)) {
        p++;
        ಅಂಕೆ_slashes++;
      }
    }
    ಅನ್ಯಥಾ {
      ಯಾವತ್(ISSLASH(*p) && (ಅಂಕೆ_slashes < 4)) {
        p++;
        ಅಂಕೆ_slashes++;
      }
      ಯದಿ(ಅಂಕೆ_slashes > 3)
        ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  ಅನ್ಯಥಾ {
    /* no scheme! */

    ಯದಿ(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_SCHEME;

    ಯದಿ(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  ಯದಿ(schemep) {
    u->scheme = curlx_strdup(schemep);
    ಯದಿ(!u->scheme)
      ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode guess_scheme(CURLU *u, ಸಂರಚನಾ dynbuf *host)
{
  ಧ್ರುವ ವರ್ಣ *hostname = curlx_dyn_ptr(host);
  ಧ್ರುವ ವರ್ಣ *schemep = ರಿಕ್ತ;
  /* legacy curl-style guess based on hostname */
  ಯದಿ(checkprefix("ftp.", hostname))
    schemep = "ftp";
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("dict.", hostname))
    schemep = "dict";
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("ldap.", hostname))
    schemep = "ldap";
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("imap.", hostname))
    schemep = "imap";
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("smtp.", hostname))
    schemep = "smtp";
  ಅನ್ಯಥಾ ಯದಿ(checkprefix("pop3.", hostname))
    schemep = "pop3";
  ಅನ್ಯಥಾ
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  ಯದಿ(!u->scheme)
    ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode handle_fragment(CURLU *u, ಧ್ರುವ ವರ್ಣ *fragment,
                                 size_t fraglen, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  ಯದಿ(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    ಯದಿ(flags & CURLU_URLENCODE) {
      ಸಂರಚನಾ dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      ಯದಿ(ures)
        ಪ್ರತ್ಯಾಗಮ ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(badoctets(fragment, fraglen, flags))
        ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      ಯದಿ(!u->fragment)
        ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode handle_query(CURLU *u, ಧ್ರುವ ವರ್ಣ *query,
                              size_t qlen, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  u->query_present = TRUE;
  ಯದಿ(qlen > 1) {
    ಯದಿ(flags & CURLU_URLENCODE) {
      ಸಂರಚನಾ dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      ಯದಿ(ures)
        ಪ್ರತ್ಯಾಗಮ ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    ಅನ್ಯಥಾ {
      ಯದಿ(badoctets(query, qlen, flags))
        ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      ಯದಿ(!u->query)
        ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
    }
  }
  ಅನ್ಯಥಾ {
    /* single byte query */
    u->query = curlx_strdup("");
    ಯದಿ(!u->query)
      ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode handle_path(CURLU *u, ಧ್ರುವ ವರ್ಣ *path,
                             size_t pathlen, unsigned ಪೂರ್ಣಾಙ್ಕ flags,
                             bool is_file)
{
  CURLUcode ures;
  ಯದಿ(pathlen && (flags & CURLU_URLENCODE)) {
    ಸಂರಚನಾ dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    ಯದಿ(ures)
      ಪ್ರತ್ಯಾಗಮ ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  ಯದಿ(pathlen >= (size_t)(1 + !is_file)) {
    ಯದಿ(badoctets(path, pathlen, flags))
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    ಯದಿ(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      ಯದಿ(!u->path)
        ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    ಅನ್ಯಥಾ ಯದಿ(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    ಯದಿ(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      ವರ್ಣ *dedot;
      ಪೂರ್ಣಾಙ್ಕ err = dedotdotify(path, pathlen, &dedot);
      ಯದಿ(err)
        ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
      ಯದಿ(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode parseurl(ಧ್ರುವ ವರ್ಣ *url, CURLU *u, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಧ್ರುವ ವರ್ಣ *path;
  size_t pathlen;
  ವರ್ಣ schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ಸಂರಚನಾ dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  ಯದಿ(urllen > CURL_MAX_ಒಳಹರಿವು_LENGTH)
    ಪ್ರತ್ಯಾಗಮ CURLUE_MALFORMED_ಒಳಹರಿವು;

  curlx_dyn_init(&host, CURL_MAX_ಒಳಹರಿವು_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, ಪರಿಮಾಣ(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  ಯದಿ(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  ಅನ್ಯಥಾ {
    ಧ್ರುವ ವರ್ಣ *hostp = ರಿಕ್ತ;
    ಧ್ರುವ ವರ್ಣ *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    ಯದಿ(ures)
      ಗಚ್ಛ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    ಯಾವತ್(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    ಯದಿ(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      ಯದಿ(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    ಅನ್ಯಥಾ ಯದಿ(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      ಯದಿ(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    ಅನ್ಯಥಾ
      ures = CURLUE_NO_HOST;
  }
  ಯದಿ(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ಧ್ರುವ ವರ್ಣ *fragment = memchr(path, '#', pathlen);
    ಯದಿ(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  ಯದಿ(!ures) {
    ಧ್ರುವ ವರ್ಣ *query = memchr(path, '?', pathlen);
    ಯದಿ(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  ಯದಿ(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  ಯದಿ(!ures) {
    u->host = curlx_dyn_ptr(&host);
    ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  ಪ್ರತ್ಯಾಗಮ ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ಸ್ಥಿರ CURLUcode parseurl_and_replace(ಧ್ರುವ ವರ್ಣ *url, CURLU *u,
                                      unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, ಪರಿಮಾಣ(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  ಯದಿ(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  ಪ್ರತ್ಯಾಗಮ ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ಸ್ಥಿರ CURLUcode redirect_url(ಧ್ರುವ ವರ್ಣ *base, ಧ್ರುವ ವರ್ಣ *relurl,
                              CURLU *u, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಸಂರಚನಾ dynbuf urlbuf;
  bool host_changed = FALSE;
  ಧ್ರುವ ವರ್ಣ *useurl = relurl;
  ಧ್ರುವ ವರ್ಣ *cutoff = ರಿಕ್ತ;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ಧ್ರುವ ವರ್ಣ *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ಧ್ರುವ ವರ್ಣ *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  ಯದಿ(!base)
    ಪ್ರತ್ಯಾಗಮ CURLUE_MALFORMED_ಒಳಹರಿವು; /* should never happen */

  /* handle different relative URL types */
  ವಿಕಲ್ಪ(relurl[0]) {
  ಪಕ್ಷ '/':
    ಯದಿ(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    ಅನ್ಯಥಾ
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    ಭಙ್ಗ;

  ಪಕ್ಷ '#':
    /* fragment-only change */
    ಯದಿ(u->fragment_present)
      cutoff = strchr(protsep, '#');
    ಭಙ್ಗ;

  ಸ್ವಭಾವ:
    /* path or query-only change */
    ಯದಿ(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    ಅನ್ಯಥಾ ಯದಿ(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    ಯದಿ(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      ಯದಿ(cutoff)
        cutoff++; /* truncate after last slash */
    }
    ಭಙ್ಗ;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_ಒಳಹರಿವು_LENGTH);

  ಯದಿ(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  ಅನ್ಯಥಾ
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  ಪ್ರತ್ಯಾಗಮ uc;
}

/*
 */
CURLU *curl_url(ಶೂನ್ಯ)
{
  ಪ್ರತ್ಯಾಗಮ curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_URL));
}

ಶೂನ್ಯ curl_url_cleanup(CURLU *u)
{
  ಯದಿ(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  ಕುರು {                                          \
    ಯದಿ((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      ಯದಿ(!(dest)->name)                         \
        ಗಚ್ಛ fail;                              \
    }                                           \
  } ಯಾವತ್(0)

CURLU *curl_url_dup(ಧ್ರುವ CURLU *in)
{
  ಸಂರಚನಾ Curl_URL *u;

  ಯದಿ(!in)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  u = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_URL));
  ಯದಿ(u) {
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
  ಪ್ರತ್ಯಾಗಮ u;
fail:
  curl_url_cleanup(u);
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#ಅನ್ಯಥಾ
ಸ್ಥಿರ CURLUcode host_decode(ಧ್ರುವ ವರ್ಣ *host, ವರ್ಣ **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode host_encode(ಧ್ರುವ ವರ್ಣ *host, ವರ್ಣ **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}
#endif

ಸ್ಥಿರ CURLUcode urlget_format(ಧ್ರುವ CURLU *u, CURLUPart what,
                               ಧ್ರುವ ವರ್ಣ *ptr, ವರ್ಣ **partp,
                               bool plusdecode, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  ವರ್ಣ *part = curlx_memdup0(ptr, partlen);
  *partp = ರಿಕ್ತ;
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
  ಯದಿ(plusdecode) {
    /* convert + to space */
    ವರ್ಣ *plus = part;
    size_t i = 0;
    ಪ್ರತಿ(i = 0; i < partlen; ++plus, i++) {
      ಯದಿ(*plus == '+')
        *plus = ' ';
    }
  }
  ಯದಿ(urldecode) {
    ವರ್ಣ *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  ಯದಿ(urlencode) {
    ಸಂರಚನಾ dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    ಯದಿ(uc)
      ಪ್ರತ್ಯಾಗಮ uc;
    part = curlx_dyn_ptr(&enc);
  }
  ಅನ್ಯಥಾ ಯದಿ(punycode) {
    ಯದಿ(!Curl_is_ASCII_name(u->host)) {
      ವರ್ಣ *punyversion = ರಿಕ್ತ;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      ಯದಿ(uc)
        ಪ್ರತ್ಯಾಗಮ uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      ಯದಿ(uc) {
        curlx_free(punyversion);
        ಪ್ರತ್ಯಾಗಮ uc;
      }
      part = punyversion;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(depunyfy && Curl_is_ASCII_name(u->host)) {
    ವರ್ಣ *unpunified = ರಿಕ್ತ;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    ಯದಿ(uc)
      ಪ್ರತ್ಯಾಗಮ uc;
    part = unpunified;
  }
  *partp = part;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode file_url(ಧ್ರುವ CURLU *u, ವರ್ಣ **part,
                          ಧ್ರುವ ವರ್ಣ *fragmentsep,
                          ಧ್ರುವ ವರ್ಣ *querysep)
{
  ವರ್ಣ *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  ಯದಿ(!url)
    ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;

  *part = url;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode urlget_url(ಧ್ರುವ CURLU *u, ವರ್ಣ **part, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  ವರ್ಣ *url;
  ವರ್ಣ *allochost = ರಿಕ್ತ;
  ಧ್ರುವ ವರ್ಣ *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  ಧ್ರುವ ವರ್ಣ *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  ವರ್ಣ portbuf[7];
  ಯದಿ(curl_strequal("file", u->scheme))
    ಪ್ರತ್ಯಾಗಮ file_url(u, part, fragmentsep, querysep);
  ಅನ್ಯಥಾ ಯದಿ(!u->host)
    ಪ್ರತ್ಯಾಗಮ CURLUE_NO_HOST;
  ಅನ್ಯಥಾ {
    ಧ್ರುವ ವರ್ಣ *scheme;
    ವರ್ಣ *options = u->options;
    ವರ್ಣ *port = ರಿಕ್ತ;
    ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *h = ರಿಕ್ತ;
    ವರ್ಣ schemebuf[MAX_SCHEME_LEN + 5];
    ಯದಿ(u->scheme)
      scheme = u->scheme;
    ಅನ್ಯಥಾ ಯದಿ(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLUE_NO_SCHEME;

    ಯದಿ(u->port_present) {
      curl_msnprintf(portbuf, ಪರಿಮಾಣ(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    ಯದಿ(h) {
      ಯದಿ(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, ಪರಿಮಾಣ(portbuf), "%u", h->defport);
        port = portbuf;
      }
      ಅನ್ಯಥಾ ಯದಿ(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = ರಿಕ್ತ;
      }

      ಯದಿ(!(h->flags & PROTOPT_URLOPTIONS))
        options = ರಿಕ್ತ;
    }

    ಯದಿ(u->host[0] == '[') {
      ಯದಿ(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ಸಂರಚನಾ dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_LENGTH);
        ಯದಿ(curlx_dyn_addf(&enc, "%.*s%%25%s]", (ಪೂರ್ಣಾಙ್ಕ)hostlen - 1, u->host,
                          u->zoneid))
          ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ರಿಕ್ತ, u->host, 0);
      ಯದಿ(!allochost)
        ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
    }
    ಅನ್ಯಥಾ ಯದಿ(flags & CURLU_PUNYCODE) {
      ಯದಿ(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        ಯದಿ(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        ಯದಿ(ret) {
          curlx_safefree(allochost);
          ಪ್ರತ್ಯಾಗಮ ret;
        }
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(flags & CURLU_PUNY2IDN) {
      ಯದಿ(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        ಯದಿ(ret)
          ಪ್ರತ್ಯಾಗಮ ret;
      }
    }

    ಯದಿ(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, ಪರಿಮಾಣ(schemebuf), "%s://", scheme);
    ಅನ್ಯಥಾ
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
  ಯದಿ(!url)
    ಪ್ರತ್ಯಾಗಮ CURLUE_OUT_OF_MEMORY;
  *part = url;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

CURLUcode curl_url_get(ಧ್ರುವ CURLU *u, CURLUPart what,
                       ವರ್ಣ **part, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  ಧ್ರುವ ವರ್ಣ *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  ವರ್ಣ portbuf[7];
  bool plusdecode = FALSE;
  ಯದಿ(!u)
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HANDLE;
  ಯದಿ(!part)
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PARTPOINTER;
  *part = ರಿಕ್ತ;

  ವಿಕಲ್ಪ(what) {
  ಪಕ್ಷ CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    ಯದಿ((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      ಪ್ರತ್ಯಾಗಮ CURLUE_NO_SCHEME;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PORT:
    ptr = ರಿಕ್ತ;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    ಯದಿ(u->port_present) {
      ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ರಿಕ್ತ;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      ಯದಿ(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = ರಿಕ್ತ;
      }
      ಅನ್ಯಥಾ {
        curl_msnprintf(portbuf, ಪರಿಮಾಣ(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *h = Curl_get_scheme(u->scheme);
      ಯದಿ(h) {
        curl_msnprintf(portbuf, ಪರಿಮಾಣ(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PATH:
    ptr = u->path;
    ಯದಿ(!ptr)
      ptr = "/";
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    ಯದಿ(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = ರಿಕ್ತ;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    ಯದಿ(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_URL:
    ಪ್ರತ್ಯಾಗಮ urlget_url(u, part, flags);
  ಸ್ವಭಾವ:
    ptr = ರಿಕ್ತ;
    ಭಙ್ಗ;
  }
  ಯದಿ(ptr)
    ಪ್ರತ್ಯಾಗಮ urlget_format(u, what, ptr, part, plusdecode, flags);

  ಪ್ರತ್ಯಾಗಮ ifmissing;
}

ಸ್ಥಿರ CURLUcode set_url_scheme(CURLU *u, ಧ್ರುವ ವರ್ಣ *scheme,
                                unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  size_t plen = strlen(scheme);
  ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *h = ರಿಕ್ತ;
  ಯದಿ((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  ಯದಿ(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    ಪ್ರತ್ಯಾಗಮ CURLUE_UNSUPPORTED_SCHEME;
  ಯದಿ(!h) {
    ಧ್ರುವ ವರ್ಣ *s = scheme;
    ಯದಿ(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      ಯಾವತ್(--plen) {
        ಯದಿ(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        ಅನ್ಯಥಾ
          ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_SCHEME;
      }
    }
    ಅನ್ಯಥಾ
      ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode set_url_port(CURLU *u, ಧ್ರುವ ವರ್ಣ *provided_port)
{
  curl_off_t port;
  ಯದಿ(!ISDIGIT(provided_port[0]))
    /* not a number */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PORT_ಸಂಖ್ಯೆ;
  ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_PORT_ಸಂಖ್ಯೆ;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode set_url(CURLU *u, ಧ್ರುವ ವರ್ಣ *url, size_t part_size,
                         unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  ವರ್ಣ *oldurl = ರಿಕ್ತ;

  ಯದಿ(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    ಯದಿ(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
    }
    ಯದಿ(uc == CURLUE_OUT_OF_MEMORY)
      ಪ್ರತ್ಯಾಗಮ uc;
    ಪ್ರತ್ಯಾಗಮ CURLUE_MALFORMED_ಒಳಹರಿವು;
  }

  /* if the new URL is absolute replace the existing with the new. */
  ಯದಿ(Curl_is_absolute_url(url, ರಿಕ್ತ, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    ಪ್ರತ್ಯಾಗಮ parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  ಯದಿ(uc == CURLUE_OUT_OF_MEMORY)
    ಪ್ರತ್ಯಾಗಮ uc;
  ಅನ್ಯಥಾ ಯದಿ(uc)
    ಪ್ರತ್ಯಾಗಮ parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  ಪ್ರತ್ಯಾಗಮ uc;
}

ಸ್ಥಿರ CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  ವಿಕಲ್ಪ(what) {
  ಪಕ್ಷ CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, ಪರಿಮಾಣ(ಸಂರಚನಾ Curl_URL));
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_USER:
    curlx_safefree(u->user);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_HOST:
    curlx_safefree(u->host);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PATH:
    curlx_safefree(u->path);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    ಭಙ್ಗ;
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLUE_UNKNOWN_PART;
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ bool allowed_in_path(unsigned ವರ್ಣ x)
{
  ವಿಕಲ್ಪ(x) {
  ಪಕ್ಷ '!':
  ಪಕ್ಷ '$':
  ಪಕ್ಷ '&':
  ಪಕ್ಷ '\'':
  ಪಕ್ಷ '(':
  ಪಕ್ಷ ')':
  ಪಕ್ಷ '{':
  ಪಕ್ಷ '}':
  ಪಕ್ಷ '[':
  ಪಕ್ಷ ']':
  ಪಕ್ಷ '*':
  ಪಕ್ಷ '+':
  ಪಕ್ಷ ',':
  ಪಕ್ಷ ';':
  ಪಕ್ಷ '=':
  ಪಕ್ಷ ':':
  ಪಕ್ಷ '@':
  ಪಕ್ಷ '/':
    ಪ್ರತ್ಯಾಗಮ TRUE;
  }
  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLUcode url_encode_part(ಸಂರಚನಾ dynbuf *encp,
                                 ಧ್ರುವ ವರ್ಣ *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ಧ್ರುವ unsigned ವರ್ಣ *i;

  ಪ್ರತಿ(i = (ಧ್ರುವ unsigned ವರ್ಣ *)part; *i; i++) {
    CURLcode result;
    ಯದಿ((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    ಅನ್ಯಥಾ ಯದಿ(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      ಯದಿ((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    ಅನ್ಯಥಾ {
      unsigned ವರ್ಣ out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ cc2cu(result);
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode url_uppercasehex_part(ಸಂರಚನಾ dynbuf *encp,
                                       ಧ್ರುವ ವರ್ಣ *part)
{
  ವರ್ಣ *p;
  CURLcode result = curlx_dyn_add(encp, part);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ cc2cu(result);
  p = curlx_dyn_ptr(encp);
  ಯಾವತ್(*p) {
    /* make sure percent encoded are upper case */
    ಯದಿ((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    ಅನ್ಯಥಾ
      p++;
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode url_append_query(CURLU *u, ಸಂರಚನಾ dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  ಯದಿ(querylen) {
    ಸಂರಚನಾ dynbuf qbuf;
    CURLcode result;
    ಧ್ರುವ ವರ್ಣ *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_ಒಳಹರಿವು_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    ಯದಿ(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    ಯದಿ(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    ಯದಿ(result)
      ಗಚ್ಛ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    ಪ್ರತ್ಯಾಗಮ cc2cu(result);
  }
  ಅನ್ಯಥಾ {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode url_sethost(CURLU *u, ಸಂರಚನಾ dynbuf *encp,
                             bool urlencode,
                             unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  ವರ್ಣ *newp = curlx_dyn_ptr(encp);
  ಯದಿ(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  ಅನ್ಯಥಾ ಯದಿ(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    ವರ್ಣ *decoded = ರಿಕ್ತ;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    ಯದಿ(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    ಅನ್ಯಥಾ {
      ಸಂರಚನಾ dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_ಒಳಹರಿವು_LENGTH);
      ಯದಿ(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      ಅನ್ಯಥಾ ಯದಿ(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        ಯದಿ(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  ಅನ್ಯಥಾ ಯದಿ(hostname_check6(u, newp, n))
    bad = TRUE;
  ಅನ್ಯಥಾ ಯದಿ(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  ಯದಿ(bad) {
    curlx_dyn_free(encp);
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HOSTNAME;
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       ಧ್ರುವ ವರ್ಣ *part, unsigned ಪೂರ್ಣಾಙ್ಕ flags)
{
  ವರ್ಣ **storep = ರಿಕ್ತ;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  ಯದಿ(!u)
    ಪ್ರತ್ಯಾಗಮ CURLUE_BAD_HANDLE;
  ಯದಿ(!part)
    /* setting a part to NULL clears it */
    ಪ್ರತ್ಯಾಗಮ urlset_clear(u, what);

  nalloc = strlen(part);
  ಯದಿ(nalloc > CURL_MAX_ಒಳಹರಿವು_LENGTH)
    /* excessive input length */
    ಪ್ರತ್ಯಾಗಮ CURLUE_MALFORMED_ಒಳಹರಿವು;

  ವಿಕಲ್ಪ(what) {
  ಪಕ್ಷ CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    ಯದಿ(status)
      ಪ್ರತ್ಯಾಗಮ status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    ಭಙ್ಗ;
  }
  ಪಕ್ಷ CURLUPART_USER:
    storep = &u->user;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PASSWORD:
    storep = &u->password;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_OPTIONS:
    storep = &u->options;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_ZONEID:
    storep = &u->zoneid;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_PORT:
    ಪ್ರತ್ಯಾಗಮ set_url_port(u, part);
  ಪಕ್ಷ CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    ಭಙ್ಗ;
  ಪಕ್ಷ CURLUPART_URL:
    ಪ್ರತ್ಯಾಗಮ set_url(u, part, nalloc, flags);
  ಸ್ವಭಾವ:
    ಪ್ರತ್ಯಾಗಮ CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ಧ್ರುವ ವರ್ಣ *newp = ರಿಕ್ತ;
    ಸಂರಚನಾ dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    ಯದಿ(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      ಯದಿ(result)
        ಪ್ರತ್ಯಾಗಮ cc2cu(result);
    }
    ಯದಿ(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    ಅನ್ಯಥಾ
      status = url_uppercasehex_part(&enc, part);
    ಯದಿ(!status) {
      newp = curlx_dyn_ptr(&enc);

      ಯದಿ(appendquery && newp)
        ಪ್ರತ್ಯಾಗಮ url_append_query(u, &enc);
      ಅನ್ಯಥಾ ಯದಿ(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    ಯದಿ(status)
      ಪ್ರತ್ಯಾಗಮ status;

    ಯದಿ(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (ವರ್ಣ *)CURL_UNCONST(newp);
  }
  ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *s = ರಿಕ್ತ;

  /* base must be an absolute URL */
  ಯದಿ(!base->scheme || !base->host)
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(href->scheme && !curl_strequal(base->scheme, href->scheme))
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಯದಿ(href->host) {
    ಯದಿ(!curl_strequal(base->host, href->host))
      ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      ಯದಿ(!s) /* Cannot match default port for unknown scheme */
        ಪ್ರತ್ಯಾಗಮ FALSE;
      /* to match, the present one must be the default port */
      ಯದಿ((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಅನ್ಯಥಾ ಯದಿ(base->portnum != href->portnum) /* both present or missing */
      ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  ಅನ್ಯಥಾ ಯದಿ(href->port_present) /* no host in href, then there must be no port */
    ಪ್ರತ್ಯಾಗಮ FALSE;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  ಯದಿ(u->port_present) {
    *pport = u->portnum;
    ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
  }
  ಅನ್ಯಥಾ ಯದಿ(u->scheme) {
    ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *s = Curl_get_scheme(u->scheme);
    ಯದಿ(s && s->defport) {
      *pport = s->defport;
      ಪ್ರತ್ಯಾಗಮ CURLUE_OK;
    }
  }
  *pport = 0;
  ಪ್ರತ್ಯಾಗಮ CURLUE_NO_PORT;
}
