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

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای

#د_که_پای /* __INTEL_COMPILER && __unix__ */

#تعریف_کړه CURL_ماسک_UCHAR   ((بینښه حرف)~0)

#تعریف_کړه CURL_ماسک_USHORT  ((بینښه لنډ)~0)

#تعریف_کړه CURL_ماسک_UINT    ((بینښه صحيح)~0)
#تعریف_کړه CURL_ماسک_SINT    (CURL_ماسک_UINT >> 1)

#تعریف_کړه CURL_ماسک_ULONG   ((بینښه اوږد)~0)

#تعریف_کړه CURL_ماسک_USIZE_T ((size_t)~0)
#تعریف_کړه CURL_ماسک_SSIZE_T (CURL_ماسک_USIZE_T >> 1)

#که_تعریف_شوی __INTEL_COMPILER
#لارښوونه warning(ټېل)
#لارښوونه warning(disable:810) /* conversion may lose significant bits */
#د_که_پای

/*
 * unsigned long to unsigned char
 */
بینښه حرف curlx_ultouc(بینښه اوږد ulnum)
{
  DEBUGASSERT(ulnum <= (بینښه اوږد)CURL_ماسک_UCHAR);
  أرجع (بینښه حرف)(ulnum & (بینښه اوږد)CURL_ماسک_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
صحيح curlx_uztosi(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ماسک_SINT);
  أرجع (صحيح)(uznum & (size_t)CURL_ماسک_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
بینښه اوږد curlx_uztoul(size_t uznum)
{
#إذا ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ماسک_ULONG);
#د_که_پای
  أرجع (بینښه اوږد)(uznum & (size_t)CURL_ماسک_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
بینښه صحيح curlx_uztoui(size_t uznum)
{
#إذا UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_ماسک_UINT);
#د_که_پای
  أرجع (بینښه صحيح)(uznum & (size_t)CURL_ماسک_UINT);
}

/*
 * signed long to signed int
 */
صحيح curlx_sltosi(اوږد slnum)
{
  DEBUGASSERT(slnum >= 0);
#إذا لوی_عدد < LONG_MAX
  DEBUGASSERT((بینښه اوږد)slnum <= (بینښه اوږد)CURL_ماسک_SINT);
#د_که_پای
  أرجع (صحيح)(slnum & (اوږد)CURL_ماسک_SINT);
}

/*
 * signed long to unsigned int
 */
بینښه صحيح curlx_sltoui(اوږد slnum)
{
  DEBUGASSERT(slnum >= 0);
#إذا UINT_MAX < LONG_MAX
  DEBUGASSERT((بینښه اوږد)slnum <= (بینښه اوږد)CURL_ماسک_UINT);
#د_که_پای
  أرجع (بینښه صحيح)(slnum & (اوږد)CURL_ماسک_UINT);
}

/*
 * signed long to unsigned short
 */
بینښه لنډ curlx_sltous(اوږد slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((بینښه اوږد)slnum <= (بینښه اوږد)CURL_ماسک_USHORT);
  أرجع (بینښه لنډ)(slnum & (اوږد)CURL_ماسک_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(size_t uznum)
{
  DEBUGASSERT(uznum <= (size_t)CURL_ماسک_SSIZE_T);
  أرجع (ssize_t)(uznum & (size_t)CURL_ماسک_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
size_t curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  أرجع (size_t)(sonum & (curl_off_t)CURL_ماسک_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
صحيح curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#إذا لوی_عدد < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_ماسک_SINT);
#د_که_پای
  أرجع (صحيح)(sznum & (ssize_t)CURL_ماسک_SINT);
}

/*
 * unsigned int to unsigned short
 */
بینښه لنډ curlx_uitous(بینښه صحيح uinum)
{
  DEBUGASSERT(uinum <= (بینښه صحيح)CURL_ماسک_USHORT);
  أرجع (بینښه لنډ)(uinum & (بینښه صحيح)CURL_ماسک_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(صحيح sinum)
{
  DEBUGASSERT(sinum >= 0);
  أرجع (size_t)sinum;
}

#که_تعریف_شوی __INTEL_COMPILER
#لارښوونه warning(راوباسه)
#د_که_پای

size_t curlx_uitouz(بینښه صحيح uinum)
{
  أرجع (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  إذا(sonum < 0)
    أرجع uzmin;
#إذا SIZEOF_CURL_OFF_T > SIZEOF_اندازه_T
  إذا(sonum > SIZE_MAX)
    أرجع uzmax;
#د_که_پای
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

bool curlx_sotouz_ځاییږي(curl_off_t sonum, size_t *puznum)
{
  إذا(sonum < 0) {
    *puznum = 0;
    أرجع FALSE;
  }
#إذا SIZEOF_CURL_OFF_T > SIZEOF_اندازه_T
  إذا(sonum > SIZE_MAX) {
    *puznum = 0;
    أرجع FALSE;
  }
#د_که_پای
  *puznum = (size_t)sonum;
  أرجع TRUE;
}

bool curlx_sltouz(اوږد slnum, size_t *puznum)
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
#إذا SIZEOF_اندازه_T >= SIZEOF_CURL_OFF_T
  إذا(uznum > (size_t)CURL_OFF_T_MAX)
    أرجع CURL_OFF_T_MAX;
#د_که_پای
  أرجع (curl_off_t)uznum;
}
