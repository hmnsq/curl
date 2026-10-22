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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "urlapi-int.h"
#அந்தர்பாவ "strcase.h"
#அந்தர்பாவ "url.h"
#அந்தர்பாவ "escape.h"
#அந்தர்பாவ "curlx/inet_pton.h"
#அந்தர்பாவ "curlx/inet_ntop.h"
#அந்தர்பாவ "curlx/strdup.h"
#அந்தர்பாவ "idn.h"
#அந்தர்பாவ "curlx/strparse.h"
#அந்தர்பாவ "curl_memrchr.h"

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
#எனில் !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

நிலையான வெறுமை free_urlhandle(கட்டமைப்பு Curl_URL *u)
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
நிலையான மாறிலி எழுத்து *find_host_sep(மாறிலி எழுத்து *url)
{
  /* Find the start of the hostname */
  மாறிலி எழுத்து *sep = strstr(url, "//");
  எனில்(!sep)
    sep = url;
  இல்லையெனில்
    sep += 2;

  /* Find first / or ? */
  போது(*sep && *sep != '/' && *sep != '?')
    sep++;

  திரும்பு sep;
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
UNITTEST CURLUcode urlencode_str(கட்டமைப்பு dynbuf *o, மாறிலி எழுத்து *url,
                                 size_t len, bool relative,
                                 unsigned முழு query);
UNITTEST CURLUcode urlencode_str(கட்டமைப்பு dynbuf *o, மாறிலி எழுத்து *url,
                                 size_t len, bool relative,
                                 unsigned முழு query)
{
  /* we must add this with whitespace-replacing */
  மாறிலி unsigned எழுத்து *iptr;
  மாறிலி unsigned எழுத்து *host_sep = (மாறிலி unsigned எழுத்து *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  எனில்(!relative) {
    size_t n;
    host_sep = (மாறிலி unsigned எழுத்து *)find_host_sep(url);

    /* output the first piece as-is */
    n = (மாறிலி எழுத்து *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  க்கு(iptr = host_sep; len && !result;) {
    எனில்(*iptr == ' ') {
      எனில்(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      இல்லையெனில்
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    இல்லையெனில் எனில்((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned எழுத்து out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    இல்லையெனில் எனில்(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned எழுத்து hex = (unsigned எழுத்து)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned எழுத்து out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    இல்லையெனில் {
      மாறிலி unsigned எழுத்து *start = iptr;
      போது(len) {
        எனில்(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          நிறுத்து;
        எனில்(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          நிறுத்து;
        எனில்(*iptr == '?') {
          எனில்(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            நிறுத்து;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (மாறிலி எழுத்து *)start, (size_t)(iptr - start));
    }
  }

  எனில்(result)
    திரும்பு cc2cu(result);
  திரும்பு CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(மாறிலி எழுத்து *url, எழுத்து *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (வெறுமை)buflen; /* only used in debug-builds */
  எனில்(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  எனில்(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    திரும்பு 0;
#endif
  எனில்(ISALPHA(url[0])) {
    எனில்(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    க்கு(i = 1; i < MAX_SCHEME_LEN; ++i) {
      எழுத்து s = url[i];
      எனில்(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        எனில்(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      இல்லையெனில் {
        நிறுத்து;
      }
    }
  }
  எனில்(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    எனில்(buf)
      buf[i] = 0;
    திரும்பு len;
  }
  எனில்(buf)
    buf[0] = 0;
  திரும்பு 0;
}

/* scan for byte values <= 31, 127 and maybe space */
நிலையான bool badoctets(மாறிலி எழுத்து *உள்ளீடு, size_t n, முழு flags)
{
  மாறிலி uint8_t *p = (மாறிலி unsigned எழுத்து *)உள்ளீடு;
  மாறிலி uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  போது(n--) {
    எனில்(*p <= control || *p == 127)
      திரும்பு TRUE;
    p++;
  }
  திரும்பு FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(கட்டமைப்பு Curl_URL *u,
                                        மாறிலி எழுத்து *login,
                                        size_t len,
                                        unsigned முழு flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(கட்டமைப்பு Curl_URL *u,
                                        மாறிலி எழுத்து *login,
                                        size_t len,
                                        unsigned முழு flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  எழுத்து *userp = வெற்று;
  எழுத்து *passwdp = வெற்று;
  எழுத்து *optionsp = வெற்று;
  மாறிலி கட்டமைப்பு Curl_scheme *h = வெற்று;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  மாறிலி எழுத்து *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  எனில்(!ptr)
    செல் out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  எனில்(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : வெற்று);
  எனில்(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    செல் out;
  }

  எனில்(userp) {
    எனில்(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      செல் out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  எனில்(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  எனில்(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  எனில்(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  இல்லையெனில் எனில்(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  இல்லையெனில் எனில்(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_உள்ளீடு;

  userp = passwdp = optionsp = வெற்று;

  எனில்(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    திரும்பு CURLUE_OK;
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

  திரும்பு ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(கட்டமைப்பு Curl_URL *u, கட்டமைப்பு dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(கட்டமைப்பு Curl_URL *u, கட்டமைப்பு dynbuf *host,
                              bool has_scheme)
{
  மாறிலி எழுத்து *portptr;
  மாறிலி எழுத்து *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  எனில்(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    எனில்(!portptr)
      திரும்பு CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    எனில்(*portptr) {
      எனில்(*portptr != ':')
        திரும்பு CURLUE_BAD_PORT_எண்;
    }
    இல்லையெனில்
      portptr = வெற்று;
  }
  இல்லையெனில்
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  எனில்(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    முழு rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    எனில்(!*portptr)
      திரும்பு has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_எண்;
    எனில்(*portptr == '\\')
      திரும்பு CURLUE_BACKSLASH;
    rc = curlx_str_எண்(&portptr, &port, 0xffff);
    எனில்(rc)
      திரும்பு CURLUE_BAD_PORT_எண்;
    இல்லையெனில் எனில்(*portptr == '\\')
      திரும்பு CURLUE_BACKSLASH;
    இல்லையெனில் எனில்(*portptr)
      திரும்பு CURLUE_BAD_PORT_எண்;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  திரும்பு CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

நிலையான மாறிலி bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(கட்டமைப்பு Curl_URL *u, எழுத்து *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(கட்டமைப்பு Curl_URL *u, எழுத்து *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  எனில்(hlen < 4) /* '[::]' is the shortest possible valid string */
    திரும்பு CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  எனில்(hlen != len) {
    hlen = len;
    எனில்(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      எழுத்து zoneid[MAX_ZONEID_LEN];
      முழு i = 0;
      எழுத்து *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      எனில்(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      போது(!invalid_host_char[(unsigned எழுத்து)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      எனில்(!i || (']' != *h))
        திரும்பு CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      எனில்(!u->zoneid)
        திரும்பு CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    இல்லையெனில்
      திரும்பு CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    எழுத்து dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    எனில்(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      திரும்பு CURLUE_BAD_IPV6;
    எனில்(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  திரும்பு CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
நிலையான CURLUcode hostname_check(எழுத்து *hostname, size_t hlen)
{
  size_t i;
  க்கு(i = 0; i < hlen; i++) {
    எனில்(invalid_host_char[(unsigned எழுத்து)hostname[i]])
      திரும்பு CURLUE_BAD_HOSTNAME;
  }
  எனில்((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    திரும்பு CURLUE_BAD_HOSTNAME;
  இல்லையெனில் எனில்((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    திரும்பு CURLUE_BAD_HOSTNAME;
  திரும்பு CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
நிலையான CURLUcode hostname_check6(கட்டமைப்பு Curl_URL *u, எழுத்து *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  எனில்(!hlen)
    திரும்பு CURLUE_NO_HOST;
  இல்லையெனில் எனில்(hostname[0] == '[')
    திரும்பு ipv6_parse(u, hostname, hlen);

  திரும்பு hostname_check(hostname, hlen);
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
UNITTEST முழு ipv4_normalize(கட்டமைப்பு dynbuf *host);
UNITTEST முழு ipv4_normalize(கட்டமைப்பு dynbuf *host)
{
  bool done = FALSE;
  முழு n = 0;
  மாறிலி எழுத்து *c = curlx_dyn_ptr(host);
  unsigned முழு parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  எனில்(!ISDIGIT(*c))
    திரும்பு HOST_NAME;

  போது(!done) {
    முழு rc;
    curl_off_t l;
    எனில்(*c == '0') {
      எனில்((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        எனில்(rc)
          திரும்பு HOST_NAME;
      }
      இல்லையெனில்
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    இல்லையெனில்
      rc = curlx_str_எண்(&c, &l, UINT_MAX);

    எனில்(rc) {
      எனில்(!n || (rc != STRE_NO_இலக்கம்) || *c)
        திரும்பு HOST_NAME;
      n--;
    }
    இல்லையெனில்
      parts[n] = (unsigned முழு)l;

    தேர்வு(*c) {
    நிலை '.':
      எனில்(n == 3) {
        எனில்(c[1])
          /* something follows this dot */
          திரும்பு HOST_NAME;
        done = TRUE;
      }
      இல்லையெனில் {
        n++;
        c++;
      }
      நிறுத்து;

    நிலை '\0':
      done = TRUE;
      நிறுத்து;

    இயல்பு:
      திரும்பு HOST_NAME;
    }
  }

  தேர்வு(n) {
  நிலை 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    நிறுத்து;
  நிலை 1: /* a.b -- 8.24 bits */
    எனில்((parts[0] > 0xff) || (parts[1] > 0xffffff))
      திரும்பு HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    நிறுத்து;
  நிலை 2: /* a.b.c -- 8.8.16 bits */
    எனில்((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      திரும்பு HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    நிறுத்து;
  நிலை 3: /* a.b.c.d -- 8.8.8.8 bits */
    எனில்((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      திரும்பு HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    நிறுத்து;
  }
  எனில்(result)
    திரும்பு HOST_ERROR;
  திரும்பு HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
நிலையான CURLUcode urldecode_host(கட்டமைப்பு dynbuf *host)
{
  மாறிலி எழுத்து *per;
  மாறிலி எழுத்து *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  எனில்(!per)
    /* nothing to decode */
    திரும்பு CURLUE_OK;
  இல்லையெனில் {
    /* encoded */
    size_t dlen;
    எழுத்து *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    எனில்(result)
      திரும்பு CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    எனில்(result)
      திரும்பு cc2cu(result);
  }

  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode parse_authority(கட்டமைப்பு Curl_URL *u,
                                 மாறிலி எழுத்து *auth, size_t authlen,
                                 unsigned முழு flags,
                                 கட்டமைப்பு dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  எனில்(uc)
    திரும்பு uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  எனில்(result) {
    uc = cc2cu(result);
    திரும்பு uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  எனில்(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  எனில்(!uc)
    uc = urldecode_host(host);
  எனில்(uc)
    ;
  இல்லையெனில் எனில்(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  இல்லையெனில் {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    முழு type = ipv4_normalize(host);

    எனில்(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    இல்லையெனில் எனில்(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  திரும்பு uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, மாறிலி எழுத்து *authority)
{
  CURLUcode ures;
  கட்டமைப்பு dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_உள்ளீடு_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  எனில்(ures)
    curlx_dyn_free(&host);
  இல்லையெனில் {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  திரும்பு ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

நிலையான bool is_dot(மாறிலி எழுத்து **str, size_t *clen)
{
  மாறிலி எழுத்து *p = *str;
  எனில்(*p == '.') {
    (*str)++;
    (*clen)--;
    திரும்பு TRUE;
  }
  இல்லையெனில் எனில்((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
நிலையான bool needs_dedotdot(மாறிலி எழுத்து *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  எனில்(pn < 2)
    திரும்பு FALSE;
  எனில்(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    திரும்பு FALSE;
  போது(pn) {
    எனில்(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      எனில்(!pn || ISSLASH(*p))
        திரும்பு TRUE;
      /* "../" or ".." before end of string */
      இல்லையெனில் எனில்(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        திரும்பு TRUE;
    }
    இல்லையெனில் {
      p++;
      pn--;
    }
  }
  திரும்பு FALSE;
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
UNITTEST முழு dedotdotify(மாறிலி எழுத்து *உள்ளீடு, size_t clen, எழுத்து **outp);
UNITTEST முழு dedotdotify(மாறிலி எழுத்து *உள்ளீடு, size_t clen, எழுத்து **outp)
{
  கட்டமைப்பு dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  மாறிலி எழுத்து *dinput = உள்ளீடு;
  size_t dlen = clen;

  *outp = வெற்று;
  எனில்(!needs_dedotdot(உள்ளீடு, clen))
    திரும்பு 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  எனில்(is_dot(&dinput, &dlen)) {
    எனில்(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      உள்ளீடு = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    இல்லையெனில் எனில்(is_dot(&dinput, &dlen)) {
      எனில்(!dlen)
        /* .. [end] */
        செல் end;
      இல்லையெனில் எனில்(ISSLASH(*dinput)) {
        /* ../ */
        உள்ளீடு = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  போது(clen && !result) { /* until end of path content */
    எனில்(ISSLASH(*உள்ளீடு)) {
      மாறிலி எழுத்து *p = &உள்ளீடு[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      எனில்(is_dot(&p, &blen)) {
        எனில்(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          நிறுத்து;
        }
        இல்லையெனில் எனில்(ISSLASH(*p)) { /* /./ */
          உள்ளீடு = p;
          clen = blen;
          தொடர்;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        இல்லையெனில் எனில்(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          எனில்(len) {
            மாறிலி எழுத்து *ptr = curlx_dyn_ptr(&out);
            மாறிலி எழுத்து *last = memrchr(ptr, '/', len);
            எனில்(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          எனில்(blen) { /* /../ */
            உள்ளீடு = p;
            clen = blen;
            தொடர்;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          நிறுத்து;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, உள்ளீடு, 1);
    உள்ளீடு++;
    clen--;
  }
end:
  எனில்(!result) {
    எனில்(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    இல்லையெனில் {
      *outp = curlx_strdup("");
      எனில்(!*outp)
        திரும்பு 1;
    }
  }
  திரும்பு result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(மாறிலி எழுத்து *url, size_t urllen, CURLU *u,
                              மாறிலி எழுத்து **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(மாறிலி எழுத்து *url, size_t urllen, CURLU *u,
                              மாறிலி எழுத்து **pathp, size_t *pathlenp)
{
  மாறிலி எழுத்து *path;
  size_t pathlen;

  *pathp = வெற்று;
  *pathlenp = 0;
  எனில்(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    திரும்பு CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  எனில்(path[0] != '/')
    திரும்பு CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  எனில்(path[1] == '/') {
    /* swallow the two slashes */
    மாறிலி எழுத்து *ptr = &path[2];

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
    எனில்(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      எனில்(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      இல்லையெனில்
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        திரும்பு CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#எனில் !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  எனில்(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    திரும்பு CURLUE_BAD_FILE_URL;
  }
#இல்லையெனில்
  /* If the path starts with a slash and a drive letter, ditch the slash */
  எனில்('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  எனில்(!u->scheme)
    திரும்பு CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode parse_scheme(மாறிலி எழுத்து *url, CURLU *u, எழுத்து *schemebuf,
                              size_t schemelen, unsigned முழு flags,
                              மாறிலி எழுத்து **hostpp)
{
  /* clear path */
  மாறிலி எழுத்து *schemep = வெற்று;

  எனில்(schemelen) {
    முழு இலக்கம்_slashes = 0;
    மாறிலி எழுத்து *p = &url[schemelen + 1];
    எனில்(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      திரும்பு CURLUE_UNSUPPORTED_SCHEME;

    எனில்(!ISSLASH(*p))
      /* less than one */
      திரும்பு CURLUE_BAD_SLASHES;
    எனில்((flags & CURLU_NO_AUTHORITY)) {
      போது(ISSLASH(*p) && (இலக்கம்_slashes < 2)) {
        p++;
        இலக்கம்_slashes++;
      }
    }
    இல்லையெனில் {
      போது(ISSLASH(*p) && (இலக்கம்_slashes < 4)) {
        p++;
        இலக்கம்_slashes++;
      }
      எனில்(இலக்கம்_slashes > 3)
        திரும்பு CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  இல்லையெனில் {
    /* no scheme! */

    எனில்(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      திரும்பு CURLUE_BAD_SCHEME;

    எனில்(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  எனில்(schemep) {
    u->scheme = curlx_strdup(schemep);
    எனில்(!u->scheme)
      திரும்பு CURLUE_OUT_OF_MEMORY;
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode guess_scheme(CURLU *u, கட்டமைப்பு dynbuf *host)
{
  மாறிலி எழுத்து *hostname = curlx_dyn_ptr(host);
  மாறிலி எழுத்து *schemep = வெற்று;
  /* legacy curl-style guess based on hostname */
  எனில்(checkprefix("ftp.", hostname))
    schemep = "ftp";
  இல்லையெனில் எனில்(checkprefix("dict.", hostname))
    schemep = "dict";
  இல்லையெனில் எனில்(checkprefix("ldap.", hostname))
    schemep = "ldap";
  இல்லையெனில் எனில்(checkprefix("imap.", hostname))
    schemep = "imap";
  இல்லையெனில் எனில்(checkprefix("smtp.", hostname))
    schemep = "smtp";
  இல்லையெனில் எனில்(checkprefix("pop3.", hostname))
    schemep = "pop3";
  இல்லையெனில்
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  எனில்(!u->scheme)
    திரும்பு CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode handle_fragment(CURLU *u, மாறிலி எழுத்து *fragment,
                                 size_t fraglen, unsigned முழு flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  எனில்(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    எனில்(flags & CURLU_URLENCODE) {
      கட்டமைப்பு dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_உள்ளீடு_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      எனில்(ures)
        திரும்பு ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    இல்லையெனில் {
      எனில்(badoctets(fragment, fraglen, flags))
        திரும்பு CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      எனில்(!u->fragment)
        திரும்பு CURLUE_OUT_OF_MEMORY;
    }
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode handle_query(CURLU *u, மாறிலி எழுத்து *query,
                              size_t qlen, unsigned முழு flags)
{
  u->query_present = TRUE;
  எனில்(qlen > 1) {
    எனில்(flags & CURLU_URLENCODE) {
      கட்டமைப்பு dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_உள்ளீடு_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      எனில்(ures)
        திரும்பு ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    இல்லையெனில் {
      எனில்(badoctets(query, qlen, flags))
        திரும்பு CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      எனில்(!u->query)
        திரும்பு CURLUE_OUT_OF_MEMORY;
    }
  }
  இல்லையெனில் {
    /* single byte query */
    u->query = curlx_strdup("");
    எனில்(!u->query)
      திரும்பு CURLUE_OUT_OF_MEMORY;
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode handle_path(CURLU *u, மாறிலி எழுத்து *path,
                             size_t pathlen, unsigned முழு flags,
                             bool is_file)
{
  CURLUcode ures;
  எனில்(pathlen && (flags & CURLU_URLENCODE)) {
    கட்டமைப்பு dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_உள்ளீடு_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    எனில்(ures)
      திரும்பு ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  எனில்(pathlen >= (size_t)(1 + !is_file)) {
    எனில்(badoctets(path, pathlen, flags))
      திரும்பு CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    எனில்(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      எனில்(!u->path)
        திரும்பு CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    இல்லையெனில் எனில்(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    எனில்(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      எழுத்து *dedot;
      முழு err = dedotdotify(path, pathlen, &dedot);
      எனில்(err)
        திரும்பு CURLUE_OUT_OF_MEMORY;
      எனில்(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode parseurl(மாறிலி எழுத்து *url, CURLU *u, unsigned முழு flags)
{
  மாறிலி எழுத்து *path;
  size_t pathlen;
  எழுத்து schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  கட்டமைப்பு dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  எனில்(urllen > CURL_MAX_உள்ளீடு_LENGTH)
    திரும்பு CURLUE_MALFORMED_உள்ளீடு;

  curlx_dyn_init(&host, CURL_MAX_உள்ளீடு_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, அளவு(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  எனில்(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  இல்லையெனில் {
    மாறிலி எழுத்து *hostp = வெற்று;
    மாறிலி எழுத்து *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    எனில்(ures)
      செல் fail;

    /* find the end of the hostname + port number */
    p = hostp;
    போது(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    எனில்(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      எனில்(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    இல்லையெனில் எனில்(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      எனில்(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    இல்லையெனில்
      ures = CURLUE_NO_HOST;
  }
  எனில்(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    மாறிலி எழுத்து *fragment = memchr(path, '#', pathlen);
    எனில்(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  எனில்(!ures) {
    மாறிலி எழுத்து *query = memchr(path, '?', pathlen);
    எனில்(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  எனில்(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  எனில்(!ures) {
    u->host = curlx_dyn_ptr(&host);
    திரும்பு CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  திரும்பு ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
நிலையான CURLUcode parseurl_and_replace(மாறிலி எழுத்து *url, CURLU *u,
                                      unsigned முழு flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, அளவு(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  எனில்(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  திரும்பு ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
நிலையான CURLUcode redirect_url(மாறிலி எழுத்து *base, மாறிலி எழுத்து *relurl,
                              CURLU *u, unsigned முழு flags)
{
  கட்டமைப்பு dynbuf urlbuf;
  bool host_changed = FALSE;
  மாறிலி எழுத்து *useurl = relurl;
  மாறிலி எழுத்து *cutoff = வெற்று;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  மாறிலி எழுத்து *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  மாறிலி எழுத்து *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  எனில்(!base)
    திரும்பு CURLUE_MALFORMED_உள்ளீடு; /* should never happen */

  /* handle different relative URL types */
  தேர்வு(relurl[0]) {
  நிலை '/':
    எனில்(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    இல்லையெனில்
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    நிறுத்து;

  நிலை '#':
    /* fragment-only change */
    எனில்(u->fragment_present)
      cutoff = strchr(protsep, '#');
    நிறுத்து;

  இயல்பு:
    /* path or query-only change */
    எனில்(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    இல்லையெனில் எனில்(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    எனில்(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      எனில்(cutoff)
        cutoff++; /* truncate after last slash */
    }
    நிறுத்து;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_உள்ளீடு_LENGTH);

  எனில்(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  இல்லையெனில்
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  திரும்பு uc;
}

/*
 */
CURLU *curl_url(வெறுமை)
{
  திரும்பு curlx_calloc(1, அளவு(கட்டமைப்பு Curl_URL));
}

வெறுமை curl_url_cleanup(CURLU *u)
{
  எனில்(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  செய் {                                          \
    எனில்((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      எனில்(!(dest)->name)                         \
        செல் fail;                              \
    }                                           \
  } போது(0)

CURLU *curl_url_dup(மாறிலி CURLU *in)
{
  கட்டமைப்பு Curl_URL *u;

  எனில்(!in)
    திரும்பு வெற்று;

  u = curlx_calloc(1, அளவு(கட்டமைப்பு Curl_URL));
  எனில்(u) {
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
  திரும்பு u;
fail:
  curl_url_cleanup(u);
  திரும்பு வெற்று;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#இல்லையெனில்
நிலையான CURLUcode host_decode(மாறிலி எழுத்து *host, எழுத்து **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  எனில்(result)
    திரும்பு (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode host_encode(மாறிலி எழுத்து *host, எழுத்து **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  எனில்(result)
    திரும்பு (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  திரும்பு CURLUE_OK;
}
#endif

நிலையான CURLUcode urlget_format(மாறிலி CURLU *u, CURLUPart what,
                               மாறிலி எழுத்து *ptr, எழுத்து **partp,
                               bool plusdecode, unsigned முழு flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  எழுத்து *part = curlx_memdup0(ptr, partlen);
  *partp = வெற்று;
  எனில்(!part)
    திரும்பு CURLUE_OUT_OF_MEMORY;
  எனில்(plusdecode) {
    /* convert + to space */
    எழுத்து *plus = part;
    size_t i = 0;
    க்கு(i = 0; i < partlen; ++plus, i++) {
      எனில்(*plus == '+')
        *plus = ' ';
    }
  }
  எனில்(urldecode) {
    எழுத்து *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    எனில்(result)
      திரும்பு CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  எனில்(urlencode) {
    கட்டமைப்பு dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_உள்ளீடு_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    எனில்(uc)
      திரும்பு uc;
    part = curlx_dyn_ptr(&enc);
  }
  இல்லையெனில் எனில்(punycode) {
    எனில்(!Curl_is_ASCII_name(u->host)) {
      எழுத்து *punyversion = வெற்று;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      எனில்(uc)
        திரும்பு uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      எனில்(uc) {
        curlx_free(punyversion);
        திரும்பு uc;
      }
      part = punyversion;
    }
  }
  இல்லையெனில் எனில்(depunyfy && Curl_is_ASCII_name(u->host)) {
    எழுத்து *unpunified = வெற்று;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    எனில்(uc)
      திரும்பு uc;
    part = unpunified;
  }
  *partp = part;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode file_url(மாறிலி CURLU *u, எழுத்து **part,
                          மாறிலி எழுத்து *fragmentsep,
                          மாறிலி எழுத்து *querysep)
{
  எழுத்து *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  எனில்(!url)
    திரும்பு CURLUE_OUT_OF_MEMORY;

  *part = url;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode urlget_url(மாறிலி CURLU *u, எழுத்து **part, unsigned முழு flags)
{
  எழுத்து *url;
  எழுத்து *allochost = வெற்று;
  மாறிலி எழுத்து *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  மாறிலி எழுத்து *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  எழுத்து portbuf[7];
  எனில்(curl_strequal("file", u->scheme))
    திரும்பு file_url(u, part, fragmentsep, querysep);
  இல்லையெனில் எனில்(!u->host)
    திரும்பு CURLUE_NO_HOST;
  இல்லையெனில் {
    மாறிலி எழுத்து *scheme;
    எழுத்து *options = u->options;
    எழுத்து *port = வெற்று;
    மாறிலி கட்டமைப்பு Curl_scheme *h = வெற்று;
    எழுத்து schemebuf[MAX_SCHEME_LEN + 5];
    எனில்(u->scheme)
      scheme = u->scheme;
    இல்லையெனில் எனில்(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    இல்லையெனில்
      திரும்பு CURLUE_NO_SCHEME;

    எனில்(u->port_present) {
      curl_msnprintf(portbuf, அளவு(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    எனில்(h) {
      எனில்(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, அளவு(portbuf), "%u", h->defport);
        port = portbuf;
      }
      இல்லையெனில் எனில்(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = வெற்று;
      }

      எனில்(!(h->flags & PROTOPT_URLOPTIONS))
        options = வெற்று;
    }

    எனில்(u->host[0] == '[') {
      எனில்(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        கட்டமைப்பு dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_உள்ளீடு_LENGTH);
        எனில்(curlx_dyn_addf(&enc, "%.*s%%25%s]", (முழு)hostlen - 1, u->host,
                          u->zoneid))
          திரும்பு CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    இல்லையெனில் எனில்(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(வெற்று, u->host, 0);
      எனில்(!allochost)
        திரும்பு CURLUE_OUT_OF_MEMORY;
    }
    இல்லையெனில் எனில்(flags & CURLU_PUNYCODE) {
      எனில்(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        எனில்(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        எனில்(ret) {
          curlx_safefree(allochost);
          திரும்பு ret;
        }
      }
    }
    இல்லையெனில் எனில்(flags & CURLU_PUNY2IDN) {
      எனில்(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        எனில்(ret)
          திரும்பு ret;
      }
    }

    எனில்(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, அளவு(schemebuf), "%s://", scheme);
    இல்லையெனில்
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
  எனில்(!url)
    திரும்பு CURLUE_OUT_OF_MEMORY;
  *part = url;
  திரும்பு CURLUE_OK;
}

CURLUcode curl_url_get(மாறிலி CURLU *u, CURLUPart what,
                       எழுத்து **part, unsigned முழு flags)
{
  மாறிலி எழுத்து *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  எழுத்து portbuf[7];
  bool plusdecode = FALSE;
  எனில்(!u)
    திரும்பு CURLUE_BAD_HANDLE;
  எனில்(!part)
    திரும்பு CURLUE_BAD_PARTPOINTER;
  *part = வெற்று;

  தேர்வு(what) {
  நிலை CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    எனில்((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      திரும்பு CURLUE_NO_SCHEME;
    நிறுத்து;
  நிலை CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    நிறுத்து;
  நிலை CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    நிறுத்து;
  நிலை CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    நிறுத்து;
  நிலை CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    நிறுத்து;
  நிலை CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    நிறுத்து;
  நிலை CURLUPART_PORT:
    ptr = வெற்று;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    எனில்(u->port_present) {
      மாறிலி கட்டமைப்பு Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : வெற்று;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      எனில்(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = வெற்று;
      }
      இல்லையெனில் {
        curl_msnprintf(portbuf, அளவு(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    இல்லையெனில் எனில்((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      மாறிலி கட்டமைப்பு Curl_scheme *h = Curl_get_scheme(u->scheme);
      எனில்(h) {
        curl_msnprintf(portbuf, அளவு(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    நிறுத்து;
  நிலை CURLUPART_PATH:
    ptr = u->path;
    எனில்(!ptr)
      ptr = "/";
    நிறுத்து;
  நிலை CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    எனில்(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = வெற்று;
    நிறுத்து;
  நிலை CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    எனில்(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    நிறுத்து;
  நிலை CURLUPART_URL:
    திரும்பு urlget_url(u, part, flags);
  இயல்பு:
    ptr = வெற்று;
    நிறுத்து;
  }
  எனில்(ptr)
    திரும்பு urlget_format(u, what, ptr, part, plusdecode, flags);

  திரும்பு ifmissing;
}

நிலையான CURLUcode set_url_scheme(CURLU *u, மாறிலி எழுத்து *scheme,
                                unsigned முழு flags)
{
  size_t plen = strlen(scheme);
  மாறிலி கட்டமைப்பு Curl_scheme *h = வெற்று;
  எனில்((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    திரும்பு CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  எனில்(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    திரும்பு CURLUE_UNSUPPORTED_SCHEME;
  எனில்(!h) {
    மாறிலி எழுத்து *s = scheme;
    எனில்(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      போது(--plen) {
        எனில்(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        இல்லையெனில்
          திரும்பு CURLUE_BAD_SCHEME;
      }
    }
    இல்லையெனில்
      திரும்பு CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode set_url_port(CURLU *u, மாறிலி எழுத்து *provided_port)
{
  curl_off_t port;
  எனில்(!ISDIGIT(provided_port[0]))
    /* not a number */
    திரும்பு CURLUE_BAD_PORT_எண்;
  எனில்(curlx_str_எண்(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    திரும்பு CURLUE_BAD_PORT_எண்;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode set_url(CURLU *u, மாறிலி எழுத்து *url, size_t part_size,
                         unsigned முழு flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  எழுத்து *oldurl = வெற்று;

  எனில்(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    எனில்(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      திரும்பு CURLUE_OK;
    }
    எனில்(uc == CURLUE_OUT_OF_MEMORY)
      திரும்பு uc;
    திரும்பு CURLUE_MALFORMED_உள்ளீடு;
  }

  /* if the new URL is absolute replace the existing with the new. */
  எனில்(Curl_is_absolute_url(url, வெற்று, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    திரும்பு parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  எனில்(uc == CURLUE_OUT_OF_MEMORY)
    திரும்பு uc;
  இல்லையெனில் எனில்(uc)
    திரும்பு parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  திரும்பு uc;
}

நிலையான CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  தேர்வு(what) {
  நிலை CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, அளவு(கட்டமைப்பு Curl_URL));
    நிறுத்து;
  நிலை CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    நிறுத்து;
  நிலை CURLUPART_USER:
    curlx_safefree(u->user);
    நிறுத்து;
  நிலை CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    நிறுத்து;
  நிலை CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    நிறுத்து;
  நிலை CURLUPART_HOST:
    curlx_safefree(u->host);
    நிறுத்து;
  நிலை CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    நிறுத்து;
  நிலை CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    நிறுத்து;
  நிலை CURLUPART_PATH:
    curlx_safefree(u->path);
    நிறுத்து;
  நிலை CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    நிறுத்து;
  நிலை CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLUE_UNKNOWN_PART;
  }
  திரும்பு CURLUE_OK;
}

நிலையான bool allowed_in_path(unsigned எழுத்து x)
{
  தேர்வு(x) {
  நிலை '!':
  நிலை '$':
  நிலை '&':
  நிலை '\'':
  நிலை '(':
  நிலை ')':
  நிலை '{':
  நிலை '}':
  நிலை '[':
  நிலை ']':
  நிலை '*':
  நிலை '+':
  நிலை ',':
  நிலை ';':
  நிலை '=':
  நிலை ':':
  நிலை '@':
  நிலை '/':
    திரும்பு TRUE;
  }
  திரும்பு FALSE;
}

நிலையான CURLUcode url_encode_part(கட்டமைப்பு dynbuf *encp,
                                 மாறிலி எழுத்து *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  மாறிலி unsigned எழுத்து *i;

  க்கு(i = (மாறிலி unsigned எழுத்து *)part; *i; i++) {
    CURLcode result;
    எனில்((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    இல்லையெனில் எனில்(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      எனில்((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    இல்லையெனில் {
      unsigned எழுத்து out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    எனில்(result)
      திரும்பு cc2cu(result);
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode url_uppercasehex_part(கட்டமைப்பு dynbuf *encp,
                                       மாறிலி எழுத்து *part)
{
  எழுத்து *p;
  CURLcode result = curlx_dyn_add(encp, part);
  எனில்(result)
    திரும்பு cc2cu(result);
  p = curlx_dyn_ptr(encp);
  போது(*p) {
    /* make sure percent encoded are upper case */
    எனில்((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    இல்லையெனில்
      p++;
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode url_append_query(CURLU *u, கட்டமைப்பு dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  எனில்(querylen) {
    கட்டமைப்பு dynbuf qbuf;
    CURLcode result;
    மாறிலி எழுத்து *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_உள்ளீடு_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    எனில்(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    எனில்(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    எனில்(result)
      செல் nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    திரும்பு CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    திரும்பு cc2cu(result);
  }
  இல்லையெனில் {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  திரும்பு CURLUE_OK;
}

நிலையான CURLUcode url_sethost(CURLU *u, கட்டமைப்பு dynbuf *encp,
                             bool urlencode,
                             unsigned முழு flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  எழுத்து *newp = curlx_dyn_ptr(encp);
  எனில்(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  இல்லையெனில் எனில்(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    எழுத்து *decoded = வெற்று;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    எனில்(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    இல்லையெனில் {
      கட்டமைப்பு dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_உள்ளீடு_LENGTH);
      எனில்(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      இல்லையெனில் எனில்(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        எனில்(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  இல்லையெனில் எனில்(hostname_check6(u, newp, n))
    bad = TRUE;
  இல்லையெனில் எனில்(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  எனில்(bad) {
    curlx_dyn_free(encp);
    திரும்பு CURLUE_BAD_HOSTNAME;
  }
  திரும்பு CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       மாறிலி எழுத்து *part, unsigned முழு flags)
{
  எழுத்து **storep = வெற்று;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  எனில்(!u)
    திரும்பு CURLUE_BAD_HANDLE;
  எனில்(!part)
    /* setting a part to NULL clears it */
    திரும்பு urlset_clear(u, what);

  nalloc = strlen(part);
  எனில்(nalloc > CURL_MAX_உள்ளீடு_LENGTH)
    /* excessive input length */
    திரும்பு CURLUE_MALFORMED_உள்ளீடு;

  தேர்வு(what) {
  நிலை CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    எனில்(status)
      திரும்பு status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    நிறுத்து;
  }
  நிலை CURLUPART_USER:
    storep = &u->user;
    நிறுத்து;
  நிலை CURLUPART_PASSWORD:
    storep = &u->password;
    நிறுத்து;
  நிலை CURLUPART_OPTIONS:
    storep = &u->options;
    நிறுத்து;
  நிலை CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    நிறுத்து;
  நிலை CURLUPART_ZONEID:
    storep = &u->zoneid;
    நிறுத்து;
  நிலை CURLUPART_PORT:
    திரும்பு set_url_port(u, part);
  நிலை CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    நிறுத்து;
  நிலை CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    நிறுத்து;
  நிலை CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    நிறுத்து;
  நிலை CURLUPART_URL:
    திரும்பு set_url(u, part, nalloc, flags);
  இயல்பு:
    திரும்பு CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    மாறிலி எழுத்து *newp = வெற்று;
    கட்டமைப்பு dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    எனில்(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      எனில்(result)
        திரும்பு cc2cu(result);
    }
    எனில்(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    இல்லையெனில்
      status = url_uppercasehex_part(&enc, part);
    எனில்(!status) {
      newp = curlx_dyn_ptr(&enc);

      எனில்(appendquery && newp)
        திரும்பு url_append_query(u, &enc);
      இல்லையெனில் எனில்(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    எனில்(status)
      திரும்பு status;

    எனில்(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (எழுத்து *)CURL_UNCONST(newp);
  }
  திரும்பு CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  மாறிலி கட்டமைப்பு Curl_scheme *s = வெற்று;

  /* base must be an absolute URL */
  எனில்(!base->scheme || !base->host)
    திரும்பு FALSE;
  எனில்(href->scheme && !curl_strequal(base->scheme, href->scheme))
    திரும்பு FALSE;
  எனில்(href->host) {
    எனில்(!curl_strequal(base->host, href->host))
      திரும்பு FALSE;

    எனில்(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      எனில்(!s) /* Cannot match default port for unknown scheme */
        திரும்பு FALSE;
      /* to match, the present one must be the default port */
      எனில்((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        திரும்பு FALSE;
    }
    இல்லையெனில் எனில்(base->portnum != href->portnum) /* both present or missing */
      திரும்பு FALSE;

    எனில்(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      திரும்பு FALSE;
  }
  இல்லையெனில் எனில்(href->port_present) /* no host in href, then there must be no port */
    திரும்பு FALSE;
  திரும்பு TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  எனில்(u->port_present) {
    *pport = u->portnum;
    திரும்பு CURLUE_OK;
  }
  இல்லையெனில் எனில்(u->scheme) {
    மாறிலி கட்டமைப்பு Curl_scheme *s = Curl_get_scheme(u->scheme);
    எனில்(s && s->defport) {
      *pport = s->defport;
      திரும்பு CURLUE_OK;
    }
  }
  *pport = 0;
  திரும்பு CURLUE_NO_PORT;
}
