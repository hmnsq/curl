#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ ਸਿਰਲੇਖ_CURL_DYNBUF_H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਸਿਰਲੇਖ_CURL_DYNBUF_H
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

ਬਣਤਰ dynbuf {
  ਅੱਖਰ *bufr;    /* point to a null-terminated allocated buffer */
  size_t leng;   /* number of bytes *EXCLUDING* the null-terminator */
  size_t allc;   /* size of the current allocation */
  size_t toobig; /* size limit for the buffer */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ DEBUGBUILD
  ਪੂਰਨ init;      /* detect API usage mistakes */
#ਜੇ_ਅੰਤ
};

ਖਾਲੀ curlx_dyn_init(ਬਣਤਰ dynbuf *s, size_t toobig);
ਖਾਲੀ curlx_dyn_free(ਬਣਤਰ dynbuf *s);
CURLcode curlx_dyn_addn(ਬਣਤਰ dynbuf *s, ਅਚਲ ਖਾਲੀ *mem, size_t len)
  WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
CURLcode curlx_dyn_ਜੋੜੋ(ਬਣਤਰ dynbuf *s, ਅਚਲ ਅੱਖਰ *str)
  WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ;
CURLcode curlx_dyn_addf(ਬਣਤਰ dynbuf *s, ਅਚਲ ਅੱਖਰ *fmt, ...)
  WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(ਬਣਤਰ dynbuf *s, ਅਚਲ ਅੱਖਰ *fmt, va_list ap)
  WARN_ਅਣਵਰਤਿਆ_ਨਤੀਜਾ CURL_PRINTF(2, 0);
ਖਾਲੀ curlx_dyn_reset(ਬਣਤਰ dynbuf *s);
CURLcode curlx_dyn_ਪੂਛ(ਬਣਤਰ dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(ਬਣਤਰ dynbuf *s, size_t ਸੈੱਟ);
ਅੱਖਰ *curlx_dyn_ptr(ਅਚਲ ਬਣਤਰ dynbuf *s);
ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *curlx_dyn_uptr(ਅਚਲ ਬਣਤਰ dynbuf *s);
size_t curlx_dyn_len(ਅਚਲ ਬਣਤਰ dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
ਪੂਰਨ curlx_dyn_vprintf(ਬਣਤਰ dynbuf *dyn, ਅਚਲ ਅੱਖਰ *format, va_list ਦਲੀਲਾਂ);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
ਅੱਖਰ *curlx_dyn_take(ਬਣਤਰ dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MAX_DYNBUF_ਮਾਪ (SIZE_MAX / 2)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_DOH_RESPONSE    3000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_DOH_CNAME       256
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_PAUSE_ਬਫਰ    (64 * 1024 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_HAXPROXY        2048
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_HTTP_REQUEST    (1024 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_APRINTF         8000000
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_RTSP_REQ_ਸਿਰਲੇਖ (64 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_TRAILERS        (64 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_PROXY_CONNECT_ਸਿਰਲੇਖਾਂ 16384
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_QLOG_NAME       1024
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_H1_TRAILER      4096
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_PINGPPONG_CMD   (64 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_IMAP_CMD        (64 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_MQTT_RECV       (64 * 1024)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_MQTT_SEND       0xFFFFFFF
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_CRLFILE_ਮਾਪ    (400 * 1024 * 1024) /* 400MiB */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_CERTFILE_ਮਾਪ   (100 * 1024) /* 100KiB */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_KEYFILE_ਮਾਪ    (100 * 1024) /* 100KiB */
#ਜੇ_ਅੰਤ
