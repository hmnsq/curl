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
#شامل "curl_setup.h"

#شامل "urldata.h"
#شامل "urlapi-int.h"
#شامل "strcase.h"
#شامل "url.h"
#شامل "escape.h"
#شامل "curlx/inet_pton.h"
#شامل "curlx/inet_ntop.h"
#شامل "curlx/strdup.h"
#شامل "idn.h"
#شامل "curlx/strparse.h"
#شامل "curl_memrchr.h"

#اگر_تعریف_شدہ _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#تعریف_کرو STARTS_WITH_DRIVE_سابقہ(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#اگر_ختم

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#تعریف_کرو STARTS_WITH_URL_DRIVE_سابقہ(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#تعریف_کرو MAX_SCHEME_LEN 40
#تعریف_کرو MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#اگر !defined(USE_IPV6) && !defined(AF_INET6)
#تعریف_کرو AF_INET6 (AF_INET + 1)
#اگر_ختم

#تعریف_کرو DEFAULT_SCHEME "https"

ساکن خالی free_urlhandle(ساخت Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->راستہ);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
ساکن ثابت حرف *ڈھونڈو_host_sep(ثابت حرف *url)
{
  /* Find the start of the hostname */
  ثابت حرف *sep = لڑی_تلاش(url, "//");
  اگر(!sep)
    sep = url;
  ورنہ
    sep += 2;

  /* Find first / or ? */
  جب(*sep && *sep != '/' && *sep != '?')
    sep++;

  واپس sep;
}

/* convert CURLcode to CURLUcode */
#تعریف_کرو cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_برآمد_کا_حافظہ)

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
UNITTEST CURLUcode urlencode_str(ساخت dynbuf *o, ثابت حرف *url,
                                 size_t len, bool relative,
                                 غیرعلامتی صحیح query);
