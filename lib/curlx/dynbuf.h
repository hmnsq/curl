#جے_بنا_تعریف سرخی_CURL_DYNBUF_H
#تعریف_کرو سرخی_CURL_DYNBUF_H
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
#جے_تعریف_شدہ DEBUGBUILD
  صحيح init;      /* detect API usage mistakes */
#جے_انت
};

فراغ curlx_dyn_init(بنية dynbuf *s, size_t toobig);
فراغ curlx_dyn_free(بنية dynbuf *s);
CURLcode curlx_dyn_addn(بنية dynbuf *s, ثابت فراغ *mem, size_t len)
  WARN_بن_ورتیا_نتیجہ;
CURLcode curlx_dyn_جوڑو(بنية dynbuf *s, ثابت حرف *str)
  WARN_بن_ورتیا_نتیجہ;
CURLcode curlx_dyn_addf(بنية dynbuf *s, ثابت حرف *fmt, ...)
  WARN_بن_ورتیا_نتیجہ CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(بنية dynbuf *s, ثابت حرف *fmt, va_list ap)
  WARN_بن_ورتیا_نتیجہ CURL_PRINTF(2, 0);
فراغ curlx_dyn_reset(بنية dynbuf *s);
CURLcode curlx_dyn_پوچھ(بنية dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(بنية dynbuf *s, size_t سیٹ);
حرف *curlx_dyn_ptr(ثابت بنية dynbuf *s);
بنانشانی حرف *curlx_dyn_uptr(ثابت بنية dynbuf *s);
size_t curlx_dyn_len(ثابت بنية dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
صحيح curlx_dyn_vprintf(بنية dynbuf *dyn, ثابت حرف *format, va_list دلیلاں);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
حرف *curlx_dyn_take(بنية dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#تعریف_کرو MAX_DYNBUF_ناپ (SIZE_MAX / 2)

#تعریف_کرو DYN_DOH_RESPONSE    3000
#تعریف_کرو DYN_DOH_CNAME       256
#تعریف_کرو DYN_PAUSE_بفر    (64 * 1024 * 1024)
#تعریف_کرو DYN_HAXPROXY        2048
#تعریف_کرو DYN_HTTP_REQUEST    (1024 * 1024)
#تعریف_کرو DYN_APRINTF         8000000
#تعریف_کرو DYN_RTSP_REQ_سرخی (64 * 1024)
#تعریف_کرو DYN_TRAILERS        (64 * 1024)
#تعریف_کرو DYN_PROXY_CONNECT_سرخیاں 16384
#تعریف_کرو DYN_QLOG_NAME       1024
#تعریف_کرو DYN_H1_TRAILER      4096
#تعریف_کرو DYN_PINGPPONG_CMD   (64 * 1024)
#تعریف_کرو DYN_IMAP_CMD        (64 * 1024)
#تعریف_کرو DYN_MQTT_RECV       (64 * 1024)
#تعریف_کرو DYN_MQTT_SEND       0xFFFFFFF
#تعریف_کرو DYN_CRLFILE_ناپ    (400 * 1024 * 1024) /* 400MiB */
#تعریف_کرو DYN_CERTFILE_ناپ   (100 * 1024) /* 100KiB */
#تعریف_کرو DYN_KEYFILE_ناپ    (100 * 1024) /* 100KiB */
#جے_انت
