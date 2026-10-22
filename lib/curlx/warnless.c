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

#ಯದಿ defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned ವರ್ಣ)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned ಪೂರ್ಣಾಙ್ಕ)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((unsigned long)~0)

#define CURL_MASK_USIZE_T ((size_t)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(ತಳ್ಳು)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
unsigned ವರ್ಣ curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  ಪ್ರತ್ಯಾಗಮ (unsigned ವರ್ಣ)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
ಪೂರ್ಣಾಙ್ಕ curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#ಯದಿ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  ಪ್ರತ್ಯಾಗಮ (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned ಪೂರ್ಣಾಙ್ಕ curlx_uztoui(size_t uznum)
{
#ಯದಿ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  ಪ್ರತ್ಯಾಗಮ (unsigned ಪೂರ್ಣಾಙ್ಕ)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
ಪೂರ್ಣಾಙ್ಕ curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#ಯದಿ INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned ಪೂರ್ಣಾಙ್ಕ curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#ಯದಿ UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  ಪ್ರತ್ಯಾಗಮ (unsigned ಪೂರ್ಣಾಙ್ಕ)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  ಪ್ರತ್ಯಾಗಮ (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  ಪ್ರತ್ಯಾಗಮ (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  ಪ್ರತ್ಯಾಗಮ (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
ಪೂರ್ಣಾಙ್ಕ curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#ಯದಿ INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  ಪ್ರತ್ಯಾಗಮ (ಪೂರ್ಣಾಙ್ಕ)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned ಪೂರ್ಣಾಙ್ಕ uinum)
{
  DEBUGASSERT(uinum <= (unsigned ಪೂರ್ಣಾಙ್ಕ)CURL_MASK_USHORT);
  ಪ್ರತ್ಯಾಗಮ (unsigned short)(uinum & (unsigned ಪೂರ್ಣಾಙ್ಕ)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(ಪೂರ್ಣಾಙ್ಕ sinum)
{
  DEBUGASSERT(sinum >= 0);
  ಪ್ರತ್ಯಾಗಮ (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(ತೆಗೆ)
#endif

size_t curlx_uitouz(unsigned ಪೂರ್ಣಾಙ್ಕ uinum)
{
  ಪ್ರತ್ಯಾಗಮ (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  ಯದಿ(sonum < 0)
    ಪ್ರತ್ಯಾಗಮ uzmin;
#ಯದಿ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  ಯದಿ(sonum > SIZE_MAX)
    ಪ್ರತ್ಯಾಗಮ uzmax;
#endif
  ಪ್ರತ್ಯಾಗಮ CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  ಯದಿ(sznum < 0) {
    *puznum = 0;
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  *puznum = (size_t)sznum;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  ಯದಿ(sonum < 0) {
    *puznum = 0;
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
#ಯದಿ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  ಯದಿ(sonum > SIZE_MAX) {
    *puznum = 0;
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  ಯದಿ(slnum < 0) {
    *puznum = 0;
    ಪ್ರತ್ಯಾಗಮ FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  ಪ್ರತ್ಯಾಗಮ TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#ಯದಿ SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  ಯದಿ(uznum > (size_t)CURL_OFF_T_MAX)
    ಪ್ರತ್ಯಾಗಮ CURL_OFF_T_MAX;
#endif
  ಪ್ರತ್ಯಾಗಮ (curl_off_t)uznum;
}
