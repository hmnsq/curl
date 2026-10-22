#यदि_अपरिभाषितम् शीर्षकम्_CURL_DYNBUF_H
#परिभाषय शीर्षकम्_CURL_DYNBUF_H
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

संरचना dynbuf {
  वर्ण *bufr;    /* point to a null-terminated allocated buffer */
  size_t leng;   /* number of bytes *EXCLUDING* the null-terminator */
  size_t allc;   /* size of the current allocation */
  size_t toobig; /* size limit for the buffer */
#यदि_परिभाषितम् DEBUGBUILD
  पूर्णाङ्क init;      /* detect API usage mistakes */
#यदि_अन्तः
};

शून्य curlx_dyn_init(संरचना dynbuf *s, size_t toobig);
शून्य curlx_dyn_free(संरचना dynbuf *s);
CURLcode curlx_dyn_addn(संरचना dynbuf *s, ध्रुव शून्य *mem, size_t len)
  WARN_अप्रयुक्तम्_परिणाम;
CURLcode curlx_dyn_सङ्कलय(संरचना dynbuf *s, ध्रुव वर्ण *str)
  WARN_अप्रयुक्तम्_परिणाम;
CURLcode curlx_dyn_addf(संरचना dynbuf *s, ध्रुव वर्ण *fmt, ...)
  WARN_अप्रयुक्तम्_परिणाम CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(संरचना dynbuf *s, ध्रुव वर्ण *fmt, va_list ap)
  WARN_अप्रयुक्तम्_परिणाम CURL_PRINTF(2, 0);
शून्य curlx_dyn_reset(संरचना dynbuf *s);
CURLcode curlx_dyn_पुच्छ(संरचना dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(संरचना dynbuf *s, size_t समुच्चयः);
वर्ण *curlx_dyn_ptr(ध्रुव संरचना dynbuf *s);
अचिह्नित वर्ण *curlx_dyn_uptr(ध्रुव संरचना dynbuf *s);
size_t curlx_dyn_len(ध्रुव संरचना dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
पूर्णाङ्क curlx_dyn_vprintf(संरचना dynbuf *dyn, ध्रुव वर्ण *format, va_list तर्काः);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
वर्ण *curlx_dyn_take(संरचना dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#परिभाषय MAX_DYNBUF_आकार (SIZE_MAX / 2)

#परिभाषय DYN_DOH_RESPONSE    3000
#परिभाषय DYN_DOH_CNAME       256
#परिभाषय DYN_PAUSE_बफर    (64 * 1024 * 1024)
#परिभाषय DYN_HAXPROXY        2048
#परिभाषय DYN_HTTP_REQUEST    (1024 * 1024)
#परिभाषय DYN_APRINTF         8000000
#परिभाषय DYN_RTSP_REQ_शीर्षकम् (64 * 1024)
#परिभाषय DYN_TRAILERS        (64 * 1024)
#परिभाषय DYN_PROXY_CONNECT_शीर्षकाणि 16384
#परिभाषय DYN_QLOG_NAME       1024
#परिभाषय DYN_H1_TRAILER      4096
#परिभाषय DYN_PINGPPONG_CMD   (64 * 1024)
#परिभाषय DYN_IMAP_CMD        (64 * 1024)
#परिभाषय DYN_MQTT_RECV       (64 * 1024)
#परिभाषय DYN_MQTT_SEND       0xFFFFFFF
#परिभाषय DYN_CRLFILE_आकार    (400 * 1024 * 1024) /* 400MiB */
#परिभाषय DYN_CERTFILE_आकार   (100 * 1024) /* 100KiB */
#परिभाषय DYN_KEYFILE_आकार    (100 * 1024) /* 100KiB */
#यदि_अन्तः
