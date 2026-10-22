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
#அந்தர்பாவ "curl_setup.h"
#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#அந்தர்பாவ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#அந்தர்பாவ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#அந்தர்பாவ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#அந்தர்பாவ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#அந்தர்பாவ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#அந்தர்பாவ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#அந்தர்பாவ <sys/select.h>
#அல்லதெனில் defined(HAVE_UNISTD_H)
#அந்தர்பாவ <unistd.h>
#endif

#அந்தர்பாவ "transfer.h"
#அந்தர்பாவ "curl_trc.h"
#அந்தர்பாவ "connect.h"
#அந்தர்பாவ "select.h"
#அந்தர்பாவ "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
நிலையான எழுத்து *unescape_word(மாறிலி எழுத்து *உள்ளீடு)
{
  கட்டமைப்பு dynbuf out;
  மாறிலி எழுத்து *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  க்கு(ptr = உள்ளீடு; *ptr; ptr++) {
    எழுத்து ch = *ptr;
    எனில்((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    எனில்(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    எனில்(result)
      திரும்பு வெற்று;
  }
  திரும்பு curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
நிலையான CURLcode sendf(கட்டமைப்பு Curl_easy *data,
                      மாறிலி எழுத்து *fmt, ...) CURL_PRINTF(2, 3);

நிலையான CURLcode sendf(கட்டமைப்பு Curl_easy *data, மாறிலி எழுத்து *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  முழு what;
  எழுத்து *s;
  எழுத்து *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  எனில்(!s)
    திரும்பு CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  க்கு(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    எனில்(result)
      நிறுத்து;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    எனில்(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    இல்லையெனில்
      நிறுத்து;

    timeout_ms = Curl_timeleft_ms(data);
    எனில்(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    }
    எனில்(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    எனில்(what < 0) {
      result = CURLE_SEND_ERROR;
      நிறுத்து;
    }
    இல்லையெனில் எனில்(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      நிறுத்து;
    }
  }

  curlx_free(s); /* free the output string */

  திரும்பு result;
}

நிலையான CURLcode dict_do(கட்டமைப்பு Curl_easy *data, bool *done)
{
  எழுத்து *word;
  எழுத்து *eword = வெற்று;
  எழுத்து *ppath;
  எழுத்து *database = வெற்று;
  எழுத்து *strategy = வெற்று;
  எழுத்து *nthdef = வெற்று; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  எழுத்து *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, வெற்று, REJECT_CTRL);
  எனில்(result)
    திரும்பு result;

  எனில்(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    எனில்(word) {
      word++;
      database = strchr(word, ':');
      எனில்(database) {
        *database++ = (எழுத்து)0;
        strategy = strchr(database, ':');
        எனில்(strategy) {
          *strategy++ = (எழுத்து)0;
          nthdef = strchr(strategy, ':');
          எனில்(nthdef) {
            *nthdef = (எழுத்து)0;
          }
        }
      }
    }

    எனில்(!word || (*word == (எழுத்து)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (எழுத்து)0)) ? "default" : word);
    எனில்(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      செல் error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (எழுத்து)0)) ? "!" : database,
                   (!strategy || (*strategy == (எழுத்து)0)) ? "." : strategy,
                   eword);

    எனில்(result) {
      failf(data, "Failed sending DICT request");
      செல் error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  இல்லையெனில் எனில்(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    எனில்(word) {
      word++;
      database = strchr(word, ':');
      எனில்(database) {
        *database++ = (எழுத்து)0;
        nthdef = strchr(database, ':');
        எனில்(nthdef) {
          *nthdef = (எழுத்து)0;
        }
      }
    }

    எனில்(!word || (*word == (எழுத்து)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (எழுத்து)0)) ? "default" : word);
    எனில்(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      செல் error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (எழுத்து)0)) ? "!" : database,
                   eword);

    எனில்(result) {
      failf(data, "Failed sending DICT request");
      செல் error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  இல்லையெனில் {

    ppath = strchr(path, '/');
    எனில்(ppath) {
      முழு i;

      ppath++;
      க்கு(i = 0; ppath[i]; i++) {
        எனில்(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      எனில்(result) {
        failf(data, "Failed sending DICT request");
        செல் error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  திரும்பு result;
}

/*
 * DICT protocol
 */
மாறிலி கட்டமைப்பு Curl_protocol Curl_protocol_dict = {
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

#endif /* CURL_DISABLE_DICT */
