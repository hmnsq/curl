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
#समावेश "curl_setup.h"

#समावेश "curlx/warnless.h"

#अगर defined(__INTEL_COMPILER) && defined(__unix__)

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर

#पूर्ण_अगर /* __INTEL_COMPILER && __unix__ */

#घोषणा CURL_MASK_UCHAR   ((अचिन्हित अक्षर)~0)

#घोषणा CURL_MASK_USHORT  ((अचिन्हित लघु)~0)

#घोषणा CURL_MASK_UINT    ((अचिन्हित पूर्णांक)~0)
#घोषणा CURL_MASK_SINT    (CURL_MASK_UINT >> 1)

#घोषणा CURL_MASK_ULONG   ((अचिन्हित दीर्घ)~0)

#घोषणा CURL_MASK_USIZE_T ((माप_प्रकार)~0)
#घोषणा CURL_MASK_SSIZE_T (CURL_MASK_USIZE_T >> 1)

#अगर_घोषित __INTEL_COMPILER
#आशय warning(धकेलो)
#आशय warning(disable:810) /* conversion may lose significant bits */
#पूर्ण_अगर

/*
 * unsigned long to unsigned char
 */
अचिन्हित अक्षर curlx_ultouc(अचिन्हित दीर्घ ulnum)
{
  DEBUGASSERT(ulnum <= (अचिन्हित दीर्घ)CURL_MASK_UCHAR);
  वापस (अचिन्हित अक्षर)(ulnum & (अचिन्हित दीर्घ)CURL_MASK_UCHAR);
}

/*
 * unsigned size_t to signed int
 */
पूर्णांक curlx_uztosi(माप_प्रकार uznum)
{
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_SINT);
  वापस (पूर्णांक)(uznum & (माप_प्रकार)CURL_MASK_SINT);
}

/*
 * unsigned size_t to unsigned long
 */
अचिन्हित दीर्घ curlx_uztoul(माप_प्रकार uznum)
{
#अगर अच_दीर्घ_उच्च < SIZE_MAX
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_ULONG);
#पूर्ण_अगर
  वापस (अचिन्हित दीर्घ)(uznum & (माप_प्रकार)CURL_MASK_ULONG);
}

/*
 * unsigned size_t to unsigned int
 */
अचिन्हित पूर्णांक curlx_uztoui(माप_प्रकार uznum)
{
#अगर अच_पूर्णांक_उच्च < SIZE_MAX
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_UINT);
#पूर्ण_अगर
  वापस (अचिन्हित पूर्णांक)(uznum & (माप_प्रकार)CURL_MASK_UINT);
}

/*
 * signed long to signed int
 */
पूर्णांक curlx_sltosi(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
#अगर पूर्णांक_उच्च < दीर्घ_उच्च
  DEBUGASSERT((अचिन्हित दीर्घ)slnum <= (अचिन्हित दीर्घ)CURL_MASK_SINT);
#पूर्ण_अगर
  वापस (पूर्णांक)(slnum & (दीर्घ)CURL_MASK_SINT);
}

/*
 * signed long to unsigned int
 */
अचिन्हित पूर्णांक curlx_sltoui(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
#अगर अच_पूर्णांक_उच्च < दीर्घ_उच्च
  DEBUGASSERT((अचिन्हित दीर्घ)slnum <= (अचिन्हित दीर्घ)CURL_MASK_UINT);
#पूर्ण_अगर
  वापस (अचिन्हित पूर्णांक)(slnum & (दीर्घ)CURL_MASK_UINT);
}

/*
 * signed long to unsigned short
 */
अचिन्हित लघु curlx_sltous(दीर्घ slnum)
{
  DEBUGASSERT(slnum >= 0);
  DEBUGASSERT((अचिन्हित दीर्घ)slnum <= (अचिन्हित दीर्घ)CURL_MASK_USHORT);
  वापस (अचिन्हित लघु)(slnum & (दीर्घ)CURL_MASK_USHORT);
}

/*
 * unsigned size_t to signed ssize_t
 */
