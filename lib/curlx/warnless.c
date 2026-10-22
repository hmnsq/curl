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

#ضمن "curlx/warnless.h"

#إذا defined(__INTEL_COMPILER) && defined(__unix__)

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* __INTEL_COMPILER && __unix__ */

#وصف_ڪريو CURL_نقاب_UCHAR   ((بنانشان حرف)~0)

#وصف_ڪريو CURL_نقاب_USHORT  ((بنانشان ننڍو)~0)

#وصف_ڪريو CURL_نقاب_UINT    ((بنانشان صحيح)~0)
#وصف_ڪريو CURL_نقاب_SINT    (CURL_نقاب_UINT >> 1)

#وصف_ڪريو CURL_نقاب_ULONG   ((بنانشان ڊگهو)~0)

#وصف_ڪريو CURL_نقاب_USIZE_T ((size_t)~0)
#وصف_ڪريو CURL_نقاب_SSIZE_T (CURL_نقاب_USIZE_T >> 1)

#جيڪڏهن_وصف_ٿيل __INTEL_COMPILER
#هدايت warning(ڌڪو)
#هدايت warning(disable:810) /* conversion may lose significant bits */
#جيڪڏهن_پورو

/*
 * unsigned long to unsigned char
 */
بنانشان حرف curlx_ultouc(بنانشان ڊگهو ulnum)
{
  DEBUGASSERT(ulnum <= (بنانشان ڊگهو)CURL_نقاب_UCHAR);
  أرجع (بنانشان حرف)(ulnum & (بنانشان ڊگهو)CURL_نقاب_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
صحيح curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_SINT);
  أرجع (صحيح)(uznum & (size_t)CURL_نقاب_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
بنانشان ڊگهو curlx_uztoul(size_t uznum)
{
#إذا ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_ULONG);
#جيڪڏهن_پورو
  أرجع (بنانشان ڊگهو)(uznum & (size_t)CURL_نقاب_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
بنانشان صحيح curlx_uztoui(size_t uznum)
{
#إذا UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_UINT);
#جيڪڏهن_پورو
  أرجع (بنانشان صحيح)(uznum & (size_t)CURL_نقاب_UINT);
}

/*
 * signed long to signed int
 */
صحيح curlx_sltosi(ڊگهو slnum)
{
  DEBUGASSERT(slnum >= 0);
#إذا وڌ_کان_وڌ_عدد < LONG_MAX
  DEBUGASSERT((بنانشان ڊگهو)slnum <= (بنانشان ڊگهو)CURL_نقاب_SINT);
#جيڪڏهن_پورو
  أرجع (صحيح)(slnum & (ڊگهو)CURL_نقاب_SINT);
}

/*
 * signed long to unsigned int
 */
بنانشان صحيح curlx_sltoui(ڊگهو slnum)
{
  DEBUGASSERT(slnum >= 0);
#إذا UINT_MAX < LONG_MAX
  DEBUGASSERT((بنانشان ڊگهو)slnum <= (بنانشان ڊگهو)CURL_نقاب_UINT);
#جيڪڏهن_پورو
  أرجع (بنانشان صحيح)(slnum & (ڊگهو)CURL_نقاب_UINT);
}

/*
 * signed long to unsigned short
 */
بنانشان ننڍو curlx_sltous(ڊگهو slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((بنانشان ڊگهو)slnum <= (بنانشان ڊگهو)CURL_نقاب_USHORT);
  أرجع (بنانشان ننڍو)(slnum & (ڊگهو)CURL_نقاب_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_SSIZE_T);
  أرجع (ssize_t)(uznum & (size_t)CURL_نقاب_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  أرجع (size_t)(sonum & (curl_off_t)CURL_نقاب_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
صحيح curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#إذا وڌ_کان_وڌ_عدد < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_نقاب_SINT);
#جيڪڏهن_پورو
  أرجع (صحيح)(sznum & (ssize_t)CURL_نقاب_SINT);
}

/*
 * unsigned int to unsigned short
 */
بنانشان ننڍو curlx_uitous(بنانشان صحيح uinum)
{
  DEBUGASSERT(uinum <= (بنانشان صحيح)CURL_نقاب_USHORT);
  أرجع (بنانشان ننڍو)(uinum & (بنانشان صحيح)CURL_نقاب_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(صحيح sinum)
{
  DEBUGASSERT(sinum >= 0);
  أرجع (size_t)sinum;
}

#جيڪڏهن_وصف_ٿيل __INTEL_COMPILER
#هدايت warning(ڪڍو)
#جيڪڏهن_پورو

size_t curlx_uitouz(بنانشان صحيح uinum)
{
  أرجع (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  إذا(sonum < 0)
    أرجع uzmin;
#إذا SIZEOF_CURL_OFF_T > SIZEOF_سائيز_T
  إذا(sonum > SIZE_MAX)
    أرجع uzmax;
#جيڪڏهن_پورو
  أرجع CURLMIN(CURLMAX((size_t)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, size_t *puznum)
{
  إذا(sznum < 0) {
    *puznum = 0;
    أرجع FALSE;
  }
  *puznum = (size_t)sznum;
  أرجع TRUE;
}

bool curlx_sotouz_سمائي(curl_off_t sonum, size_t *puznum)
{
  إذا(sonum < 0) {
    *puznum = 0;
    أرجع FALSE;
  }
#إذا SIZEOF_CURL_OFF_T > SIZEOF_سائيز_T
  إذا(sonum > SIZE_MAX) {
    *puznum = 0;
    أرجع FALSE;
  }
#جيڪڏهن_پورو
  *puznum = (size_t)sonum;
  أرجع TRUE;
}

bool curlx_sltouz(ڊگهو slnum, size_t *puznum)
{
  إذا(slnum < 0) {
    *puznum = 0;
    أرجع FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (size_t)slnum;
  أرجع TRUE;
}

curl_off_t curlx_uztoso(size_t uznum)
{
#إذا SIZEOF_سائيز_T >= SIZEOF_CURL_OFF_T
  إذا(uznum > (size_t)CURL_OFF_T_MAX)
    أرجع CURL_OFF_T_MAX;
#جيڪڏهن_پورو
  أرجع (curl_off_t)uznum;
}
