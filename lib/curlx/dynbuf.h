#اگر_تعریف_نشده سرآیند_CURL_DYNBUF_H
#تعریف_کن سرآیند_CURL_DYNBUF_H
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

بنية dynbuf {
  حرف *bufr;    /* point to a null-terminated allocated buffer */
  size_t leng;   /* number of bytes *EXCLUDING* the null-terminator */
  size_t allc;   /* size of the current allocation */
  size_t toobig; /* size limit for the buffer */
#اگر_تعریف_شده DEBUGBUILD
  صحيح init;      /* detect API usage mistakes */
#پایان_اگر
};

فراغ curlx_dyn_init(بنية dynbuf *s, size_t toobig);
فراغ curlx_dyn_free(بنية dynbuf *s);
CURLcode curlx_dyn_addn(بنية dynbuf *s, ثابت فراغ *mem, size_t len)
  WARN_استفادهنشده_نتیجه;
CURLcode curlx_dyn_جمع_کن(بنية dynbuf *s, ثابت حرف *str)
  WARN_استفادهنشده_نتیجه;
CURLcode curlx_dyn_addf(بنية dynbuf *s, ثابت حرف *fmt, ...)
  WARN_استفادهنشده_نتیجه CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(بنية dynbuf *s, ثابت حرف *fmt, va_list ap)
  WARN_استفادهنشده_نتیجه CURL_PRINTF(2, 0);
فراغ curlx_dyn_reset(بنية dynbuf *s);
CURLcode curlx_dyn_دم(بنية dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(بنية dynbuf *s, size_t مجموعه);
حرف *curlx_dyn_ptr(ثابت بنية dynbuf *s);
بیعلامت حرف *curlx_dyn_uptr(ثابت بنية dynbuf *s);
size_t curlx_dyn_len(ثابت بنية dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
صحيح curlx_dyn_vprintf(بنية dynbuf *dyn, ثابت حرف *format, va_list آرگومانها);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
حرف *curlx_dyn_take(بنية dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#تعریف_کن MAX_DYNBUF_اندازه (SIZE_MAX / 2)

#تعریف_کن DYN_DOH_RESPONSE    3000
#تعریف_کن DYN_DOH_CNAME       256
#تعریف_کن DYN_PAUSE_میانگیر    (64 * 1024 * 1024)
#تعریف_کن DYN_HAXPROXY        2048
#تعریف_کن DYN_HTTP_REQUEST    (1024 * 1024)
#تعریف_کن DYN_APRINTF         8000000
#تعریف_کن DYN_RTSP_REQ_سرآیند (64 * 1024)
#تعریف_کن DYN_TRAILERS        (64 * 1024)
#تعریف_کن DYN_PROXY_CONNECT_سرآیندها 16384
#تعریف_کن DYN_QLOG_NAME       1024
#تعریف_کن DYN_H1_TRAILER      4096
#تعریف_کن DYN_PINGPPONG_CMD   (64 * 1024)
#تعریف_کن DYN_IMAP_CMD        (64 * 1024)
#تعریف_کن DYN_MQTT_RECV       (64 * 1024)
#تعریف_کن DYN_MQTT_SEND       0xFFFFFFF
#تعریف_کن DYN_CRLFILE_اندازه    (400 * 1024 * 1024) /* 400MiB */
#تعریف_کن DYN_CERTFILE_اندازه   (100 * 1024) /* 100KiB */
#تعریف_کن DYN_KEYFILE_اندازه    (100 * 1024) /* 100KiB */
#پایان_اگر
