#अगर_अघोषित HEADER_CURL_DYNBUF_H
#घोषणा HEADER_CURL_DYNBUF_H
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

काष्ठा dynbuf {
  अक्षर *bufr;    /* point to a null-terminated allocated buffer */
  माप_प्रकार leng;   /* number of bytes *EXCLUDING* the null-terminator */
  माप_प्रकार allc;   /* size of the current allocation */
  माप_प्रकार toobig; /* size limit for the buffer */
#अगर_घोषित DEBUGBUILD
  पूर्णांक init;      /* detect API usage mistakes */
#पूर्ण_अगर
};

व्योम curlx_dyn_init(काष्ठा dynbuf *s, माप_प्रकार toobig);
व्योम curlx_dyn_free(काष्ठा dynbuf *s);
CURLcode curlx_dyn_addn(काष्ठा dynbuf *s, स्थिर व्योम *mem, माप_प्रकार len)
  WARN_UNUSED_परिणाम;
CURLcode curlx_dyn_add(काष्ठा dynbuf *s, स्थिर अक्षर *str)
  WARN_UNUSED_परिणाम;
CURLcode curlx_dyn_addf(काष्ठा dynbuf *s, स्थिर अक्षर *fmt, ...)
  WARN_UNUSED_परिणाम CURL_PRINTF(2, 3);
CURLcode curlx_dyn_vaddf(काष्ठा dynbuf *s, स्थिर अक्षर *fmt, बहु_सूची ap)
  WARN_UNUSED_परिणाम CURL_PRINTF(2, 0);
व्योम curlx_dyn_reset(काष्ठा dynbuf *s);
CURLcode curlx_dyn_पुच्छ(काष्ठा dynbuf *s, माप_प्रकार trail);
CURLcode curlx_dyn_setlen(काष्ठा dynbuf *s, माप_प्रकार set);
अक्षर *curlx_dyn_ptr(स्थिर काष्ठा dynbuf *s);
अचिन्हित अक्षर *curlx_dyn_uptr(स्थिर काष्ठा dynbuf *s);
माप_प्रकार curlx_dyn_len(स्थिर काष्ठा dynbuf *s);

/* returns 0 on success, -1 on error */
/* The implementation of this function exists in mprintf.c */
पूर्णांक curlx_dyn_vprintf(काष्ठा dynbuf *dyn, स्थिर अक्षर *format, बहु_सूची args);

/* Take the buffer out of the dynbuf. Caller has ownership and
 * dynbuf resets to initial state. */
अक्षर *curlx_dyn_take(काष्ठा dynbuf *s, माप_प्रकार *plen);

/* Dynamic buffer max sizes */
#घोषणा MAX_DYNBUF_आकार (SIZE_MAX / 2)

#घोषणा DYN_DOH_RESPONSE    3000
#घोषणा DYN_DOH_CNAME       256
#घोषणा DYN_PAUSE_बफर    (64 * 1024 * 1024)
#घोषणा DYN_HAXPROXY        2048
#घोषणा DYN_HTTP_REQUEST    (1024 * 1024)
#घोषणा DYN_APRINTF         8000000
#घोषणा DYN_RTSP_REQ_HEADER (64 * 1024)
#घोषणा DYN_TRAILERS        (64 * 1024)
#घोषणा DYN_PROXY_CONNECT_HEADERS 16384
#घोषणा DYN_QLOG_NAME       1024
#घोषणा DYN_H1_TRAILER      4096
#घोषणा DYN_PINGPPONG_CMD   (64 * 1024)
#घोषणा DYN_IMAP_CMD        (64 * 1024)
#घोषणा DYN_MQTT_RECV       (64 * 1024)
#घोषणा DYN_MQTT_SEND       0xFFFFFFF
#घोषणा DYN_CRLFILE_आकार    (400 * 1024 * 1024) /* 400MiB */
#घोषणा DYN_CERTFILE_आकार   (100 * 1024) /* 100KiB */
#घोषणा DYN_KEYFILE_आकार    (100 * 1024) /* 100KiB */
#पूर्ण_अगर
