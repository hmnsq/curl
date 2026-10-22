#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ ಶೀರ್ಷಿಕೆ_CURL_DYNBUF_H
#ವ್ಯಾಖ್ಯಾನಿಸು ಶೀರ್ಷಿಕೆ_CURL_DYNBUF_H
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

ರಚನೆ dynbuf {
  ಅಕ್ಷರ *bufr;    /* point to a null-terminated allocated buffer */
  size_t leng;   /* number of bytes *EXCLUDING* the null-terminator */
  size_t allc;   /* size of the current allocation */
  size_t toobig; /* size limit for the buffer */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ DEBUGBUILD
  ಪೂರ್ಣಾಂಕ init;      /* detect API usage mistakes */
#ಆದರೆ_ಅಂತ್ಯ
};

ಶೂನ್ಯ curlx_dyn_init(ರಚನೆ dynbuf *s, size_t toobig);
ಶೂನ್ಯ curlx_dyn_free(ರಚನೆ dynbuf *s);
CURLcode curlx_dyn_addn(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *mem, size_t len)
  WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
CURLcode curlx_dyn_ಕೂಡಿಸು(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
  WARN_ಬಳಸದ_ಫಲಿತಾಂಶ;
CURLcode curlx_dyn_addf(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
  WARN_ಬಳಸದ_ಫಲಿತಾಂಶ CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(ರಚನೆ dynbuf *s, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, va_list ap)
  WARN_ಬಳಸದ_ಫಲಿತಾಂಶ CURL_PRINTF(2, 0);
ಶೂನ್ಯ curlx_dyn_reset(ರಚನೆ dynbuf *s);
CURLcode curlx_dyn_ಬಾಲ(ರಚನೆ dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(ರಚನೆ dynbuf *s, size_t ಗಣ);
ಅಕ್ಷರ *curlx_dyn_ptr(ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *s);
ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *curlx_dyn_uptr(ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *s);
size_t curlx_dyn_len(ಸ್ಥಿರಾಂಕ ರಚನೆ dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
ಪೂರ್ಣಾಂಕ curlx_dyn_vprintf(ರಚನೆ dynbuf *dyn, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *format, va_list ವಾದಗಳು);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
ಅಕ್ಷರ *curlx_dyn_take(ರಚನೆ dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#ವ್ಯಾಖ್ಯಾನಿಸು MAX_DYNBUF_ಗಾತ್ರ (SIZE_MAX / 2)

#ವ್ಯಾಖ್ಯಾನಿಸು DYN_DOH_RESPONSE    3000
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_DOH_CNAME       256
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_PAUSE_ಬಫರ್    (64 * 1024 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_HAXPROXY        2048
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_HTTP_REQUEST    (1024 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_APRINTF         8000000
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_RTSP_REQ_ಶೀರ್ಷಿಕೆ (64 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_TRAILERS        (64 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_PROXY_CONNECT_ಶೀರ್ಷಿಕೆಗಳು 16384
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_QLOG_NAME       1024
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_H1_TRAILER      4096
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_PINGPPONG_CMD   (64 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_IMAP_CMD        (64 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_MQTT_RECV       (64 * 1024)
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_MQTT_SEND       0xFFFFFFF
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_CRLFILE_ಗಾತ್ರ    (400 * 1024 * 1024) /* 400MiB */
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_CERTFILE_ಗಾತ್ರ   (100 * 1024) /* 100KiB */
#ವ್ಯಾಖ್ಯಾನಿಸು DYN_KEYFILE_ಗಾತ್ರ    (100 * 1024) /* 100KiB */
#ಆದರೆ_ಅಂತ್ಯ
