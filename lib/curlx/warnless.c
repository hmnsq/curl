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
#include "curl_setup.h"

#include "curlx/warnless.h"

#གལ་ཏེ defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned var_na)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned ཧྲིལ)~0)
#define CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#define CURL_MASK_ULONG   ((unsigned long)~0)

#define CURL_MASK_USIZE_T ((size_t)~0)
#define CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#ifdef __INTEL_COMPILER
#pragma warning(push)
#pragma warning(disable:810) /* conversion may lose significant bits */
#endif

/*
 * unsigned long to unsigned char
 */
unsigned var_na curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  ཕྱིར (unsigned var_na)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
ཧྲིལ curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  ཕྱིར (ཧྲིལ)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#གལ་ཏེ ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  ཕྱིར (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned ཧྲིལ curlx_uztoui(size_t uznum)
{
#གལ་ཏེ UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  ཕྱིར (unsigned ཧྲིལ)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
ཧྲིལ curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#གལ་ཏེ INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  ཕྱིར (ཧྲིལ)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned ཧྲིལ curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#གལ་ཏེ UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  ཕྱིར (unsigned ཧྲིལ)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  ཕྱིར (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  ཕྱིར (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  ཕྱིར (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
ཧྲིལ curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#གལ་ཏེ INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  ཕྱིར (ཧྲིལ)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned ཧྲིལ uinum)
{
  DEBUGASSERT(uinum <= (unsigned ཧྲིལ)CURL_MASK_USHORT);
  ཕྱིར (unsigned short)(uinum & (unsigned ཧྲིལ)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(ཧྲིལ sinum)
{
  DEBUGASSERT(sinum >= 0);
  ཕྱིར (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

size_t curlx_uitouz(unsigned ཧྲིལ uinum)
{
  ཕྱིར (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  གལ་ཏེ(sonum < 0)
    ཕྱིར uzmin;
#གལ་ཏེ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  གལ་ཏེ(sonum > SIZE_MAX)
    ཕྱིར uzmax;
#endif
  ཕྱིར CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  གལ་ཏེ(sznum < 0) {
    *puznum = 0;
    ཕྱིར FALSE;
  }
  *puznum = (size_t)sznum;
  ཕྱིར TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  གལ་ཏེ(sonum < 0) {
    *puznum = 0;
    ཕྱིར FALSE;
  }
#གལ་ཏེ SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  གལ་ཏེ(sonum > SIZE_MAX) {
    *puznum = 0;
    ཕྱིར FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  ཕྱིར TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  གལ་ཏེ(slnum < 0) {
    *puznum = 0;
    ཕྱིར FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  ཕྱིར TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#གལ་ཏེ SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  གལ་ཏེ(uznum > (size_t)CURL_OFF_T_MAX)
    ཕྱིར CURL_OFF_T_MAX;
#endif
  ཕྱིར (curl_off_t)uznum;
}
