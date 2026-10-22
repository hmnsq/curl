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

#ضمن "curl_endian.h"

/*
 * Curl_read16_le()
 *
 * This function converts a 16-bit integer from the little endian format, as
 * used in the incoming package to whatever endian format we are using
 * natively.
 *
 * Parameters:
 *
 * buf      [in]     - A pointer to a 2-byte buffer.
 *
 * Returns the integer.
 */
بینښه لنډ Curl_read16_le(ثابت بینښه حرف *buf)
{
  أرجع (بینښه لنډ)(((بینښه لنډ)buf[0]) |
                          ((بینښه لنډ)buf[1] << 8));
}

/*
 * Curl_read32_le()
 *
 * This function converts a 32-bit integer from the little endian format, as
 * used in the incoming package to whatever endian format we are using
 * natively.
 *
 * Parameters:
 *
 * buf      [in]     - A pointer to a 4-byte buffer.
 *
 * Returns the integer.
 */
بینښه صحيح Curl_read32_le(ثابت بینښه حرف *buf)
{
  أرجع ((بینښه صحيح)buf[0]) | ((بینښه صحيح)buf[1] << 8) |
         ((بینښه صحيح)buf[2] << 16) | ((بینښه صحيح)buf[3] << 24);
}

/*
 * Curl_read16_be()
 *
 * This function converts a 16-bit integer from the big endian format, as
 * used in the incoming package to whatever endian format we are using
 * natively.
 *
 * Parameters:
 *
 * buf      [in]     - A pointer to a 2-byte buffer.
 *
 * Returns the integer.
 */
بینښه لنډ Curl_read16_be(ثابت بینښه حرف *buf)
{
  أرجع (بینښه لنډ)(((بینښه لنډ)buf[0] << 8) |
                          ((بینښه لنډ)buf[1]));
}
