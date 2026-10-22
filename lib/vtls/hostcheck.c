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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਜੇ defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_IN6_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in6.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "curl_memrchr.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vdns/hostip.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
ਸਥਿਰ bool pmatch(ਅਚਲ ਅੱਖਰ *hostname, size_t hostlen,
                   ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ, size_t patternlen)
{
  ਜੇ(hostlen != patternlen)
    ਵਾਪਸ FALSE;
  ਵਾਪਸ curl_strnequal(hostname, ਨਮੂਨਾ, hostlen);
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

ਸਥਿਰ bool hostmatch(ਅਚਲ ਅੱਖਰ *hostname,
                      size_t hostlen,
                      ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ,
                      size_t patternlen)
{
  ਅਚਲ ਅੱਖਰ *ਨਮੂਨਾ_label_ਸਿਰਾ;

  DEBUGASSERT(ਨਮੂਨਾ);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  ਜੇ(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  ਜੇ(patternlen > 0 && ਨਮੂਨਾ[patternlen - 1] == '.')
    patternlen--;

  ਜੇ(patternlen > 2 && ਲੜੀ_ਤੁਲਨਾ(ਨਮੂਨਾ, "*.", 2))
    ਵਾਪਸ pmatch(hostname, hostlen, ਨਮੂਨਾ, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  ਨਹੀਂਤਾਂ ਜੇ(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    ਵਾਪਸ FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  ਨਮੂਨਾ_label_ਸਿਰਾ = memchr(ਨਮੂਨਾ, '.', patternlen);
  ਜੇ(!ਨਮੂਨਾ_label_ਸਿਰਾ ||
     (memrchr(ਨਮੂਨਾ, '.', patternlen) == ਨਮੂਨਾ_label_ਸਿਰਾ))
    ਵਾਪਸ pmatch(hostname, hostlen, ਨਮੂਨਾ, patternlen);
  ਨਹੀਂਤਾਂ {
    ਅਚਲ ਅੱਖਰ *hostname_label_ਸਿਰਾ = memchr(hostname, '.', hostlen);
    ਜੇ(hostname_label_ਸਿਰਾ) {
      size_t skiphost = hostname_label_ਸਿਰਾ - hostname;
      size_t skiplen = ਨਮੂਨਾ_label_ਸਿਰਾ - ਨਮੂਨਾ;
      ਵਾਪਸ pmatch(hostname_label_ਸਿਰਾ, hostlen - skiphost,
                    ਨਮੂਨਾ_label_ਸਿਰਾ, patternlen - skiplen);
    }
  }
  ਵਾਪਸ FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(ਅਚਲ ਅੱਖਰ *ਮੇਲ, size_t matchlen,
                         ਅਚਲ ਅੱਖਰ *hostname, size_t hostlen)
{
  ਜੇ(matchlen > 0 && hostlen > 0)
    ਵਾਪਸ hostmatch(hostname, hostlen, ਮੇਲ, matchlen);
  ਵਾਪਸ FALSE;
}

#ਜੇ_ਅੰਤ /* USE_OPENSSL || USE_SCHANNEL */