ssize_t curlx_uztosz(माप_प्रकार uznum)
{
  DEBUGASSERT(uznum <= (माप_प्रकार)CURL_MASK_SSIZE_T);
  वापस (ssize_t)(uznum & (माप_प्रकार)CURL_MASK_SSIZE_T);
}

/*
 * signed curl_off_t to unsigned size_t
 */
माप_प्रकार curlx_sotouz(curl_off_t sonum)
{
  DEBUGASSERT(sonum >= 0);
  वापस (माप_प्रकार)(sonum & (curl_off_t)CURL_MASK_USIZE_T);
}

/*
 * signed ssize_t to signed int
 */
पूर्णांक curlx_sztosi(ssize_t sznum)
{
  DEBUGASSERT(sznum >= 0);
#अगर पूर्णांक_उच्च < SSIZE_MAX
  DEBUGASSERT((माप_प्रकार)sznum <= (माप_प्रकार)CURL_MASK_SINT);
#पूर्ण_अगर
  वापस (पूर्णांक)(sznum & (ssize_t)CURL_MASK_SINT);
}

/*
 * unsigned int to unsigned short
 */
अचिन्हित लघु curlx_uitous(अचिन्हित पूर्णांक uinum)
{
  DEBUGASSERT(uinum <= (अचिन्हित पूर्णांक)CURL_MASK_USHORT);
  वापस (अचिन्हित लघु)(uinum & (अचिन्हित पूर्णांक)CURL_MASK_USHORT);
}

/*
 * signed int to unsigned size_t
 */
माप_प्रकार curlx_sitouz(पूर्णांक sinum)
{
  DEBUGASSERT(sinum >= 0);
  वापस (माप_प्रकार)sinum;
}

#अगर_घोषित __INTEL_COMPILER
#आशय warning(निकालो)
#पूर्ण_अगर

माप_प्रकार curlx_uitouz(अचिन्हित पूर्णांक uinum)
{
  वापस (माप_प्रकार)uinum;
}

माप_प्रकार curlx_sotouz_range(curl_off_t sonum, माप_प्रकार uzmin, माप_प्रकार uzmax)
{
  अगर(sonum < 0)
    वापस uzmin;
#अगर SIZEOF_CURL_OFF_T > SIZEOF_आकार_T
  अगर(sonum > SIZE_MAX)
    वापस uzmax;
#पूर्ण_अगर
  वापस CURLMIN(CURLMAX((माप_प्रकार)sonum, uzmin), uzmax);
}

bool curlx_sztouz(ssize_t sznum, माप_प्रकार *puznum)
{
  अगर(sznum < 0) {
    *puznum = 0;
    वापस FALSE;
  }
  *puznum = (माप_प्रकार)sznum;
  वापस TRUE;
}

bool curlx_sotouz_fits(curl_off_t sonum, माप_प्रकार *puznum)
{
  अगर(sonum < 0) {
    *puznum = 0;
    वापस FALSE;
  }
#अगर SIZEOF_CURL_OFF_T > SIZEOF_आकार_T
  अगर(sonum > SIZE_MAX) {
    *puznum = 0;
    वापस FALSE;
  }
#पूर्ण_अगर
  *puznum = (माप_प्रकार)sonum;
  वापस TRUE;
}

bool curlx_sltouz(दीर्घ slnum, माप_प्रकार *puznum)
{
  अगर(slnum < 0) {
    *puznum = 0;
    वापस FALSE;
  }
  /* We error in curl_setup.h if SIZEOF_LONG > SIZEOF_SIZE_T */
  *puznum = (माप_प्रकार)slnum;
  वापस TRUE;
}

curl_off_t curlx_uztoso(माप_प्रकार uznum)
{
#अगर SIZEOF_आकार_T >= SIZEOF_CURL_OFF_T
  अगर(uznum > (माप_प्रकार)CURL_OFF_T_MAX)
    वापस CURL_OFF_T_MAX;
#पूर्ण_अगर
  वापस (curl_off_t)uznum;
}
