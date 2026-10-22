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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "dict.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DICT

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_SELECT_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/select.h>
#ਜਾਂਜੇ defined(HAVE_UNISTD_H)
#ਅਨ੍ਤਰ੍ਭਾਵ <unistd.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "escape.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_ਮੇਲ   "/MATCH:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_MATCH2  "/M:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_MATCH3  "/FIND:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_DEFINE  "/DEFINE:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_DEFINE2 "/D:"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DICT_DEFINE3 "/LOOKUP:"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DYN_DICT_ਸ਼ਬਦ 10000
ਸਥਿਰ ਅੱਖਰ *unescape_ਸ਼ਬਦ(ਅਚਲ ਅੱਖਰ *ਨਿਵੇਸ਼)
{
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;
  ਅਚਲ ਅੱਖਰ *ptr;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  curlx_dyn_init(&ਬਾਹਰਲਾ, DYN_DICT_ਸ਼ਬਦ);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  ਲਈ(ptr = ਨਿਵੇਸ਼; *ptr; ptr++) {
    ਅੱਖਰ ch = *ptr;
    ਜੇ((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, "\\", 1);
    ਜੇ(!ਨਤੀਜਾ)
      ਨਤੀਜਾ = curlx_dyn_addn(&ਬਾਹਰਲਾ, ptr, 1);
    ਜੇ(ਨਤੀਜਾ)
      ਵਾਪਸ ਨਿਰਾ;
  }
  ਵਾਪਸ curlx_dyn_ptr(&ਬਾਹਰਲਾ);
}

/* sendf() sends formatted data to the server */
ਸਥਿਰ CURLcode sendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                      ਅਚਲ ਅੱਖਰ *fmt, ...) CURL_PRINTF(2, 3);

ਸਥਿਰ CURLcode sendf(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਅਚਲ ਅੱਖਰ *fmt, ...)
{
  curl_socket_t sockfd = ਅੰਕੜੇ->conn->sock[FIRSTSOCKET];
  size_t ਬਾਈਟਾਂ_written;
  size_t write_len;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  timediff_t timeout_ms;
  ਪੂਰਨ what;
  ਅੱਖਰ *s;
  ਅੱਖਰ *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  ਜੇ(!s)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ; /* failure */

  ਬਾਈਟਾਂ_written = 0;
  write_len = ਲੜੀ_ਲੰਬਾਈ(s);
  sptr = s;

  ਲਈ(;;) {
    /* Write the buffer to the socket */
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, sptr, write_len, FALSE, &ਬਾਈਟਾਂ_written);

    ਜੇ(ਨਤੀਜਾ)
      ਰੋਕੋ;

    Curl_debug(ਅੰਕੜੇ, CURLINFO_ਅੰਕੜੇ_ਬਾਹਰਲਾ, sptr, ਬਾਈਟਾਂ_written);

    ਜੇ(ਬਾਈਟਾਂ_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= ਬਾਈਟਾਂ_written;
      sptr += ਬਾਈਟਾਂ_written;
    }
    ਨਹੀਂਤਾਂ
      ਰੋਕੋ;

    timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
    ਜੇ(timeout_ms < 0) {
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    }
    ਜੇ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ਜੇ(what < 0) {
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਰੋਕੋ;
    }
    ਨਹੀਂਤਾਂ ਜੇ(!what) {
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    }
  }

  curlx_free(s); /* free the output string */

  ਵਾਪਸ ਨਤੀਜਾ;
}

