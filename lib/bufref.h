#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_BUFREF_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_BUFREF_H
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

/*
 * Generic buffer reference.
 */
ರಚನೆ bufref {
  ಶೂನ್ಯ (*dtor)(ಶೂನ್ಯ *);         /* Associated destructor. */
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ptr;     /* Referenced data buffer. */
  size_t len;                   /* The data size in bytes. */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ signature;                /* Detect API use mistakes. */
#ಆದರೆ_ಅಂತ್ಯ
};

ಶೂನ್ಯ Curl_bufref_init(ರಚನೆ bufref *br);
ಶೂನ್ಯ Curl_bufref_ಗಣ(ರಚನೆ bufref *br, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr, size_t len,
                     ಶೂನ್ಯ (*dtor)(ಶೂನ್ಯ *));
ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *Curl_bufref_ptr(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *br);
ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *Curl_bufref_uptr(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *br);
size_t Curl_bufref_len(ಸ್ಥಿರಾಂಕ ರಚನೆ bufref *br);
CURLcode Curl_bufref_memdup0(ರಚನೆ bufref *br, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ptr, size_t len);
ಶೂನ್ಯ Curl_bufref_free(ರಚನೆ bufref *br);

/* return a strdup() version of the buffer */
#ವ್ಯಾಖ್ಯಾನಿಸು Curl_bufref_dup(x) curlx_strdup(Curl_bufref_ptr(x))

#ಆದರೆ_ಅಂತ್ಯ
