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

#もし defined(__INTEL_COMPILER) && defined(__unix__)

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif

#endif /* __INTEL_COMPILER && __unix__ */

#define CURL_MASK_UCHAR   ((unsigned char)~0)

#define CURL_MASK_USHORT  ((unsigned short)~0)

#define CURL_MASK_UINT    ((unsigned 整数)~0)
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
unsigned char curlx_ultouc(unsigned long ulnum)
{
  DEBUGASSERT(ulnum <= (unsigned long)CURL_MASK_UCHAR);
  戻る (unsigned char)(ulnum & (unsigned long)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
整数 curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SINT);
  戻る (整数)(uznum & (size_t)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
unsigned long curlx_uztoul(size_t uznum)
{
#もし ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_ULONG);
#endif
  戻る (unsigned long)(uznum & (size_t)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
unsigned 整数 curlx_uztoui(size_t uznum)
{
#もし UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_UINT);
#endif
  戻る (unsigned 整数)(uznum & (size_t)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
整数 curlx_sltosi(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#もし INT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_SINT);
#endif
  戻る (整数)(slnum & (long)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
unsigned 整数 curlx_sltoui(long slnum)
{
  DEBUGASSERT(slnum >= 0);
#もし UINT_MAX < LONG_MAX
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_UINT);
#endif
  戻る (unsigned 整数)(slnum & (long)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
unsigned short curlx_sltous(long slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((unsigned long)slnum <= (unsigned long)CURL_MASK_USHORT);
  戻る (unsigned short)(slnum & (long)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_MASK_SSIZE_T);
  戻る (ssize_t)(uznum & (size_t)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  戻る (size_t)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
整数 curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#もし INT_MAX < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_MASK_SINT);
#endif
  戻る (整数)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
unsigned short curlx_uitous(unsigned 整数 uinum)
{
  DEBUGASSERT(uinum <= (unsigned 整数)CURL_MASK_USHORT);
  戻る (unsigned short)(uinum & (unsigned 整数)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(整数 sinum)
{
  DEBUGASSERT(sinum >= 0);
  戻る (size_t)sinum;
}

#ifdef __INTEL_COMPILER
#pragma warning(pop)
#endif

size_t curlx_uitouz(unsigned 整数 uinum)
{
  戻る (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  もし(sonum < 0)
    戻る uzmin;
#もし SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  もし(sonum > SIZE_MAX)
    戻る uzmax;
#endif
  戻る CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  もし(sznum < 0) {
    *puznum = 0;
    戻る FALSE;
  }
  *puznum = (size_t)sznum;
  戻る TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, size_t *puznum)
{
  もし(sonum < 0) {
    *puznum = 0;
    戻る FALSE;
  }
#もし SIZEOF_CURL_OFF_T > SIZEOF_SIZE_T
  もし(sonum > SIZE_MAX) {
    *puznum = 0;
    戻る FALSE;
  }
#endif
  *puznum = (size_t)sonum;
  戻る TRUE;
}

bool curlx_sltouz(long slnum, size_t *puznum)
{
  もし(slnum < 0) {
    *puznum = 0;
    戻る FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  戻る TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#もし SIZEOF_SIZE_T >= SIZEOF_CURL_OFF_T
  もし(uznum > (size_t)CURL_OFF_T_MAX)
    戻る CURL_OFF_T_MAX;
#endif
  戻る (curl_off_t)uznum;
}
