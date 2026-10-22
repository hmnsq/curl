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

#ಅನ್ತರ್ಭಾವ "curlx/warnless.h"

#ಆದರೆ defined(__INTEL_COMPILER) && defined(__unix__)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ_ಅಂತ್ಯ /* __INTEL_COMPILER && __unix__ */

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_UCHAR   ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)~0)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_USHORT  ((ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)~0)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_UINT    ((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)~0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_SINT    (CURL_ಮುಖವಾಡ_UINT >> 1)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_ULONG   ((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)~0)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_USIZE_T ((size_t)~0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಮುಖವಾಡ_SSIZE_T (CURL_ಮುಖವಾಡ_USIZE_T >> 1)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTEL_COMPILER
#ಸೂಚನೆ warning(ತಳ್ಳು)
#ಸೂಚನೆ warning(disable:810) /* conversion may lose significant bits */
#ಆದರೆ_ಅಂತ್ಯ

/*
 * unsigned long to unsigned char
 */
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ curlx_ultouc(ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ ulnum)
{
  DEBUGASSERT(ulnum <= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)CURL_ಮುಖವಾಡ_UCHAR);
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)(ulnum & (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)CURL_ಮುಖವಾಡ_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
ಪೂರ್ಣಾಂಕ curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ಮುಖವಾಡ_SINT);
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(uznum & (size_t)CURL_ಮುಖವಾಡ_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ curlx_uztoul(size_t uznum)
{
#ಆದರೆ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ಮುಖವಾಡ_ULONG);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)(uznum & (size_t)CURL_ಮುಖವಾಡ_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curlx_uztoui(size_t uznum)
{
#ಆದರೆ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ಮುಖವಾಡ_UINT);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(uznum & (size_t)CURL_ಮುಖವಾಡ_UINT);
}

/*
 * signed long to signed int
 */
ಪೂರ್ಣಾಂಕ curlx_sltosi(ದೀರ್ಘ slnum)
{
  DEBUGASSERT(slnum >= 0);
#ಆದರೆ ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ < LONG_MAX
  DEBUGASSERT((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)slnum <= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)CURL_ಮುಖವಾಡ_SINT);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(slnum & (ದೀರ್ಘ)CURL_ಮುಖವಾಡ_SINT);
}

/*
 * signed long to unsigned int
 */
ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ curlx_sltoui(ದೀರ್ಘ slnum)
{
  DEBUGASSERT(slnum >= 0);
#ಆದರೆ UINT_MAX < LONG_MAX
  DEBUGASSERT((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)slnum <= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)CURL_ಮುಖವಾಡ_UINT);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(slnum & (ದೀರ್ಘ)CURL_ಮುಖವಾಡ_UINT);
}

/*
 * signed long to unsigned short
 */
ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ curlx_sltous(ದೀರ್ಘ slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)slnum <= (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)CURL_ಮುಖವಾಡ_USHORT);
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)(slnum & (ದೀರ್ಘ)CURL_ಮುಖವಾಡ_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ಮುಖವಾಡ_SSIZE_T);
  ಹಿಂತಿರುಗಿ (ssize_t)(uznum & (size_t)CURL_ಮುಖವಾಡ_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  ಹಿಂತಿರುಗಿ (size_t)(sonum & (curl_off_t)CURL_ಮುಖವಾಡ_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
ಪೂರ್ಣಾಂಕ curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#ಆದರೆ ಗರಿಷ್ಠ_ಪೂರ್ಣಾಂಕ < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_ಮುಖವಾಡ_SINT);
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (ಪೂರ್ಣಾಂಕ)(sznum & (ssize_t)CURL_ಮುಖವಾಡ_SINT);
}

/*
 * unsigned int to unsigned short
 */
ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ curlx_uitous(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uinum)
{
  DEBUGASSERT(uinum <= (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURL_ಮುಖವಾಡ_USHORT);
  ಹಿಂತಿರುಗಿ (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)(uinum & (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)CURL_ಮುಖವಾಡ_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(ಪೂರ್ಣಾಂಕ sinum)
{
  DEBUGASSERT(sinum >= 0);
  ಹಿಂತಿರುಗಿ (size_t)sinum;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTEL_COMPILER
#ಸೂಚನೆ warning(ತೆಗೆ)
#ಆದರೆ_ಅಂತ್ಯ

size_t curlx_uitouz(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ uinum)
{
  ಹಿಂತಿರುಗಿ (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  ಆದರೆ(sonum < 0)
    ಹಿಂತಿರುಗಿ uzmin;
#ಆದರೆ SIZEOF_CURL_OFF_T > SIZEOF_ಗಾತ್ರ_T
  ಆದರೆ(sonum > SIZE_MAX)
    ಹಿಂತಿರುಗಿ uzmax;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  ಆದರೆ(sznum < 0) {
    *puznum = 0;
    ಹಿಂತಿರುಗಿ FALSE;
  }
  *puznum = (size_t)sznum;
  ಹಿಂತಿರುಗಿ TRUE;
}

bool curlx_sotouz_ಹೊಂದುತ್ತದೆ(curl_off_t sonum, size_t *puznum)
{
  ಆದರೆ(sonum < 0) {
    *puznum = 0;
    ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ SIZEOF_CURL_OFF_T > SIZEOF_ಗಾತ್ರ_T
  ಆದರೆ(sonum > SIZE_MAX) {
    *puznum = 0;
    ಹಿಂತಿರುಗಿ FALSE;
  }
#ಆದರೆ_ಅಂತ್ಯ
  *puznum = (size_t)sonum;
  ಹಿಂತಿರುಗಿ TRUE;
}

bool curlx_sltouz(ದೀರ್ಘ slnum, size_t *puznum)
{
  ಆದರೆ(slnum < 0) {
    *puznum = 0;
    ಹಿಂತಿರುಗಿ FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  ಹಿಂತಿರುಗಿ TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#ಆದರೆ SIZEOF_ಗಾತ್ರ_T >= SIZEOF_CURL_OFF_T
  ಆದರೆ(uznum > (size_t)CURL_OFF_T_MAX)
    ಹಿಂತಿರುಗಿ CURL_OFF_T_MAX;
#ಆದರೆ_ಅಂತ್ಯ
  ಹಿಂತಿರುಗಿ (curl_off_t)uznum;
}
