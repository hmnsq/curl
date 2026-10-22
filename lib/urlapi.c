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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "urlapi-int.h"
#അന്തര്ഭാവ "strcase.h"
#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "escape.h"
#അന്തര്ഭാവ "curlx/inet_pton.h"
#അന്തര്ഭാവ "curlx/inet_ntop.h"
#അന്തര്ഭാവ "curlx/strdup.h"
#അന്തര്ഭാവ "idn.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curl_memrchr.h"

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
#എങ്കിൽ !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

സ്ഥിതം ശൂന്യം free_urlhandle(ഘടന Curl_URL *u)
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
സ്ഥിതം സ്ഥിരം അക്ഷരം *find_host_sep(സ്ഥിരം അക്ഷരം *url)
{
  /* Find the start of the hostname */
  സ്ഥിരം അക്ഷരം *sep = strstr(url, "//");
  എങ്കിൽ(!sep)
    sep = url;
  അല്ലെങ്കിൽ
    sep += 2;

  /* Find first / or ? */
  ആകുമ്പോൾ(*sep && *sep != '/' && *sep != '?')
    sep++;

  തിരികെ sep;
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
UNITTEST CURLUcode urlencode_str(ഘടന dynbuf *o, സ്ഥിരം അക്ഷരം *url,
                                 size_t len, bool relative,
                                 unsigned പൂർണ്ണസംഖ്യ query);
UNITTEST CURLUcode urlencode_str(ഘടന dynbuf *o, സ്ഥിരം അക്ഷരം *url,
                                 size_t len, bool relative,
                                 unsigned പൂർണ്ണസംഖ്യ query)
{
  /* we must add this with whitespace-replacing */
  സ്ഥിരം unsigned അക്ഷരം *iptr;
  സ്ഥിരം unsigned അക്ഷരം *host_sep = (സ്ഥിരം unsigned അക്ഷരം *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  എങ്കിൽ(!relative) {
    size_t n;
    host_sep = (സ്ഥിരം unsigned അക്ഷരം *)find_host_sep(url);

    /* output the first piece as-is */
    n = (സ്ഥിരം അക്ഷരം *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  വേണ്ടി(iptr = host_sep; len && !result;) {
    എങ്കിൽ(*iptr == ' ') {
      എങ്കിൽ(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      അല്ലെങ്കിൽ
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned അക്ഷരം out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned അക്ഷരം hex = (unsigned അക്ഷരം)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned അക്ഷരം out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    അല്ലെങ്കിൽ {
      സ്ഥിരം unsigned അക്ഷരം *start = iptr;
      ആകുമ്പോൾ(len) {
        എങ്കിൽ(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          നിർത്തുക;
        എങ്കിൽ(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          നിർത്തുക;
        എങ്കിൽ(*iptr == '?') {
          എങ്കിൽ(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            നിർത്തുക;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (സ്ഥിരം അക്ഷരം *)start, (size_t)(iptr - start));
    }
  }

  എങ്കിൽ(result)
    തിരികെ cc2cu(result);
  തിരികെ CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(സ്ഥിരം അക്ഷരം *url, അക്ഷരം *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (ശൂന്യം)buflen; /* only used in debug-builds */
  എങ്കിൽ(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  എങ്കിൽ(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    തിരികെ 0;
#endif
  എങ്കിൽ(ISALPHA(url[0])) {
    എങ്കിൽ(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    വേണ്ടി(i = 1; i < MAX_SCHEME_LEN; ++i) {
      അക്ഷരം s = url[i];
      എങ്കിൽ(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        എങ്കിൽ(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      അല്ലെങ്കിൽ {
        നിർത്തുക;
      }
    }
  }
  എങ്കിൽ(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    എങ്കിൽ(buf)
      buf[i] = 0;
    തിരികെ len;
  }
  എങ്കിൽ(buf)
    buf[0] = 0;
  തിരികെ 0;
}

/* scan for byte values <= 31, 127 and maybe space */
സ്ഥിതം bool badoctets(സ്ഥിരം അക്ഷരം *നിവേശം, size_t n, പൂർണ്ണസംഖ്യ flags)
{
  സ്ഥിരം uint8_t *p = (സ്ഥിരം unsigned അക്ഷരം *)നിവേശം;
  സ്ഥിരം uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  ആകുമ്പോൾ(n--) {
    എങ്കിൽ(*p <= control || *p == 127)
      തിരികെ TRUE;
    p++;
  }
  തിരികെ FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ഘടന Curl_URL *u,
                                        സ്ഥിരം അക്ഷരം *login,
                                        size_t len,
                                        unsigned പൂർണ്ണസംഖ്യ flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(ഘടന Curl_URL *u,
                                        സ്ഥിരം അക്ഷരം *login,
                                        size_t len,
                                        unsigned പൂർണ്ണസംഖ്യ flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  അക്ഷരം *userp = ശൂന്യ;
  അക്ഷരം *passwdp = ശൂന്യ;
  അക്ഷരം *optionsp = ശൂന്യ;
  സ്ഥിരം ഘടന Curl_scheme *h = ശൂന്യ;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  സ്ഥിരം അക്ഷരം *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  എങ്കിൽ(!ptr)
    പോകുക out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  എങ്കിൽ(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ശൂന്യ);
  എങ്കിൽ(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    പോകുക out;
  }

  എങ്കിൽ(userp) {
    എങ്കിൽ(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      പോകുക out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  എങ്കിൽ(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  എങ്കിൽ(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  എങ്കിൽ(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  അല്ലെങ്കിൽ എങ്കിൽ(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  അല്ലെങ്കിൽ എങ്കിൽ(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_നിവേശം;

  userp = passwdp = optionsp = ശൂന്യ;

  എങ്കിൽ(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    തിരികെ CURLUE_OK;
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

  തിരികെ ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(ഘടന Curl_URL *u, ഘടന dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(ഘടന Curl_URL *u, ഘടന dynbuf *host,
                              bool has_scheme)
{
  സ്ഥിരം അക്ഷരം *portptr;
  സ്ഥിരം അക്ഷരം *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  എങ്കിൽ(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    എങ്കിൽ(!portptr)
      തിരികെ CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    എങ്കിൽ(*portptr) {
      എങ്കിൽ(*portptr != ':')
        തിരികെ CURLUE_BAD_PORT_സംഖ്യ;
    }
    അല്ലെങ്കിൽ
      portptr = ശൂന്യ;
  }
  അല്ലെങ്കിൽ
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  എങ്കിൽ(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    പൂർണ്ണസംഖ്യ rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    എങ്കിൽ(!*portptr)
      തിരികെ has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_സംഖ്യ;
    എങ്കിൽ(*portptr == '\\')
      തിരികെ CURLUE_BACKSLASH;
    rc = curlx_str_സംഖ്യ(&portptr, &port, 0xffff);
    എങ്കിൽ(rc)
      തിരികെ CURLUE_BAD_PORT_സംഖ്യ;
    അല്ലെങ്കിൽ എങ്കിൽ(*portptr == '\\')
      തിരികെ CURLUE_BACKSLASH;
    അല്ലെങ്കിൽ എങ്കിൽ(*portptr)
      തിരികെ CURLUE_BAD_PORT_സംഖ്യ;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  തിരികെ CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

സ്ഥിതം സ്ഥിരം bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(ഘടന Curl_URL *u, അക്ഷരം *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ഘടന Curl_URL *u, അക്ഷരം *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  എങ്കിൽ(hlen < 4) /* '[::]' is the shortest possible valid string */
    തിരികെ CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  എങ്കിൽ(hlen != len) {
    hlen = len;
    എങ്കിൽ(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      അക്ഷരം zoneid[MAX_ZONEID_LEN];
      പൂർണ്ണസംഖ്യ i = 0;
      അക്ഷരം *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      എങ്കിൽ(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      ആകുമ്പോൾ(!invalid_host_char[(unsigned അക്ഷരം)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      എങ്കിൽ(!i || (']' != *h))
        തിരികെ CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      എങ്കിൽ(!u->zoneid)
        തിരികെ CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    അക്ഷരം dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    എങ്കിൽ(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      തിരികെ CURLUE_BAD_IPV6;
    എങ്കിൽ(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  തിരികെ CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
സ്ഥിതം CURLUcode hostname_check(അക്ഷരം *hostname, size_t hlen)
{
  size_t i;
  വേണ്ടി(i = 0; i < hlen; i++) {
    എങ്കിൽ(invalid_host_char[(unsigned അക്ഷരം)hostname[i]])
      തിരികെ CURLUE_BAD_HOSTNAME;
  }
  എങ്കിൽ((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    തിരികെ CURLUE_BAD_HOSTNAME;
  അല്ലെങ്കിൽ എങ്കിൽ((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    തിരികെ CURLUE_BAD_HOSTNAME;
  തിരികെ CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
സ്ഥിതം CURLUcode hostname_check6(ഘടന Curl_URL *u, അക്ഷരം *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  എങ്കിൽ(!hlen)
    തിരികെ CURLUE_NO_HOST;
  അല്ലെങ്കിൽ എങ്കിൽ(hostname[0] == '[')
    തിരികെ ipv6_parse(u, hostname, hlen);

  തിരികെ hostname_check(hostname, hlen);
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
UNITTEST പൂർണ്ണസംഖ്യ ipv4_normalize(ഘടന dynbuf *host);
UNITTEST പൂർണ്ണസംഖ്യ ipv4_normalize(ഘടന dynbuf *host)
{
  bool done = FALSE;
  പൂർണ്ണസംഖ്യ n = 0;
  സ്ഥിരം അക്ഷരം *c = curlx_dyn_ptr(host);
  unsigned പൂർണ്ണസംഖ്യ parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!ISDIGIT(*c))
    തിരികെ HOST_NAME;

  ആകുമ്പോൾ(!done) {
    പൂർണ്ണസംഖ്യ rc;
    curl_off_t l;
    എങ്കിൽ(*c == '0') {
      എങ്കിൽ((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        എങ്കിൽ(rc)
          തിരികെ HOST_NAME;
      }
      അല്ലെങ്കിൽ
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    അല്ലെങ്കിൽ
      rc = curlx_str_സംഖ്യ(&c, &l, UINT_MAX);

    എങ്കിൽ(rc) {
      എങ്കിൽ(!n || (rc != STRE_NO_അക്കം) || *c)
        തിരികെ HOST_NAME;
      n--;
    }
    അല്ലെങ്കിൽ
      parts[n] = (unsigned പൂർണ്ണസംഖ്യ)l;

    തിരഞ്ഞെടുപ്പ്(*c) {
    അവസ്ഥ '.':
      എങ്കിൽ(n == 3) {
        എങ്കിൽ(c[1])
          /* something follows this dot */
          തിരികെ HOST_NAME;
        done = TRUE;
      }
      അല്ലെങ്കിൽ {
        n++;
        c++;
      }
      നിർത്തുക;

    അവസ്ഥ '\0':
      done = TRUE;
      നിർത്തുക;

    സ്ഥിരസ്ഥിതി:
      തിരികെ HOST_NAME;
    }
  }

  തിരഞ്ഞെടുപ്പ്(n) {
  അവസ്ഥ 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    നിർത്തുക;
  അവസ്ഥ 1: /* a.b -- 8.24 bits */
    എങ്കിൽ((parts[0] > 0xff) || (parts[1] > 0xffffff))
      തിരികെ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    നിർത്തുക;
  അവസ്ഥ 2: /* a.b.c -- 8.8.16 bits */
    എങ്കിൽ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      തിരികെ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    നിർത്തുക;
  അവസ്ഥ 3: /* a.b.c.d -- 8.8.8.8 bits */
    എങ്കിൽ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      തിരികെ HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    നിർത്തുക;
  }
  എങ്കിൽ(result)
    തിരികെ HOST_ERROR;
  തിരികെ HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
സ്ഥിതം CURLUcode urldecode_host(ഘടന dynbuf *host)
{
  സ്ഥിരം അക്ഷരം *per;
  സ്ഥിരം അക്ഷരം *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  എങ്കിൽ(!per)
    /* nothing to decode */
    തിരികെ CURLUE_OK;
  അല്ലെങ്കിൽ {
    /* encoded */
    size_t dlen;
    അക്ഷരം *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    എങ്കിൽ(result)
      തിരികെ CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    എങ്കിൽ(result)
      തിരികെ cc2cu(result);
  }

  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode parse_authority(ഘടന Curl_URL *u,
                                 സ്ഥിരം അക്ഷരം *auth, size_t authlen,
                                 unsigned പൂർണ്ണസംഖ്യ flags,
                                 ഘടന dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  എങ്കിൽ(uc)
    തിരികെ uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  എങ്കിൽ(result) {
    uc = cc2cu(result);
    തിരികെ uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  എങ്കിൽ(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  എങ്കിൽ(!uc)
    uc = urldecode_host(host);
  എങ്കിൽ(uc)
    ;
  അല്ലെങ്കിൽ എങ്കിൽ(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  അല്ലെങ്കിൽ {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    പൂർണ്ണസംഖ്യ type = ipv4_normalize(host);

    എങ്കിൽ(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    അല്ലെങ്കിൽ എങ്കിൽ(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  തിരികെ uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, സ്ഥിരം അക്ഷരം *authority)
{
  CURLUcode ures;
  ഘടന dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_നിവേശം_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  എങ്കിൽ(ures)
    curlx_dyn_free(&host);
  അല്ലെങ്കിൽ {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  തിരികെ ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

സ്ഥിതം bool is_dot(സ്ഥിരം അക്ഷരം **str, size_t *clen)
{
  സ്ഥിരം അക്ഷരം *p = *str;
  എങ്കിൽ(*p == '.') {
    (*str)++;
    (*clen)--;
    തിരികെ TRUE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
സ്ഥിതം bool needs_dedotdot(സ്ഥിരം അക്ഷരം *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  എങ്കിൽ(pn < 2)
    തിരികെ FALSE;
  എങ്കിൽ(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    തിരികെ FALSE;
  ആകുമ്പോൾ(pn) {
    എങ്കിൽ(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      എങ്കിൽ(!pn || ISSLASH(*p))
        തിരികെ TRUE;
      /* "../" or ".." before end of string */
      അല്ലെങ്കിൽ എങ്കിൽ(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        തിരികെ TRUE;
    }
    അല്ലെങ്കിൽ {
      p++;
      pn--;
    }
  }
  തിരികെ FALSE;
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
UNITTEST പൂർണ്ണസംഖ്യ dedotdotify(സ്ഥിരം അക്ഷരം *നിവേശം, size_t clen, അക്ഷരം **outp);
UNITTEST പൂർണ്ണസംഖ്യ dedotdotify(സ്ഥിരം അക്ഷരം *നിവേശം, size_t clen, അക്ഷരം **outp)
{
  ഘടന dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  സ്ഥിരം അക്ഷരം *dinput = നിവേശം;
  size_t dlen = clen;

  *outp = ശൂന്യ;
  എങ്കിൽ(!needs_dedotdot(നിവേശം, clen))
    തിരികെ 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  എങ്കിൽ(is_dot(&dinput, &dlen)) {
    എങ്കിൽ(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      നിവേശം = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    അല്ലെങ്കിൽ എങ്കിൽ(is_dot(&dinput, &dlen)) {
      എങ്കിൽ(!dlen)
        /* .. [end] */
        പോകുക end;
      അല്ലെങ്കിൽ എങ്കിൽ(ISSLASH(*dinput)) {
        /* ../ */
        നിവേശം = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  ആകുമ്പോൾ(clen && !result) { /* until end of path content */
    എങ്കിൽ(ISSLASH(*നിവേശം)) {
      സ്ഥിരം അക്ഷരം *p = &നിവേശം[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      എങ്കിൽ(is_dot(&p, &blen)) {
        എങ്കിൽ(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          നിർത്തുക;
        }
        അല്ലെങ്കിൽ എങ്കിൽ(ISSLASH(*p)) { /* /./ */
          നിവേശം = p;
          clen = blen;
          തുടരുക;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        അല്ലെങ്കിൽ എങ്കിൽ(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          എങ്കിൽ(len) {
            സ്ഥിരം അക്ഷരം *ptr = curlx_dyn_ptr(&out);
            സ്ഥിരം അക്ഷരം *last = memrchr(ptr, '/', len);
            എങ്കിൽ(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          എങ്കിൽ(blen) { /* /../ */
            നിവേശം = p;
            clen = blen;
            തുടരുക;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          നിർത്തുക;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, നിവേശം, 1);
    നിവേശം++;
    clen--;
  }
end:
  എങ്കിൽ(!result) {
    എങ്കിൽ(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    അല്ലെങ്കിൽ {
      *outp = curlx_strdup("");
      എങ്കിൽ(!*outp)
        തിരികെ 1;
    }
  }
  തിരികെ result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(സ്ഥിരം അക്ഷരം *url, size_t urllen, CURLU *u,
                              സ്ഥിരം അക്ഷരം **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(സ്ഥിരം അക്ഷരം *url, size_t urllen, CURLU *u,
                              സ്ഥിരം അക്ഷരം **pathp, size_t *pathlenp)
{
  സ്ഥിരം അക്ഷരം *path;
  size_t pathlen;

  *pathp = ശൂന്യ;
  *pathlenp = 0;
  എങ്കിൽ(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    തിരികെ CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  എങ്കിൽ(path[0] != '/')
    തിരികെ CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  എങ്കിൽ(path[1] == '/') {
    /* swallow the two slashes */
    സ്ഥിരം അക്ഷരം *ptr = &path[2];

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
    എങ്കിൽ(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      എങ്കിൽ(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      അല്ലെങ്കിൽ
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        തിരികെ CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#എങ്കിൽ !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  എങ്കിൽ(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    തിരികെ CURLUE_BAD_FILE_URL;
  }
#അല്ലെങ്കിൽ
  /* If the path starts with a slash and a drive letter, ditch the slash */
  എങ്കിൽ('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  എങ്കിൽ(!u->scheme)
    തിരികെ CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode parse_scheme(സ്ഥിരം അക്ഷരം *url, CURLU *u, അക്ഷരം *schemebuf,
                              size_t schemelen, unsigned പൂർണ്ണസംഖ്യ flags,
                              സ്ഥിരം അക്ഷരം **hostpp)
{
  /* clear path */
  സ്ഥിരം അക്ഷരം *schemep = ശൂന്യ;

  എങ്കിൽ(schemelen) {
    പൂർണ്ണസംഖ്യ അക്കം_slashes = 0;
    സ്ഥിരം അക്ഷരം *p = &url[schemelen + 1];
    എങ്കിൽ(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      തിരികെ CURLUE_UNSUPPORTED_SCHEME;

    എങ്കിൽ(!ISSLASH(*p))
      /* less than one */
      തിരികെ CURLUE_BAD_SLASHES;
    എങ്കിൽ((flags & CURLU_NO_AUTHORITY)) {
      ആകുമ്പോൾ(ISSLASH(*p) && (അക്കം_slashes < 2)) {
        p++;
        അക്കം_slashes++;
      }
    }
    അല്ലെങ്കിൽ {
      ആകുമ്പോൾ(ISSLASH(*p) && (അക്കം_slashes < 4)) {
        p++;
        അക്കം_slashes++;
      }
      എങ്കിൽ(അക്കം_slashes > 3)
        തിരികെ CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  അല്ലെങ്കിൽ {
    /* no scheme! */

    എങ്കിൽ(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      തിരികെ CURLUE_BAD_SCHEME;

    എങ്കിൽ(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  എങ്കിൽ(schemep) {
    u->scheme = curlx_strdup(schemep);
    എങ്കിൽ(!u->scheme)
      തിരികെ CURLUE_OUT_OF_MEMORY;
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode guess_scheme(CURLU *u, ഘടന dynbuf *host)
{
  സ്ഥിരം അക്ഷരം *hostname = curlx_dyn_ptr(host);
  സ്ഥിരം അക്ഷരം *schemep = ശൂന്യ;
  /* legacy curl-style guess based on hostname */
  എങ്കിൽ(checkprefix("ftp.", hostname))
    schemep = "ftp";
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("dict.", hostname))
    schemep = "dict";
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("ldap.", hostname))
    schemep = "ldap";
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("imap.", hostname))
    schemep = "imap";
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("smtp.", hostname))
    schemep = "smtp";
  അല്ലെങ്കിൽ എങ്കിൽ(checkprefix("pop3.", hostname))
    schemep = "pop3";
  അല്ലെങ്കിൽ
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  എങ്കിൽ(!u->scheme)
    തിരികെ CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode handle_fragment(CURLU *u, സ്ഥിരം അക്ഷരം *fragment,
                                 size_t fraglen, unsigned പൂർണ്ണസംഖ്യ flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  എങ്കിൽ(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    എങ്കിൽ(flags & CURLU_URLENCODE) {
      ഘടന dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_നിവേശം_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      എങ്കിൽ(ures)
        തിരികെ ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(badoctets(fragment, fraglen, flags))
        തിരികെ CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      എങ്കിൽ(!u->fragment)
        തിരികെ CURLUE_OUT_OF_MEMORY;
    }
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode handle_query(CURLU *u, സ്ഥിരം അക്ഷരം *query,
                              size_t qlen, unsigned പൂർണ്ണസംഖ്യ flags)
{
  u->query_present = TRUE;
  എങ്കിൽ(qlen > 1) {
    എങ്കിൽ(flags & CURLU_URLENCODE) {
      ഘടന dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_നിവേശം_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      എങ്കിൽ(ures)
        തിരികെ ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    അല്ലെങ്കിൽ {
      എങ്കിൽ(badoctets(query, qlen, flags))
        തിരികെ CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      എങ്കിൽ(!u->query)
        തിരികെ CURLUE_OUT_OF_MEMORY;
    }
  }
  അല്ലെങ്കിൽ {
    /* single byte query */
    u->query = curlx_strdup("");
    എങ്കിൽ(!u->query)
      തിരികെ CURLUE_OUT_OF_MEMORY;
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode handle_path(CURLU *u, സ്ഥിരം അക്ഷരം *path,
                             size_t pathlen, unsigned പൂർണ്ണസംഖ്യ flags,
                             bool is_file)
{
  CURLUcode ures;
  എങ്കിൽ(pathlen && (flags & CURLU_URLENCODE)) {
    ഘടന dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_നിവേശം_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    എങ്കിൽ(ures)
      തിരികെ ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  എങ്കിൽ(pathlen >= (size_t)(1 + !is_file)) {
    എങ്കിൽ(badoctets(path, pathlen, flags))
      തിരികെ CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    എങ്കിൽ(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      എങ്കിൽ(!u->path)
        തിരികെ CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    എങ്കിൽ(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      അക്ഷരം *dedot;
      പൂർണ്ണസംഖ്യ err = dedotdotify(path, pathlen, &dedot);
      എങ്കിൽ(err)
        തിരികെ CURLUE_OUT_OF_MEMORY;
      എങ്കിൽ(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode parseurl(സ്ഥിരം അക്ഷരം *url, CURLU *u, unsigned പൂർണ്ണസംഖ്യ flags)
{
  സ്ഥിരം അക്ഷരം *path;
  size_t pathlen;
  അക്ഷരം schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ഘടന dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  എങ്കിൽ(urllen > CURL_MAX_നിവേശം_LENGTH)
    തിരികെ CURLUE_MALFORMED_നിവേശം;

  curlx_dyn_init(&host, CURL_MAX_നിവേശം_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, വലുപ്പം(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  എങ്കിൽ(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  അല്ലെങ്കിൽ {
    സ്ഥിരം അക്ഷരം *hostp = ശൂന്യ;
    സ്ഥിരം അക്ഷരം *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    എങ്കിൽ(ures)
      പോകുക fail;

    /* find the end of the hostname + port number */
    p = hostp;
    ആകുമ്പോൾ(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    എങ്കിൽ(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      എങ്കിൽ(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      എങ്കിൽ(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    അല്ലെങ്കിൽ
      ures = CURLUE_NO_HOST;
  }
  എങ്കിൽ(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    സ്ഥിരം അക്ഷരം *fragment = memchr(path, '#', pathlen);
    എങ്കിൽ(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  എങ്കിൽ(!ures) {
    സ്ഥിരം അക്ഷരം *query = memchr(path, '?', pathlen);
    എങ്കിൽ(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  എങ്കിൽ(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  എങ്കിൽ(!ures) {
    u->host = curlx_dyn_ptr(&host);
    തിരികെ CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  തിരികെ ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
സ്ഥിതം CURLUcode parseurl_and_replace(സ്ഥിരം അക്ഷരം *url, CURLU *u,
                                      unsigned പൂർണ്ണസംഖ്യ flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, വലുപ്പം(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  എങ്കിൽ(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  തിരികെ ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
സ്ഥിതം CURLUcode redirect_url(സ്ഥിരം അക്ഷരം *base, സ്ഥിരം അക്ഷരം *relurl,
                              CURLU *u, unsigned പൂർണ്ണസംഖ്യ flags)
{
  ഘടന dynbuf urlbuf;
  bool host_changed = FALSE;
  സ്ഥിരം അക്ഷരം *useurl = relurl;
  സ്ഥിരം അക്ഷരം *cutoff = ശൂന്യ;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  സ്ഥിരം അക്ഷരം *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  സ്ഥിരം അക്ഷരം *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  എങ്കിൽ(!base)
    തിരികെ CURLUE_MALFORMED_നിവേശം; /* should never happen */

  /* handle different relative URL types */
  തിരഞ്ഞെടുപ്പ്(relurl[0]) {
  അവസ്ഥ '/':
    എങ്കിൽ(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    അല്ലെങ്കിൽ
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    നിർത്തുക;

  അവസ്ഥ '#':
    /* fragment-only change */
    എങ്കിൽ(u->fragment_present)
      cutoff = strchr(protsep, '#');
    നിർത്തുക;

  സ്ഥിരസ്ഥിതി:
    /* path or query-only change */
    എങ്കിൽ(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    അല്ലെങ്കിൽ എങ്കിൽ(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    എങ്കിൽ(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      എങ്കിൽ(cutoff)
        cutoff++; /* truncate after last slash */
    }
    നിർത്തുക;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_നിവേശം_LENGTH);

  എങ്കിൽ(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  അല്ലെങ്കിൽ
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  തിരികെ uc;
}

/*
 */
CURLU *curl_url(ശൂന്യം)
{
  തിരികെ curlx_calloc(1, വലുപ്പം(ഘടന Curl_URL));
}

ശൂന്യം curl_url_cleanup(CURLU *u)
{
  എങ്കിൽ(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  ചെയ്യുക {                                          \
    എങ്കിൽ((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      എങ്കിൽ(!(dest)->name)                         \
        പോകുക fail;                              \
    }                                           \
  } ആകുമ്പോൾ(0)

CURLU *curl_url_dup(സ്ഥിരം CURLU *in)
{
  ഘടന Curl_URL *u;

  എങ്കിൽ(!in)
    തിരികെ ശൂന്യ;

  u = curlx_calloc(1, വലുപ്പം(ഘടന Curl_URL));
  എങ്കിൽ(u) {
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
  തിരികെ u;
fail:
  curl_url_cleanup(u);
  തിരികെ ശൂന്യ;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#അല്ലെങ്കിൽ
സ്ഥിതം CURLUcode host_decode(സ്ഥിരം അക്ഷരം *host, അക്ഷരം **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  എങ്കിൽ(result)
    തിരികെ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode host_encode(സ്ഥിരം അക്ഷരം *host, അക്ഷരം **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  എങ്കിൽ(result)
    തിരികെ (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  തിരികെ CURLUE_OK;
}
#endif

സ്ഥിതം CURLUcode urlget_format(സ്ഥിരം CURLU *u, CURLUPart what,
                               സ്ഥിരം അക്ഷരം *ptr, അക്ഷരം **partp,
                               bool plusdecode, unsigned പൂർണ്ണസംഖ്യ flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  അക്ഷരം *part = curlx_memdup0(ptr, partlen);
  *partp = ശൂന്യ;
  എങ്കിൽ(!part)
    തിരികെ CURLUE_OUT_OF_MEMORY;
  എങ്കിൽ(plusdecode) {
    /* convert + to space */
    അക്ഷരം *plus = part;
    size_t i = 0;
    വേണ്ടി(i = 0; i < partlen; ++plus, i++) {
      എങ്കിൽ(*plus == '+')
        *plus = ' ';
    }
  }
  എങ്കിൽ(urldecode) {
    അക്ഷരം *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    എങ്കിൽ(result)
      തിരികെ CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  എങ്കിൽ(urlencode) {
    ഘടന dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_നിവേശം_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    എങ്കിൽ(uc)
      തിരികെ uc;
    part = curlx_dyn_ptr(&enc);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(punycode) {
    എങ്കിൽ(!Curl_is_ASCII_name(u->host)) {
      അക്ഷരം *punyversion = ശൂന്യ;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      എങ്കിൽ(uc)
        തിരികെ uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      എങ്കിൽ(uc) {
        curlx_free(punyversion);
        തിരികെ uc;
      }
      part = punyversion;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(depunyfy && Curl_is_ASCII_name(u->host)) {
    അക്ഷരം *unpunified = ശൂന്യ;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    എങ്കിൽ(uc)
      തിരികെ uc;
    part = unpunified;
  }
  *partp = part;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode file_url(സ്ഥിരം CURLU *u, അക്ഷരം **part,
                          സ്ഥിരം അക്ഷരം *fragmentsep,
                          സ്ഥിരം അക്ഷരം *querysep)
{
  അക്ഷരം *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  എങ്കിൽ(!url)
    തിരികെ CURLUE_OUT_OF_MEMORY;

  *part = url;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode urlget_url(സ്ഥിരം CURLU *u, അക്ഷരം **part, unsigned പൂർണ്ണസംഖ്യ flags)
{
  അക്ഷരം *url;
  അക്ഷരം *allochost = ശൂന്യ;
  സ്ഥിരം അക്ഷരം *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  സ്ഥിരം അക്ഷരം *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  അക്ഷരം portbuf[7];
  എങ്കിൽ(curl_strequal("file", u->scheme))
    തിരികെ file_url(u, part, fragmentsep, querysep);
  അല്ലെങ്കിൽ എങ്കിൽ(!u->host)
    തിരികെ CURLUE_NO_HOST;
  അല്ലെങ്കിൽ {
    സ്ഥിരം അക്ഷരം *scheme;
    അക്ഷരം *options = u->options;
    അക്ഷരം *port = ശൂന്യ;
    സ്ഥിരം ഘടന Curl_scheme *h = ശൂന്യ;
    അക്ഷരം schemebuf[MAX_SCHEME_LEN + 5];
    എങ്കിൽ(u->scheme)
      scheme = u->scheme;
    അല്ലെങ്കിൽ എങ്കിൽ(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    അല്ലെങ്കിൽ
      തിരികെ CURLUE_NO_SCHEME;

    എങ്കിൽ(u->port_present) {
      curl_msnprintf(portbuf, വലുപ്പം(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    എങ്കിൽ(h) {
      എങ്കിൽ(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, വലുപ്പം(portbuf), "%u", h->defport);
        port = portbuf;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = ശൂന്യ;
      }

      എങ്കിൽ(!(h->flags & PROTOPT_URLOPTIONS))
        options = ശൂന്യ;
    }

    എങ്കിൽ(u->host[0] == '[') {
      എങ്കിൽ(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ഘടന dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_നിവേശം_LENGTH);
        എങ്കിൽ(curlx_dyn_addf(&enc, "%.*s%%25%s]", (പൂർണ്ണസംഖ്യ)hostlen - 1, u->host,
                          u->zoneid))
          തിരികെ CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ശൂന്യ, u->host, 0);
      എങ്കിൽ(!allochost)
        തിരികെ CURLUE_OUT_OF_MEMORY;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(flags & CURLU_PUNYCODE) {
      എങ്കിൽ(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        എങ്കിൽ(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        എങ്കിൽ(ret) {
          curlx_safefree(allochost);
          തിരികെ ret;
        }
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(flags & CURLU_PUNY2IDN) {
      എങ്കിൽ(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        എങ്കിൽ(ret)
          തിരികെ ret;
      }
    }

    എങ്കിൽ(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, വലുപ്പം(schemebuf), "%s://", scheme);
    അല്ലെങ്കിൽ
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
  എങ്കിൽ(!url)
    തിരികെ CURLUE_OUT_OF_MEMORY;
  *part = url;
  തിരികെ CURLUE_OK;
}

CURLUcode curl_url_get(സ്ഥിരം CURLU *u, CURLUPart what,
                       അക്ഷരം **part, unsigned പൂർണ്ണസംഖ്യ flags)
{
  സ്ഥിരം അക്ഷരം *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  അക്ഷരം portbuf[7];
  bool plusdecode = FALSE;
  എങ്കിൽ(!u)
    തിരികെ CURLUE_BAD_HANDLE;
  എങ്കിൽ(!part)
    തിരികെ CURLUE_BAD_PARTPOINTER;
  *part = ശൂന്യ;

  തിരഞ്ഞെടുപ്പ്(what) {
  അവസ്ഥ CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    എങ്കിൽ((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      തിരികെ CURLUE_NO_SCHEME;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PORT:
    ptr = ശൂന്യ;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    എങ്കിൽ(u->port_present) {
      സ്ഥിരം ഘടന Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ശൂന്യ;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      എങ്കിൽ(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = ശൂന്യ;
      }
      അല്ലെങ്കിൽ {
        curl_msnprintf(portbuf, വലുപ്പം(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      സ്ഥിരം ഘടന Curl_scheme *h = Curl_get_scheme(u->scheme);
      എങ്കിൽ(h) {
        curl_msnprintf(portbuf, വലുപ്പം(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PATH:
    ptr = u->path;
    എങ്കിൽ(!ptr)
      ptr = "/";
    നിർത്തുക;
  അവസ്ഥ CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    എങ്കിൽ(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = ശൂന്യ;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    എങ്കിൽ(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    നിർത്തുക;
  അവസ്ഥ CURLUPART_URL:
    തിരികെ urlget_url(u, part, flags);
  സ്ഥിരസ്ഥിതി:
    ptr = ശൂന്യ;
    നിർത്തുക;
  }
  എങ്കിൽ(ptr)
    തിരികെ urlget_format(u, what, ptr, part, plusdecode, flags);

  തിരികെ ifmissing;
}

സ്ഥിതം CURLUcode set_url_scheme(CURLU *u, സ്ഥിരം അക്ഷരം *scheme,
                                unsigned പൂർണ്ണസംഖ്യ flags)
{
  size_t plen = strlen(scheme);
  സ്ഥിരം ഘടന Curl_scheme *h = ശൂന്യ;
  എങ്കിൽ((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    തിരികെ CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  എങ്കിൽ(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    തിരികെ CURLUE_UNSUPPORTED_SCHEME;
  എങ്കിൽ(!h) {
    സ്ഥിരം അക്ഷരം *s = scheme;
    എങ്കിൽ(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      ആകുമ്പോൾ(--plen) {
        എങ്കിൽ(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        അല്ലെങ്കിൽ
          തിരികെ CURLUE_BAD_SCHEME;
      }
    }
    അല്ലെങ്കിൽ
      തിരികെ CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode set_url_port(CURLU *u, സ്ഥിരം അക്ഷരം *provided_port)
{
  curl_off_t port;
  എങ്കിൽ(!ISDIGIT(provided_port[0]))
    /* not a number */
    തിരികെ CURLUE_BAD_PORT_സംഖ്യ;
  എങ്കിൽ(curlx_str_സംഖ്യ(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    തിരികെ CURLUE_BAD_PORT_സംഖ്യ;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode set_url(CURLU *u, സ്ഥിരം അക്ഷരം *url, size_t part_size,
                         unsigned പൂർണ്ണസംഖ്യ flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  അക്ഷരം *oldurl = ശൂന്യ;

  എങ്കിൽ(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    എങ്കിൽ(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      തിരികെ CURLUE_OK;
    }
    എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY)
      തിരികെ uc;
    തിരികെ CURLUE_MALFORMED_നിവേശം;
  }

  /* if the new URL is absolute replace the existing with the new. */
  എങ്കിൽ(Curl_is_absolute_url(url, ശൂന്യ, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    തിരികെ parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  എങ്കിൽ(uc == CURLUE_OUT_OF_MEMORY)
    തിരികെ uc;
  അല്ലെങ്കിൽ എങ്കിൽ(uc)
    തിരികെ parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  തിരികെ uc;
}

സ്ഥിതം CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  തിരഞ്ഞെടുപ്പ്(what) {
  അവസ്ഥ CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, വലുപ്പം(ഘടന Curl_URL));
    നിർത്തുക;
  അവസ്ഥ CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_USER:
    curlx_safefree(u->user);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_HOST:
    curlx_safefree(u->host);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PATH:
    curlx_safefree(u->path);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    നിർത്തുക;
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLUE_UNKNOWN_PART;
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം bool allowed_in_path(unsigned അക്ഷരം x)
{
  തിരഞ്ഞെടുപ്പ്(x) {
  അവസ്ഥ '!':
  അവസ്ഥ '$':
  അവസ്ഥ '&':
  അവസ്ഥ '\'':
  അവസ്ഥ '(':
  അവസ്ഥ ')':
  അവസ്ഥ '{':
  അവസ്ഥ '}':
  അവസ്ഥ '[':
  അവസ്ഥ ']':
  അവസ്ഥ '*':
  അവസ്ഥ '+':
  അവസ്ഥ ',':
  അവസ്ഥ ';':
  അവസ്ഥ '=':
  അവസ്ഥ ':':
  അവസ്ഥ '@':
  അവസ്ഥ '/':
    തിരികെ TRUE;
  }
  തിരികെ FALSE;
}

സ്ഥിതം CURLUcode url_encode_part(ഘടന dynbuf *encp,
                                 സ്ഥിരം അക്ഷരം *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  സ്ഥിരം unsigned അക്ഷരം *i;

  വേണ്ടി(i = (സ്ഥിരം unsigned അക്ഷരം *)part; *i; i++) {
    CURLcode result;
    എങ്കിൽ((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    അല്ലെങ്കിൽ എങ്കിൽ(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      എങ്കിൽ((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    അല്ലെങ്കിൽ {
      unsigned അക്ഷരം out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    എങ്കിൽ(result)
      തിരികെ cc2cu(result);
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode url_uppercasehex_part(ഘടന dynbuf *encp,
                                       സ്ഥിരം അക്ഷരം *part)
{
  അക്ഷരം *p;
  CURLcode result = curlx_dyn_add(encp, part);
  എങ്കിൽ(result)
    തിരികെ cc2cu(result);
  p = curlx_dyn_ptr(encp);
  ആകുമ്പോൾ(*p) {
    /* make sure percent encoded are upper case */
    എങ്കിൽ((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    അല്ലെങ്കിൽ
      p++;
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode url_append_query(CURLU *u, ഘടന dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  എങ്കിൽ(querylen) {
    ഘടന dynbuf qbuf;
    CURLcode result;
    സ്ഥിരം അക്ഷരം *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_നിവേശം_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    എങ്കിൽ(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    എങ്കിൽ(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    എങ്കിൽ(result)
      പോകുക nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    തിരികെ CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    തിരികെ cc2cu(result);
  }
  അല്ലെങ്കിൽ {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  തിരികെ CURLUE_OK;
}

സ്ഥിതം CURLUcode url_sethost(CURLU *u, ഘടന dynbuf *encp,
                             bool urlencode,
                             unsigned പൂർണ്ണസംഖ്യ flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  അക്ഷരം *newp = curlx_dyn_ptr(encp);
  എങ്കിൽ(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  അല്ലെങ്കിൽ എങ്കിൽ(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    അക്ഷരം *decoded = ശൂന്യ;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    എങ്കിൽ(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    അല്ലെങ്കിൽ {
      ഘടന dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_നിവേശം_LENGTH);
      എങ്കിൽ(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      അല്ലെങ്കിൽ എങ്കിൽ(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        എങ്കിൽ(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(hostname_check6(u, newp, n))
    bad = TRUE;
  അല്ലെങ്കിൽ എങ്കിൽ(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  എങ്കിൽ(bad) {
    curlx_dyn_free(encp);
    തിരികെ CURLUE_BAD_HOSTNAME;
  }
  തിരികെ CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       സ്ഥിരം അക്ഷരം *part, unsigned പൂർണ്ണസംഖ്യ flags)
{
  അക്ഷരം **storep = ശൂന്യ;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  എങ്കിൽ(!u)
    തിരികെ CURLUE_BAD_HANDLE;
  എങ്കിൽ(!part)
    /* setting a part to NULL clears it */
    തിരികെ urlset_clear(u, what);

  nalloc = strlen(part);
  എങ്കിൽ(nalloc > CURL_MAX_നിവേശം_LENGTH)
    /* excessive input length */
    തിരികെ CURLUE_MALFORMED_നിവേശം;

  തിരഞ്ഞെടുപ്പ്(what) {
  അവസ്ഥ CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    എങ്കിൽ(status)
      തിരികെ status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    നിർത്തുക;
  }
  അവസ്ഥ CURLUPART_USER:
    storep = &u->user;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PASSWORD:
    storep = &u->password;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_OPTIONS:
    storep = &u->options;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    നിർത്തുക;
  അവസ്ഥ CURLUPART_ZONEID:
    storep = &u->zoneid;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_PORT:
    തിരികെ set_url_port(u, part);
  അവസ്ഥ CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    നിർത്തുക;
  അവസ്ഥ CURLUPART_URL:
    തിരികെ set_url(u, part, nalloc, flags);
  സ്ഥിരസ്ഥിതി:
    തിരികെ CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    സ്ഥിരം അക്ഷരം *newp = ശൂന്യ;
    ഘടന dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    എങ്കിൽ(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      എങ്കിൽ(result)
        തിരികെ cc2cu(result);
    }
    എങ്കിൽ(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    അല്ലെങ്കിൽ
      status = url_uppercasehex_part(&enc, part);
    എങ്കിൽ(!status) {
      newp = curlx_dyn_ptr(&enc);

      എങ്കിൽ(appendquery && newp)
        തിരികെ url_append_query(u, &enc);
      അല്ലെങ്കിൽ എങ്കിൽ(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    എങ്കിൽ(status)
      തിരികെ status;

    എങ്കിൽ(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (അക്ഷരം *)CURL_UNCONST(newp);
  }
  തിരികെ CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  സ്ഥിരം ഘടന Curl_scheme *s = ശൂന്യ;

  /* base must be an absolute URL */
  എങ്കിൽ(!base->scheme || !base->host)
    തിരികെ FALSE;
  എങ്കിൽ(href->scheme && !curl_strequal(base->scheme, href->scheme))
    തിരികെ FALSE;
  എങ്കിൽ(href->host) {
    എങ്കിൽ(!curl_strequal(base->host, href->host))
      തിരികെ FALSE;

    എങ്കിൽ(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      എങ്കിൽ(!s) /* Cannot match default port for unknown scheme */
        തിരികെ FALSE;
      /* to match, the present one must be the default port */
      എങ്കിൽ((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        തിരികെ FALSE;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(base->portnum != href->portnum) /* both present or missing */
      തിരികെ FALSE;

    എങ്കിൽ(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      തിരികെ FALSE;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(href->port_present) /* no host in href, then there must be no port */
    തിരികെ FALSE;
  തിരികെ TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  എങ്കിൽ(u->port_present) {
    *pport = u->portnum;
    തിരികെ CURLUE_OK;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(u->scheme) {
    സ്ഥിരം ഘടന Curl_scheme *s = Curl_get_scheme(u->scheme);
    എങ്കിൽ(s && s->defport) {
      *pport = s->defport;
      തിരികെ CURLUE_OK;
    }
  }
  *pport = 0;
  തിരികെ CURLUE_NO_PORT;
}
