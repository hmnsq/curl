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

#اگر defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETINET_IN6_H
#شامل <netinet/in6.h>
#اگر_ختم

#شامل "curl_memrchr.h"
#شامل "vdns/hostip.h"
#شامل "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
ساکن bool pmatch(ثابت حرف *hostname, size_t hostlen,
                   ثابت حرف *نمونہ, size_t patternlen)
{
  اگر(hostlen != patternlen)
    واپس FALSE;
  واپس curl_strnequal(hostname, نمونہ, hostlen);
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

ساکن bool hostmatch(ثابت حرف *hostname,
                      size_t hostlen,
                      ثابت حرف *نمونہ,
                      size_t patternlen)
{
  ثابت حرف *نمونہ_label_انتہا;

  DEBUGASSERT(نمونہ);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  اگر(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  اگر(patternlen > 0 && نمونہ[patternlen - 1] == '.')
    patternlen--;

  اگر(patternlen > 2 && لڑی_موازنہ(نمونہ, "*.", 2))
    واپس pmatch(hostname, hostlen, نمونہ, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  ورنہ اگر(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    واپس FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  نمونہ_label_انتہا = memchr(نمونہ, '.', patternlen);
  اگر(!نمونہ_label_انتہا ||
     (memrchr(نمونہ, '.', patternlen) == نمونہ_label_انتہا))
    واپس pmatch(hostname, hostlen, نمونہ, patternlen);
  ورنہ {
    ثابت حرف *hostname_label_انتہا = memchr(hostname, '.', hostlen);
    اگر(hostname_label_انتہا) {
      size_t skiphost = hostname_label_انتہا - hostname;
      size_t skiplen = نمونہ_label_انتہا - نمونہ;
      واپس pmatch(hostname_label_انتہا, hostlen - skiphost,
                    نمونہ_label_انتہا, patternlen - skiplen);
    }
  }
  واپس FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(ثابت حرف *مطابقت, size_t matchlen,
                         ثابت حرف *hostname, size_t hostlen)
{
  اگر(matchlen > 0 && hostlen > 0)
    واپس hostmatch(hostname, hostlen, مطابقت, matchlen);
  واپس FALSE;
}

#اگر_ختم /* USE_OPENSSL || USE_SCHANNEL */