UNITTEST CURLUcode urlencode_str(ساخت dynbuf *o, ثابت حرف *url,
                                 size_t len, bool relative,
                                 غیرعلامتی صحیح query)
{
  /* we must add this with whitespace-replacing */
  ثابت غیرعلامتی حرف *iptr;
  ثابت غیرعلامتی حرف *host_sep = (ثابت غیرعلامتی حرف *)url;
  CURLcode نتیجہ = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  اگر(!relative) {
    size_t n;
    host_sep = (ثابت غیرعلامتی حرف *)ڈھونڈو_host_sep(url);

    /* output the first piece as-is */
    n = (ثابت حرف *)host_sep - url;
    نتیجہ = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  برائے(iptr = host_sep; len && !نتیجہ;) {
    اگر(*iptr == ' ') {
      اگر(query != QUERY_YES)
        نتیجہ = curlx_dyn_addn(o, "%20", 3);
      ورنہ
        نتیجہ = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    ورنہ اگر((*iptr < ' ') || (*iptr >= 0x7f)) {
      غیرعلامتی حرف برآمد[3] = { '%' };
      Curl_hexbyte(&برآمد[1], *iptr);
      نتیجہ = curlx_dyn_addn(o, برآمد, 3);
      iptr++;
      len--;
    }
    ورنہ اگر(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      غیرعلامتی حرف hex = (غیرعلامتی حرف)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      غیرعلامتی حرف برآمد[3] = { '%' };
      Curl_hexbyte(&برآمد[1], hex);
      نتیجہ = curlx_dyn_addn(o, برآمد, 3);
      iptr += 3;
      len -= 3;
    }
    ورنہ {
      ثابت غیرعلامتی حرف *آغاز = iptr;
      جب(len) {
        اگر(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          توڑو;
        اگر(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          توڑو;
        اگر(*iptr == '?') {
          اگر(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            توڑو;
          }
        }
        iptr++;
        len--;
      }
      نتیجہ = curlx_dyn_addn(o, (ثابت حرف *)آغاز, (size_t)(iptr - آغاز));
    }
  }

  اگر(نتیجہ)
    واپس cc2cu(نتیجہ);
  واپس CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ثابت حرف *url, حرف *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (خالی)buflen; /* only used in debug-builds */
  اگر(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#اگر_تعریف_شدہ _WIN32
  اگر(guess_scheme && STARTS_WITH_DRIVE_سابقہ(url))
    واپس 0;
#اگر_ختم
  اگر(ISALPHA(url[0])) {
    اگر(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    برائے(i = 1; i < MAX_SCHEME_LEN; ++i) {
      حرف s = url[i];
      اگر(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        اگر(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      ورنہ {
        توڑو;
      }
    }
  }
  اگر(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    اگر(buf)
      buf[i] = 0;
    واپس len;
  }
  اگر(buf)
    buf[0] = 0;
  واپس 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ساکن bool badoctets(ثابت حرف *داخلہ, size_t n, صحیح flags)
{
  ثابت uint8_t *p = (ثابت غیرعلامتی حرف *)داخلہ;
  ثابت uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  جب(n--) {
    اگر(*p <= control || *p == 127)
      واپس TRUE;
    p++;
  }
  واپس FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ساخت Curl_URL *u,
                                        ثابت حرف *login,
                                        size_t len,
                                        غیرعلامتی صحیح flags,
                                        size_t *hostname_انحراف);
UNITTEST CURLUcode parse_hostname_login(ساخت Curl_URL *u,
                                        ثابت حرف *login,
                                        size_t len,
                                        غیرعلامتی صحیح flags,
                                        size_t *hostname_انحراف)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode نتیجہ;
  حرف *userp = تہی;
  حرف *passwdp = تہی;
  حرف *optionsp = تہی;
  ثابت ساخت Curl_scheme *h = تہی;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ثابت حرف *ptr;

  DEBUGASSERT(login);

  *hostname_انحراف = 0;
  ptr = memchr(login, '@', len);
  اگر(!ptr)
    جاؤ برآمد;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  اگر(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  نتیجہ = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : تہی);
  اگر(نتیجہ) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_برآمد_کا_حافظہ;
    جاؤ برآمد;
  }

  اگر(userp) {
    اگر(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      جاؤ برآمد;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  اگر(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  اگر(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  اگر(userp && badoctets(userp, لڑی_لمبائی(userp), flags))
    ures = CURLUE_BAD_USER;
  ورنہ اگر(passwdp && badoctets(passwdp, لڑی_لمبائی(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  ورنہ اگر(optionsp && badoctets(optionsp, لڑی_لمبائی(optionsp), flags))
    ures = CURLUE_MALFORMED_داخلہ;

  userp = passwdp = optionsp = تہی;

  اگر(!ures) {
    /* the hostname starts at this offset */
    *hostname_انحراف = ptr - login;
    واپس CURLUE_OK;
  }

برآمد:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  واپس ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_پورٹ(ساخت Curl_URL *u, ساخت dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_پورٹ(ساخت Curl_URL *u, ساخت dynbuf *host,
                              bool has_scheme)
{
  ثابت حرف *portptr;
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->پورٹ_present = FALSE;
  اگر(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    اگر(!portptr)
      واپس CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    اگر(*portptr) {
      اگر(*portptr != ':')
        واپس CURLUE_BAD_پورٹ_نمبر;
    }
    ورنہ
      portptr = تہی;
  }
  ورنہ
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  اگر(portptr) {
    curl_off_t پورٹ;
    size_t keep = portptr - hostname;
    صحیح rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    اگر(!*portptr)
      واپس has_scheme ? CURLUE_OK : CURLUE_BAD_پورٹ_نمبر;
    اگر(*portptr == '\\')
      واپس CURLUE_BACKSLASH;
    rc = curlx_str_نمبر(&portptr, &پورٹ, 0xffff);
    اگر(rc)
      واپس CURLUE_BAD_پورٹ_نمبر;
    ورنہ اگر(*portptr == '\\')
      واپس CURLUE_BACKSLASH;
    ورنہ اگر(*portptr)
      واپس CURLUE_BAD_پورٹ_نمبر;

    u->portnum = (uint16_t)پورٹ;
    u->پورٹ_present = TRUE;
  }

  واپس CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ساکن ثابت bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(ساخت Curl_URL *u, حرف *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ساخت Curl_URL *u, حرف *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  اگر(hlen < 4) /* '[::]' is the shortest possible valid string */
    واپس CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  اگر(hlen != len) {
    hlen = len;
    اگر(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      حرف zoneid[MAX_ZONEID_LEN];
      صحیح i = 0;
      حرف *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      اگر(!لڑی_موازنہ(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      جب(!invalid_host_char[(غیرعلامتی حرف)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      اگر(!i || (']' != *h))
        واپس CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      اگر(!u->zoneid)
        واپس CURLUE_برآمد_کا_حافظہ;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    ورنہ
      واپس CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    حرف dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    اگر(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      واپس CURLUE_BAD_IPV6;
    اگر(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = لڑی_لمبائی(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  واپس CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ساکن CURLUcode hostname_check(حرف *hostname, size_t hlen)
{
  size_t i;
  برائے(i = 0; i < hlen; i++) {
    اگر(invalid_host_char[(غیرعلامتی حرف)hostname[i]])
      واپس CURLUE_BAD_HOSTNAME;
  }
  اگر((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    واپس CURLUE_BAD_HOSTNAME;
  ورنہ اگر((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    واپس CURLUE_BAD_HOSTNAME;
  واپس CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ساکن CURLUcode hostname_check6(ساخت Curl_URL *u, حرف *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  اگر(!hlen)
    واپس CURLUE_NO_HOST;
  ورنہ اگر(hostname[0] == '[')
    واپس ipv6_parse(u, hostname, hlen);

  واپس hostname_check(hostname, hlen);
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
UNITTEST صحیح ipv4_normalize(ساخت dynbuf *host);
UNITTEST صحیح ipv4_normalize(ساخت dynbuf *host)
{
  bool مکمل = FALSE;
  صحیح n = 0;
  ثابت حرف *c = curlx_dyn_ptr(host);
  غیرعلامتی صحیح parts[4] = { 0, 0, 0, 0 };
  CURLcode نتیجہ = CURLE_OK;

  اگر(!ISDIGIT(*c))
    واپس HOST_NAME;

  جب(!مکمل) {
    صحیح rc;
    curl_off_t l;
    اگر(*c == '0') {
      اگر((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        اگر(rc)
          واپس HOST_NAME;
      }
      ورنہ
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    ورنہ
      rc = curlx_str_نمبر(&c, &l, UINT_MAX);

    اگر(rc) {
      اگر(!n || (rc != STRE_NO_عدد) || *c)
        واپس HOST_NAME;
      n--;
    }
    ورنہ
      parts[n] = (غیرعلامتی صحیح)l;

    انتخاب(*c) {
    حالت '.':
      اگر(n == 3) {
        اگر(c[1])
          /* something follows this dot */
          واپس HOST_NAME;
        مکمل = TRUE;
      }
      ورنہ {
        n++;
        c++;
      }
      توڑو;

    حالت '\0':
      مکمل = TRUE;
      توڑو;

    طےشدہ:
      واپس HOST_NAME;
    }
  }

  انتخاب(n) {
  حالت 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    نتیجہ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    توڑو;
  حالت 1: /* a.b -- 8.24 bits */
    اگر((parts[0] > 0xff) || (parts[1] > 0xffffff))
      واپس HOST_NAME;
    curlx_dyn_reset(host);
    نتیجہ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    توڑو;
  حالت 2: /* a.b.c -- 8.8.16 bits */
    اگر((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      واپس HOST_NAME;
    curlx_dyn_reset(host);
    نتیجہ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    توڑو;
  حالت 3: /* a.b.c.d -- 8.8.8.8 bits */
    اگر((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      واپس HOST_NAME;
    curlx_dyn_reset(host);
    نتیجہ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    توڑو;
  }
  اگر(نتیجہ)
    واپس HOST_ERROR;
  واپس HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ساکن CURLUcode urldecode_host(ساخت dynbuf *host)
{
  ثابت حرف *فی;
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  فی = memchr(hostname, '%', curlx_dyn_len(host));
  اگر(!فی)
    /* nothing to decode */
    واپس CURLUE_OK;
  ورنہ {
    /* encoded */
    size_t dlen;
    حرف *decoded;
    CURLcode نتیجہ = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    اگر(نتیجہ)
      واپس CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    نتیجہ = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    اگر(نتیجہ)
      واپس cc2cu(نتیجہ);
  }

  واپس CURLUE_OK;
}

ساکن CURLUcode parse_authority(ساخت Curl_URL *u,
                                 ثابت حرف *auth, size_t authlen,
                                 غیرعلامتی صحیح flags,
                                 ساخت dynbuf *host,
                                 bool has_scheme)
{
  size_t انحراف;
  CURLUcode uc;
  CURLcode نتیجہ;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &انحراف);
  اگر(uc)
    واپس uc;

  نتیجہ = curlx_dyn_addn(host, auth + انحراف, authlen - انحراف);
  اگر(نتیجہ) {
    uc = cc2cu(نتیجہ);
    واپس uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_پورٹ(u, host, has_scheme);

  اگر(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  اگر(!uc)
    uc = urldecode_host(host);
  اگر(uc)
    ;
  ورنہ اگر(auth[انحراف] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  ورنہ {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    صحیح type = ipv4_normalize(host);

    اگر(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    ورنہ اگر(type == HOST_ERROR)
      uc = CURLUE_برآمد_کا_حافظہ;
  }

  واپس uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_سیٹ_authority(CURLU *u, ثابت حرف *authority)
{
  CURLUcode ures;
  ساخت dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_داخلہ_لمبائی);

  ures = parse_authority(u, authority, لڑی_لمبائی(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  اگر(ures)
    curlx_dyn_free(&host);
  ورنہ {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  واپس ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ساکن bool is_dot(ثابت حرف **str, size_t *clen)
{
  ثابت حرف *p = *str;
  اگر(*p == '.') {
    (*str)++;
    (*clen)--;
    واپس TRUE;
  }
  ورنہ اگر((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    واپس TRUE;
  }
  واپس FALSE;
}

#تعریف_کرو ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ساکن bool needs_dedotdot(ثابت حرف *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  اگر(pn < 2)
    واپس FALSE;
  اگر(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    واپس FALSE;
  جب(pn) {
    اگر(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      اگر(!pn || ISSLASH(*p))
        واپس TRUE;
      /* "../" or ".." before end of string */
      ورنہ اگر(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        واپس TRUE;
    }
    ورنہ {
      p++;
      pn--;
    }
  }
  واپس FALSE;
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
UNITTEST صحیح dedotdotify(ثابت حرف *داخلہ, size_t clen, حرف **outp);
UNITTEST صحیح dedotdotify(ثابت حرف *داخلہ, size_t clen, حرف **outp)
{
  ساخت dynbuf برآمد;
  CURLcode نتیجہ = CURLE_OK;

  /* variables for leading dot checks */
  ثابت حرف *dinput = داخلہ;
  size_t dlen = clen;

  *outp = تہی;
  اگر(!needs_dedotdot(داخلہ, clen))
    واپس 0;

  curlx_dyn_init(&برآمد, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  اگر(is_dot(&dinput, &dlen)) {
    اگر(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      داخلہ = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    ورنہ اگر(is_dot(&dinput, &dlen)) {
      اگر(!dlen)
        /* .. [end] */
        جاؤ انتہا;
      ورنہ اگر(ISSLASH(*dinput)) {
        /* ../ */
        داخلہ = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  جب(clen && !نتیجہ) { /* until end of path content */
    اگر(ISSLASH(*داخلہ)) {
      ثابت حرف *p = &داخلہ[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      اگر(is_dot(&p, &blen)) {
        اگر(!blen) { /* /. */
          نتیجہ = curlx_dyn_addn(&برآمد, "/", 1);
          توڑو;
        }
        ورنہ اگر(ISSLASH(*p)) { /* /./ */
          داخلہ = p;
          clen = blen;
          جاری;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        ورنہ اگر(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&برآمد);
          اگر(len) {
            ثابت حرف *ptr = curlx_dyn_ptr(&برآمد);
            ثابت حرف *آخری = memrchr(ptr, '/', len);
            اگر(آخری)
              /* trim the output at the slash */
              curlx_dyn_setlen(&برآمد, آخری - ptr);
          }

          اگر(blen) { /* /../ */
            داخلہ = p;
            clen = blen;
            جاری;
          }
          نتیجہ = curlx_dyn_addn(&برآمد, "/", 1);
          توڑو;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    نتیجہ = curlx_dyn_addn(&برآمد, داخلہ, 1);
    داخلہ++;
    clen--;
  }
انتہا:
  اگر(!نتیجہ) {
    اگر(curlx_dyn_len(&برآمد))
      *outp = curlx_dyn_ptr(&برآمد);
    ورنہ {
      *outp = curlx_strdup("");
      اگر(!*outp)
        واپس 1;
    }
  }
  واپس نتیجہ ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ثابت حرف *url, size_t urllen, CURLU *u,
                              ثابت حرف **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ثابت حرف *url, size_t urllen, CURLU *u,
                              ثابت حرف **pathp, size_t *pathlenp)
{
  ثابت حرف *راستہ;
  size_t pathlen;

  *pathp = تہی;
  *pathlenp = 0;
  اگر(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    واپس CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  راستہ = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  اگر(راستہ[0] != '/')
    واپس CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  اگر(راستہ[1] == '/') {
    /* swallow the two slashes */
    ثابت حرف *ptr = &راستہ[2];

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
    اگر(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_سابقہ(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      اگر(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      ورنہ
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        واپس CURLUE_BAD_FILE_URL;
    }

    راستہ = ptr;
    pathlen = urllen - (ptr - url);
  }

#اگر !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  اگر(('/' == راستہ[0] && STARTS_WITH_URL_DRIVE_سابقہ(&راستہ[1])) ||
     STARTS_WITH_URL_DRIVE_سابقہ(راستہ)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    واپس CURLUE_BAD_FILE_URL;
  }
#ورنہ
  /* If the path starts with a slash and a drive letter, ditch the slash */
  اگر('/' == راستہ[0] && STARTS_WITH_URL_DRIVE_سابقہ(&راستہ[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    راستہ++;
    pathlen--;
  }
#اگر_ختم
  u->scheme = curlx_strdup("file");
  اگر(!u->scheme)
    واپس CURLUE_برآمد_کا_حافظہ;

  *pathp = راستہ;
  *pathlenp = pathlen;
  واپس CURLUE_OK;
}

ساکن CURLUcode parse_scheme(ثابت حرف *url, CURLU *u, حرف *schemebuf,
                              size_t schemelen, غیرعلامتی صحیح flags,
                              ثابت حرف **hostpp)
{
  /* clear path */
  ثابت حرف *schemep = تہی;

  اگر(schemelen) {
    صحیح عدد_slashes = 0;
    ثابت حرف *p = &url[schemelen + 1];
    اگر(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      واپس CURLUE_UNSUPPORTED_SCHEME;

    اگر(!ISSLASH(*p))
      /* less than one */
      واپس CURLUE_BAD_SLASHES;
    اگر((flags & CURLU_NO_AUTHORITY)) {
      جب(ISSLASH(*p) && (عدد_slashes < 2)) {
        p++;
        عدد_slashes++;
      }
    }
    ورنہ {
      جب(ISSLASH(*p) && (عدد_slashes < 4)) {
        p++;
        عدد_slashes++;
      }
      اگر(عدد_slashes > 3)
        واپس CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  ورنہ {
    /* no scheme! */

    اگر(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      واپس CURLUE_BAD_SCHEME;

    اگر(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  اگر(schemep) {
    u->scheme = curlx_strdup(schemep);
    اگر(!u->scheme)
      واپس CURLUE_برآمد_کا_حافظہ;
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode guess_scheme(CURLU *u, ساخت dynbuf *host)
{
  ثابت حرف *hostname = curlx_dyn_ptr(host);
  ثابت حرف *schemep = تہی;
  /* legacy curl-style guess based on hostname */
  اگر(checkprefix("ftp.", hostname))
    schemep = "ftp";
  ورنہ اگر(checkprefix("dict.", hostname))
    schemep = "dict";
  ورنہ اگر(checkprefix("ldap.", hostname))
    schemep = "ldap";
  ورنہ اگر(checkprefix("imap.", hostname))
    schemep = "imap";
  ورنہ اگر(checkprefix("smtp.", hostname))
    schemep = "smtp";
  ورنہ اگر(checkprefix("pop3.", hostname))
    schemep = "pop3";
  ورنہ
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  اگر(!u->scheme)
    واپس CURLUE_برآمد_کا_حافظہ;

  u->guessed_scheme = TRUE;
  واپس CURLUE_OK;
}

ساکن CURLUcode handle_fragment(CURLU *u, ثابت حرف *fragment,
                                 size_t fraglen, غیرعلامتی صحیح flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  اگر(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    اگر(flags & CURLU_URLENCODE) {
      ساخت dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_داخلہ_لمبائی);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      اگر(ures)
        واپس ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    ورنہ {
      اگر(badoctets(fragment, fraglen, flags))
        واپس CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      اگر(!u->fragment)
        واپس CURLUE_برآمد_کا_حافظہ;
    }
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode handle_query(CURLU *u, ثابت حرف *query,
                              size_t qlen, غیرعلامتی صحیح flags)
{
  u->query_present = TRUE;
  اگر(qlen > 1) {
    اگر(flags & CURLU_URLENCODE) {
      ساخت dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_داخلہ_لمبائی);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      اگر(ures)
        واپس ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    ورنہ {
      اگر(badoctets(query, qlen, flags))
        واپس CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      اگر(!u->query)
        واپس CURLUE_برآمد_کا_حافظہ;
    }
  }
  ورنہ {
    /* single byte query */
    u->query = curlx_strdup("");
    اگر(!u->query)
      واپس CURLUE_برآمد_کا_حافظہ;
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode handle_راستہ(CURLU *u, ثابت حرف *راستہ,
                             size_t pathlen, غیرعلامتی صحیح flags,
                             bool is_file)
{
  CURLUcode ures;
  اگر(pathlen && (flags & CURLU_URLENCODE)) {
    ساخت dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_داخلہ_لمبائی);
    ures = urlencode_str(&enc, راستہ, pathlen, TRUE, QUERY_NO);
    اگر(ures)
      واپس ures;
    pathlen = curlx_dyn_len(&enc);
    راستہ = u->راستہ = curlx_dyn_ptr(&enc);
  }

  اگر(pathlen >= (size_t)(1 + !is_file)) {
    اگر(badoctets(راستہ, pathlen, flags))
      واپس CURLUE_BAD_راستہ;

    /* paths for file:// scheme can be one byte, others need to be two */
    اگر(!u->راستہ) {
      u->راستہ = curlx_memdup0(راستہ, pathlen);
      اگر(!u->راستہ)
        واپس CURLUE_برآمد_کا_حافظہ;
      راستہ = u->راستہ;
    }
    ورنہ اگر(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->راستہ[pathlen] = 0;

    اگر(!(flags & CURLU_راستہ_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      حرف *dedot;
      صحیح err = dedotdotify(راستہ, pathlen, &dedot);
      اگر(err)
        واپس CURLUE_برآمد_کا_حافظہ;
      اگر(dedot) {
        curlx_free(u->راستہ);
        u->راستہ = dedot;
      }
    }
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode parseurl(ثابت حرف *url, CURLU *u, غیرعلامتی صحیح flags)
{
  ثابت حرف *راستہ;
  size_t pathlen;
  حرف schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ساخت dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = لڑی_لمبائی(url);
  اگر(urllen > CURL_MAX_داخلہ_لمبائی)
    واپس CURLUE_MALFORMED_داخلہ;

  curlx_dyn_init(&host, CURL_MAX_داخلہ_لمبائی);

  schemelen = Curl_is_absolute_url(url, schemebuf, حجم(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  اگر(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &راستہ, &pathlen);
  }
  ورنہ {
    ثابت حرف *hostp = تہی;
    ثابت حرف *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    اگر(ures)
      جاؤ fail;

    /* find the end of the hostname + port number */
    p = hostp;
    جب(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    راستہ = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (راستہ - url);
    اگر(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      اگر(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    ورنہ اگر(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      اگر(curlx_dyn_جوڑو(&host, ""))
        ures = CURLUE_برآمد_کا_حافظہ;
    }
    ورنہ
      ures = CURLUE_NO_HOST;
  }
  اگر(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ثابت حرف *fragment = memchr(راستہ, '#', pathlen);
    اگر(fragment) {
      size_t fraglen = pathlen - (fragment - راستہ);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  اگر(!ures) {
    ثابت حرف *query = memchr(راستہ, '?', pathlen);
    اگر(query) {
      size_t qlen = pathlen - (query - راستہ);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  اگر(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_راستہ(u, راستہ, pathlen, flags, is_file);
  اگر(!ures) {
    u->host = curlx_dyn_ptr(&host);
    واپس CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  واپس ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ساکن CURLUcode parseurl_and_replace(ثابت حرف *url, CURLU *u,
                                      غیرعلامتی صحیح flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, حجم(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  اگر(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  واپس ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ساکن CURLUcode redirect_url(ثابت حرف *بنیاد, ثابت حرف *relurl,
                              CURLU *u, غیرعلامتی صحیح flags)
{
  ساخت dynbuf urlbuf;
  bool host_بدلا = FALSE;
  ثابت حرف *useurl = relurl;
  ثابت حرف *cutoff = تہی;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ثابت حرف *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ثابت حرف *protsep = بنیاد + لڑی_لمبائی(scheme) + 3;
  DEBUGASSERT(بنیاد && relurl && u); /* all set here */
  اگر(!بنیاد)
    واپس CURLUE_MALFORMED_داخلہ; /* should never happen */

  /* handle different relative URL types */
  انتخاب(relurl[0]) {
  حالت '/':
    اگر(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_بدلا = TRUE;
    }
    ورنہ
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    توڑو;

  حالت '#':
    /* fragment-only change */
    اگر(u->fragment_present)
      cutoff = strchr(protsep, '#');
    توڑو;

  طےشدہ:
    /* path or query-only change */
    اگر(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    ورنہ اگر(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    اگر(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : لڑی_لمبائی(protsep));
      اگر(cutoff)
        cutoff++; /* truncate after last slash */
    }
    توڑو;
  }

  prelen = cutoff ? (size_t)(cutoff - بنیاد) : لڑی_لمبائی(بنیاد);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_داخلہ_لمبائی);

  اگر(!curlx_dyn_addn(&urlbuf, بنیاد, prelen) &&
     !urlencode_str(&urlbuf, useurl, لڑی_لمبائی(useurl), !host_بدلا,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_راستہ_AS_IS);
  }
  ورنہ
    uc = CURLUE_برآمد_کا_حافظہ;

  curlx_dyn_free(&urlbuf);
  واپس uc;
}

/*
 */
CURLU *curl_url(خالی)
{
  واپس curlx_calloc(1, حجم(ساخت Curl_URL));
}

خالی curl_url_cleanup(CURLU *u)
{
  اگر(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#تعریف_کرو DUP(dest, src, name)                    \
  کرو {                                          \
    اگر((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      اگر(!(dest)->name)                         \
        جاؤ fail;                              \
    }                                           \
  } جب(0)

CURLU *curl_url_dup(ثابت CURLU *اندر)
{
  ساخت Curl_URL *u;

  اگر(!اندر)
    واپس تہی;

  u = curlx_calloc(1, حجم(ساخت Curl_URL));
  اگر(u) {
    DUP(u, اندر, scheme);
    DUP(u, اندر, user);
    DUP(u, اندر, password);
    DUP(u, اندر, options);
    DUP(u, اندر, host);
    DUP(u, اندر, راستہ);
    DUP(u, اندر, query);
    DUP(u, اندر, fragment);
    DUP(u, اندر, zoneid);
    u->portnum = اندر->portnum;
    u->پورٹ_present = اندر->پورٹ_present;
    u->fragment_present = اندر->fragment_present;
    u->query_present = اندر->query_present;
    u->guessed_scheme = اندر->guessed_scheme;
  }
  واپس u;
fail:
  curl_url_cleanup(u);
  واپس تہی;
}

#اگر_غیر_تعریف_شدہ USE_IDN
#تعریف_کرو host_decode(x, y) CURLUE_LACKS_IDN
#تعریف_کرو host_encode(x, y) CURLUE_LACKS_IDN
#ورنہ
ساکن CURLUcode host_decode(ثابت حرف *host, حرف **allochost)
{
  CURLcode نتیجہ = Curl_idn_decode(host, allochost);
  اگر(نتیجہ)
    واپس (نتیجہ == CURLE_برآمد_کا_حافظہ) ?
      CURLUE_برآمد_کا_حافظہ : CURLUE_BAD_HOSTNAME;
  واپس CURLUE_OK;
}

ساکن CURLUcode host_encode(ثابت حرف *host, حرف **allochost)
{
  CURLcode نتیجہ = Curl_idn_encode(host, allochost);
  اگر(نتیجہ)
    واپس (نتیجہ == CURLE_برآمد_کا_حافظہ) ?
      CURLUE_برآمد_کا_حافظہ : CURLUE_BAD_HOSTNAME;
  واپس CURLUE_OK;
}
#اگر_ختم

ساکن CURLUcode urlget_format(ثابت CURLU *u, CURLUPart what,
                               ثابت حرف *ptr, حرف **partp,
                               bool plusdecode, غیرعلامتی صحیح flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = لڑی_لمبائی(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  حرف *part = curlx_memdup0(ptr, partlen);
  *partp = تہی;
  اگر(!part)
    واپس CURLUE_برآمد_کا_حافظہ;
  اگر(plusdecode) {
    /* convert + to space */
    حرف *plus = part;
    size_t i = 0;
    برائے(i = 0; i < partlen; ++plus, i++) {
      اگر(*plus == '+')
        *plus = ' ';
    }
  }
  اگر(urldecode) {
    حرف *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode نتیجہ = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    اگر(نتیجہ)
      واپس CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  اگر(urlencode) {
    ساخت dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_داخلہ_لمبائی);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    اگر(uc)
      واپس uc;
    part = curlx_dyn_ptr(&enc);
  }
  ورنہ اگر(punycode) {
    اگر(!Curl_is_ASCII_name(u->host)) {
      حرف *punyversion = تہی;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      اگر(uc)
        واپس uc;
      uc = hostname_check(punyversion, لڑی_لمبائی(punyversion));
      اگر(uc) {
        curlx_free(punyversion);
        واپس uc;
      }
      part = punyversion;
    }
  }
  ورنہ اگر(depunyfy && Curl_is_ASCII_name(u->host)) {
    حرف *unpunified = تہی;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    اگر(uc)
      واپس uc;
    part = unpunified;
  }
  *partp = part;
  واپس CURLUE_OK;
}

ساکن CURLUcode file_url(ثابت CURLU *u, حرف **part,
                          ثابت حرف *fragmentsep,
                          ثابت حرف *querysep)
{
  حرف *url = curl_maprintf("file://%s%s%s%s%s",
                            u->راستہ, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  اگر(!url)
    واپس CURLUE_برآمد_کا_حافظہ;

  *part = url;
  واپس CURLUE_OK;
}

ساکن CURLUcode urlget_url(ثابت CURLU *u, حرف **part, غیرعلامتی صحیح flags)
{
  حرف *url;
  حرف *allochost = تہی;
  ثابت حرف *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_خالی_خانے)) ?
    "#" : "";
  ثابت حرف *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_خالی_خانے)) ?
    "?" : "";
  حرف portbuf[7];
  اگر(curl_strequal("file", u->scheme))
    واپس file_url(u, part, fragmentsep, querysep);
  ورنہ اگر(!u->host)
    واپس CURLUE_NO_HOST;
  ورنہ {
    ثابت حرف *scheme;
    حرف *options = u->options;
    حرف *پورٹ = تہی;
    ثابت ساخت Curl_scheme *h = تہی;
    حرف schemebuf[MAX_SCHEME_LEN + 5];
    اگر(u->scheme)
      scheme = u->scheme;
    ورنہ اگر(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    ورنہ
      واپس CURLUE_NO_SCHEME;

    اگر(u->پورٹ_present) {
      curl_msnprintf(portbuf, حجم(portbuf), "%u", u->portnum);
      پورٹ = portbuf;
    }

    h = Curl_get_scheme(scheme);
    اگر(h) {
      اگر(!u->پورٹ_present && (flags & CURLU_DEFAULT_پورٹ)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, حجم(portbuf), "%u", h->defport);
        پورٹ = portbuf;
      }
      ورنہ اگر(u->پورٹ_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_پورٹ)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        پورٹ = تہی;
      }

      اگر(!(h->flags & PROTOPT_URLOPTIONS))
        options = تہی;
    }

    اگر(u->host[0] == '[') {
      اگر(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ساخت dynbuf enc;
        size_t hostlen = لڑی_لمبائی(u->host);
        curlx_dyn_init(&enc, CURL_MAX_داخلہ_لمبائی);
        اگر(curlx_dyn_addf(&enc, "%.*s%%25%s]", (صحیح)hostlen - 1, u->host,
                          u->zoneid))
          واپس CURLUE_برآمد_کا_حافظہ;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    ورنہ اگر(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(تہی, u->host, 0);
      اگر(!allochost)
        واپس CURLUE_برآمد_کا_حافظہ;
    }
    ورنہ اگر(flags & CURLU_PUNYCODE) {
      اگر(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        اگر(!ret)
          ret = hostname_check(allochost, لڑی_لمبائی(allochost));
        اگر(ret) {
          curlx_safefree(allochost);
          واپس ret;
        }
      }
    }
    ورنہ اگر(flags & CURLU_PUNY2IDN) {
      اگر(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        اگر(ret)
          واپس ret;
      }
    }

    اگر(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, حجم(schemebuf), "%s://", scheme);
    ورنہ
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
                        پورٹ ? ":" : "",
                        پورٹ ? پورٹ : "",
                        u->راستہ ? u->راستہ : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  اگر(!url)
    واپس CURLUE_برآمد_کا_حافظہ;
  *part = url;
  واپس CURLUE_OK;
}

CURLUcode curl_url_get(ثابت CURLU *u, CURLUPart what,
                       حرف **part, غیرعلامتی صحیح flags)
{
  ثابت حرف *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  حرف portbuf[7];
  bool plusdecode = FALSE;
  اگر(!u)
    واپس CURLUE_BAD_HANDLE;
  اگر(!part)
    واپس CURLUE_BAD_PARTPOINTER;
  *part = تہی;

  انتخاب(what) {
  حالت CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    اگر((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      واپس CURLUE_NO_SCHEME;
    توڑو;
  حالت CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    توڑو;
  حالت CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    توڑو;
  حالت CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    توڑو;
  حالت CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    توڑو;
  حالت CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    توڑو;
  حالت CURLUPART_پورٹ:
    ptr = تہی;
    ifmissing = CURLUE_NO_پورٹ;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    اگر(u->پورٹ_present) {
      ثابت ساخت Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : تہی;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      اگر(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_پورٹ)) {
        ptr = تہی;
      }
      ورنہ {
        curl_msnprintf(portbuf, حجم(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    ورنہ اگر((flags & CURLU_DEFAULT_پورٹ) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ثابت ساخت Curl_scheme *h = Curl_get_scheme(u->scheme);
      اگر(h) {
        curl_msnprintf(portbuf, حجم(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    توڑو;
  حالت CURLUPART_راستہ:
    ptr = u->راستہ;
    اگر(!ptr)
      ptr = "/";
    توڑو;
  حالت CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    اگر(ptr && !ptr[0] && !(flags & CURLU_GET_خالی_خانے))
      /* there was a blank query and the user does not ask for it */
      ptr = تہی;
    توڑو;
  حالت CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    اگر(!ptr && u->fragment_present && flags & CURLU_GET_خالی_خانے)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    توڑو;
  حالت CURLUPART_URL:
    واپس urlget_url(u, part, flags);
  طےشدہ:
    ptr = تہی;
    توڑو;
  }
  اگر(ptr)
    واپس urlget_format(u, what, ptr, part, plusdecode, flags);

  واپس ifmissing;
}

ساکن CURLUcode سیٹ_url_scheme(CURLU *u, ثابت حرف *scheme,
                                غیرعلامتی صحیح flags)
{
  size_t plen = لڑی_لمبائی(scheme);
  ثابت ساخت Curl_scheme *h = تہی;
  اگر((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    واپس CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  اگر(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->تسلسل))
    واپس CURLUE_UNSUPPORTED_SCHEME;
  اگر(!h) {
    ثابت حرف *s = scheme;
    اگر(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      جب(--plen) {
        اگر(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        ورنہ
          واپس CURLUE_BAD_SCHEME;
      }
    }
    ورنہ
      واپس CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  واپس CURLUE_OK;
}

ساکن CURLUcode سیٹ_url_پورٹ(CURLU *u, ثابت حرف *provided_پورٹ)
{
  curl_off_t پورٹ;
  اگر(!ISDIGIT(provided_پورٹ[0]))
    /* not a number */
    واپس CURLUE_BAD_پورٹ_نمبر;
  اگر(curlx_str_نمبر(&provided_پورٹ, &پورٹ, 0xffff) || *provided_پورٹ)
    /* weirdly provided number, not good! */
    واپس CURLUE_BAD_پورٹ_نمبر;
  u->portnum = (uint16_t)پورٹ;
  u->پورٹ_present = TRUE;
  واپس CURLUE_OK;
}

ساکن CURLUcode سیٹ_url(CURLU *u, ثابت حرف *url, size_t part_سائز,
                         غیرعلامتی صحیح flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  حرف *oldurl = تہی;

  اگر(!part_سائز) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    اگر(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      واپس CURLUE_OK;
    }
    اگر(uc == CURLUE_برآمد_کا_حافظہ)
      واپس uc;
    واپس CURLUE_MALFORMED_داخلہ;
  }

  /* if the new URL is absolute replace the existing with the new. */
  اگر(Curl_is_absolute_url(url, تہی, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    واپس parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_خالی_خانے);
  اگر(uc == CURLUE_برآمد_کا_حافظہ)
    واپس uc;
  ورنہ اگر(uc)
    واپس parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  واپس uc;
}

ساکن CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  انتخاب(what) {
  حالت CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, حجم(ساخت Curl_URL));
    توڑو;
  حالت CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    توڑو;
  حالت CURLUPART_USER:
    curlx_safefree(u->user);
    توڑو;
  حالت CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    توڑو;
  حالت CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    توڑو;
  حالت CURLUPART_HOST:
    curlx_safefree(u->host);
    توڑو;
  حالت CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    توڑو;
  حالت CURLUPART_پورٹ:
    u->portnum = 0;
    u->پورٹ_present = FALSE;
    توڑو;
  حالت CURLUPART_راستہ:
    curlx_safefree(u->راستہ);
    توڑو;
  حالت CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    توڑو;
  حالت CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    توڑو;
  طےشدہ:
    واپس CURLUE_UNKNOWN_PART;
  }
  واپس CURLUE_OK;
}

ساکن bool allowed_اندر_راستہ(غیرعلامتی حرف x)
{
  انتخاب(x) {
  حالت '!':
  حالت '$':
  حالت '&':
  حالت '\'':
  حالت '(':
  حالت ')':
  حالت '{':
  حالت '}':
  حالت '[':
  حالت ']':
  حالت '*':
  حالت '+':
  حالت ',':
  حالت ';':
  حالت '=':
  حالت ':':
  حالت '@':
  حالت '/':
    واپس TRUE;
  }
  واپس FALSE;
}

ساکن CURLUcode url_encode_part(ساخت dynbuf *encp,
                                 ثابت حرف *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ثابت غیرعلامتی حرف *i;

  برائے(i = (ثابت غیرعلامتی حرف *)part; *i; i++) {
    CURLcode نتیجہ;
    اگر((*i == ' ') && plusencode)
      نتیجہ = curlx_dyn_addn(encp, "+", 1);
    ورنہ اگر(ISUNRESERVED(*i) ||
            (pathmode && allowed_اندر_راستہ(*i)) ||
            ((*i == '=') && equalsencode)) {
      اگر((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      نتیجہ = curlx_dyn_addn(encp, i, 1);
    }
    ورنہ {
      غیرعلامتی حرف برآمد[3] = { '%' };
      Curl_hexbyte(&برآمد[1], *i);
      نتیجہ = curlx_dyn_addn(encp, برآمد, 3);
    }
    اگر(نتیجہ)
      واپس cc2cu(نتیجہ);
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode url_uppercasehex_part(ساخت dynbuf *encp,
                                       ثابت حرف *part)
{
  حرف *p;
  CURLcode نتیجہ = curlx_dyn_جوڑو(encp, part);
  اگر(نتیجہ)
    واپس cc2cu(نتیجہ);
  p = curlx_dyn_ptr(encp);
  جب(*p) {
    /* make sure percent encoded are upper case */
    اگر((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    ورنہ
      p++;
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode url_append_query(CURLU *u, ساخت dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? لڑی_لمبائی(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  اگر(querylen) {
    ساخت dynbuf qbuf;
    CURLcode نتیجہ;
    ثابت حرف *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_داخلہ_لمبائی);

    /* add original query */
    نتیجہ = curlx_dyn_addn(&qbuf, u->query, querylen);
    اگر(!نتیجہ && addamperand)
      /* add ampersand */
      نتیجہ = curlx_dyn_addn(&qbuf, "&", 1);
    اگر(!نتیجہ)
      /* add new query part */
      نتیجہ = curlx_dyn_جوڑو(&qbuf, newp);
    اگر(نتیجہ)
      جاؤ nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    واپس CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    واپس cc2cu(نتیجہ);
  }
  ورنہ {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  واپس CURLUE_OK;
}

ساکن CURLUcode url_sethost(CURLU *u, ساخت dynbuf *encp,
                             bool urlencode,
                             غیرعلامتی صحیح flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  حرف *newp = curlx_dyn_ptr(encp);
  اگر(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  ورنہ اگر(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    حرف *decoded = تہی;
    CURLcode نتیجہ = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    اگر(نتیجہ || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    ورنہ {
      ساخت dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_داخلہ_لمبائی);
      اگر(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      ورنہ اگر(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        اگر(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  ورنہ اگر(hostname_check6(u, newp, n))
    bad = TRUE;
  ورنہ اگر(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  اگر(bad) {
    curlx_dyn_free(encp);
    واپس CURLUE_BAD_HOSTNAME;
  }
  واپس CURLUE_OK;
}

CURLUcode curl_url_سیٹ(CURLU *u, CURLUPart what,
                       ثابت حرف *part, غیرعلامتی صحیح flags)
{
  حرف **storep = تہی;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  اگر(!u)
    واپس CURLUE_BAD_HANDLE;
  اگر(!part)
    /* setting a part to NULL clears it */
    واپس urlset_clear(u, what);

  nalloc = لڑی_لمبائی(part);
  اگر(nalloc > CURL_MAX_داخلہ_لمبائی)
    /* excessive input length */
    واپس CURLUE_MALFORMED_داخلہ;

  انتخاب(what) {
  حالت CURLUPART_SCHEME: {
    CURLUcode حالت_2 = سیٹ_url_scheme(u, part, flags);
    اگر(حالت_2)
      واپس حالت_2;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    توڑو;
  }
  حالت CURLUPART_USER:
    storep = &u->user;
    توڑو;
  حالت CURLUPART_PASSWORD:
    storep = &u->password;
    توڑو;
  حالت CURLUPART_OPTIONS:
    storep = &u->options;
    توڑو;
  حالت CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    توڑو;
  حالت CURLUPART_ZONEID:
    storep = &u->zoneid;
    توڑو;
  حالت CURLUPART_پورٹ:
    واپس سیٹ_url_پورٹ(u, part);
  حالت CURLUPART_راستہ:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->راستہ;
    توڑو;
  حالت CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    توڑو;
  حالت CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    توڑو;
  حالت CURLUPART_URL:
    واپس سیٹ_url(u, part, nalloc, flags);
  طےشدہ:
    واپس CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ثابت حرف *newp = تہی;
    ساخت dynbuf enc;
    CURLUcode حالت_2;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    اگر(leadingslash && (part[0] != '/')) {
      CURLcode نتیجہ = curlx_dyn_addn(&enc, "/", 1);
      اگر(نتیجہ)
        واپس cc2cu(نتیجہ);
    }
    اگر(urlencode)
      حالت_2 = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    ورنہ
      حالت_2 = url_uppercasehex_part(&enc, part);
    اگر(!حالت_2) {
      newp = curlx_dyn_ptr(&enc);

      اگر(appendquery && newp)
        واپس url_append_query(u, &enc);
      ورنہ اگر(what == CURLUPART_HOST)
        حالت_2 = url_sethost(u, &enc, urlencode, flags);
    }
    اگر(حالت_2)
      واپس حالت_2;

    اگر(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (حرف *)CURL_UNCONST(newp);
  }
  واپس CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *بنیاد, CURLU *href)
{
  ثابت ساخت Curl_scheme *s = تہی;

  /* base must be an absolute URL */
  اگر(!بنیاد->scheme || !بنیاد->host)
    واپس FALSE;
  اگر(href->scheme && !curl_strequal(بنیاد->scheme, href->scheme))
    واپس FALSE;
  اگر(href->host) {
    اگر(!curl_strequal(بنیاد->host, href->host))
      واپس FALSE;

    اگر(بنیاد->پورٹ_present != href->پورٹ_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(بنیاد->scheme);
      اگر(!s) /* Cannot match default port for unknown scheme */
        واپس FALSE;
      /* to match, the present one must be the default port */
      اگر((بنیاد->پورٹ_present && (بنیاد->portnum != s->defport)) ||
         (href->پورٹ_present && (href->portnum != s->defport)))
        واپس FALSE;
    }
    ورنہ اگر(بنیاد->portnum != href->portnum) /* both present or missing */
      واپس FALSE;

    اگر(!curl_strequal(بنیاد->zoneid ? بنیاد->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      واپس FALSE;
  }
  ورنہ اگر(href->پورٹ_present) /* no host in href, then there must be no port */
    واپس FALSE;
  واپس TRUE;
}

CURLUcode Curl_url_get_پورٹ(CURLU *u, uint16_t *pport)
{
  اگر(u->پورٹ_present) {
    *pport = u->portnum;
    واپس CURLUE_OK;
  }
  ورنہ اگر(u->scheme) {
    ثابت ساخت Curl_scheme *s = Curl_get_scheme(u->scheme);
    اگر(s && s->defport) {
      *pport = s->defport;
      واپس CURLUE_OK;
    }
  }
  *pport = 0;
  واپس CURLUE_NO_پورٹ;
}
