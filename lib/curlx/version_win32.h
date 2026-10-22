#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_ಆವೃತ್ತಿ_WIN32_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_ಆವೃತ್ತಿ_WIN32_H
/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) Steve Holme, <steve_holme@hotmail.com>.
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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _WIN32

/* Version condition */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  ಆವೃತ್ತಿ_LESS_THAN,
  ಆವೃತ್ತಿ_LESS_THAN_EQUAL,
  ಆವೃತ್ತಿ_EQUAL,
  ಆವೃತ್ತಿ_GREATER_THAN_EQUAL,
  ಆವೃತ್ತಿ_GREATER_THAN
} ಆವೃತ್ತಿ_Condition;

/* Platform identifier */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  PLATFORM_DONT_CARE,
  PLATFORM_WINDOWS,
  PLATFORM_WINNT
} PlatformIdentifier;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_WINDOWS_UWP
#ವ್ಯಾಖ್ಯಾನಿಸು curlx_verify_windows_init() Curl_nop_stmt
#ಇಲ್ಲದಿದ್ದರೆ
ಶೂನ್ಯ curlx_verify_windows_init(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ
/* This is used to verify if we are running on a specific Windows version */
bool curlx_verify_windows_ಆವೃತ್ತಿ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ major_ಆವೃತ್ತಿ,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ minor_ಆವೃತ್ತಿ,
                                  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ build_ಆವೃತ್ತಿ,
                                  ಸ್ಥಿರಾಂಕ PlatformIdentifier platform,
                                  ಸ್ಥಿರಾಂಕ ಆವೃತ್ತಿ_Condition condition);

#ಆದರೆ_ಅಂತ್ಯ /* _WIN32 */

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_VERSION_WIN32_H */
