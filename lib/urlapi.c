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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "urlapi-int.h"
#અન્તર્ભાવ "strcase.h"
#અન્તર્ભાવ "url.h"
#અન્તર્ભાવ "escape.h"
#અન્તર્ભાવ "curlx/inet_pton.h"
#અન્તર્ભાવ "curlx/inet_ntop.h"
#અન્તર્ભાવ "curlx/strdup.h"
#અન્તર્ભાવ "idn.h"
#અન્તર્ભાવ "curlx/strparse.h"
#અન્તર્ભાવ "curl_memrchr.h"

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
#જો !defined(USE_IPV6) && !defined(AF_INET6)
#define AF_INET6 (AF_INET + 1)
#endif

#define DEFAULT_SCHEME "https"

સ્થિર શૂન્ય free_urlhandle(સંરચના Curl_URL *u)
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
સ્થિર અચલ અક્ષર *find_host_sep(અચલ અક્ષર *url)
{
  /* Find the start of the hostname */
  અચલ અક્ષર *sep = strstr(url, "//");
  જો(!sep)
    sep = url;
  નહીંતર
    sep += 2;

  /* Find first / or ? */
  જ્યારે(*sep && *sep != '/' && *sep != '?')
    sep++;

  પાછા sep;
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
UNITTEST CURLUcode urlencode_str(સંરચના dynbuf *o, અચલ અક્ષર *url,
                                 size_t len, bool relative,
                                 unsigned પૂર્ણાંક query);
UNITTEST CURLUcode urlencode_str(સંરચના dynbuf *o, અચલ અક્ષર *url,
                                 size_t len, bool relative,
                                 unsigned પૂર્ણાંક query)
{
  /* we must add this with whitespace-replacing */
  અચલ unsigned અક્ષર *iptr;
  અચલ unsigned અક્ષર *host_sep = (અચલ unsigned અક્ષર *)url;
  CURLcode result = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  જો(!relative) {
    size_t n;
    host_sep = (અચલ unsigned અક્ષર *)find_host_sep(url);

    /* output the first piece as-is */
    n = (અચલ અક્ષર *)host_sep - url;
    result = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  માટે(iptr = host_sep; len && !result;) {
    જો(*iptr == ' ') {
      જો(query != QUERY_YES)
        result = curlx_dyn_addn(o, "%20", 3);
      નહીંતર
        result = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    નહીંતર જો((*iptr < ' ') || (*iptr >= 0x7f)) {
      unsigned અક્ષર out[3] = { '%' };
      Curl_hexbyte(&out[1], *iptr);
      result = curlx_dyn_addn(o, out, 3);
      iptr++;
      len--;
    }
    નહીંતર જો(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      unsigned અક્ષર hex = (unsigned અક્ષર)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      unsigned અક્ષર out[3] = { '%' };
      Curl_hexbyte(&out[1], hex);
      result = curlx_dyn_addn(o, out, 3);
      iptr += 3;
      len -= 3;
    }
    નહીંતર {
      અચલ unsigned અક્ષર *start = iptr;
      જ્યારે(len) {
        જો(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          તોડો;
        જો(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          તોડો;
        જો(*iptr == '?') {
          જો(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            તોડો;
          }
        }
        iptr++;
        len--;
      }
      result = curlx_dyn_addn(o, (અચલ અક્ષર *)start, (size_t)(iptr - start));
    }
  }

  જો(result)
    પાછા cc2cu(result);
  પાછા CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(અચલ અક્ષર *url, અક્ષર *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (શૂન્ય)buflen; /* only used in debug-builds */
  જો(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ifdef _WIN32
  જો(guess_scheme && STARTS_WITH_DRIVE_PREFIX(url))
    પાછા 0;
#endif
  જો(ISALPHA(url[0])) {
    જો(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    માટે(i = 1; i < MAX_SCHEME_LEN; ++i) {
      અક્ષર s = url[i];
      જો(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        જો(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      નહીંતર {
        તોડો;
      }
    }
  }
  જો(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    જો(buf)
      buf[i] = 0;
    પાછા len;
  }
  જો(buf)
    buf[0] = 0;
  પાછા 0;
}

/* scan for byte values <= 31, 127 and maybe space */
સ્થિર bool badoctets(અચલ અક્ષર *આગત, size_t n, પૂર્ણાંક flags)
{
  અચલ uint8_t *p = (અચલ unsigned અક્ષર *)આગત;
  અચલ uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  જ્યારે(n--) {
    જો(*p <= control || *p == 127)
      પાછા TRUE;
    p++;
  }
  પાછા FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(સંરચના Curl_URL *u,
                                        અચલ અક્ષર *login,
                                        size_t len,
                                        unsigned પૂર્ણાંક flags,
                                        size_t *hostname_offset);
UNITTEST CURLUcode parse_hostname_login(સંરચના Curl_URL *u,
                                        અચલ અક્ષર *login,
                                        size_t len,
                                        unsigned પૂર્ણાંક flags,
                                        size_t *hostname_offset)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode result;
  અક્ષર *userp = ખાલી;
  અક્ષર *passwdp = ખાલી;
  અક્ષર *optionsp = ખાલી;
  અચલ સંરચના Curl_scheme *h = ખાલી;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  અચલ અક્ષર *ptr;

  DEBUGASSERT(login);

  *hostname_offset = 0;
  ptr = memchr(login, '@', len);
  જો(!ptr)
    જાઓ out;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  જો(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  result = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ખાલી);
  જો(result) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_OUT_OF_MEMORY;
    જાઓ out;
  }

  જો(userp) {
    જો(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      જાઓ out;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  જો(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  જો(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  જો(userp && badoctets(userp, strlen(userp), flags))
    ures = CURLUE_BAD_USER;
  નહીંતર જો(passwdp && badoctets(passwdp, strlen(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  નહીંતર જો(optionsp && badoctets(optionsp, strlen(optionsp), flags))
    ures = CURLUE_MALFORMED_આગત;

  userp = passwdp = optionsp = ખાલી;

  જો(!ures) {
    /* the hostname starts at this offset */
    *hostname_offset = ptr - login;
    પાછા CURLUE_OK;
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

  પાછા ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_port(સંરચના Curl_URL *u, સંરચના dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_port(સંરચના Curl_URL *u, સંરચના dynbuf *host,
                              bool has_scheme)
{
  અચલ અક્ષર *portptr;
  અચલ અક્ષર *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->port_present = FALSE;
  જો(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    જો(!portptr)
      પાછા CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    જો(*portptr) {
      જો(*portptr != ':')
        પાછા CURLUE_BAD_PORT_સંખ્યા;
    }
    નહીંતર
      portptr = ખાલી;
  }
  નહીંતર
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  જો(portptr) {
    curl_off_t port;
    size_t keep = portptr - hostname;
    પૂર્ણાંક rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    જો(!*portptr)
      પાછા has_scheme ? CURLUE_OK : CURLUE_BAD_PORT_સંખ્યા;
    જો(*portptr == '\\')
      પાછા CURLUE_BACKSLASH;
    rc = curlx_str_સંખ્યા(&portptr, &port, 0xffff);
    જો(rc)
      પાછા CURLUE_BAD_PORT_સંખ્યા;
    નહીંતર જો(*portptr == '\\')
      પાછા CURLUE_BACKSLASH;
    નહીંતર જો(*portptr)
      પાછા CURLUE_BAD_PORT_સંખ્યા;

    u->portnum = (uint16_t)port;
    u->port_present = TRUE;
  }

  પાછા CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

સ્થિર અચલ bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(સંરચના Curl_URL *u, અક્ષર *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(સંરચના Curl_URL *u, અક્ષર *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  જો(hlen < 4) /* '[::]' is the shortest possible valid string */
    પાછા CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  જો(hlen != len) {
    hlen = len;
    જો(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      અક્ષર zoneid[MAX_ZONEID_LEN];
      પૂર્ણાંક i = 0;
      અક્ષર *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      જો(!strncmp(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      જ્યારે(!invalid_host_char[(unsigned અક્ષર)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      જો(!i || (']' != *h))
        પાછા CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      જો(!u->zoneid)
        પાછા CURLUE_OUT_OF_MEMORY;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    નહીંતર
      પાછા CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    અક્ષર dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    જો(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      પાછા CURLUE_BAD_IPV6;
    જો(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = strlen(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  પાછા CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
સ્થિર CURLUcode hostname_check(અક્ષર *hostname, size_t hlen)
{
  size_t i;
  માટે(i = 0; i < hlen; i++) {
    જો(invalid_host_char[(unsigned અક્ષર)hostname[i]])
      પાછા CURLUE_BAD_HOSTNAME;
  }
  જો((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    પાછા CURLUE_BAD_HOSTNAME;
  નહીંતર જો((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    પાછા CURLUE_BAD_HOSTNAME;
  પાછા CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
સ્થિર CURLUcode hostname_check6(સંરચના Curl_URL *u, અક્ષર *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  જો(!hlen)
    પાછા CURLUE_NO_HOST;
  નહીંતર જો(hostname[0] == '[')
    પાછા ipv6_parse(u, hostname, hlen);

  પાછા hostname_check(hostname, hlen);
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
UNITTEST પૂર્ણાંક ipv4_normalize(સંરચના dynbuf *host);
UNITTEST પૂર્ણાંક ipv4_normalize(સંરચના dynbuf *host)
{
  bool done = FALSE;
  પૂર્ણાંક n = 0;
  અચલ અક્ષર *c = curlx_dyn_ptr(host);
  unsigned પૂર્ણાંક parts[4] = { 0, 0, 0, 0 };
  CURLcode result = CURLE_OK;

  જો(!ISDIGIT(*c))
    પાછા HOST_NAME;

  જ્યારે(!done) {
    પૂર્ણાંક rc;
    curl_off_t l;
    જો(*c == '0') {
      જો((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        જો(rc)
          પાછા HOST_NAME;
      }
      નહીંતર
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    નહીંતર
      rc = curlx_str_સંખ્યા(&c, &l, UINT_MAX);

    જો(rc) {
      જો(!n || (rc != STRE_NO_અંક) || *c)
        પાછા HOST_NAME;
      n--;
    }
    નહીંતર
      parts[n] = (unsigned પૂર્ણાંક)l;

    પસંદગી(*c) {
    સ્થિતિ '.':
      જો(n == 3) {
        જો(c[1])
          /* something follows this dot */
          પાછા HOST_NAME;
        done = TRUE;
      }
      નહીંતર {
        n++;
        c++;
      }
      તોડો;

    સ્થિતિ '\0':
      done = TRUE;
      તોડો;

    મૂળભૂત:
      પાછા HOST_NAME;
    }
  }

  પસંદગી(n) {
  સ્થિતિ 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    તોડો;
  સ્થિતિ 1: /* a.b -- 8.24 bits */
    જો((parts[0] > 0xff) || (parts[1] > 0xffffff))
      પાછા HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    તોડો;
  સ્થિતિ 2: /* a.b.c -- 8.8.16 bits */
    જો((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      પાછા HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    તોડો;
  સ્થિતિ 3: /* a.b.c.d -- 8.8.8.8 bits */
    જો((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      પાછા HOST_NAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    તોડો;
  }
  જો(result)
    પાછા HOST_ERROR;
  પાછા HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
સ્થિર CURLUcode urldecode_host(સંરચના dynbuf *host)
{
  અચલ અક્ષર *per;
  અચલ અક્ષર *hostname = curlx_dyn_ptr(host);
  per = memchr(hostname, '%', curlx_dyn_len(host));
  જો(!per)
    /* nothing to decode */
    પાછા CURLUE_OK;
  નહીંતર {
    /* encoded */
    size_t dlen;
    અક્ષર *decoded;
    CURLcode result = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    જો(result)
      પાછા CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    result = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    જો(result)
      પાછા cc2cu(result);
  }

  પાછા CURLUE_OK;
}

સ્થિર CURLUcode parse_authority(સંરચના Curl_URL *u,
                                 અચલ અક્ષર *auth, size_t authlen,
                                 unsigned પૂર્ણાંક flags,
                                 સંરચના dynbuf *host,
                                 bool has_scheme)
{
  size_t offset;
  CURLUcode uc;
  CURLcode result;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &offset);
  જો(uc)
    પાછા uc;

  result = curlx_dyn_addn(host, auth + offset, authlen - offset);
  જો(result) {
    uc = cc2cu(result);
    પાછા uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_port(u, host, has_scheme);

  જો(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  જો(!uc)
    uc = urldecode_host(host);
  જો(uc)
    ;
  નહીંતર જો(auth[offset] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  નહીંતર {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    પૂર્ણાંક type = ipv4_normalize(host);

    જો(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    નહીંતર જો(type == HOST_ERROR)
      uc = CURLUE_OUT_OF_MEMORY;
  }

  પાછા uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_set_authority(CURLU *u, અચલ અક્ષર *authority)
{
  CURLUcode ures;
  સંરચના dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_આગત_LENGTH);

  ures = parse_authority(u, authority, strlen(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  જો(ures)
    curlx_dyn_free(&host);
  નહીંતર {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  પાછા ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

સ્થિર bool is_dot(અચલ અક્ષર **str, size_t *clen)
{
  અચલ અક્ષર *p = *str;
  જો(*p == '.') {
    (*str)++;
    (*clen)--;
    પાછા TRUE;
  }
  નહીંતર જો((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    પાછા TRUE;
  }
  પાછા FALSE;
}

#define ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
સ્થિર bool needs_dedotdot(અચલ અક્ષર *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  જો(pn < 2)
    પાછા FALSE;
  જો(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    પાછા FALSE;
  જ્યારે(pn) {
    જો(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      જો(!pn || ISSLASH(*p))
        પાછા TRUE;
      /* "../" or ".." before end of string */
      નહીંતર જો(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        પાછા TRUE;
    }
    નહીંતર {
      p++;
      pn--;
    }
  }
  પાછા FALSE;
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
UNITTEST પૂર્ણાંક dedotdotify(અચલ અક્ષર *આગત, size_t clen, અક્ષર **outp);
UNITTEST પૂર્ણાંક dedotdotify(અચલ અક્ષર *આગત, size_t clen, અક્ષર **outp)
{
  સંરચના dynbuf out;
  CURLcode result = CURLE_OK;

  /* variables for leading dot checks */
  અચલ અક્ષર *dinput = આગત;
  size_t dlen = clen;

  *outp = ખાલી;
  જો(!needs_dedotdot(આગત, clen))
    પાછા 0;

  curlx_dyn_init(&out, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  જો(is_dot(&dinput, &dlen)) {
    જો(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      આગત = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    નહીંતર જો(is_dot(&dinput, &dlen)) {
      જો(!dlen)
        /* .. [end] */
        જાઓ end;
      નહીંતર જો(ISSLASH(*dinput)) {
        /* ../ */
        આગત = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  જ્યારે(clen && !result) { /* until end of path content */
    જો(ISSLASH(*આગત)) {
      અચલ અક્ષર *p = &આગત[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      જો(is_dot(&p, &blen)) {
        જો(!blen) { /* /. */
          result = curlx_dyn_addn(&out, "/", 1);
          તોડો;
        }
        નહીંતર જો(ISSLASH(*p)) { /* /./ */
          આગત = p;
          clen = blen;
          ચાલુ;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        નહીંતર જો(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&out);
          જો(len) {
            અચલ અક્ષર *ptr = curlx_dyn_ptr(&out);
            અચલ અક્ષર *last = memrchr(ptr, '/', len);
            જો(last)
              /* trim the output at the slash */
              curlx_dyn_setlen(&out, last - ptr);
          }

          જો(blen) { /* /../ */
            આગત = p;
            clen = blen;
            ચાલુ;
          }
          result = curlx_dyn_addn(&out, "/", 1);
          તોડો;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    result = curlx_dyn_addn(&out, આગત, 1);
    આગત++;
    clen--;
  }
end:
  જો(!result) {
    જો(curlx_dyn_len(&out))
      *outp = curlx_dyn_ptr(&out);
    નહીંતર {
      *outp = curlx_strdup("");
      જો(!*outp)
        પાછા 1;
    }
  }
  પાછા result ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(અચલ અક્ષર *url, size_t urllen, CURLU *u,
                              અચલ અક્ષર **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(અચલ અક્ષર *url, size_t urllen, CURLU *u,
                              અચલ અક્ષર **pathp, size_t *pathlenp)
{
  અચલ અક્ષર *path;
  size_t pathlen;

  *pathp = ખાલી;
  *pathlenp = 0;
  જો(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    પાછા CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  path = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  જો(path[0] != '/')
    પાછા CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  જો(path[1] == '/') {
    /* swallow the two slashes */
    અચલ અક્ષર *ptr = &path[2];

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
    જો(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_PREFIX(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      જો(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      નહીંતર
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        પાછા CURLUE_BAD_FILE_URL;
    }

    path = ptr;
    pathlen = urllen - (ptr - url);
  }

#જો !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  જો(('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) ||
     STARTS_WITH_URL_DRIVE_PREFIX(path)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    પાછા CURLUE_BAD_FILE_URL;
  }
#નહીંતર
  /* If the path starts with a slash and a drive letter, ditch the slash */
  જો('/' == path[0] && STARTS_WITH_URL_DRIVE_PREFIX(&path[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    path++;
    pathlen--;
  }
#endif
  u->scheme = curlx_strdup("file");
  જો(!u->scheme)
    પાછા CURLUE_OUT_OF_MEMORY;

  *pathp = path;
  *pathlenp = pathlen;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode parse_scheme(અચલ અક્ષર *url, CURLU *u, અક્ષર *schemebuf,
                              size_t schemelen, unsigned પૂર્ણાંક flags,
                              અચલ અક્ષર **hostpp)
{
  /* clear path */
  અચલ અક્ષર *schemep = ખાલી;

  જો(schemelen) {
    પૂર્ણાંક અંક_slashes = 0;
    અચલ અક્ષર *p = &url[schemelen + 1];
    જો(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      પાછા CURLUE_UNSUPPORTED_SCHEME;

    જો(!ISSLASH(*p))
      /* less than one */
      પાછા CURLUE_BAD_SLASHES;
    જો((flags & CURLU_NO_AUTHORITY)) {
      જ્યારે(ISSLASH(*p) && (અંક_slashes < 2)) {
        p++;
        અંક_slashes++;
      }
    }
    નહીંતર {
      જ્યારે(ISSLASH(*p) && (અંક_slashes < 4)) {
        p++;
        અંક_slashes++;
      }
      જો(અંક_slashes > 3)
        પાછા CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  નહીંતર {
    /* no scheme! */

    જો(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      પાછા CURLUE_BAD_SCHEME;

    જો(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  જો(schemep) {
    u->scheme = curlx_strdup(schemep);
    જો(!u->scheme)
      પાછા CURLUE_OUT_OF_MEMORY;
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode guess_scheme(CURLU *u, સંરચના dynbuf *host)
{
  અચલ અક્ષર *hostname = curlx_dyn_ptr(host);
  અચલ અક્ષર *schemep = ખાલી;
  /* legacy curl-style guess based on hostname */
  જો(checkprefix("ftp.", hostname))
    schemep = "ftp";
  નહીંતર જો(checkprefix("dict.", hostname))
    schemep = "dict";
  નહીંતર જો(checkprefix("ldap.", hostname))
    schemep = "ldap";
  નહીંતર જો(checkprefix("imap.", hostname))
    schemep = "imap";
  નહીંતર જો(checkprefix("smtp.", hostname))
    schemep = "smtp";
  નહીંતર જો(checkprefix("pop3.", hostname))
    schemep = "pop3";
  નહીંતર
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  જો(!u->scheme)
    પાછા CURLUE_OUT_OF_MEMORY;

  u->guessed_scheme = TRUE;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode handle_fragment(CURLU *u, અચલ અક્ષર *fragment,
                                 size_t fraglen, unsigned પૂર્ણાંક flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  જો(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    જો(flags & CURLU_URLENCODE) {
      સંરચના dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_આગત_LENGTH);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      જો(ures)
        પાછા ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    નહીંતર {
      જો(badoctets(fragment, fraglen, flags))
        પાછા CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      જો(!u->fragment)
        પાછા CURLUE_OUT_OF_MEMORY;
    }
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode handle_query(CURLU *u, અચલ અક્ષર *query,
                              size_t qlen, unsigned પૂર્ણાંક flags)
{
  u->query_present = TRUE;
  જો(qlen > 1) {
    જો(flags & CURLU_URLENCODE) {
      સંરચના dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_આગત_LENGTH);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      જો(ures)
        પાછા ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    નહીંતર {
      જો(badoctets(query, qlen, flags))
        પાછા CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      જો(!u->query)
        પાછા CURLUE_OUT_OF_MEMORY;
    }
  }
  નહીંતર {
    /* single byte query */
    u->query = curlx_strdup("");
    જો(!u->query)
      પાછા CURLUE_OUT_OF_MEMORY;
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode handle_path(CURLU *u, અચલ અક્ષર *path,
                             size_t pathlen, unsigned પૂર્ણાંક flags,
                             bool is_file)
{
  CURLUcode ures;
  જો(pathlen && (flags & CURLU_URLENCODE)) {
    સંરચના dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_આગત_LENGTH);
    ures = urlencode_str(&enc, path, pathlen, TRUE, QUERY_NO);
    જો(ures)
      પાછા ures;
    pathlen = curlx_dyn_len(&enc);
    path = u->path = curlx_dyn_ptr(&enc);
  }

  જો(pathlen >= (size_t)(1 + !is_file)) {
    જો(badoctets(path, pathlen, flags))
      પાછા CURLUE_BAD_PATH;

    /* paths for file:// scheme can be one byte, others need to be two */
    જો(!u->path) {
      u->path = curlx_memdup0(path, pathlen);
      જો(!u->path)
        પાછા CURLUE_OUT_OF_MEMORY;
      path = u->path;
    }
    નહીંતર જો(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->path[pathlen] = 0;

    જો(!(flags & CURLU_PATH_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      અક્ષર *dedot;
      પૂર્ણાંક err = dedotdotify(path, pathlen, &dedot);
      જો(err)
        પાછા CURLUE_OUT_OF_MEMORY;
      જો(dedot) {
        curlx_free(u->path);
        u->path = dedot;
      }
    }
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode parseurl(અચલ અક્ષર *url, CURLU *u, unsigned પૂર્ણાંક flags)
{
  અચલ અક્ષર *path;
  size_t pathlen;
  અક્ષર schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  સંરચના dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = strlen(url);
  જો(urllen > CURL_MAX_આગત_LENGTH)
    પાછા CURLUE_MALFORMED_આગત;

  curlx_dyn_init(&host, CURL_MAX_આગત_LENGTH);

  schemelen = Curl_is_absolute_url(url, schemebuf, કદ(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  જો(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &path, &pathlen);
  }
  નહીંતર {
    અચલ અક્ષર *hostp = ખાલી;
    અચલ અક્ષર *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    જો(ures)
      જાઓ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    જ્યારે(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    path = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (path - url);
    જો(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      જો(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    નહીંતર જો(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      જો(curlx_dyn_add(&host, ""))
        ures = CURLUE_OUT_OF_MEMORY;
    }
    નહીંતર
      ures = CURLUE_NO_HOST;
  }
  જો(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    અચલ અક્ષર *fragment = memchr(path, '#', pathlen);
    જો(fragment) {
      size_t fraglen = pathlen - (fragment - path);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  જો(!ures) {
    અચલ અક્ષર *query = memchr(path, '?', pathlen);
    જો(query) {
      size_t qlen = pathlen - (query - path);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  જો(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_path(u, path, pathlen, flags, is_file);
  જો(!ures) {
    u->host = curlx_dyn_ptr(&host);
    પાછા CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  પાછા ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
સ્થિર CURLUcode parseurl_and_replace(અચલ અક્ષર *url, CURLU *u,
                                      unsigned પૂર્ણાંક flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, કદ(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  જો(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  પાછા ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
સ્થિર CURLUcode redirect_url(અચલ અક્ષર *base, અચલ અક્ષર *relurl,
                              CURLU *u, unsigned પૂર્ણાંક flags)
{
  સંરચના dynbuf urlbuf;
  bool host_changed = FALSE;
  અચલ અક્ષર *useurl = relurl;
  અચલ અક્ષર *cutoff = ખાલી;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  અચલ અક્ષર *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  અચલ અક્ષર *protsep = base + strlen(scheme) + 3;
  DEBUGASSERT(base && relurl && u); /* all set here */
  જો(!base)
    પાછા CURLUE_MALFORMED_આગત; /* should never happen */

  /* handle different relative URL types */
  પસંદગી(relurl[0]) {
  સ્થિતિ '/':
    જો(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_changed = TRUE;
    }
    નહીંતર
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    તોડો;

  સ્થિતિ '#':
    /* fragment-only change */
    જો(u->fragment_present)
      cutoff = strchr(protsep, '#');
    તોડો;

  મૂળભૂત:
    /* path or query-only change */
    જો(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    નહીંતર જો(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    જો(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : strlen(protsep));
      જો(cutoff)
        cutoff++; /* truncate after last slash */
    }
    તોડો;
  }

  prelen = cutoff ? (size_t)(cutoff - base) : strlen(base);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_આગત_LENGTH);

  જો(!curlx_dyn_addn(&urlbuf, base, prelen) &&
     !urlencode_str(&urlbuf, useurl, strlen(useurl), !host_changed,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_PATH_AS_IS);
  }
  નહીંતર
    uc = CURLUE_OUT_OF_MEMORY;

  curlx_dyn_free(&urlbuf);
  પાછા uc;
}

/*
 */
CURLU *curl_url(શૂન્ય)
{
  પાછા curlx_calloc(1, કદ(સંરચના Curl_URL));
}

શૂન્ય curl_url_cleanup(CURLU *u)
{
  જો(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#define DUP(dest, src, name)                    \
  કરો {                                          \
    જો((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      જો(!(dest)->name)                         \
        જાઓ fail;                              \
    }                                           \
  } જ્યારે(0)

CURLU *curl_url_dup(અચલ CURLU *in)
{
  સંરચના Curl_URL *u;

  જો(!in)
    પાછા ખાલી;

  u = curlx_calloc(1, કદ(સંરચના Curl_URL));
  જો(u) {
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
  પાછા u;
fail:
  curl_url_cleanup(u);
  પાછા ખાલી;
}

#ifndef USE_IDN
#define host_decode(x, y) CURLUE_LACKS_IDN
#define host_encode(x, y) CURLUE_LACKS_IDN
#નહીંતર
સ્થિર CURLUcode host_decode(અચલ અક્ષર *host, અક્ષર **allochost)
{
  CURLcode result = Curl_idn_decode(host, allochost);
  જો(result)
    પાછા (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode host_encode(અચલ અક્ષર *host, અક્ષર **allochost)
{
  CURLcode result = Curl_idn_encode(host, allochost);
  જો(result)
    પાછા (result == CURLE_OUT_OF_MEMORY) ?
      CURLUE_OUT_OF_MEMORY : CURLUE_BAD_HOSTNAME;
  પાછા CURLUE_OK;
}
#endif

સ્થિર CURLUcode urlget_format(અચલ CURLU *u, CURLUPart what,
                               અચલ અક્ષર *ptr, અક્ષર **partp,
                               bool plusdecode, unsigned પૂર્ણાંક flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = strlen(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  અક્ષર *part = curlx_memdup0(ptr, partlen);
  *partp = ખાલી;
  જો(!part)
    પાછા CURLUE_OUT_OF_MEMORY;
  જો(plusdecode) {
    /* convert + to space */
    અક્ષર *plus = part;
    size_t i = 0;
    માટે(i = 0; i < partlen; ++plus, i++) {
      જો(*plus == '+')
        *plus = ' ';
    }
  }
  જો(urldecode) {
    અક્ષર *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode result = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    જો(result)
      પાછા CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  જો(urlencode) {
    સંરચના dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_આગત_LENGTH);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    જો(uc)
      પાછા uc;
    part = curlx_dyn_ptr(&enc);
  }
  નહીંતર જો(punycode) {
    જો(!Curl_is_ASCII_name(u->host)) {
      અક્ષર *punyversion = ખાલી;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      જો(uc)
        પાછા uc;
      uc = hostname_check(punyversion, strlen(punyversion));
      જો(uc) {
        curlx_free(punyversion);
        પાછા uc;
      }
      part = punyversion;
    }
  }
  નહીંતર જો(depunyfy && Curl_is_ASCII_name(u->host)) {
    અક્ષર *unpunified = ખાલી;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    જો(uc)
      પાછા uc;
    part = unpunified;
  }
  *partp = part;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode file_url(અચલ CURLU *u, અક્ષર **part,
                          અચલ અક્ષર *fragmentsep,
                          અચલ અક્ષર *querysep)
{
  અક્ષર *url = curl_maprintf("file://%s%s%s%s%s",
                            u->path, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  જો(!url)
    પાછા CURLUE_OUT_OF_MEMORY;

  *part = url;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode urlget_url(અચલ CURLU *u, અક્ષર **part, unsigned પૂર્ણાંક flags)
{
  અક્ષર *url;
  અક્ષર *allochost = ખાલી;
  અચલ અક્ષર *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_EMPTY)) ?
    "#" : "";
  અચલ અક્ષર *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_EMPTY)) ?
    "?" : "";
  અક્ષર portbuf[7];
  જો(curl_strequal("file", u->scheme))
    પાછા file_url(u, part, fragmentsep, querysep);
  નહીંતર જો(!u->host)
    પાછા CURLUE_NO_HOST;
  નહીંતર {
    અચલ અક્ષર *scheme;
    અક્ષર *options = u->options;
    અક્ષર *port = ખાલી;
    અચલ સંરચના Curl_scheme *h = ખાલી;
    અક્ષર schemebuf[MAX_SCHEME_LEN + 5];
    જો(u->scheme)
      scheme = u->scheme;
    નહીંતર જો(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    નહીંતર
      પાછા CURLUE_NO_SCHEME;

    જો(u->port_present) {
      curl_msnprintf(portbuf, કદ(portbuf), "%u", u->portnum);
      port = portbuf;
    }

    h = Curl_get_scheme(scheme);
    જો(h) {
      જો(!u->port_present && (flags & CURLU_DEFAULT_PORT)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, કદ(portbuf), "%u", h->defport);
        port = portbuf;
      }
      નહીંતર જો(u->port_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_PORT)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        port = ખાલી;
      }

      જો(!(h->flags & PROTOPT_URLOPTIONS))
        options = ખાલી;
    }

    જો(u->host[0] == '[') {
      જો(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        સંરચના dynbuf enc;
        size_t hostlen = strlen(u->host);
        curlx_dyn_init(&enc, CURL_MAX_આગત_LENGTH);
        જો(curlx_dyn_addf(&enc, "%.*s%%25%s]", (પૂર્ણાંક)hostlen - 1, u->host,
                          u->zoneid))
          પાછા CURLUE_OUT_OF_MEMORY;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    નહીંતર જો(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ખાલી, u->host, 0);
      જો(!allochost)
        પાછા CURLUE_OUT_OF_MEMORY;
    }
    નહીંતર જો(flags & CURLU_PUNYCODE) {
      જો(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        જો(!ret)
          ret = hostname_check(allochost, strlen(allochost));
        જો(ret) {
          curlx_safefree(allochost);
          પાછા ret;
        }
      }
    }
    નહીંતર જો(flags & CURLU_PUNY2IDN) {
      જો(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        જો(ret)
          પાછા ret;
      }
    }

    જો(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, કદ(schemebuf), "%s://", scheme);
    નહીંતર
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
  જો(!url)
    પાછા CURLUE_OUT_OF_MEMORY;
  *part = url;
  પાછા CURLUE_OK;
}

CURLUcode curl_url_get(અચલ CURLU *u, CURLUPart what,
                       અક્ષર **part, unsigned પૂર્ણાંક flags)
{
  અચલ અક્ષર *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  અક્ષર portbuf[7];
  bool plusdecode = FALSE;
  જો(!u)
    પાછા CURLUE_BAD_HANDLE;
  જો(!part)
    પાછા CURLUE_BAD_PARTPOINTER;
  *part = ખાલી;

  પસંદગી(what) {
  સ્થિતિ CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    જો((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      પાછા CURLUE_NO_SCHEME;
    તોડો;
  સ્થિતિ CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    તોડો;
  સ્થિતિ CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    તોડો;
  સ્થિતિ CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    તોડો;
  સ્થિતિ CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    તોડો;
  સ્થિતિ CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    તોડો;
  સ્થિતિ CURLUPART_PORT:
    ptr = ખાલી;
    ifmissing = CURLUE_NO_PORT;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    જો(u->port_present) {
      અચલ સંરચના Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ખાલી;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      જો(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_PORT)) {
        ptr = ખાલી;
      }
      નહીંતર {
        curl_msnprintf(portbuf, કદ(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    નહીંતર જો((flags & CURLU_DEFAULT_PORT) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      અચલ સંરચના Curl_scheme *h = Curl_get_scheme(u->scheme);
      જો(h) {
        curl_msnprintf(portbuf, કદ(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    તોડો;
  સ્થિતિ CURLUPART_PATH:
    ptr = u->path;
    જો(!ptr)
      ptr = "/";
    તોડો;
  સ્થિતિ CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    જો(ptr && !ptr[0] && !(flags & CURLU_GET_EMPTY))
      /* there was a blank query and the user does not ask for it */
      ptr = ખાલી;
    તોડો;
  સ્થિતિ CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    જો(!ptr && u->fragment_present && flags & CURLU_GET_EMPTY)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    તોડો;
  સ્થિતિ CURLUPART_URL:
    પાછા urlget_url(u, part, flags);
  મૂળભૂત:
    ptr = ખાલી;
    તોડો;
  }
  જો(ptr)
    પાછા urlget_format(u, what, ptr, part, plusdecode, flags);

  પાછા ifmissing;
}

સ્થિર CURLUcode set_url_scheme(CURLU *u, અચલ અક્ષર *scheme,
                                unsigned પૂર્ણાંક flags)
{
  size_t plen = strlen(scheme);
  અચલ સંરચના Curl_scheme *h = ખાલી;
  જો((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    પાછા CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  જો(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->run))
    પાછા CURLUE_UNSUPPORTED_SCHEME;
  જો(!h) {
    અચલ અક્ષર *s = scheme;
    જો(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      જ્યારે(--plen) {
        જો(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        નહીંતર
          પાછા CURLUE_BAD_SCHEME;
      }
    }
    નહીંતર
      પાછા CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode set_url_port(CURLU *u, અચલ અક્ષર *provided_port)
{
  curl_off_t port;
  જો(!ISDIGIT(provided_port[0]))
    /* not a number */
    પાછા CURLUE_BAD_PORT_સંખ્યા;
  જો(curlx_str_સંખ્યા(&provided_port, &port, 0xffff) || *provided_port)
    /* weirdly provided number, not good! */
    પાછા CURLUE_BAD_PORT_સંખ્યા;
  u->portnum = (uint16_t)port;
  u->port_present = TRUE;
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode set_url(CURLU *u, અચલ અક્ષર *url, size_t part_size,
                         unsigned પૂર્ણાંક flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  અક્ષર *oldurl = ખાલી;

  જો(!part_size) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    જો(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      પાછા CURLUE_OK;
    }
    જો(uc == CURLUE_OUT_OF_MEMORY)
      પાછા uc;
    પાછા CURLUE_MALFORMED_આગત;
  }

  /* if the new URL is absolute replace the existing with the new. */
  જો(Curl_is_absolute_url(url, ખાલી, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    પાછા parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_EMPTY);
  જો(uc == CURLUE_OUT_OF_MEMORY)
    પાછા uc;
  નહીંતર જો(uc)
    પાછા parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  પાછા uc;
}

સ્થિર CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  પસંદગી(what) {
  સ્થિતિ CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, કદ(સંરચના Curl_URL));
    તોડો;
  સ્થિતિ CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    તોડો;
  સ્થિતિ CURLUPART_USER:
    curlx_safefree(u->user);
    તોડો;
  સ્થિતિ CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    તોડો;
  સ્થિતિ CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    તોડો;
  સ્થિતિ CURLUPART_HOST:
    curlx_safefree(u->host);
    તોડો;
  સ્થિતિ CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    તોડો;
  સ્થિતિ CURLUPART_PORT:
    u->portnum = 0;
    u->port_present = FALSE;
    તોડો;
  સ્થિતિ CURLUPART_PATH:
    curlx_safefree(u->path);
    તોડો;
  સ્થિતિ CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    તોડો;
  સ્થિતિ CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    તોડો;
  મૂળભૂત:
    પાછા CURLUE_UNKNOWN_PART;
  }
  પાછા CURLUE_OK;
}

સ્થિર bool allowed_in_path(unsigned અક્ષર x)
{
  પસંદગી(x) {
  સ્થિતિ '!':
  સ્થિતિ '$':
  સ્થિતિ '&':
  સ્થિતિ '\'':
  સ્થિતિ '(':
  સ્થિતિ ')':
  સ્થિતિ '{':
  સ્થિતિ '}':
  સ્થિતિ '[':
  સ્થિતિ ']':
  સ્થિતિ '*':
  સ્થિતિ '+':
  સ્થિતિ ',':
  સ્થિતિ ';':
  સ્થિતિ '=':
  સ્થિતિ ':':
  સ્થિતિ '@':
  સ્થિતિ '/':
    પાછા TRUE;
  }
  પાછા FALSE;
}

સ્થિર CURLUcode url_encode_part(સંરચના dynbuf *encp,
                                 અચલ અક્ષર *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  અચલ unsigned અક્ષર *i;

  માટે(i = (અચલ unsigned અક્ષર *)part; *i; i++) {
    CURLcode result;
    જો((*i == ' ') && plusencode)
      result = curlx_dyn_addn(encp, "+", 1);
    નહીંતર જો(ISUNRESERVED(*i) ||
            (pathmode && allowed_in_path(*i)) ||
            ((*i == '=') && equalsencode)) {
      જો((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      result = curlx_dyn_addn(encp, i, 1);
    }
    નહીંતર {
      unsigned અક્ષર out[3] = { '%' };
      Curl_hexbyte(&out[1], *i);
      result = curlx_dyn_addn(encp, out, 3);
    }
    જો(result)
      પાછા cc2cu(result);
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode url_uppercasehex_part(સંરચના dynbuf *encp,
                                       અચલ અક્ષર *part)
{
  અક્ષર *p;
  CURLcode result = curlx_dyn_add(encp, part);
  જો(result)
    પાછા cc2cu(result);
  p = curlx_dyn_ptr(encp);
  જ્યારે(*p) {
    /* make sure percent encoded are upper case */
    જો((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    નહીંતર
      p++;
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode url_append_query(CURLU *u, સંરચના dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? strlen(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  જો(querylen) {
    સંરચના dynbuf qbuf;
    CURLcode result;
    અચલ અક્ષર *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_આગત_LENGTH);

    /* add original query */
    result = curlx_dyn_addn(&qbuf, u->query, querylen);
    જો(!result && addamperand)
      /* add ampersand */
      result = curlx_dyn_addn(&qbuf, "&", 1);
    જો(!result)
      /* add new query part */
      result = curlx_dyn_add(&qbuf, newp);
    જો(result)
      જાઓ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    પાછા CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    પાછા cc2cu(result);
  }
  નહીંતર {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  પાછા CURLUE_OK;
}

સ્થિર CURLUcode url_sethost(CURLU *u, સંરચના dynbuf *encp,
                             bool urlencode,
                             unsigned પૂર્ણાંક flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  અક્ષર *newp = curlx_dyn_ptr(encp);
  જો(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  નહીંતર જો(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    અક્ષર *decoded = ખાલી;
    CURLcode result = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    જો(result || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    નહીંતર {
      સંરચના dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_આગત_LENGTH);
      જો(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      નહીંતર જો(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        જો(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  નહીંતર જો(hostname_check6(u, newp, n))
    bad = TRUE;
  નહીંતર જો(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  જો(bad) {
    curlx_dyn_free(encp);
    પાછા CURLUE_BAD_HOSTNAME;
  }
  પાછા CURLUE_OK;
}

CURLUcode curl_url_set(CURLU *u, CURLUPart what,
                       અચલ અક્ષર *part, unsigned પૂર્ણાંક flags)
{
  અક્ષર **storep = ખાલી;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  જો(!u)
    પાછા CURLUE_BAD_HANDLE;
  જો(!part)
    /* setting a part to NULL clears it */
    પાછા urlset_clear(u, what);

  nalloc = strlen(part);
  જો(nalloc > CURL_MAX_આગત_LENGTH)
    /* excessive input length */
    પાછા CURLUE_MALFORMED_આગત;

  પસંદગી(what) {
  સ્થિતિ CURLUPART_SCHEME: {
    CURLUcode status = set_url_scheme(u, part, flags);
    જો(status)
      પાછા status;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    તોડો;
  }
  સ્થિતિ CURLUPART_USER:
    storep = &u->user;
    તોડો;
  સ્થિતિ CURLUPART_PASSWORD:
    storep = &u->password;
    તોડો;
  સ્થિતિ CURLUPART_OPTIONS:
    storep = &u->options;
    તોડો;
  સ્થિતિ CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    તોડો;
  સ્થિતિ CURLUPART_ZONEID:
    storep = &u->zoneid;
    તોડો;
  સ્થિતિ CURLUPART_PORT:
    પાછા set_url_port(u, part);
  સ્થિતિ CURLUPART_PATH:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->path;
    તોડો;
  સ્થિતિ CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    તોડો;
  સ્થિતિ CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    તોડો;
  સ્થિતિ CURLUPART_URL:
    પાછા set_url(u, part, nalloc, flags);
  મૂળભૂત:
    પાછા CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    અચલ અક્ષર *newp = ખાલી;
    સંરચના dynbuf enc;
    CURLUcode status;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    જો(leadingslash && (part[0] != '/')) {
      CURLcode result = curlx_dyn_addn(&enc, "/", 1);
      જો(result)
        પાછા cc2cu(result);
    }
    જો(urlencode)
      status = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    નહીંતર
      status = url_uppercasehex_part(&enc, part);
    જો(!status) {
      newp = curlx_dyn_ptr(&enc);

      જો(appendquery && newp)
        પાછા url_append_query(u, &enc);
      નહીંતર જો(what == CURLUPART_HOST)
        status = url_sethost(u, &enc, urlencode, flags);
    }
    જો(status)
      પાછા status;

    જો(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (અક્ષર *)CURL_UNCONST(newp);
  }
  પાછા CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *base, CURLU *href)
{
  અચલ સંરચના Curl_scheme *s = ખાલી;

  /* base must be an absolute URL */
  જો(!base->scheme || !base->host)
    પાછા FALSE;
  જો(href->scheme && !curl_strequal(base->scheme, href->scheme))
    પાછા FALSE;
  જો(href->host) {
    જો(!curl_strequal(base->host, href->host))
      પાછા FALSE;

    જો(base->port_present != href->port_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(base->scheme);
      જો(!s) /* Cannot match default port for unknown scheme */
        પાછા FALSE;
      /* to match, the present one must be the default port */
      જો((base->port_present && (base->portnum != s->defport)) ||
         (href->port_present && (href->portnum != s->defport)))
        પાછા FALSE;
    }
    નહીંતર જો(base->portnum != href->portnum) /* both present or missing */
      પાછા FALSE;

    જો(!curl_strequal(base->zoneid ? base->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      પાછા FALSE;
  }
  નહીંતર જો(href->port_present) /* no host in href, then there must be no port */
    પાછા FALSE;
  પાછા TRUE;
}

CURLUcode Curl_url_get_port(CURLU *u, uint16_t *pport)
{
  જો(u->port_present) {
    *pport = u->portnum;
    પાછા CURLUE_OK;
  }
  નહીંતર જો(u->scheme) {
    અચલ સંરચના Curl_scheme *s = Curl_get_scheme(u->scheme);
    જો(s && s->defport) {
      *pport = s->defport;
      પાછા CURLUE_OK;
    }
  }
  *pport = 0;
  પાછા CURLUE_NO_PORT;
}
