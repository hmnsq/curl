#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_CW_ಹೊರಹೋಗು_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_CW_ಹೊರಹೋಗು_H
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

ರಚನೆ Curl_easy;

/**
 * The client writer type "cw-out" that does the actual writing to
 * the client callbacks. Intended to be the last installed in the
 * client writer stack of a transfer.
 */
ಬಾಹ್ಯ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_cwtype Curl_cwt_ಹೊರಹೋಗು;

/**
 * Mark EndOfStream reached and flush ALL data to the client.
 */
CURLcode Curl_cw_ಹೊರಹೋಗು_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ);

#ಆದರೆ_ಅಂತ್ಯ /* HEADER_CURL_CW_OUT_H */
