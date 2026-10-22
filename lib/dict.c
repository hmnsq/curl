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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#شامل <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#شامل <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#شامل <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#شامل <sys/select.h>
#وگرنهاگر defined(HAVE_UNISTD_H)
#شامل <unistd.h>
#endif

#شامل "transfer.h"
#شامل "curl_trc.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
ایستا نویسه *unescape_word(ثابت نویسه *درون_داد)
{
  ساختار dynbuf out;
  ثابت نویسه *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  برای(ptr = درون_داد; *ptr; ptr++) {
    نویسه ch = *ptr;
    اگر((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    اگر(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    اگر(result)
      بازگشت پوچ;
  }
  بازگشت curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
ایستا CURLcode sendf(ساختار Curl_easy *data,
                      ثابت نویسه *fmt, ...) CURL_PRINTF(2, 3);

ایستا CURLcode sendf(ساختار Curl_easy *data, ثابت نویسه *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  صحیح what;
  نویسه *s;
  نویسه *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  اگر(!s)
    بازگشت CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  برای(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    اگر(result)
      بشکن;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    اگر(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    وگرنه
      بشکن;

    timeout_ms = Curl_timeleft_ms(data);
    اگر(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      بشکن;
    }
    اگر(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    اگر(what < 0) {
      result = CURLE_SEND_ERROR;
      بشکن;
    }
    وگرنه اگر(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      بشکن;
    }
  }

  curlx_free(s); /* free the output string */

  بازگشت result;
}

ایستا CURLcode dict_do(ساختار Curl_easy *data, bool *done)
{
  نویسه *word;
  نویسه *eword = پوچ;
  نویسه *ppath;
  نویسه *database = پوچ;
  نویسه *strategy = پوچ;
  نویسه *nthdef = پوچ; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  نویسه *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, پوچ, REJECT_CTRL);
  اگر(result)
    بازگشت result;

  اگر(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    اگر(word) {
      word++;
      database = strchr(word, ':');
      اگر(database) {
        *database++ = (نویسه)0;
        strategy = strchr(database, ':');
        اگر(strategy) {
          *strategy++ = (نویسه)0;
          nthdef = strchr(strategy, ':');
          اگر(nthdef) {
            *nthdef = (نویسه)0;
          }
        }
      }
    }

    اگر(!word || (*word == (نویسه)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (نویسه)0)) ? "default" : word);
    اگر(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      برو error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (نویسه)0)) ? "!" : database,
                   (!strategy || (*strategy == (نویسه)0)) ? "." : strategy,
                   eword);

    اگر(result) {
      failf(data, "Failed sending DICT request");
      برو error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  وگرنه اگر(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    اگر(word) {
      word++;
      database = strchr(word, ':');
      اگر(database) {
        *database++ = (نویسه)0;
        nthdef = strchr(database, ':');
        اگر(nthdef) {
          *nthdef = (نویسه)0;
        }
      }
    }

    اگر(!word || (*word == (نویسه)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (نویسه)0)) ? "default" : word);
    اگر(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      برو error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (نویسه)0)) ? "!" : database,
                   eword);

    اگر(result) {
      failf(data, "Failed sending DICT request");
      برو error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  وگرنه {

    ppath = strchr(path, '/');
    اگر(ppath) {
      صحیح i;

      ppath++;
      برای(i = 0; ppath[i]; i++) {
        اگر(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      اگر(result) {
        failf(data, "Failed sending DICT request");
        برو error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  بازگشت result;
}

/*
 * DICT protocol
 */
ثابت ساختار Curl_protocol Curl_protocol_dict = {
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
