#ifndef HEADER_CURL_DYNBUF_H
#define HEADER_CURL_DYNBUF_H
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
  माप_प्रकार leng;   /* number of bytes *EXCLUDING* the null-terminator */
  माप_प्रकार allc;   /* size of the current allocation */
  माप_प्रकार toobig; /* size limit for the buffer */
#ifdef DEBUGBUILD
  पूर्णाङ्क init;      /* detect API usage mistakes */
#endif
};

शून्य curlx_dyn_init(संरचना dynbuf *s, माप_प्रकार toobig);
शून्य curlx_dyn_free(संरचना dynbuf *s);
CURLcode curlx_dyn_addn(संरचना dynbuf *s, अचर शून्य *mem, माप_प्रकार len)
  WARN_UNUSED_RESULT;
CURLcode curlx_dyn_add(संरचना dynbuf *s, अचर वर्ण *str)
  WARN_UNUSED_RESULT;
CURLcode curlx_dyn_addf(संरचना dynbuf *s, अचर वर्ण *fmt, ...)
  WARN_UNUSED_RESULT CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(संरचना dynbuf *s, अचर वर्ण *fmt, बहु_सूची ap)
  WARN_UNUSED_RESULT CURL_PRINTF(2, 0);
शून्य curlx_dyn_reset(संरचना dynbuf *s);
CURLcode curlx_dyn_tail(संरचना dynbuf *s, माप_प्रकार trail);
CURLcode curlx_dyn_setlen(संरचना dynbuf *s, माप_प्रकार set);
वर्ण *curlx_dyn_ptr(अचर संरचना dynbuf *s);
अचिन्हित वर्ण *curlx_dyn_uptr(अचर संरचना dynbuf *s);
माप_प्रकार curlx_dyn_len(अचर संरचना dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
पूर्णाङ्क curlx_dyn_vprintf(संरचना dynbuf *dyn, अचर वर्ण *format, बहु_सूची args);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
वर्ण *curlx_dyn_take(संरचना dynbuf *s, माप_प्रकार *plen);

/* Dynamic buffer max sizes */
#define MAX_DYNBUF_SIZE (SIZE_MAX / 2)

#define DYN_DOH_RESPONSE    3000
#define DYN_DOH_CNAME       256
#define DYN_PAUSE_BUFFER    (64 * 1024 * 1024)
#define DYN_HAXPROXY        2048
#define DYN_HTTP_REQUEST    (1024 * 1024)
#define DYN_APRINTF         8000000
#define DYN_RTSP_REQ_HEADER (64 * 1024)
#define DYN_TRAILERS        (64 * 1024)
#define DYN_PROXY_CONNECT_HEADERS 16384
#define DYN_QLOG_NAME       1024
#define DYN_H1_TRAILER      4096
#define DYN_PINGPPONG_CMD   (64 * 1024)
#define DYN_IMAP_CMD        (64 * 1024)
#define DYN_MQTT_RECV       (64 * 1024)
#define DYN_MQTT_SEND       0xFFFFFFF
#define DYN_CRLFILE_SIZE    (400 * 1024 * 1024) /* 400MiB */
#define DYN_CERTFILE_SIZE   (100 * 1024) /* 100KiB */
#define DYN_KEYFILE_SIZE    (100 * 1024) /* 100KiB */
#endif
