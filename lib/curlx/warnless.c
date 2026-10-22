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

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت

#جے_انت /* __INTEL_COMPILER && __unix__ */

#تعریف_کرو CURL_نقاب_UCHAR   ((بنانشانی حرف)~0)

#تعریف_کرو CURL_نقاب_USHORT  ((بنانشانی چھوٹا)~0)

#تعریف_کرو CURL_نقاب_UINT    ((بنانشانی صحيح)~0)
#تعریف_کرو CURL_نقاب_SINT    (CURL_نقاب_UINT >> 1)

#تعریف_کرو CURL_نقاب_ULONG   ((بنانشانی لما)~0)

#تعریف_کرو CURL_نقاب_USIZE_T ((size_t)~0)
#تعریف_کرو CURL_نقاب_SSIZE_T (CURL_نقاب_USIZE_T >> 1)

#جے_تعریف_شدہ __INTEL_COMPILER
#ہدایت warning(ڈالو)
#ہدایت warning(disable:810) /* conversion may lose significant bits */
#جے_انت

/*
 * unsigned long to unsigned char
 */
بنانشانی حرف curlx_ultouc(بنانشانی لما ulnum)
{
  DEBUGASSERT(ulnum <= (بنانشانی لما)CURL_نقاب_UCHAR);
  أرجع (بنانشانی حرف)(ulnum & (بنانشانی لما)CURL_نقاب_UCHAR);
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
بنانشانی لما curlx_uztoul(size_t uznum)
{
#إذا ULONG_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_ULONG);
#جے_انت
  أرجع (بنانشانی لما)(uznum & (size_t)CURL_نقاب_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
بنانشانی صحيح curlx_uztoui(size_t uznum)
{
#إذا UINT_MAX < SIZE_MAX
  DEBUGASSERT(uznum <= (size_t)CURL_نقاب_UINT);
#جے_انت
  أرجع (بنانشانی صحيح)(uznum & (size_t)CURL_نقاب_UINT);
}

/*
 * signed long to signed int
 */
صحيح curlx_sltosi(لما slnum)
{
  DEBUGASSERT(slnum >= 0);
#إذا ودھ_توں_ودھ_عدد < LONG_MAX
  DEBUGASSERT((بنانشانی لما)slnum <= (بنانشانی لما)CURL_نقاب_SINT);
#جے_انت
  أرجع (صحيح)(slnum & (لما)CURL_نقاب_SINT);
}

/*
 * signed long to unsigned int
 */
بنانشانی صحيح curlx_sltoui(لما slnum)
{
  DEBUGASSERT(slnum >= 0);
#إذا UINT_MAX < LONG_MAX
  DEBUGASSERT((بنانشانی لما)slnum <= (بنانشانی لما)CURL_نقاب_UINT);
#جے_انت
  أرجع (بنانشانی صحيح)(slnum & (لما)CURL_نقاب_UINT);
}

/*
 * signed long to unsigned short
 */
بنانشانی چھوٹا curlx_sltous(لما slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((بنانشانی لما)slnum <= (بنانشانی لما)CURL_نقاب_USHORT);
  أرجع (بنانشانی چھوٹا)(slnum & (لما)CURL_نقاب_USHORT);
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
#إذا ودھ_توں_ودھ_عدد < SSIZE_MAX
  DEBUGASSERT((size_t)sznum <= (size_t)CURL_نقاب_SINT);
#جے_انت
  أرجع (صحيح)(sznum & (ssize_t)CURL_نقاب_SINT);
}

/*
 * unsigned int to unsigned short
 */
بنانشانی چھوٹا curlx_uitous(بنانشانی صحيح uinum)
{
  DEBUGASSERT(uinum <= (بنانشانی صحيح)CURL_نقاب_USHORT);
  أرجع (بنانشانی چھوٹا)(uinum & (بنانشانی صحيح)CURL_نقاب_USHORT);
}

/*
 * signed int to unsigned size_t
 */
size_t curlx_sitouz(صحيح sinum)
{
  DEBUGASSERT(sinum >= 0);
  أرجع (size_t)sinum;
}

#جے_تعریف_شدہ __INTEL_COMPILER
#ہدایت warning(نکالو)
#جے_انت

size_t curlx_uitouz(بنانشانی صحيح uinum)
{
  أرجع (size_t)uinum;
}

size_t curlx_sotouz_range(curl_off_t sonum, size_t uzmin, size_t uzmax)
{
  إذا(sonum < 0)
    أرجع uzmin;
#إذا SIZEOF_CURL_OFF_T > SIZEOF_ناپ_T
  إذا(sonum > SIZE_MAX)
    أرجع uzmax;
#جے_انت
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

bool curlx_sotouz_سماندا(curl_off_t sonum, size_t *puznum)
{
  إذا(sonum < 0) {
    *puznum = 0;
    أرجع FALSE;
  }
#إذا SIZEOF_CURL_OFF_T > SIZEOF_ناپ_T
  إذا(sonum > SIZE_MAX) {
    *puznum = 0;
    أرجع FALSE;
  }
#جے_انت
  *puznum = (size_t)sonum;
  أرجع TRUE;
}

bool curlx_sltouz(لما slnum, size_t *puznum)
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
#إذا SIZEOF_ناپ_T >= SIZEOF_CURL_OFF_T
  إذا(uznum > (size_t)CURL_OFF_T_MAX)
    أرجع CURL_OFF_T_MAX;
#جے_انت
  أرجع (curl_off_t)uznum;
}
