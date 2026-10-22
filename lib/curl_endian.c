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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "curl_endian.h"

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
గుర్తులేని పొట్టి Curl_read16_le(స్థిరరాశి గుర్తులేని అక్షరం *buf)
{
  తిరిగి (గుర్తులేని పొట్టి)(((గుర్తులేని పొట్టి)buf[0]) |
                          ((గుర్తులేని పొట్టి)buf[1] << 8));
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
గుర్తులేని పూర్ణాంకం Curl_read32_le(స్థిరరాశి గుర్తులేని అక్షరం *buf)
{
  తిరిగి ((గుర్తులేని పూర్ణాంకం)buf[0]) | ((గుర్తులేని పూర్ణాంకం)buf[1] << 8) |
         ((గుర్తులేని పూర్ణాంకం)buf[2] << 16) | ((గుర్తులేని పూర్ణాంకం)buf[3] << 24);
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
గుర్తులేని పొట్టి Curl_read16_be(స్థిరరాశి గుర్తులేని అక్షరం *buf)
{
  తిరిగి (గుర్తులేని పొట్టి)(((గుర్తులేని పొట్టి)buf[0] << 8) |
                          ((గుర్తులేని పొట్టి)buf[1]));
}
