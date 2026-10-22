#నిర్వచితం_కాకపోతే శీర్షిక_CURL_DYNBUF_H
#నిర్వచించు శీర్షిక_CURL_DYNBUF_H
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

నిర్మాణం dynbuf {
  అక్షరం *bufr;    /* point to a null-terminated allocated buffer */
  size_t leng;   /* number of bytes *EXCLUDING* the null-terminator */
  size_t allc;   /* size of the current allocation */
  size_t toobig; /* size limit for the buffer */
#నిర్వచితమైతే DEBUGBUILD
  పూర్ణాంకం init;      /* detect API usage mistakes */
#అయితే_ముగింపు
};

శూన్య curlx_dyn_init(నిర్మాణం dynbuf *s, size_t toobig);
శూన్య curlx_dyn_free(నిర్మాణం dynbuf *s);
CURLcode curlx_dyn_addn(నిర్మాణం dynbuf *s, స్థిరరాశి శూన్య *mem, size_t len)
  WARN_వాడని_ఫలితం;
CURLcode curlx_dyn_కూడు(నిర్మాణం dynbuf *s, స్థిరరాశి అక్షరం *str)
  WARN_వాడని_ఫలితం;
CURLcode curlx_dyn_addf(నిర్మాణం dynbuf *s, స్థిరరాశి అక్షరం *fmt, ...)
  WARN_వాడని_ఫలితం CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(నిర్మాణం dynbuf *s, స్థిరరాశి అక్షరం *fmt, va_list ap)
  WARN_వాడని_ఫలితం CURL_PRINTF(2, 0);
శూన్య curlx_dyn_reset(నిర్మాణం dynbuf *s);
CURLcode curlx_dyn_తోక(నిర్మాణం dynbuf *s, size_t trail);
CURLcode curlx_dyn_setlen(నిర్మాణం dynbuf *s, size_t సమితి);
అక్షరం *curlx_dyn_ptr(స్థిరరాశి నిర్మాణం dynbuf *s);
గుర్తులేని అక్షరం *curlx_dyn_uptr(స్థిరరాశి నిర్మాణం dynbuf *s);
size_t curlx_dyn_len(స్థిరరాశి నిర్మాణం dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
పూర్ణాంకం curlx_dyn_vprintf(నిర్మాణం dynbuf *dyn, స్థిరరాశి అక్షరం *format, va_list వాదనలు);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
అక్షరం *curlx_dyn_take(నిర్మాణం dynbuf *s, size_t *plen);

/* Dynamic buffer max sizes */
#నిర్వచించు MAX_DYNBUF_కొలత (SIZE_MAX / 2)

#నిర్వచించు DYN_DOH_RESPONSE    3000
#నిర్వచించు DYN_DOH_CNAME       256
#నిర్వచించు DYN_PAUSE_బఫరు    (64 * 1024 * 1024)
#నిర్వచించు DYN_HAXPROXY        2048
#నిర్వచించు DYN_HTTP_REQUEST    (1024 * 1024)
#నిర్వచించు DYN_APRINTF         8000000
#నిర్వచించు DYN_RTSP_REQ_శీర్షిక (64 * 1024)
#నిర్వచించు DYN_TRAILERS        (64 * 1024)
#నిర్వచించు DYN_PROXY_CONNECT_శీర్షికలు 16384
#నిర్వచించు DYN_QLOG_NAME       1024
#నిర్వచించు DYN_H1_TRAILER      4096
#నిర్వచించు DYN_PINGPPONG_CMD   (64 * 1024)
#నిర్వచించు DYN_IMAP_CMD        (64 * 1024)
#నిర్వచించు DYN_MQTT_RECV       (64 * 1024)
#నిర్వచించు DYN_MQTT_SEND       0xFFFFFFF
#నిర్వచించు DYN_CRLFILE_కొలత    (400 * 1024 * 1024) /* 400MiB */
#నిర్వచించు DYN_CERTFILE_కొలత   (100 * 1024) /* 100KiB */
#నిర్వచించు DYN_KEYFILE_కొలత    (100 * 1024) /* 100KiB */
#అయితే_ముగింపు
