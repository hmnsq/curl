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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
/* MS-DOS/Windows style drive prefix, eg c: in c:foo */
#ವ್ಯಾಖ್ಯಾನಿಸು STARTS_WITH_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(str)        \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||  \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) && \
   ((str)[1] == ':'))
#ಆದರೆ_ಅಂತ್ಯ

/* MS-DOS/Windows style drive prefix, optionally with
 * a '|' instead of ':', followed by a slash or NUL */
#ವ್ಯಾಖ್ಯಾನಿಸು STARTS_WITH_URL_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(str)                  \
  ((('a' <= (str)[0] && (str)[0] <= 'z') ||                \
    ('A' <= (str)[0] && (str)[0] <= 'Z')) &&               \
   ((str)[1] == ':' || (str)[1] == '|') &&                 \
   ((str)[2] == '/' || (str)[2] == '\\' || (str)[2] == 0))

/* scheme is not URL encoded, the longest libcurl supported ones are... */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_SCHEME_LEN 40
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_ZONEID_LEN 16

/*
 * If USE_IPV6 is disabled, we still want to parse IPv6 addresses, so make
 * sure we have _some_ value for AF_INET6 without polluting our fake value
 * everywhere.
 */
#ಆದರೆ !defined(USE_IPV6) && !defined(AF_INET6)
#ವ್ಯಾಖ್ಯಾನಿಸು AF_INET6 (AF_INET + 1)
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_SCHEME "https"

ಸ್ಥಿರ ಶೂನ್ಯ free_urlhandle(ರಚನೆ Curl_URL *u)
{
  curlx_free(u->scheme);
  curlx_free(u->user);
  curlx_strzero(u->password);
  curlx_free(u->password);
  curlx_free(u->options);
  curlx_free(u->host);
  curlx_free(u->zoneid);
  curlx_free(u->ಮಾರ್ಗ);
  curlx_free(u->query);
  curlx_free(u->fragment);
}

/*
 * Find the separator at the end of the hostname, or the '?' in cases like
 * http://www.example.com?id=2380
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕಂಡುಹಿಡಿ_host_sep(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url)
{
  /* Find the start of the hostname */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(url, "//");
  ಆದರೆ(!sep)
    sep = url;
  ಇಲ್ಲದಿದ್ದರೆ
    sep += 2;

  /* Find first / or ? */
  ಆಗ(*sep && *sep != '/' && *sep != '?')
    sep++;

  ಹಿಂತಿರುಗಿ sep;
}

/* convert CURLcode to CURLUcode */
#ವ್ಯಾಖ್ಯಾನಿಸು cc2cu(x) \
  ((x) == CURLE_TOO_LARGE ? CURLUE_TOO_LARGE : CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)

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
UNITTEST CURLUcode urlencode_str(ರಚನೆ dynbuf *o, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url,
                                 size_t len, bool relative,
                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ query);
