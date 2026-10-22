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

#ಆದರೆ defined(USE_OPENSSL) || defined(USE_SCHANNEL)
/* these backends use functions from this file */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_IN6_H
#ಅನ್ತರ್ಭಾವ <netinet/in6.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "curl_memrchr.h"
#ಅನ್ತರ್ಭಾವ "vdns/hostip.h"
#ಅನ್ತರ್ಭಾವ "vtls/hostcheck.h"

/* check the two input strings with given length, but do not
   assume they are null-terminated */
ಸ್ಥಿರ bool pmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, size_t hostlen,
                   ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ, size_t patternlen)
{
  ಆದರೆ(hostlen != patternlen)
    ಹಿಂತಿರುಗಿ FALSE;
  ಹಿಂತಿರುಗಿ curl_strnequal(hostname, ನಮೂನೆ, hostlen);
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

ಸ್ಥಿರ bool hostmatch(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname,
                      size_t hostlen,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ,
                      size_t patternlen)
{
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ನಮೂನೆ_label_ಕೊನೆ;

  DEBUGASSERT(ನಮೂನೆ);
  DEBUGASSERT(patternlen);
  DEBUGASSERT(hostname);
  DEBUGASSERT(hostlen);

  /* normalize pattern and hostname by stripping off trailing dots */
  ಆದರೆ(hostlen > 0 && hostname[hostlen - 1] == '.')
    hostlen--;
  ಆದರೆ(patternlen > 0 && ನಮೂನೆ[patternlen - 1] == '.')
    patternlen--;

  ಆದರೆ(patternlen > 2 && ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(ನಮೂನೆ, "*.", 2))
    ಹಿಂತಿರುಗಿ pmatch(hostname, hostlen, ನಮೂನೆ, patternlen);

  /* detect host as IP address or starting with a dot and fail if so */
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(Curl_host_is_ipnum(hostname) || (hostlen > 0 && hostname[0] == '.'))
    ಹಿಂತಿರುಗಿ FALSE;

  /* We require at least 2 dots in the pattern to avoid too wide wildcard
     match. */
  ನಮೂನೆ_label_ಕೊನೆ = memchr(ನಮೂನೆ, '.', patternlen);
  ಆದರೆ(!ನಮೂನೆ_label_ಕೊನೆ ||
     (memrchr(ನಮೂನೆ, '.', patternlen) == ನಮೂನೆ_label_ಕೊನೆ))
    ಹಿಂತಿರುಗಿ pmatch(hostname, hostlen, ನಮೂನೆ, patternlen);
  ಇಲ್ಲದಿದ್ದರೆ {
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname_label_ಕೊನೆ = memchr(hostname, '.', hostlen);
    ಆದರೆ(hostname_label_ಕೊನೆ) {
      size_t skiphost = hostname_label_ಕೊನೆ - hostname;
      size_t skiplen = ನಮೂನೆ_label_ಕೊನೆ - ನಮೂನೆ;
      ಹಿಂತಿರುಗಿ pmatch(hostname_label_ಕೊನೆ, hostlen - skiphost,
                    ನಮೂನೆ_label_ಕೊನೆ, patternlen - skiplen);
    }
  }
  ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Curl_cert_hostcheck() returns TRUE if a match and FALSE if not.
 */
bool Curl_cert_hostcheck(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಹೊಂದಿಕೆ, size_t matchlen,
                         ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *hostname, size_t hostlen)
{
  ಆದರೆ(matchlen > 0 && hostlen > 0)
    ಹಿಂತಿರುಗಿ hostmatch(hostname, hostlen, ಹೊಂದಿಕೆ, matchlen);
  ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ /* USE_OPENSSL || USE_SCHANNEL */
