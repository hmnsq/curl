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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#ଅନ୍ତର୍ଭାଵ <sys/select.h>
#ନଚେତଯଦି defined(HAVE_UNISTD_H)
#ଅନ୍ତର୍ଭାଵ <unistd.h>
#endif

#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
ସ୍ଥିର ଅକ୍ଷର *unescape_word(ଅଚଳ ଅକ୍ଷର *ନିବେଶ)
{
  ସଂରଚନା dynbuf out;
  ଅଚଳ ଅକ୍ଷର *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  ପାଇଁ(ptr = ନିବେଶ; *ptr; ptr++) {
    ଅକ୍ଷର ch = *ptr;
    ଯଦି((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    ଯଦି(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    ଯଦି(result)
      ଫେରନ୍ତୁ ଖାଲି;
  }
  ଫେରନ୍ତୁ curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
ସ୍ଥିର CURLcode sendf(ସଂରଚନା Curl_easy *data,
                      ଅଚଳ ଅକ୍ଷର *fmt, ...) CURL_PRINTF(2, 3);

ସ୍ଥିର CURLcode sendf(ସଂରଚନା Curl_easy *data, ଅଚଳ ଅକ୍ଷର *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା what;
  ଅକ୍ଷର *s;
  ଅକ୍ଷର *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  ଯଦି(!s)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  ପାଇଁ(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    ଯଦି(result)
      ଭାଙ୍ଗ;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    ଯଦି(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    ଅନ୍ୟଥା
      ଭାଙ୍ଗ;

    timeout_ms = Curl_timeleft_ms(data);
    ଯଦି(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    }
    ଯଦି(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ଯଦି(what < 0) {
      result = CURLE_SEND_ERROR;
      ଭାଙ୍ଗ;
    }
    ଅନ୍ୟଥା ଯଦି(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    }
  }

  curlx_free(s); /* free the output string */

  ଫେରନ୍ତୁ result;
}

ସ୍ଥିର CURLcode dict_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  ଅକ୍ଷର *word;
  ଅକ୍ଷର *eword = ଖାଲି;
  ଅକ୍ଷର *ppath;
  ଅକ୍ଷର *database = ଖାଲି;
  ଅକ୍ଷର *strategy = ଖାଲି;
  ଅକ୍ଷର *nthdef = ଖାଲି; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  ଅକ୍ଷର *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, ଖାଲି, REJECT_CTRL);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  ଯଦି(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    ଯଦି(word) {
      word++;
      database = strchr(word, ':');
      ଯଦି(database) {
        *database++ = (ଅକ୍ଷର)0;
        strategy = strchr(database, ':');
        ଯଦି(strategy) {
          *strategy++ = (ଅକ୍ଷର)0;
          nthdef = strchr(strategy, ':');
          ଯଦି(nthdef) {
            *nthdef = (ଅକ୍ଷର)0;
          }
        }
      }
    }

    ଯଦି(!word || (*word == (ଅକ୍ଷର)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (ଅକ୍ଷର)0)) ? "default" : word);
    ଯଦି(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (ଅକ୍ଷର)0)) ? "!" : database,
                   (!strategy || (*strategy == (ଅକ୍ଷର)0)) ? "." : strategy,
                   eword);

    ଯଦି(result) {
      failf(data, "Failed sending DICT request");
      ଯାଅ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  ଅନ୍ୟଥା ଯଦି(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    ଯଦି(word) {
      word++;
      database = strchr(word, ':');
      ଯଦି(database) {
        *database++ = (ଅକ୍ଷର)0;
        nthdef = strchr(database, ':');
        ଯଦି(nthdef) {
          *nthdef = (ଅକ୍ଷର)0;
        }
      }
    }

    ଯଦି(!word || (*word == (ଅକ୍ଷର)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (ଅକ୍ଷର)0)) ? "default" : word);
    ଯଦି(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      ଯାଅ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (ଅକ୍ଷର)0)) ? "!" : database,
                   eword);

    ଯଦି(result) {
      failf(data, "Failed sending DICT request");
      ଯାଅ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  ଅନ୍ୟଥା {

    ppath = strchr(path, '/');
    ଯଦି(ppath) {
      ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

      ppath++;
      ପାଇଁ(i = 0; ppath[i]; i++) {
        ଯଦି(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      ଯଦି(result) {
        failf(data, "Failed sending DICT request");
        ଯାଅ error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  ଫେରନ୍ତୁ result;
}

/*
 * DICT protocol
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_dict = {
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