UNITTEST CURLUcode urlencode_str(ರಚನೆ dynbuf *o, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url,
                                 size_t len, bool relative,
                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ query)
{
  /* we must add this with whitespace-replacing */
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *iptr;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *host_sep = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)url;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  DEBUGASSERT((query >= QUERY_NO) && (query <= QUERY_YES));

  ಆದರೆ(!relative) {
    size_t n;
    host_sep = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಕಂಡುಹಿಡಿ_host_sep(url);

    /* output the first piece as-is */
    n = (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)host_sep - url;
    ಫಲಿತಾಂಶ = curlx_dyn_addn(o, url, n);
    len -= n;
  }

  ಗಾಗಿ(iptr = host_sep; len && !ಫಲಿತಾಂಶ;) {
    ಆದರೆ(*iptr == ' ') {
      ಆದರೆ(query != QUERY_YES)
        ಫಲಿತಾಂಶ = curlx_dyn_addn(o, "%20", 3);
      ಇಲ್ಲದಿದ್ದರೆ
        ಫಲಿತಾಂಶ = curlx_dyn_addn(o, "+", 1);
      iptr++;
      len--;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((*iptr < ' ') || (*iptr >= 0x7f)) {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಹೊರಹೋಗು[3] = { '%' };
      Curl_hexbyte(&ಹೊರಹೋಗು[1], *iptr);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(o, ಹೊರಹೋಗು, 3);
      iptr++;
      len--;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*iptr == '%' && (len >= 3) &&
            ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
            (ISLOWER(iptr[1]) || ISLOWER(iptr[2]))) {
      /* uppercase it */
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ hex = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)((curlx_hexval(iptr[1]) << 4) |
                                          curlx_hexval(iptr[2]));
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಹೊರಹೋಗು[3] = { '%' };
      Curl_hexbyte(&ಹೊರಹೋಗು[1], hex);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(o, ಹೊರಹೋಗು, 3);
      iptr += 3;
      len -= 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಆರಂಭ = iptr;
      ಆಗ(len) {
        ಆದರೆ(*iptr == ' ' || *iptr < ' ' || *iptr >= 0x7f)
          ನಿಲ್ಲಿಸು;
        ಆದರೆ(*iptr == '%' && (len >= 3) &&
           ISXDIGIT(iptr[1]) && ISXDIGIT(iptr[2]) &&
           (ISLOWER(iptr[1]) || ISLOWER(iptr[2])))
          ನಿಲ್ಲಿಸು;
        ಆದರೆ(*iptr == '?') {
          ಆದರೆ(query == QUERY_NOT_YET) {
            iptr++;
            len--;
            query = QUERY_YES;
            ನಿಲ್ಲಿಸು;
          }
        }
        iptr++;
        len--;
      }
      ಫಲಿತಾಂಶ = curlx_dyn_addn(o, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ಆರಂಭ, (size_t)(iptr - ಆರಂಭ));
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ cc2cu(ಫಲಿತಾಂಶ);
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

/*
 * Returns the length of the scheme if the given URL is absolute (as opposed
 * to relative). Stores the scheme in the buffer if TRUE and 'buf' is
 * non-NULL. The buflen must be larger than MAX_SCHEME_LEN if buf is set.
 *
 * If 'guess_scheme' is TRUE, it means the URL might be provided without
 * scheme.
 */
size_t Curl_is_absolute_url(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, ಅಕ್ಷರ *buf, size_t buflen,
                            bool guess_scheme)
{
  size_t i = 0;
  DEBUGASSERT(!buf || (buflen > MAX_SCHEME_LEN));
  (ಶೂನ್ಯ)buflen; /* only used in debug-builds */
  ಆದರೆ(buf)
    buf[0] = 0; /* always leave a defined value in buf */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32
  ಆದರೆ(guess_scheme && STARTS_WITH_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(url))
    ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ_ಅಂತ್ಯ
  ಆದರೆ(ISALPHA(url[0])) {
    ಆದರೆ(buf)
      buf[0] = Curl_raw_tolower(url[0]);
    ಗಾಗಿ(i = 1; i < MAX_SCHEME_LEN; ++i) {
      ಅಕ್ಷರ s = url[i];
      ಆದರೆ(s && (ISALNUM(s) || (s == '+') || (s == '-') || (s == '.'))) {
        ಆದರೆ(buf)
          buf[i] = Curl_raw_tolower(s);
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        ನಿಲ್ಲಿಸು;
      }
    }
  }
  ಆದರೆ(i && (url[i] == ':') && ((url[i + 1] == '/') || !guess_scheme)) {
    /* If this does not guess scheme, the scheme always ends with the colon so
       that this also detects data: URLs etc. In guessing mode, data: could
       be the hostname "data" with a specified port number. */

    /* the length of the scheme is the name part only */
    size_t len = i;
    ಆದರೆ(buf)
      buf[i] = 0;
    ಹಿಂತಿರುಗಿ len;
  }
  ಆದರೆ(buf)
    buf[0] = 0;
  ಹಿಂತಿರುಗಿ 0;
}

/* scan for byte values <= 31, 127 and maybe space */
ಸ್ಥಿರ bool badoctets(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, size_t n, ಪೂರ್ಣಾಂಕ flags)
{
  ಸ್ಥಿರಾಂಕ uint8_t *p = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಒಳಹರಿವು;
  ಸ್ಥಿರಾಂಕ uint8_t control = flags & CURLU_ALLOW_SPACE ? 0x1f : 0x20;
  ಆಗ(n--) {
    ಆದರೆ(*p <= control || *p == 127)
      ಹಿಂತಿರುಗಿ TRUE;
    p++;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * parse_hostname_login()
 *
 * Parse the login details (username, password and options) from the URL and
 * strip them out of the hostname
 *
 * @unittest 1675
 */
UNITTEST CURLUcode parse_hostname_login(ರಚನೆ Curl_URL *u,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *login,
                                        size_t len,
                                        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                        size_t *hostname_ಸ್ಥಾನಾಂತರ);
UNITTEST CURLUcode parse_hostname_login(ರಚನೆ Curl_URL *u,
                                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *login,
                                        size_t len,
                                        ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                        size_t *hostname_ಸ್ಥಾನಾಂತರ)
{
  CURLUcode ures = CURLUE_OK;
  CURLcode ಫಲಿತಾಂಶ;
  ಅಕ್ಷರ *userp = ಖಾಲಿ;
  ಅಕ್ಷರ *passwdp = ಖಾಲಿ;
  ಅಕ್ಷರ *optionsp = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *h = ಖಾಲಿ;

  /* At this point, we assume all the other special cases have been taken
   * care of, so the host is at most
   *
   *   [user[:password][;options]]@]hostname
   *
   * We need somewhere to put the embedded details, so do that first.
   */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;

  DEBUGASSERT(login);

  *hostname_ಸ್ಥಾನಾಂತರ = 0;
  ptr = memchr(login, '@', len);
  ಆದರೆ(!ptr)
    ಹೋಗು ಹೊರಹೋಗು;

  /* We will now try to extract the
   * possible login information in a string like:
   * ftp://user:password@ftp.site.example:8021/README */
  ptr++;

  /* if this is a known scheme, get some details */
  ಆದರೆ(u->scheme)
    h = Curl_get_scheme(u->scheme);

  /* We could use the login information in the URL so extract it. Only parse
     options if the handler says we should. Note that 'h' might be NULL! */
  ಫಲಿತಾಂಶ = Curl_parse_login_details(login, ptr - login - 1,
                                    &userp, &passwdp,
                                    (h && (h->flags & PROTOPT_URLOPTIONS)) ?
                                    &optionsp : ಖಾಲಿ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    /* the only possible error from Curl_parse_login_details is out of
       memory: */
    ures = CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    ಹೋಗು ಹೊರಹೋಗು;
  }

  ಆದರೆ(userp) {
    ಆದರೆ(flags & CURLU_DISALLOW_USER) {
      /* Option DISALLOW_USER is set and URL contains username. */
      ures = CURLUE_USER_NOT_ALLOWED;
      ಹೋಗು ಹೊರಹೋಗು;
    }
    curlx_free(u->user);
    u->user = userp;
  }

  ಆದರೆ(passwdp) {
    curlx_strzero(u->password);
    curlx_free(u->password);
    u->password = passwdp;
  }

  ಆದರೆ(optionsp) {
    curlx_free(u->options);
    u->options = optionsp;
  }

  ಆದರೆ(userp && badoctets(userp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(userp), flags))
    ures = CURLUE_BAD_USER;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(passwdp && badoctets(passwdp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(passwdp), flags))
    ures = CURLUE_BAD_PASSWORD;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(optionsp && badoctets(optionsp, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(optionsp), flags))
    ures = CURLUE_MALFORMED_ಒಳಹರಿವು;

  userp = passwdp = optionsp = ಖಾಲಿ;

  ಆದರೆ(!ures) {
    /* the hostname starts at this offset */
    *hostname_ಸ್ಥಾನಾಂತರ = ptr - login;
    ಹಿಂತಿರುಗಿ CURLUE_OK;
  }

ಹೊರಹೋಗು:

  curlx_free(userp);
  curlx_strzero(passwdp);
  curlx_free(passwdp);
  curlx_free(optionsp);
  curlx_safefree(u->user);
  curlx_strzero(u->password);
  curlx_safefree(u->password);
  curlx_safefree(u->options);

  ಹಿಂತಿರುಗಿ ures;
}

/* @unittest 1653 */
UNITTEST CURLUcode parse_ಪೋರ್ಟ್(ರಚನೆ Curl_URL *u, ರಚನೆ dynbuf *host,
                              bool has_scheme);
UNITTEST CURLUcode parse_ಪೋರ್ಟ್(ರಚನೆ Curl_URL *u, ರಚನೆ dynbuf *host,
                              bool has_scheme)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *portptr;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = curlx_dyn_ptr(host);
  /*
   * Find the end of an IPv6 address on the ']' ending bracket.
   */
  u->portnum = 0;
  u->ಪೋರ್ಟ್_present = FALSE;
  ಆದರೆ(hostname[0] == '[') {
    portptr = memchr(hostname + 1, ']', curlx_dyn_len(host) - 1);
    ಆದರೆ(!portptr)
      ಹಿಂತಿರುಗಿ CURLUE_BAD_IPV6;
    portptr++;
    /* this is a RFC2732-style specified IP-address */
    ಆದರೆ(*portptr) {
      ಆದರೆ(*portptr != ':')
        ಹಿಂತಿರುಗಿ CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      portptr = ಖಾಲಿ;
  }
  ಇಲ್ಲದಿದ್ದರೆ
    portptr = memchr(hostname, ':', curlx_dyn_len(host));

  ಆದರೆ(portptr) {
    curl_off_t ಪೋರ್ಟ್;
    size_t keep = portptr - hostname;
    ಪೂರ್ಣಾಂಕ rc;

    /* Browser behavior adaptation. If there is a colon with no digits after,
       cut off the name there which makes us ignore the colon and use the
       default port. Firefox, Chrome and Safari all do that.

       Do not do it if the URL has no scheme, to make something that looks like
       a scheme not work! */
    curlx_dyn_setlen(host, keep);
    portptr++;
    ಆದರೆ(!*portptr)
      ಹಿಂತಿರುಗಿ has_scheme ? CURLUE_OK : CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ;
    ಆದರೆ(*portptr == '\\')
      ಹಿಂತಿರುಗಿ CURLUE_BACKSLASH;
    rc = curlx_str_ಸಂಖ್ಯೆ(&portptr, &ಪೋರ್ಟ್, 0xffff);
    ಆದರೆ(rc)
      ಹಿಂತಿರುಗಿ CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*portptr == '\\')
      ಹಿಂತಿರುಗಿ CURLUE_BACKSLASH;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(*portptr)
      ಹಿಂತಿರುಗಿ CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ;

    u->portnum = (uint16_t)ಪೋರ್ಟ್;
    u->ಪೋರ್ಟ್_present = TRUE;
  }

  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

/* characters not allowed in hostnames:
   " \r\n\t/:#?!@{}[]\\$\'\"^`*<>=;,+&()%|" */

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ bool invalid_host_char[256] = {
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
UNITTEST CURLUcode ipv6_parse(ರಚನೆ Curl_URL *u, ಅಕ್ಷರ *hostname,
                              size_t hlen);
UNITTEST CURLUcode ipv6_parse(ರಚನೆ Curl_URL *u, ಅಕ್ಷರ *hostname,
                              size_t hlen) /* length of hostname */
{
  size_t len;
  DEBUGASSERT(*hostname == '[');
  ಆದರೆ(hlen < 4) /* '[::]' is the shortest possible valid string */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_IPV6;
  hostname++;
  hlen -= 2;

  /* only valid IPv6 letters are ok */
  len = strspn(hostname, "0123456789abcdefABCDEF:.");

  ಆದರೆ(hlen != len) {
    hlen = len;
    ಆದರೆ(hostname[len] == '%') {
      /* this could now be '%[zone id]' */
      ಅಕ್ಷರ zoneid[MAX_ZONEID_LEN];
      ಪೂರ್ಣಾಂಕ i = 0;
      ಅಕ್ಷರ *h = &hostname[len + 1];
      /* pass '25' if present and is a URL encoded percent sign */
      ಆದರೆ(!ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(h, "25", 2) && h[2] && (h[2] != ']'))
        h += 2;
      ಆಗ(!invalid_host_char[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)*h] && (*h != ']') &&
            (i < (MAX_ZONEID_LEN - 1)))
        zoneid[i++] = *h++;
      ಆದರೆ(!i || (']' != *h))
        ಹಿಂತಿರುಗಿ CURLUE_BAD_IPV6;
      zoneid[i] = 0;
      u->zoneid = curlx_strdup(zoneid);
      ಆದರೆ(!u->zoneid)
        ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      hostname[len] = ']'; /* insert end bracket */
      hostname[len + 1] = 0; /* terminate the hostname */
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLUE_BAD_IPV6;
    /* hostname is fine */
  }

  /* Normalize the IPv6 address */
  {
    ಅಕ್ಷರ dest[16]; /* fits a binary IPv6 address */
    hostname[hlen] = 0; /* end the address there */
    ಆದರೆ(curlx_inet_pton(AF_INET6, hostname, dest) != 1)
      ಹಿಂತಿರುಗಿ CURLUE_BAD_IPV6;
    ಆದರೆ(!curlx_inet_ntop(AF_INET6, dest, hostname, hlen + 1)) {
      hlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(hostname); /* might be shorter now */
      hostname[hlen + 1] = 0;
    }
    hostname[hlen] = ']'; /* restore ending bracket */
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

/* the input is a confirmed hostname, never an IPv6 address */
ಸ್ಥಿರ CURLUcode hostname_check(ಅಕ್ಷರ *hostname, size_t hlen)
{
  size_t i;
  ಗಾಗಿ(i = 0; i < hlen; i++) {
    ಆದರೆ(invalid_host_char[(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)hostname[i]])
      ಹಿಂತಿರುಗಿ CURLUE_BAD_HOSTNAME;
  }
  ಆದರೆ((hlen >= 2) &&
     (hostname[hlen - 1] == '.') && (hostname[hlen - 2] == '.'))
    /* more than one trailing dot is not allowed */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_HOSTNAME;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((hlen == 1) && (hostname[0] == '.'))
    /* a single dot alone is not allowed */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_HOSTNAME;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

/* the input is a hostname or perhaps an IPv6 address */
ಸ್ಥಿರ CURLUcode hostname_check6(ರಚನೆ Curl_URL *u, ಅಕ್ಷರ *hostname,
                                 size_t hlen) /* length of hostname */
{
  DEBUGASSERT(hostname);

  ಆದರೆ(!hlen)
    ಹಿಂತಿರುಗಿ CURLUE_NO_HOST;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(hostname[0] == '[')
    ಹಿಂತಿರುಗಿ ipv6_parse(u, hostname, hlen);

  ಹಿಂತಿರುಗಿ hostname_check(hostname, hlen);
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
UNITTEST ಪೂರ್ಣಾಂಕ ipv4_normalize(ರಚನೆ dynbuf *host);
UNITTEST ಪೂರ್ಣಾಂಕ ipv4_normalize(ರಚನೆ dynbuf *host)
{
  bool ಮುಗಿದ = FALSE;
  ಪೂರ್ಣಾಂಕ n = 0;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *c = curlx_dyn_ptr(host);
  ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ parts[4] = { 0, 0, 0, 0 };
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!ISDIGIT(*c))
    ಹಿಂತಿರುಗಿ HOST_NAME;

  ಆಗ(!ಮುಗಿದ) {
    ಪೂರ್ಣಾಂಕ rc;
    curl_off_t l;
    ಆದರೆ(*c == '0') {
      ಆದರೆ((c[1] | 0x20) == 'x') {
        c += 2; /* skip the prefix */
        rc = curlx_str_hex(&c, &l, UINT_MAX);
        ಆದರೆ(rc)
          ಹಿಂತಿರುಗಿ HOST_NAME;
      }
      ಇಲ್ಲದಿದ್ದರೆ
        rc = curlx_str_octal(&c, &l, UINT_MAX);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      rc = curlx_str_ಸಂಖ್ಯೆ(&c, &l, UINT_MAX);

    ಆದರೆ(rc) {
      ಆದರೆ(!n || (rc != STRE_NO_ಅಂಕೆ) || *c)
        ಹಿಂತಿರುಗಿ HOST_NAME;
      n--;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      parts[n] = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)l;

    ಆಯ್ಕೆ(*c) {
    ಸ್ಥಿತಿ '.':
      ಆದರೆ(n == 3) {
        ಆದರೆ(c[1])
          /* something follows this dot */
          ಹಿಂತಿರುಗಿ HOST_NAME;
        ಮುಗಿದ = TRUE;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        n++;
        c++;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ '\0':
      ಮುಗಿದ = TRUE;
      ನಿಲ್ಲಿಸು;

    ಪೂರ್ವನಿಯೋಜಿತ:
      ಹಿಂತಿರುಗಿ HOST_NAME;
    }
  }

  ಆಯ್ಕೆ(n) {
  ಸ್ಥಿತಿ 0: /* a -- 32 bits */
    curlx_dyn_reset(host);

    ಫಲಿತಾಂಶ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            (parts[0] >> 24),
                            ((parts[0] >> 16) & 0xff),
                            ((parts[0] >> 8) & 0xff),
                            (parts[0] & 0xff));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 1: /* a.b -- 8.24 bits */
    ಆದರೆ((parts[0] > 0xff) || (parts[1] > 0xffffff))
      ಹಿಂತಿರುಗಿ HOST_NAME;
    curlx_dyn_reset(host);
    ಫಲಿತಾಂಶ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            ((parts[1] >> 16) & 0xff),
                            ((parts[1] >> 8) & 0xff),
                            (parts[1] & 0xff));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 2: /* a.b.c -- 8.8.16 bits */
    ಆದರೆ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xffff))
      ಹಿಂತಿರುಗಿ HOST_NAME;
    curlx_dyn_reset(host);
    ಫಲಿತಾಂಶ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            ((parts[2] >> 8) & 0xff),
                            (parts[2] & 0xff));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 3: /* a.b.c.d -- 8.8.8.8 bits */
    ಆದರೆ((parts[0] > 0xff) || (parts[1] > 0xff) || (parts[2] > 0xff) ||
       (parts[3] > 0xff))
      ಹಿಂತಿರುಗಿ HOST_NAME;
    curlx_dyn_reset(host);
    ಫಲಿತಾಂಶ = curlx_dyn_addf(host, "%u.%u.%u.%u",
                            parts[0],
                            parts[1],
                            parts[2],
                            parts[3]);
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ HOST_ERROR;
  ಹಿಂತಿರುಗಿ HOST_IPV4;
}

/* if necessary, replace the host content with a URL decoded version */
ಸ್ಥಿರ CURLUcode urldecode_host(ರಚನೆ dynbuf *host)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಪ್ರತಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = curlx_dyn_ptr(host);
  ಪ್ರತಿ = memchr(hostname, '%', curlx_dyn_len(host));
  ಆದರೆ(!ಪ್ರತಿ)
    /* nothing to decode */
    ಹಿಂತಿರುಗಿ CURLUE_OK;
  ಇಲ್ಲದಿದ್ದರೆ {
    /* encoded */
    size_t dlen;
    ಅಕ್ಷರ *decoded;
    CURLcode ಫಲಿತಾಂಶ = Curl_urldecode(hostname, 0, &decoded, &dlen,
                                     REJECT_CTRL);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ CURLUE_BAD_HOSTNAME;
    curlx_dyn_reset(host);
    ಫಲಿತಾಂಶ = curlx_dyn_addn(host, decoded, dlen);
    curlx_free(decoded);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ cc2cu(ಫಲಿತಾಂಶ);
  }

  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode parse_authority(ರಚನೆ Curl_URL *u,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *auth, size_t authlen,
                                 ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                                 ರಚನೆ dynbuf *host,
                                 bool has_scheme)
{
  size_t ಸ್ಥಾನಾಂತರ;
  CURLUcode uc;
  CURLcode ಫಲಿತಾಂಶ;

  /*
   * Parse the login details and strip them out of the hostname.
   */
  uc = parse_hostname_login(u, auth, authlen, flags, &ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(uc)
    ಹಿಂತಿರುಗಿ uc;

  ಫಲಿತಾಂಶ = curlx_dyn_addn(host, auth + ಸ್ಥಾನಾಂತರ, authlen - ಸ್ಥಾನಾಂತರ);
  ಆದರೆ(ಫಲಿತಾಂಶ) {
    uc = cc2cu(ಫಲಿತಾಂಶ);
    ಹಿಂತಿರುಗಿ uc;
  }

  /* parse_port() also sets the hostname length correctly */
  uc = parse_ಪೋರ್ಟ್(u, host, has_scheme);

  ಆದರೆ(!curlx_dyn_len(host))
    /* this makes no-host errors override port number problems */
    uc = CURLUE_NO_HOST;
  ಆದರೆ(!uc)
    uc = urldecode_host(host);
  ಆದರೆ(uc)
    ;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(auth[ಸ್ಥಾನಾಂತರ] == '[')
    uc = ipv6_parse(u, curlx_dyn_ptr(host), curlx_dyn_len(host));
  ಇಲ್ಲದಿದ್ದರೆ {
    /* ipv4_normalize() returns *NAME, *IPV4 or *ERROR */
    ಪೂರ್ಣಾಂಕ type = ipv4_normalize(host);

    ಆದರೆ(type == HOST_NAME)
      uc = hostname_check(curlx_dyn_ptr(host), curlx_dyn_len(host));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(type == HOST_ERROR)
      uc = CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }

  ಹಿಂತಿರುಗಿ uc;
}

/* used for HTTP/2 server push */
CURLUcode Curl_url_ಗಣ_authority(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *authority)
{
  CURLUcode ures;
  ರಚನೆ dynbuf host;

  DEBUGASSERT(authority);
  curlx_dyn_init(&host, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

  ures = parse_authority(u, authority, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(authority),
                         CURLU_DISALLOW_USER, &host, !!u->scheme);
  ಆದರೆ(ures)
    curlx_dyn_free(&host);
  ಇಲ್ಲದಿದ್ದರೆ {
    curlx_free(u->host);
    u->host = curlx_dyn_ptr(&host);
  }
  ಹಿಂತಿರುಗಿ ures;
}

/*
 * "Remove Dot Segments"
 * https://datatracker.ietf.org/doc/html/rfc3986#section-5.2.4
 */

ಸ್ಥಿರ bool is_dot(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **str, size_t *clen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = *str;
  ಆದರೆ(*p == '.') {
    (*str)++;
    (*clen)--;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((*clen >= 3) &&
          (p[0] == '%') && (p[1] == '2') && ((p[2] | 0x20) == 'e')) {
    *str += 3;
    *clen -= 3;
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು ISSLASH(x) ((x) == '/')

/* prescan the string to see if it needs work */
ಸ್ಥಿರ bool needs_dedotdot(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p, size_t pn)
{
  /* a single byte path cannot be cleaned up */
  ಆದರೆ(pn < 2)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(!memchr(p, '.', pn) && !memchr(p, '%', pn))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆಗ(pn) {
    ಆದರೆ(is_dot(&p, &pn)) {
      /* "./" or dot before end of string */
      ಆದರೆ(!pn || ISSLASH(*p))
        ಹಿಂತಿರುಗಿ TRUE;
      /* "../" or ".." before end of string */
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_dot(&p, &pn) && (!pn || ISSLASH(*p)))
        ಹಿಂತಿರುಗಿ TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      p++;
      pn--;
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
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
UNITTEST ಪೂರ್ಣಾಂಕ dedotdotify(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, size_t clen, ಅಕ್ಷರ **outp);
UNITTEST ಪೂರ್ಣಾಂಕ dedotdotify(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು, size_t clen, ಅಕ್ಷರ **outp)
{
  ರಚನೆ dynbuf ಹೊರಹೋಗು;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* variables for leading dot checks */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dinput = ಒಳಹರಿವು;
  size_t dlen = clen;

  *outp = ಖಾಲಿ;
  ಆದರೆ(!needs_dedotdot(ಒಳಹರಿವು, clen))
    ಹಿಂತಿರುಗಿ 0;

  curlx_dyn_init(&ಹೊರಹೋಗು, clen + 1);

  /* if the input buffer begins with a prefix of "../" or "./", then remove
     that prefix from the input buffer; otherwise, */
  ಆದರೆ(is_dot(&dinput, &dlen)) {
    ಆದರೆ(ISSLASH(*dinput)) {
      /* one dot followed by a slash */
      ಒಳಹರಿವು = dinput + 1;
      clen = dlen - 1;
    }

    /* if the input buffer consists only of "." or "..", then remove
       that from the input buffer; otherwise, */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_dot(&dinput, &dlen)) {
      ಆದರೆ(!dlen)
        /* .. [end] */
        ಹೋಗು ಕೊನೆ;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ISSLASH(*dinput)) {
        /* ../ */
        ಒಳಹರಿವು = dinput + 1;
        clen = dlen - 1;
      }
    }
  }

  ಆಗ(clen && !ಫಲಿತಾಂಶ) { /* until end of path content */
    ಆದರೆ(ISSLASH(*ಒಳಹರಿವು)) {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &ಒಳಹರಿವು[1];
      size_t blen = clen - 1;
      /* if the input buffer begins with a prefix of "/./" or "/.", where "."
         is a complete path segment, then replace that prefix with "/" in the
         input buffer; otherwise, */
      ಆದರೆ(is_dot(&p, &blen)) {
        ಆದರೆ(!blen) { /* /. */
          ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, "/", 1);
          ನಿಲ್ಲಿಸು;
        }
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ISSLASH(*p)) { /* /./ */
          ಒಳಹರಿವು = p;
          clen = blen;
          ಮುಂದುವರಿ;
        }

        /* if the input buffer begins with a prefix of "/../" or "/..", where
           ".." is a complete path segment, then replace that prefix with "/"
           in the input buffer and remove the last segment and its preceding
           "/" (if any) from the output buffer; otherwise, */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(is_dot(&p, &blen) && (ISSLASH(*p) || !blen)) {
          /* remove the last segment from the output buffer */
          size_t len = curlx_dyn_len(&ಹೊರಹೋಗು);
          ಆದರೆ(len) {
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = curlx_dyn_ptr(&ಹೊರಹೋಗು);
            ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಕೊನೆಯ = memrchr(ptr, '/', len);
            ಆದರೆ(ಕೊನೆಯ)
              /* trim the output at the slash */
              curlx_dyn_setlen(&ಹೊರಹೋಗು, ಕೊನೆಯ - ptr);
          }

          ಆದರೆ(blen) { /* /../ */
            ಒಳಹರಿವು = p;
            clen = blen;
            ಮುಂದುವರಿ;
          }
          ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, "/", 1);
          ನಿಲ್ಲಿಸು;
        }
      }
    }

    /* move the first path segment in the input buffer to the end of the
       output buffer, including the initial "/" character (if any) and any
       subsequent characters up to, but not including, the next "/" character
       or the end of the input buffer. */

    ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, ಒಳಹರಿವು, 1);
    ಒಳಹರಿವು++;
    clen--;
  }
ಕೊನೆ:
  ಆದರೆ(!ಫಲಿತಾಂಶ) {
    ಆದರೆ(curlx_dyn_len(&ಹೊರಹೋಗು))
      *outp = curlx_dyn_ptr(&ಹೊರಹೋಗು);
    ಇಲ್ಲದಿದ್ದರೆ {
      *outp = curlx_strdup("");
      ಆದರೆ(!*outp)
        ಹಿಂತಿರುಗಿ 1;
    }
  }
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ ? 1 : 0; /* success */
}

/*
 * @unittest 1675
 */
UNITTEST CURLUcode parse_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, size_t urllen, CURLU *u,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pathp, size_t *pathlenp);
UNITTEST CURLUcode parse_file(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, size_t urllen, CURLU *u,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **pathp, size_t *pathlenp)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;
  size_t pathlen;

  *pathp = ಖಾಲಿ;
  *pathlenp = 0;
  ಆದರೆ(urllen <= 6)
    /* file:/ is not enough to actually be a complete file: URL */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_FILE_URL;

  /* path has been allocated large enough to hold this */
  ಮಾರ್ಗ = &url[5];
  pathlen = urllen - 5;

  /* RFC 8089: file-hier-part = ( "//" auth-path ) / local-path, where
     local-path also starts with a "/". So reject anything that does not
     start with at least one "/" */
  ಆದರೆ(ಮಾರ್ಗ[0] != '/')
    ಹಿಂತಿರುಗಿ CURLUE_BAD_FILE_URL;

  /* Extra handling URLs with an authority component (i.e. that start with
   * "file://")
   *
   * We allow omitted hostname (e.g. file:/<path>) -- valid according to
   * RFC 8089, but not the (current) WHAT-WG URL spec.
   */
  ಆದರೆ(ಮಾರ್ಗ[1] == '/') {
    /* swallow the two slashes */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr = &ಮಾರ್ಗ[2];

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
    ಆದರೆ(ptr[0] != '/' && !STARTS_WITH_URL_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(ptr)) {
      /* the URL includes a hostname, it must match "localhost" or
         "127.0.0.1" to be valid */
      ಆದರೆ(checkprefix("localhost/", ptr) ||
         checkprefix("127.0.0.1/", ptr)) {
        ptr += 9; /* now points to the slash after the host */
      }
      ಇಲ್ಲದಿದ್ದರೆ
        /* Invalid file://hostname/, expected localhost or 127.0.0.1 or
           none */
        ಹಿಂತಿರುಗಿ CURLUE_BAD_FILE_URL;
    }

    ಮಾರ್ಗ = ptr;
    pathlen = urllen - (ptr - url);
  }

#ಆದರೆ !defined(_WIN32) && !defined(MSDOS) && !defined(__CYGWIN__)
  /* Do not allow Windows drive letters when not in Windows.
   * This catches both "file:/c:" and "file:c:" */
  ಆದರೆ(('/' == ಮಾರ್ಗ[0] && STARTS_WITH_URL_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(&ಮಾರ್ಗ[1])) ||
     STARTS_WITH_URL_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(ಮಾರ್ಗ)) {
    /* File drive letters are only accepted in MS-DOS/Windows */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_FILE_URL;
  }
#ಇಲ್ಲದಿದ್ದರೆ
  /* If the path starts with a slash and a drive letter, ditch the slash */
  ಆದರೆ('/' == ಮಾರ್ಗ[0] && STARTS_WITH_URL_DRIVE_ಪೂರ್ವಪ್ರತ್ಯಯ(&ಮಾರ್ಗ[1])) {
    /* This cannot be done with strcpy, as the memory chunks overlap! */
    ಮಾರ್ಗ++;
    pathlen--;
  }
#ಆದರೆ_ಅಂತ್ಯ
  u->scheme = curlx_strdup("file");
  ಆದರೆ(!u->scheme)
    ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  *pathp = ಮಾರ್ಗ;
  *pathlenp = pathlen;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode parse_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, CURLU *u, ಅಕ್ಷರ *schemebuf,
                              size_t schemelen, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                              ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ **hostpp)
{
  /* clear path */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *schemep = ಖಾಲಿ;

  ಆದರೆ(schemelen) {
    ಪೂರ್ಣಾಂಕ ಅಂಕೆ_slashes = 0;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = &url[schemelen + 1];
    ಆದರೆ(!Curl_getn_scheme(schemebuf, schemelen) &&
       !(flags & CURLU_NON_SUPPORT_SCHEME))
      ಹಿಂತಿರುಗಿ CURLUE_UNSUPPORTED_SCHEME;

    ಆದರೆ(!ISSLASH(*p))
      /* less than one */
      ಹಿಂತಿರುಗಿ CURLUE_BAD_SLASHES;
    ಆದರೆ((flags & CURLU_NO_AUTHORITY)) {
      ಆಗ(ISSLASH(*p) && (ಅಂಕೆ_slashes < 2)) {
        p++;
        ಅಂಕೆ_slashes++;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆಗ(ISSLASH(*p) && (ಅಂಕೆ_slashes < 4)) {
        p++;
        ಅಂಕೆ_slashes++;
      }
      ಆದರೆ(ಅಂಕೆ_slashes > 3)
        ಹಿಂತಿರುಗಿ CURLUE_BAD_SLASHES;
    }

    schemep = schemebuf;
    *hostpp = p; /* hostname starts here */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* no scheme! */

    ಆದರೆ(!(flags & (CURLU_DEFAULT_SCHEME | CURLU_GUESS_SCHEME)))
      ಹಿಂತಿರುಗಿ CURLUE_BAD_SCHEME;

    ಆದರೆ(flags & CURLU_DEFAULT_SCHEME)
      schemep = DEFAULT_SCHEME;

    /*
     * The URL was badly formatted, let's try without scheme specified.
     */
    *hostpp = url;
  }

  ಆದರೆ(schemep) {
    u->scheme = curlx_strdup(schemep);
    ಆದರೆ(!u->scheme)
      ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode guess_scheme(CURLU *u, ರಚನೆ dynbuf *host)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname = curlx_dyn_ptr(host);
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *schemep = ಖಾಲಿ;
  /* legacy curl-style guess based on hostname */
  ಆದರೆ(checkprefix("ftp.", hostname))
    schemep = "ftp";
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("dict.", hostname))
    schemep = "dict";
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("ldap.", hostname))
    schemep = "ldap";
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("imap.", hostname))
    schemep = "imap";
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("smtp.", hostname))
    schemep = "smtp";
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(checkprefix("pop3.", hostname))
    schemep = "pop3";
  ಇಲ್ಲದಿದ್ದರೆ
    schemep = "http";

  u->scheme = curlx_strdup(schemep);
  ಆದರೆ(!u->scheme)
    ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  u->guessed_scheme = TRUE;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode handle_fragment(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fragment,
                                 size_t fraglen, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  CURLUcode ures;
  u->fragment_present = TRUE;
  ಆದರೆ(fraglen > 1) {
    /* skip the leading '#' in the copy but include the null-terminator */
    ಆದರೆ(flags & CURLU_URLENCODE) {
      ರಚನೆ dynbuf enc;
      curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
      ures = urlencode_str(&enc, fragment + 1, fraglen - 1, TRUE, QUERY_NO);
      ಆದರೆ(ures)
        ಹಿಂತಿರುಗಿ ures;
      u->fragment = curlx_dyn_ptr(&enc);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(badoctets(fragment, fraglen, flags))
        ಹಿಂತಿರುಗಿ CURLUE_BAD_FRAGMENT;
      u->fragment = curlx_memdup0(fragment + 1, fraglen - 1);
      ಆದರೆ(!u->fragment)
        ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode handle_query(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query,
                              size_t qlen, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  u->query_present = TRUE;
  ಆದರೆ(qlen > 1) {
    ಆದರೆ(flags & CURLU_URLENCODE) {
      ರಚನೆ dynbuf enc;
      CURLUcode ures;
      curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
      /* skip the leading question mark */
      ures = urlencode_str(&enc, query + 1, qlen - 1, TRUE, QUERY_YES);
      ಆದರೆ(ures)
        ಹಿಂತಿರುಗಿ ures;
      u->query = curlx_dyn_ptr(&enc);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಆದರೆ(badoctets(query, qlen, flags))
        ಹಿಂತಿರುಗಿ CURLUE_BAD_QUERY;

      u->query = curlx_memdup0(query + 1, qlen - 1);
      ಆದರೆ(!u->query)
        ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* single byte query */
    u->query = curlx_strdup("");
    ಆದರೆ(!u->query)
      ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode handle_ಮಾರ್ಗ(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ,
                             size_t pathlen, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags,
                             bool is_file)
{
  CURLUcode ures;
  ಆದರೆ(pathlen && (flags & CURLU_URLENCODE)) {
    ರಚನೆ dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
    ures = urlencode_str(&enc, ಮಾರ್ಗ, pathlen, TRUE, QUERY_NO);
    ಆದರೆ(ures)
      ಹಿಂತಿರುಗಿ ures;
    pathlen = curlx_dyn_len(&enc);
    ಮಾರ್ಗ = u->ಮಾರ್ಗ = curlx_dyn_ptr(&enc);
  }

  ಆದರೆ(pathlen >= (size_t)(1 + !is_file)) {
    ಆದರೆ(badoctets(ಮಾರ್ಗ, pathlen, flags))
      ಹಿಂತಿರುಗಿ CURLUE_BAD_ಮಾರ್ಗ;

    /* paths for file:// scheme can be one byte, others need to be two */
    ಆದರೆ(!u->ಮಾರ್ಗ) {
      u->ಮಾರ್ಗ = curlx_memdup0(ಮಾರ್ಗ, pathlen);
      ಆದರೆ(!u->ಮಾರ್ಗ)
        ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಮಾರ್ಗ = u->ಮಾರ್ಗ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & CURLU_URLENCODE)
      /* it might have encoded more than the path so cut it */
      u->ಮಾರ್ಗ[pathlen] = 0;

    ಆದರೆ(!(flags & CURLU_ಮಾರ್ಗ_AS_IS)) {
      /* remove ../ and ./ sequences according to RFC3986 */
      ಅಕ್ಷರ *dedot;
      ಪೂರ್ಣಾಂಕ err = dedotdotify(ಮಾರ್ಗ, pathlen, &dedot);
      ಆದರೆ(err)
        ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಆದರೆ(dedot) {
        curlx_free(u->ಮಾರ್ಗ);
        u->ಮಾರ್ಗ = dedot;
      }
    }
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode parseurl(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, CURLU *u, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಮಾರ್ಗ;
  size_t pathlen;
  ಅಕ್ಷರ schemebuf[MAX_SCHEME_LEN + 1];
  size_t schemelen = 0;
  size_t urllen;
  CURLUcode ures = CURLUE_OK;
  ರಚನೆ dynbuf host;
  bool is_file = FALSE;

  DEBUGASSERT(url);

  urllen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(url);
  ಆದರೆ(urllen > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
    ಹಿಂತಿರುಗಿ CURLUE_MALFORMED_ಒಳಹರಿವು;

  curlx_dyn_init(&host, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

  schemelen = Curl_is_absolute_url(url, schemebuf, ಗಾತ್ರ(schemebuf),
                                   flags & (CURLU_GUESS_SCHEME |
                                            CURLU_DEFAULT_SCHEME));

  /* handle the file: scheme */
  ಆದರೆ(schemelen == 4 && !memcmp(schemebuf, "file", 4)) {
    is_file = TRUE;
    ures = parse_file(url, urllen, u, &ಮಾರ್ಗ, &pathlen);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostp = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p;
    size_t hostlen;
    ures = parse_scheme(url, u, schemebuf, schemelen, flags, &hostp);
    ಆದರೆ(ures)
      ಹೋಗು fail;

    /* find the end of the hostname + port number */
    p = hostp;
    ಆಗ(*p && *p != '/' && *p != '?' && *p != '#')
      p++;
    hostlen = p - hostp;
    ಮಾರ್ಗ = p;

    /* this pathlen also contains the query and the fragment */
    pathlen = urllen - (ಮಾರ್ಗ - url);
    ಆದರೆ(hostlen) {
      ures = parse_authority(u, hostp, hostlen, flags, &host, !!u->scheme);
      ಆದರೆ(!ures && (flags & CURLU_GUESS_SCHEME) && !u->scheme)
        ures = guess_scheme(u, &host);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & CURLU_NO_AUTHORITY) {
      /* allowed to be empty. */
      ಆದರೆ(curlx_dyn_ಕೂಡಿಸು(&host, ""))
        ures = CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ures = CURLUE_NO_HOST;
  }
  ಆದರೆ(!ures) {
    /* The path might at this point contain a fragment and/or a query to
       handle */
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fragment = memchr(ಮಾರ್ಗ, '#', pathlen);
    ಆದರೆ(fragment) {
      size_t fraglen = pathlen - (fragment - ಮಾರ್ಗ);
      ures = handle_fragment(u, fragment, fraglen, flags);
      /* after this, pathlen still contains the query */
      pathlen -= fraglen;
    }
  }
  ಆದರೆ(!ures) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *query = memchr(ಮಾರ್ಗ, '?', pathlen);
    ಆದರೆ(query) {
      size_t qlen = pathlen - (query - ಮಾರ್ಗ);
      ures = handle_query(u, query, qlen, flags);
      pathlen -= qlen;
    }
  }
  ಆದರೆ(!ures)
    /* the fragment and query parts are trimmed off from the path */
    ures = handle_ಮಾರ್ಗ(u, ಮಾರ್ಗ, pathlen, flags, is_file);
  ಆದರೆ(!ures) {
    u->host = curlx_dyn_ptr(&host);
    ಹಿಂತಿರುಗಿ CURLUE_OK;
  }
fail:
  curlx_dyn_free(&host);
  free_urlhandle(u);
  ಹಿಂತಿರುಗಿ ures;
}

/*
 * Parse the URL and, if successful, replace everything in the Curl_URL struct.
 */
ಸ್ಥಿರ CURLUcode parseurl_and_replace(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, CURLU *u,
                                      ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  CURLUcode ures;
  CURLU tmpurl;
  memset(&tmpurl, 0, ಗಾತ್ರ(tmpurl));
  ures = parseurl(url, &tmpurl, flags);
  ಆದರೆ(!ures) {
    free_urlhandle(u);
    *u = tmpurl;
  }
  ಹಿಂತಿರುಗಿ ures;
}

/*
 * Concatenate a relative URL onto a base URL making it absolute.
 */
ಸ್ಥಿರ CURLUcode redirect_url(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ತಳ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *relurl,
                              CURLU *u, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  ರಚನೆ dynbuf urlbuf;
  bool host_ಬದಲಾಯಿತು = FALSE;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *useurl = relurl;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *cutoff = ಖಾಲಿ;
  size_t prelen;
  CURLUcode uc;
  /* this can get here with a NULL u->scheme only if asked to use the default
     scheme, so allow fallback to that */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme = u->scheme ? u->scheme : DEFAULT_SCHEME;

  /* protsep points to the start of the hostname, after [scheme]:// */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *protsep = ತಳ + ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(scheme) + 3;
  DEBUGASSERT(ತಳ && relurl && u); /* all set here */
  ಆದರೆ(!ತಳ)
    ಹಿಂತಿರುಗಿ CURLUE_MALFORMED_ಒಳಹರಿವು; /* should never happen */

  /* handle different relative URL types */
  ಆಯ್ಕೆ(relurl[0]) {
  ಸ್ಥಿತಿ '/':
    ಆದರೆ(relurl[1] == '/') {
      /* protocol-relative URL: //example.com/path */
      cutoff = protsep;
      useurl = &relurl[2];
      host_ಬದಲಾಯಿತು = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      /* absolute /path */
      cutoff = strchr(protsep, '/');
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ '#':
    /* fragment-only change */
    ಆದರೆ(u->fragment_present)
      cutoff = strchr(protsep, '#');
    ನಿಲ್ಲಿಸು;

  ಪೂರ್ವನಿಯೋಜಿತ:
    /* path or query-only change */
    ಆದರೆ(u->query_present)
      /* remove existing query */
      cutoff = strchr(protsep, '?');
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(u->fragment_present)
      /* Remove existing fragment */
      cutoff = strchr(protsep, '#');

    ಆದರೆ(relurl[0] != '?') {
      /* append a relative path after the last slash */
      cutoff = memrchr(protsep, '/',
                       cutoff ? (size_t)(cutoff - protsep) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(protsep));
      ಆದರೆ(cutoff)
        cutoff++; /* truncate after last slash */
    }
    ನಿಲ್ಲಿಸು;
  }

  prelen = cutoff ? (size_t)(cutoff - ತಳ) : ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ತಳ);

  /* build new URL */
  curlx_dyn_init(&urlbuf, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

  ಆದರೆ(!curlx_dyn_addn(&urlbuf, ತಳ, prelen) &&
     !urlencode_str(&urlbuf, useurl, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(useurl), !host_ಬದಲಾಯಿತು,
                    QUERY_NOT_YET)) {
    uc = parseurl_and_replace(curlx_dyn_ptr(&urlbuf), u,
                              flags & ~U_CURLU_ಮಾರ್ಗ_AS_IS);
  }
  ಇಲ್ಲದಿದ್ದರೆ
    uc = CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  curlx_dyn_free(&urlbuf);
  ಹಿಂತಿರುಗಿ uc;
}

/*
 */
CURLU *curl_url(ಶೂನ್ಯ)
{
  ಹಿಂತಿರುಗಿ curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_URL));
}

ಶೂನ್ಯ curl_url_cleanup(CURLU *u)
{
  ಆದರೆ(u) {
    free_urlhandle(u);
    curlx_free(u);
  }
}

#ವ್ಯಾಖ್ಯಾನಿಸು DUP(dest, src, name)                    \
  ಮಾಡು {                                          \
    ಆದರೆ((src)->name) {                           \
      (dest)->name = curlx_strdup((src)->name); \
      ಆದರೆ(!(dest)->name)                         \
        ಹೋಗು fail;                              \
    }                                           \
  } ಆಗ(0)

CURLU *curl_url_dup(ಸ್ಥಿರಾಂಕ CURLU *ಒಳ)
{
  ರಚನೆ Curl_URL *u;

  ಆದರೆ(!ಒಳ)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  u = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ Curl_URL));
  ಆದರೆ(u) {
    DUP(u, ಒಳ, scheme);
    DUP(u, ಒಳ, user);
    DUP(u, ಒಳ, password);
    DUP(u, ಒಳ, options);
    DUP(u, ಒಳ, host);
    DUP(u, ಒಳ, ಮಾರ್ಗ);
    DUP(u, ಒಳ, query);
    DUP(u, ಒಳ, fragment);
    DUP(u, ಒಳ, zoneid);
    u->portnum = ಒಳ->portnum;
    u->ಪೋರ್ಟ್_present = ಒಳ->ಪೋರ್ಟ್_present;
    u->fragment_present = ಒಳ->fragment_present;
    u->query_present = ಒಳ->query_present;
    u->guessed_scheme = ಒಳ->guessed_scheme;
  }
  ಹಿಂತಿರುಗಿ u;
fail:
  curl_url_cleanup(u);
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_IDN
#ವ್ಯಾಖ್ಯಾನಿಸು host_decode(x, y) CURLUE_LACKS_IDN
#ವ್ಯಾಖ್ಯಾನಿಸು host_encode(x, y) CURLUE_LACKS_IDN
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ CURLUcode host_decode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host, ಅಕ್ಷರ **allochost)
{
  CURLcode ಫಲಿತಾಂಶ = Curl_idn_decode(host, allochost);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) ?
      CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : CURLUE_BAD_HOSTNAME;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode host_encode(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *host, ಅಕ್ಷರ **allochost)
{
  CURLcode ಫಲಿತಾಂಶ = Curl_idn_encode(host, allochost);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ == CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ) ?
      CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ : CURLUE_BAD_HOSTNAME;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ CURLUcode urlget_format(ಸ್ಥಿರಾಂಕ CURLU *u, CURLUPart what,
                               ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr, ಅಕ್ಷರ **partp,
                               bool plusdecode, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  CURLUcode uc = CURLUE_OK;
  size_t partlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(ptr);
  bool urldecode = (flags & CURLU_URLDECODE) ? 1 : 0;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool punycode = (flags & CURLU_PUNYCODE) && (what == CURLUPART_HOST);
  bool depunyfy = (flags & CURLU_PUNY2IDN) && (what == CURLUPART_HOST);
  ಅಕ್ಷರ *part = curlx_memdup0(ptr, partlen);
  *partp = ಖಾಲಿ;
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  ಆದರೆ(plusdecode) {
    /* convert + to space */
    ಅಕ್ಷರ *plus = part;
    size_t i = 0;
    ಗಾಗಿ(i = 0; i < partlen; ++plus, i++) {
      ಆದರೆ(*plus == '+')
        *plus = ' ';
    }
  }
  ಆದರೆ(urldecode) {
    ಅಕ್ಷರ *decoded;
    size_t dlen;
    /* this unconditional rejection of control bytes is documented API
       behavior */
    CURLcode ಫಲಿತಾಂಶ = Curl_urldecode(part, partlen, &decoded, &dlen,
                                     REJECT_CTRL);
    curlx_free(part);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ CURLUE_URLDECODE;
    part = decoded;
    partlen = dlen;
  }
  ಆದರೆ(urlencode) {
    ರಚನೆ dynbuf enc;
    curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
    uc = urlencode_str(&enc, part, partlen, TRUE, what == CURLUPART_QUERY ?
                       QUERY_YES : QUERY_NO);
    curlx_free(part);
    ಆದರೆ(uc)
      ಹಿಂತಿರುಗಿ uc;
    part = curlx_dyn_ptr(&enc);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(punycode) {
    ಆದರೆ(!Curl_is_ASCII_name(u->host)) {
      ಅಕ್ಷರ *punyversion = ಖಾಲಿ;
      uc = host_decode(part, &punyversion);
      curlx_free(part);
      ಆದರೆ(uc)
        ಹಿಂತಿರುಗಿ uc;
      uc = hostname_check(punyversion, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(punyversion));
      ಆದರೆ(uc) {
        curlx_free(punyversion);
        ಹಿಂತಿರುಗಿ uc;
      }
      part = punyversion;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(depunyfy && Curl_is_ASCII_name(u->host)) {
    ಅಕ್ಷರ *unpunified = ಖಾಲಿ;
    uc = host_encode(part, &unpunified);
    curlx_free(part);
    ಆದರೆ(uc)
      ಹಿಂತಿರುಗಿ uc;
    part = unpunified;
  }
  *partp = part;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode file_url(ಸ್ಥಿರಾಂಕ CURLU *u, ಅಕ್ಷರ **part,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fragmentsep,
                          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *querysep)
{
  ಅಕ್ಷರ *url = curl_maprintf("file://%s%s%s%s%s",
                            u->ಮಾರ್ಗ, querysep, u->query ? u->query : "",
                            fragmentsep, u->fragment ? u->fragment : "");
  ಆದರೆ(!url)
    ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  *part = url;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode urlget_url(ಸ್ಥಿರಾಂಕ CURLU *u, ಅಕ್ಷರ **part, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  ಅಕ್ಷರ *url;
  ಅಕ್ಷರ *allochost = ಖಾಲಿ;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fragmentsep =
    (u->fragment || (u->fragment_present && flags & CURLU_GET_ಖಾಲಿಗಳು)) ?
    "#" : "";
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *querysep = ((u->query && u->query[0]) ||
                          (u->query_present && flags & CURLU_GET_ಖಾಲಿಗಳು)) ?
    "?" : "";
  ಅಕ್ಷರ portbuf[7];
  ಆದರೆ(curl_strequal("file", u->scheme))
    ಹಿಂತಿರುಗಿ file_url(u, part, fragmentsep, querysep);
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!u->host)
    ಹಿಂತಿರುಗಿ CURLUE_NO_HOST;
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme;
    ಅಕ್ಷರ *options = u->options;
    ಅಕ್ಷರ *ಪೋರ್ಟ್ = ಖಾಲಿ;
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *h = ಖಾಲಿ;
    ಅಕ್ಷರ schemebuf[MAX_SCHEME_LEN + 5];
    ಆದರೆ(u->scheme)
      scheme = u->scheme;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & CURLU_DEFAULT_SCHEME)
      scheme = DEFAULT_SCHEME;
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLUE_NO_SCHEME;

    ಆದರೆ(u->ಪೋರ್ಟ್_present) {
      curl_msnprintf(portbuf, ಗಾತ್ರ(portbuf), "%u", u->portnum);
      ಪೋರ್ಟ್ = portbuf;
    }

    h = Curl_get_scheme(scheme);
    ಆದರೆ(h) {
      ಆದರೆ(!u->ಪೋರ್ಟ್_present && (flags & CURLU_DEFAULT_ಪೋರ್ಟ್)) {
        /* there is no stored port number, but asked to deliver a default one
           for the scheme */
        curl_msnprintf(portbuf, ಗಾತ್ರ(portbuf), "%u", h->defport);
        ಪೋರ್ಟ್ = portbuf;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(u->ಪೋರ್ಟ್_present && (h->defport == u->portnum) &&
              (flags & CURLU_NO_DEFAULT_ಪೋರ್ಟ್)) {
        /* there is a stored port number, but asked to inhibit if it matches
           the default port for the scheme */
        ಪೋರ್ಟ್ = ಖಾಲಿ;
      }

      ಆದರೆ(!(h->flags & PROTOPT_URLOPTIONS))
        options = ಖಾಲಿ;
    }

    ಆದರೆ(u->host[0] == '[') {
      ಆದರೆ(u->zoneid) {
        /* make it '[ host %25 zoneid ]' */
        ರಚನೆ dynbuf enc;
        size_t hostlen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(u->host);
        curlx_dyn_init(&enc, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
        ಆದರೆ(curlx_dyn_addf(&enc, "%.*s%%25%s]", (ಪೂರ್ಣಾಂಕ)hostlen - 1, u->host,
                          u->zoneid))
          ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
        allochost = curlx_dyn_ptr(&enc);
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & CURLU_URLENCODE) {
      allochost = curl_easy_escape(ಖಾಲಿ, u->host, 0);
      ಆದರೆ(!allochost)
        ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & CURLU_PUNYCODE) {
      ಆದರೆ(!Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_decode(u->host, &allochost);
        ಆದರೆ(!ret)
          ret = hostname_check(allochost, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(allochost));
        ಆದರೆ(ret) {
          curlx_safefree(allochost);
          ಹಿಂತಿರುಗಿ ret;
        }
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(flags & CURLU_PUNY2IDN) {
      ಆದರೆ(Curl_is_ASCII_name(u->host)) {
        CURLUcode ret = host_encode(u->host, &allochost);
        ಆದರೆ(ret)
          ಹಿಂತಿರುಗಿ ret;
      }
    }

    ಆದರೆ(!(flags & CURLU_NO_GUESS_SCHEME) || !u->guessed_scheme)
      curl_msnprintf(schemebuf, ಗಾತ್ರ(schemebuf), "%s://", scheme);
    ಇಲ್ಲದಿದ್ದರೆ
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
                        ಪೋರ್ಟ್ ? ":" : "",
                        ಪೋರ್ಟ್ ? ಪೋರ್ಟ್ : "",
                        u->ಮಾರ್ಗ ? u->ಮಾರ್ಗ : "/",
                        querysep,
                        u->query ? u->query : "",
                        fragmentsep,
                        u->fragment ? u->fragment : "");
    curlx_free(allochost);
  }
  ಆದರೆ(!url)
    ಹಿಂತಿರುಗಿ CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
  *part = url;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

CURLUcode curl_url_get(ಸ್ಥಿರಾಂಕ CURLU *u, CURLUPart what,
                       ಅಕ್ಷರ **part, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;
  CURLUcode ifmissing = CURLUE_UNKNOWN_PART;
  ಅಕ್ಷರ portbuf[7];
  bool plusdecode = FALSE;
  ಆದರೆ(!u)
    ಹಿಂತಿರುಗಿ CURLUE_BAD_HANDLE;
  ಆದರೆ(!part)
    ಹಿಂತಿರುಗಿ CURLUE_BAD_PARTPOINTER;
  *part = ಖಾಲಿ;

  ಆಯ್ಕೆ(what) {
  ಸ್ಥಿತಿ CURLUPART_SCHEME:
    ptr = u->scheme;
    ifmissing = CURLUE_NO_SCHEME;
    flags &= ~U_CURLU_URLDECODE; /* never for schemes */
    ಆದರೆ((flags & CURLU_NO_GUESS_SCHEME) && u->guessed_scheme)
      ಹಿಂತಿರುಗಿ CURLUE_NO_SCHEME;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_USER:
    ptr = u->user;
    ifmissing = CURLUE_NO_USER;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_PASSWORD:
    ptr = u->password;
    ifmissing = CURLUE_NO_PASSWORD;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_OPTIONS:
    ptr = u->options;
    ifmissing = CURLUE_NO_OPTIONS;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_HOST:
    ptr = u->host;
    ifmissing = CURLUE_NO_HOST;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ZONEID:
    ptr = u->zoneid;
    ifmissing = CURLUE_NO_ZONEID;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ಪೋರ್ಟ್:
    ptr = ಖಾಲಿ;
    ifmissing = CURLUE_NO_ಪೋರ್ಟ್;
    flags &= ~U_CURLU_URLDECODE; /* never for port */
    ಆದರೆ(u->ಪೋರ್ಟ್_present) {
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *h = u->scheme ?
                                    Curl_get_scheme(u->scheme) : ಖಾಲಿ;
      /* there is a stored port number, but ask to inhibit if
         it matches the default one for the scheme */
      ಆದರೆ(h && (h->defport == u->portnum) &&
         (flags & CURLU_NO_DEFAULT_ಪೋರ್ಟ್)) {
        ptr = ಖಾಲಿ;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        curl_msnprintf(portbuf, ಗಾತ್ರ(portbuf), "%u", u->portnum);
        ptr = portbuf;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ((flags & CURLU_DEFAULT_ಪೋರ್ಟ್) && u->scheme) {
      /* there is no stored port number, but asked to deliver
         a default one for the scheme */
      ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *h = Curl_get_scheme(u->scheme);
      ಆದರೆ(h) {
        curl_msnprintf(portbuf, ಗಾತ್ರ(portbuf), "%u", h->defport);
        ptr = portbuf;
      }
    }
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ಮಾರ್ಗ:
    ptr = u->ಮಾರ್ಗ;
    ಆದರೆ(!ptr)
      ptr = "/";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_QUERY:
    ptr = u->query;
    ifmissing = CURLUE_NO_QUERY;
    plusdecode = flags & CURLU_URLDECODE;
    ಆದರೆ(ptr && !ptr[0] && !(flags & CURLU_GET_ಖಾಲಿಗಳು))
      /* there was a blank query and the user does not ask for it */
      ptr = ಖಾಲಿ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_FRAGMENT:
    ptr = u->fragment;
    ifmissing = CURLUE_NO_FRAGMENT;
    ಆದರೆ(!ptr && u->fragment_present && flags & CURLU_GET_ಖಾಲಿಗಳು)
      /* there was a blank fragment and the user asks for it */
      ptr = "";
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_URL:
    ಹಿಂತಿರುಗಿ urlget_url(u, part, flags);
  ಪೂರ್ವನಿಯೋಜಿತ:
    ptr = ಖಾಲಿ;
    ನಿಲ್ಲಿಸು;
  }
  ಆದರೆ(ptr)
    ಹಿಂತಿರುಗಿ urlget_format(u, what, ptr, part, plusdecode, flags);

  ಹಿಂತಿರುಗಿ ifmissing;
}

ಸ್ಥಿರ CURLUcode ಗಣ_url_scheme(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme,
                                ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  size_t plen = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(scheme);
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *h = ಖಾಲಿ;
  ಆದರೆ((plen > MAX_SCHEME_LEN) || (plen < 1))
    /* too long or too short */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_SCHEME;
  /* verify that it is a fine scheme */
  h = Curl_get_scheme(scheme);
  ಆದರೆ(!(flags & CURLU_NON_SUPPORT_SCHEME) && (!h || !h->ಸತತ))
    ಹಿಂತಿರುಗಿ CURLUE_UNSUPPORTED_SCHEME;
  ಆದರೆ(!h) {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = scheme;
    ಆದರೆ(ISALPHA(*s)) {
      /* ALPHA *( ALPHA / DIGIT / "+" / "-" / "." ) */
      s++;
      ಆಗ(--plen) {
        ಆದರೆ(ISALNUM(*s) || (*s == '+') || (*s == '-') || (*s == '.'))
          s++; /* fine */
        ಇಲ್ಲದಿದ್ದರೆ
          ಹಿಂತಿರುಗಿ CURLUE_BAD_SCHEME;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ಹಿಂತಿರುಗಿ CURLUE_BAD_SCHEME;
  }
  u->guessed_scheme = FALSE;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode ಗಣ_url_ಪೋರ್ಟ್(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *provided_ಪೋರ್ಟ್)
{
  curl_off_t ಪೋರ್ಟ್;
  ಆದರೆ(!ISDIGIT(provided_ಪೋರ್ಟ್[0]))
    /* not a number */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ;
  ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&provided_ಪೋರ್ಟ್, &ಪೋರ್ಟ್, 0xffff) || *provided_ಪೋರ್ಟ್)
    /* weirdly provided number, not good! */
    ಹಿಂತಿರುಗಿ CURLUE_BAD_ಪೋರ್ಟ್_ಸಂಖ್ಯೆ;
  u->portnum = (uint16_t)ಪೋರ್ಟ್;
  u->ಪೋರ್ಟ್_present = TRUE;
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode ಗಣ_url(CURLU *u, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *url, size_t part_ಗಾತ್ರ,
                         ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  /*
   * Allow a new URL to replace the existing (if any) contents.
   *
   * If the existing contents is enough for a URL, allow a relative URL to
   * replace it.
   */
  CURLUcode uc;
  ಅಕ್ಷರ *oldurl = ಖಾಲಿ;

  ಆದರೆ(!part_ಗಾತ್ರ) {
    /* a blank URL is not a valid URL unless we already have a complete one
       and this is a redirect */
    uc = curl_url_get(u, CURLUPART_URL, &oldurl, flags);
    ಆದರೆ(!uc) {
      /* success, meaning the "" is a fine relative URL, and the new URL
         inherits scheme/authority/path/query, but not fragment, from the
         existing URL (RFC 3986 section 5.2.2) */
      curlx_safefree(u->fragment);
      u->fragment_present = FALSE;
      curlx_free(oldurl);
      ಹಿಂತಿರುಗಿ CURLUE_OK;
    }
    ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
      ಹಿಂತಿರುಗಿ uc;
    ಹಿಂತಿರುಗಿ CURLUE_MALFORMED_ಒಳಹರಿವು;
  }

  /* if the new URL is absolute replace the existing with the new. */
  ಆದರೆ(Curl_is_absolute_url(url, ಖಾಲಿ, 0,
                          flags & (CURLU_GUESS_SCHEME | CURLU_DEFAULT_SCHEME)))
    ಹಿಂತಿರುಗಿ parseurl_and_replace(url, u, flags);

  /* if the old URL is incomplete (we cannot get an absolute URL in
     'oldurl'), replace the existing with the new.
     Always include "scheme://" to make the URL "complete" */
  /* Preserve empty query/fragment separators: they affect where relative
     references splice into the base URL. */
  uc = curl_url_get(u, CURLUPART_URL, &oldurl,
                    (flags & ~CURLU_NO_GUESS_SCHEME) | CURLU_GET_ಖಾಲಿಗಳು);
  ಆದರೆ(uc == CURLUE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ)
    ಹಿಂತಿರುಗಿ uc;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(uc)
    ಹಿಂತಿರುಗಿ parseurl_and_replace(url, u, flags);

  DEBUGASSERT(oldurl); /* it is set here */
  /* apply the relative part to create a new URL */
  uc = redirect_url(oldurl, url, u, flags);
  curlx_free(oldurl);
  ಹಿಂತಿರುಗಿ uc;
}

ಸ್ಥಿರ CURLUcode urlset_clear(CURLU *u, CURLUPart what)
{
  ಆಯ್ಕೆ(what) {
  ಸ್ಥಿತಿ CURLUPART_URL:
    free_urlhandle(u);
    memset(u, 0, ಗಾತ್ರ(ರಚನೆ Curl_URL));
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_SCHEME:
    curlx_safefree(u->scheme);
    u->guessed_scheme = FALSE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_USER:
    curlx_safefree(u->user);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_PASSWORD:
    curlx_strzero(u->password);
    curlx_safefree(u->password);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_OPTIONS:
    curlx_safefree(u->options);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_HOST:
    curlx_safefree(u->host);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ZONEID:
    curlx_safefree(u->zoneid);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ಪೋರ್ಟ್:
    u->portnum = 0;
    u->ಪೋರ್ಟ್_present = FALSE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ಮಾರ್ಗ:
    curlx_safefree(u->ಮಾರ್ಗ);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_QUERY:
    curlx_safefree(u->query);
    u->query_present = FALSE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_FRAGMENT:
    curlx_safefree(u->fragment);
    u->fragment_present = FALSE;
    ನಿಲ್ಲಿಸು;
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLUE_UNKNOWN_PART;
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ bool allowed_ಒಳ_ಮಾರ್ಗ(ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ x)
{
  ಆಯ್ಕೆ(x) {
  ಸ್ಥಿತಿ '!':
  ಸ್ಥಿತಿ '$':
  ಸ್ಥಿತಿ '&':
  ಸ್ಥಿತಿ '\'':
  ಸ್ಥಿತಿ '(':
  ಸ್ಥಿತಿ ')':
  ಸ್ಥಿತಿ '{':
  ಸ್ಥಿತಿ '}':
  ಸ್ಥಿತಿ '[':
  ಸ್ಥಿತಿ ']':
  ಸ್ಥಿತಿ '*':
  ಸ್ಥಿತಿ '+':
  ಸ್ಥಿತಿ ',':
  ಸ್ಥಿತಿ ';':
  ಸ್ಥಿತಿ '=':
  ಸ್ಥಿತಿ ':':
  ಸ್ಥಿತಿ '@':
  ಸ್ಥಿತಿ '/':
    ಹಿಂತಿರುಗಿ TRUE;
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLUcode url_encode_part(ರಚನೆ dynbuf *encp,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *part,
                                 bool plusencode,
                                 bool pathmode,
                                 bool equalsencode)
{
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *i;

  ಗಾಗಿ(i = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)part; *i; i++) {
    CURLcode ಫಲಿತಾಂಶ;
    ಆದರೆ((*i == ' ') && plusencode)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(encp, "+", 1);
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ISUNRESERVED(*i) ||
            (pathmode && allowed_ಒಳ_ಮಾರ್ಗ(*i)) ||
            ((*i == '=') && equalsencode)) {
      ಆದರೆ((*i == '=') && equalsencode)
        /* only skip the first equals sign */
        equalsencode = FALSE;
      ಫಲಿತಾಂಶ = curlx_dyn_addn(encp, i, 1);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ಹೊರಹೋಗು[3] = { '%' };
      Curl_hexbyte(&ಹೊರಹೋಗು[1], *i);
      ಫಲಿತಾಂಶ = curlx_dyn_addn(encp, ಹೊರಹೋಗು, 3);
    }
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ cc2cu(ಫಲಿತಾಂಶ);
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode url_uppercasehex_part(ರಚನೆ dynbuf *encp,
                                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *part)
{
  ಅಕ್ಷರ *p;
  CURLcode ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(encp, part);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ cc2cu(ಫಲಿತಾಂಶ);
  p = curlx_dyn_ptr(encp);
  ಆಗ(*p) {
    /* make sure percent encoded are upper case */
    ಆದರೆ((*p == '%') && ISXDIGIT(p[1]) && ISXDIGIT(p[2]) &&
       (ISLOWER(p[1]) || ISLOWER(p[2]))) {
      p[1] = Curl_raw_toupper(p[1]);
      p[2] = Curl_raw_toupper(p[2]);
      p += 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      p++;
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode url_append_query(CURLU *u, ರಚನೆ dynbuf *encp)
{
  /* Append the 'encp' string onto the old query. Add a '&' separator if none
     is already present at the end of the existing query */

  size_t querylen = u->query ? ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(u->query) : 0;
  bool addamperand = querylen && (u->query[querylen - 1] != '&');
  ಆದರೆ(querylen) {
    ರಚನೆ dynbuf qbuf;
    CURLcode ಫಲಿತಾಂಶ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newp = curlx_dyn_ptr(encp);
    curlx_dyn_init(&qbuf, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);

    /* add original query */
    ಫಲಿತಾಂಶ = curlx_dyn_addn(&qbuf, u->query, querylen);
    ಆದರೆ(!ಫಲಿತಾಂಶ && addamperand)
      /* add ampersand */
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&qbuf, "&", 1);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      /* add new query part */
      ಫಲಿತಾಂಶ = curlx_dyn_ಕೂಡಿಸು(&qbuf, newp);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹೋಗು nomem;
    curlx_dyn_free(encp);
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(&qbuf);
    ಹಿಂತಿರುಗಿ CURLUE_OK;
nomem:
    curlx_dyn_free(encp);
    ಹಿಂತಿರುಗಿ cc2cu(ಫಲಿತಾಂಶ);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    curlx_free(u->query);
    u->query = curlx_dyn_ptr(encp);
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

ಸ್ಥಿರ CURLUcode url_sethost(CURLU *u, ರಚನೆ dynbuf *encp,
                             bool urlencode,
                             ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  size_t n = curlx_dyn_len(encp);
  bool bad = FALSE;
  ಅಕ್ಷರ *newp = curlx_dyn_ptr(encp);
  ಆದರೆ(!n)
    /* an empty hostname is okay if told so */
    bad = (flags & CURLU_NO_AUTHORITY) ? FALSE : TRUE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!urlencode) {
    /* if the hostname part was not URL encoded here, it was set already URL
       encoded so we need to decode it to check */
    size_t dlen;
    ಅಕ್ಷರ *decoded = ಖಾಲಿ;
    CURLcode ಫಲಿತಾಂಶ = Curl_urldecode(newp, n, &decoded, &dlen, REJECT_CTRL);
    ಆದರೆ(ಫಲಿತಾಂಶ || hostname_check6(u, decoded, dlen))
      bad = TRUE;
    ಇಲ್ಲದಿದ್ದರೆ {
      ರಚನೆ dynbuf dbuf;
      curlx_dyn_init(&dbuf, CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ);
      ಆದರೆ(curlx_dyn_addn(&dbuf, decoded, dlen))
        bad = TRUE;
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ipv4_normalize(&dbuf) == HOST_IPV4) {
        curlx_dyn_reset(encp);
        ಆದರೆ(curlx_dyn_addn(encp, curlx_dyn_ptr(&dbuf), curlx_dyn_len(&dbuf)))
          bad = TRUE;
      }
      curlx_dyn_free(&dbuf);
    }
    curlx_free(decoded);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(hostname_check6(u, newp, n))
    bad = TRUE;
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ipv4_normalize(encp) == HOST_ERROR)
    bad = TRUE;
  ಆದರೆ(bad) {
    curlx_dyn_free(encp);
    ಹಿಂತಿರುಗಿ CURLUE_BAD_HOSTNAME;
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

CURLUcode curl_url_ಗಣ(CURLU *u, CURLUPart what,
                       ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *part, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ flags)
{
  ಅಕ್ಷರ **storep = ಖಾಲಿ;
  bool urlencode = (flags & CURLU_URLENCODE) ? 1 : 0;
  bool plusencode = FALSE;
  bool pathmode = FALSE;
  bool leadingslash = FALSE;
  bool appendquery = FALSE;
  bool equalsencode = FALSE;
  size_t nalloc;

  ಆದರೆ(!u)
    ಹಿಂತಿರುಗಿ CURLUE_BAD_HANDLE;
  ಆದರೆ(!part)
    /* setting a part to NULL clears it */
    ಹಿಂತಿರುಗಿ urlset_clear(u, what);

  nalloc = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(part);
  ಆದರೆ(nalloc > CURL_MAX_ಒಳಹರಿವು_ಉದ್ದಳತೆ)
    /* excessive input length */
    ಹಿಂತಿರುಗಿ CURLUE_MALFORMED_ಒಳಹರಿವು;

  ಆಯ್ಕೆ(what) {
  ಸ್ಥಿತಿ CURLUPART_SCHEME: {
    CURLUcode ಸ್ಥಿತಿ_2 = ಗಣ_url_scheme(u, part, flags);
    ಆದರೆ(ಸ್ಥಿತಿ_2)
      ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
    storep = &u->scheme;
    urlencode = FALSE; /* never */
    ನಿಲ್ಲಿಸು;
  }
  ಸ್ಥಿತಿ CURLUPART_USER:
    storep = &u->user;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_PASSWORD:
    storep = &u->password;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_OPTIONS:
    storep = &u->options;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_HOST:
    storep = &u->host;
    curlx_safefree(u->zoneid);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ZONEID:
    storep = &u->zoneid;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_ಪೋರ್ಟ್:
    ಹಿಂತಿರುಗಿ ಗಣ_url_ಪೋರ್ಟ್(u, part);
  ಸ್ಥಿತಿ CURLUPART_ಮಾರ್ಗ:
    pathmode = TRUE;
    leadingslash = TRUE; /* enforce */
    storep = &u->ಮಾರ್ಗ;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_QUERY:
    plusencode = urlencode;
    appendquery = (flags & CURLU_APPENDQUERY) ? 1 : 0;
    equalsencode = appendquery;
    storep = &u->query;
    u->query_present = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_FRAGMENT:
    storep = &u->fragment;
    u->fragment_present = TRUE;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURLUPART_URL:
    ಹಿಂತಿರುಗಿ ಗಣ_url(u, part, nalloc, flags);
  ಪೂರ್ವನಿಯೋಜಿತ:
    ಹಿಂತಿರುಗಿ CURLUE_UNKNOWN_PART;
  }
  DEBUGASSERT(storep);
  {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *newp = ಖಾಲಿ;
    ರಚನೆ dynbuf enc;
    CURLUcode ಸ್ಥಿತಿ_2;
    curlx_dyn_init(&enc, (what == CURLUPART_HOST) ?
                   CURLMAX((nalloc * 3) + 1 + leadingslash, 16) :
                   (nalloc * 3) + 1 + leadingslash);

    ಆದರೆ(leadingslash && (part[0] != '/')) {
      CURLcode ಫಲಿತಾಂಶ = curlx_dyn_addn(&enc, "/", 1);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        ಹಿಂತಿರುಗಿ cc2cu(ಫಲಿತಾಂಶ);
    }
    ಆದರೆ(urlencode)
      ಸ್ಥಿತಿ_2 = url_encode_part(&enc, part, plusencode, pathmode, equalsencode);
    ಇಲ್ಲದಿದ್ದರೆ
      ಸ್ಥಿತಿ_2 = url_uppercasehex_part(&enc, part);
    ಆದರೆ(!ಸ್ಥಿತಿ_2) {
      newp = curlx_dyn_ptr(&enc);

      ಆದರೆ(appendquery && newp)
        ಹಿಂತಿರುಗಿ url_append_query(u, &enc);
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(what == CURLUPART_HOST)
        ಸ್ಥಿತಿ_2 = url_sethost(u, &enc, urlencode, flags);
    }
    ಆದರೆ(ಸ್ಥಿತಿ_2)
      ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;

    ಆದರೆ(what == CURLUPART_PASSWORD)
      curlx_strzero(*storep);
    curlx_free(*storep);
    *storep = (ಅಕ್ಷರ *)CURL_UNCONST(newp);
  }
  ಹಿಂತಿರುಗಿ CURLUE_OK;
}

bool Curl_url_same_origin(CURLU *ತಳ, CURLU *href)
{
  ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *s = ಖಾಲಿ;

  /* base must be an absolute URL */
  ಆದರೆ(!ತಳ->scheme || !ತಳ->host)
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(href->scheme && !curl_strequal(ತಳ->scheme, href->scheme))
    ಹಿಂತಿರುಗಿ FALSE;
  ಆದರೆ(href->host) {
    ಆದರೆ(!curl_strequal(ತಳ->host, href->host))
      ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ(ತಳ->ಪೋರ್ಟ್_present != href->ಪೋರ್ಟ್_present) {
      /* one is present, one is not */
      s = Curl_get_scheme(ತಳ->scheme);
      ಆದರೆ(!s) /* Cannot match default port for unknown scheme */
        ಹಿಂತಿರುಗಿ FALSE;
      /* to match, the present one must be the default port */
      ಆದರೆ((ತಳ->ಪೋರ್ಟ್_present && (ತಳ->portnum != s->defport)) ||
         (href->ಪೋರ್ಟ್_present && (href->portnum != s->defport)))
        ಹಿಂತಿರುಗಿ FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ತಳ->portnum != href->portnum) /* both present or missing */
      ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ(!curl_strequal(ತಳ->zoneid ? ತಳ->zoneid : "",
                      href->zoneid ? href->zoneid : ""))
      ಹಿಂತಿರುಗಿ FALSE;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(href->ಪೋರ್ಟ್_present) /* no host in href, then there must be no port */
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ TRUE;
}

CURLUcode Curl_url_get_ಪೋರ್ಟ್(CURLU *u, uint16_t *pport)
{
  ಆದರೆ(u->ಪೋರ್ಟ್_present) {
    *pport = u->portnum;
    ಹಿಂತಿರುಗಿ CURLUE_OK;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(u->scheme) {
    ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *s = Curl_get_scheme(u->scheme);
    ಆದರೆ(s && s->defport) {
      *pport = s->defport;
      ಹಿಂತಿರುಗಿ CURLUE_OK;
    }
  }
  *pport = 0;
  ಹಿಂತಿರುಗಿ CURLUE_NO_ಪೋರ್ಟ್;
}
