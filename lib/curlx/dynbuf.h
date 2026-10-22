#اگر_غیر_تعریف_شدہ سرخی_CURL_DYNBUF_H
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

ساخت dynbuf {
  حرف *bufr;    /* point to a null-terminated allocated buffer */
  size_t leng;   /* number of bytes *EXCLUDING* the null-terminator */
  size_t allc;   /* size of the current allocation */
  size_t toobig; /* size limit for the buffer */
#اگر_تعریف_شدہ DEBUGBUILD
  صحیح init;      /* detect API usage mistakes */
#اگر_ختم
};

خالی curlx_dyn_init(ساخت dynbuf *s, size_t toobig);
خالی curlx_dyn_free(ساخت dynbuf *s);
CURLcode curlx_dyn_addn(ساخت dynbuf *s, ثابت خالی *mem, size_t len)
  WARN_غیر_مستعمل_نتیجہ;
CURLcode curlx_dyn_جوڑو(ساخت dynbuf *s, ثابت حرف *str)
  WARN_غیر_مستعمل_نتیجہ;
CURLcode curlx_dyn_addf(ساخت dynbuf *s, ثابت حرف *fmt, ...)
  WARN_غیر_مستعمل_نتیجہ CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(ساخت dynbuf *s, ثابت حرف *fmt, va_list ap)
  WARN_غیر_مستعمل_نتیجہ CURL_PRINTF(2, 0);
خالی curlx_dyn_reset(ساخت dynbuf *s);
CURLcode curlx_dyn_دم(ساخت dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(ساخت dynbuf *s, size_t سیٹ);
حرف *curlx_dyn_ptr(ثابت ساخت dynbuf *s);
غیرعلامتی حرف *curlx_dyn_uptr(ثابت ساخت dynbuf *s);
size_t curlx_dyn_len(ثابت ساخت dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
صحیح curlx_dyn_vprintf(ساخت dynbuf *dyn, ثابت حرف *format, va_list دلائل);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
حرف *curlx_dyn_take(ساخت dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#تعریف_کرو MAX_DYNBUF_سائز (SIZE_MAX / 2)

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
#تعریف_کرو DYN_CRLFILE_سائز    (400 * 1024 * 1024) /* 400MiB */
#تعریف_کرو DYN_CERTFILE_سائز   (100 * 1024) /* 100KiB */
#تعریف_کرو DYN_KEYFILE_سائز    (100 * 1024) /* 100KiB */
#اگر_ختم
