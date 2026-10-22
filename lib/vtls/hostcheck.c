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
#ضمن "curl_setup.h"

#إذا defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETINET_IN6_H
#ضمن <netinet/in6.h>
#جے_انت

#ضمن "curl_memrchr.h"
#ضمن "vdns/hostip.h"
#ضمن "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
ساكن bool pmatch(ثابت حرف *hostname, size_t hostlen,
                   ثابت حرف *نمونہ, size_t patternlen)
{
  إذا(hostlen != patternlen)
    أرجع FALSE;
  أرجع curl_strnequal(hostname, نمونہ, hostlen);
}

/*
 * Match a hostname against a wildcard pattern.
 * E.g.
 *  "foo.host.com" matches "*.host.com".
 *
 * We use the matching rule described in RFC6125, section 6.4.3.
 * https://datatracker.ietf.org/doc/html/rfc6125#section-6.4.3
 *
 * In addition: ignore trailing dots in the hostnames and wildcards, so that
 * the names are used normalized. This is what the browsers do.
 *
 * Do not allow wildcard matching on IP numbers. There are apparently
 * certificates being used with an IP address in the CN field, thus making no
 * apparent distinction between a name and an IP. We need to detect the use of
 * an IP address and not wildcard match on such names.
 *
 * Only match on "*" being used for the leftmost label, not "a*", "a*b" nor
 * "*b".
 *
 * The pattern string does not necessarily need to be null-terminated.
 *
 * Return TRUE on a match. FALSE if not.
 *
 * @unittest: 1397
 */

ساكن bool hostmatch(ثابت حرف *hostname,
                      size_t hostlen,
                      ثابت حرف *نمونہ,
                      size_t patternlen)
{
  ثابت حرف *نمونہ_label_سرا;

  DEBUGASSERT(نمونہ);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  إذا(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  إذا(patternlen > 0 && نمونہ[patternlen - 1] == '.')
    patternlen--;

  إذا(patternlen > 2 && لڑی_مقابلہ(نمونہ, "*.", 2))
    أرجع pmatch(hostname, hostlen, نمونہ, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  وإلا إذا(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    أرجع FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  نمونہ_label_سرا = memchr(نمونہ, '.', patternlen);
  إذا(!نمونہ_label_سرا ||
     (memrchr(نمونہ, '.', patternlen) == نمونہ_label_سرا))
    أرجع pmatch(hostname, hostlen, نمونہ, patternlen);
  وإلا {
    ثابت حرف *hostname_label_سرا = memchr(hostname, '.', hostlen);
    إذا(hostname_label_سرا) {
      size_t skiphost = hostname_label_سرا - hostname;
      size_t skiplen = نمونہ_label_سرا - نمونہ;
      أرجع pmatch(hostname_label_سرا, hostlen - skiphost,
                    نمونہ_label_سرا, patternlen - skiplen);
    }
  }
  أرجع FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(ثابت حرف *میل, size_t matchlen,
                         ثابت حرف *hostname, size_t hostlen)
{
  إذا(matchlen > 0 && hostlen > 0)
    أرجع hostmatch(hostname, hostlen, میل, matchlen);
  أرجع FALSE;
}

#جے_انت /* USE_OPENSSL || USE_SCHANNEL */