ਸਥਿਰ CURLcode dict_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  ਅੱਖਰ *ਸ਼ਬਦ;
  ਅੱਖਰ *eword = ਨਿਰਾ;
  ਅੱਖਰ *ppath;
  ਅੱਖਰ *database = ਨਿਰਾ;
  ਅੱਖਰ *strategy = ਨਿਰਾ;
  ਅੱਖਰ *nthdef = ਨਿਰਾ; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode ਨਤੀਜਾ;

  ਅੱਖਰ *ਰਸਤਾ;

  *ਪੂਰਾ = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  ਨਤੀਜਾ = Curl_urldecode(ਅੰਕੜੇ->state.ਉੱਪਰ.ਰਸਤਾ, 0, &ਰਸਤਾ, ਨਿਰਾ, REJECT_CTRL);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  ਜੇ(curl_strnequal(ਰਸਤਾ, DICT_ਮੇਲ, CURL_CSTRLEN(DICT_ਮੇਲ)) ||
     curl_strnequal(ਰਸਤਾ, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(ਰਸਤਾ, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    ਸ਼ਬਦ = strchr(ਰਸਤਾ, ':');
    ਜੇ(ਸ਼ਬਦ) {
      ਸ਼ਬਦ++;
      database = strchr(ਸ਼ਬਦ, ':');
      ਜੇ(database) {
        *database++ = (ਅੱਖਰ)0;
        strategy = strchr(database, ':');
        ਜੇ(strategy) {
          *strategy++ = (ਅੱਖਰ)0;
          nthdef = strchr(strategy, ':');
          ਜੇ(nthdef) {
            *nthdef = (ਅੱਖਰ)0;
          }
        }
      }
    }

    ਜੇ(!ਸ਼ਬਦ || (*ਸ਼ਬਦ == (ਅੱਖਰ)0)) {
      infof(ਅੰਕੜੇ, "lookup word is missing");
    }
    eword = unescape_ਸ਼ਬਦ((!ਸ਼ਬਦ || (*ਸ਼ਬਦ == (ਅੱਖਰ)0)) ? "default" : ਸ਼ਬਦ);
    ਜੇ(!eword) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ error;
    }

    ਨਤੀਜਾ = sendf(ਅੰਕੜੇ,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_ਸੰਸਕਰਣ "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*ਸ਼ਬਦ*/
                   "QUIT\r\n",
                   (!database || (*database == (ਅੱਖਰ)0)) ? "!" : database,
                   (!strategy || (*strategy == (ਅੱਖਰ)0)) ? "." : strategy,
                   eword);

    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Failed sending DICT request");
      ਜਾਓ error;
    }
    Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
  }
  ਨਹੀਂਤਾਂ ਜੇ(curl_strnequal(ਰਸਤਾ, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(ਰਸਤਾ, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(ਰਸਤਾ, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    ਸ਼ਬਦ = strchr(ਰਸਤਾ, ':');
    ਜੇ(ਸ਼ਬਦ) {
      ਸ਼ਬਦ++;
      database = strchr(ਸ਼ਬਦ, ':');
      ਜੇ(database) {
        *database++ = (ਅੱਖਰ)0;
        nthdef = strchr(database, ':');
        ਜੇ(nthdef) {
          *nthdef = (ਅੱਖਰ)0;
        }
      }
    }

    ਜੇ(!ਸ਼ਬਦ || (*ਸ਼ਬਦ == (ਅੱਖਰ)0)) {
      infof(ਅੰਕੜੇ, "lookup word is missing");
    }
    eword = unescape_ਸ਼ਬਦ((!ਸ਼ਬਦ || (*ਸ਼ਬਦ == (ਅੱਖਰ)0)) ? "default" : ਸ਼ਬਦ);
    ਜੇ(!eword) {
      ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
      ਜਾਓ error;
    }

    ਨਤੀਜਾ = sendf(ਅੰਕੜੇ,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_ਸੰਸਕਰਣ "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*ਸ਼ਬਦ*/
                   "QUIT\r\n",
                   (!database || (*database == (ਅੱਖਰ)0)) ? "!" : database,
                   eword);

    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Failed sending DICT request");
      ਜਾਓ error;
    }
    Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
  }
  ਨਹੀਂਤਾਂ {

    ppath = strchr(ਰਸਤਾ, '/');
    ਜੇ(ppath) {
      ਪੂਰਨ i;

      ppath++;
      ਲਈ(i = 0; ppath[i]; i++) {
        ਜੇ(ppath[i] == ':')
          ppath[i] = ' ';
      }
      ਨਤੀਜਾ = sendf(ਅੰਕੜੇ,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_ਸੰਸਕਰਣ "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      ਜੇ(ਨਤੀਜਾ) {
        failf(ਅੰਕੜੇ, "Failed sending DICT request");
        ਜਾਓ error;
      }

      Curl_xfer_ਤਿਆਰੀ_recv(ਅੰਕੜੇ, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(ਰਸਤਾ);
  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * DICT protocol
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_dict = {
  ZERO_NULL,                            /* setup_connection */
  dict_do,                              /* do_it */
  ZERO_NULL,                            /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#ਜੇ_ਅੰਤ /* CURL_DISABLE_DICT */
