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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#ಅನ್ತರ್ಭಾವ <sys/select.h>
#ನೋಚೇದ್ಯದಿ defined(HAVE_UNISTD_H)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#endif

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
ಸ್ಥಿರ ವರ್ಣ *unescape_word(ಧ್ರುವ ವರ್ಣ *ಒಳಹರಿವು)
{
  ಸಂರಚನಾ dynbuf out;
  ಧ್ರುವ ವರ್ಣ *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  ಪ್ರತಿ(ptr = ಒಳಹರಿವು; *ptr; ptr++) {
    ವರ್ಣ ch = *ptr;
    ಯದಿ((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    ಯದಿ(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    ಯದಿ(result)
      ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
  }
  ಪ್ರತ್ಯಾಗಮ curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
ಸ್ಥಿರ CURLcode sendf(ಸಂರಚನಾ Curl_easy *data,
                      ಧ್ರುವ ವರ್ಣ *fmt, ...) CURL_PRINTF(2, 3);

ಸ್ಥಿರ CURLcode sendf(ಸಂರಚನಾ Curl_easy *data, ಧ್ರುವ ವರ್ಣ *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಙ್ಕ what;
  ವರ್ಣ *s;
  ವರ್ಣ *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  ಯದಿ(!s)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  ಪ್ರತಿ(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    ಯದಿ(result)
      ಭಙ್ಗ;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    ಯದಿ(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    ಅನ್ಯಥಾ
      ಭಙ್ಗ;

    timeout_ms = Curl_timeleft_ms(data);
    ಯದಿ(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ಭಙ್ಗ;
    }
    ಯದಿ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ಯದಿ(what < 0) {
      result = CURLE_SEND_ERROR;
      ಭಙ್ಗ;
    }
    ಅನ್ಯಥಾ ಯದಿ(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      ಭಙ್ಗ;
    }
  }

  curlx_free(s); /* free the output string */

  ಪ್ರತ್ಯಾಗಮ result;
}

ಸ್ಥಿರ CURLcode dict_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  ವರ್ಣ *word;
  ವರ್ಣ *eword = ರಿಕ್ತ;
  ವರ್ಣ *ppath;
  ವರ್ಣ *database = ರಿಕ್ತ;
  ವರ್ಣ *strategy = ರಿಕ್ತ;
  ವರ್ಣ *nthdef = ರಿಕ್ತ; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  ವರ್ಣ *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, ರಿಕ್ತ, REJECT_CTRL);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  ಯದಿ(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    ಯದಿ(word) {
      word++;
      database = strchr(word, ':');
      ಯದಿ(database) {
        *database++ = (ವರ್ಣ)0;
        strategy = strchr(database, ':');
        ಯದಿ(strategy) {
          *strategy++ = (ವರ್ಣ)0;
          nthdef = strchr(strategy, ':');
          ಯದಿ(nthdef) {
            *nthdef = (ವರ್ಣ)0;
          }
        }
      }
    }

    ಯದಿ(!word || (*word == (ವರ್ಣ)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (ವರ್ಣ)0)) ? "default" : word);
    ಯದಿ(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (ವರ್ಣ)0)) ? "!" : database,
                   (!strategy || (*strategy == (ವರ್ಣ)0)) ? "." : strategy,
                   eword);

    ಯದಿ(result) {
      failf(data, "Failed sending DICT request");
      ಗಚ್ಛ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  ಅನ್ಯಥಾ ಯದಿ(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    ಯದಿ(word) {
      word++;
      database = strchr(word, ':');
      ಯದಿ(database) {
        *database++ = (ವರ್ಣ)0;
        nthdef = strchr(database, ':');
        ಯದಿ(nthdef) {
          *nthdef = (ವರ್ಣ)0;
        }
      }
    }

    ಯದಿ(!word || (*word == (ವರ್ಣ)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (ವರ್ಣ)0)) ? "default" : word);
    ಯದಿ(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      ಗಚ್ಛ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (ವರ್ಣ)0)) ? "!" : database,
                   eword);

    ಯದಿ(result) {
      failf(data, "Failed sending DICT request");
      ಗಚ್ಛ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  ಅನ್ಯಥಾ {

    ppath = strchr(path, '/');
    ಯದಿ(ppath) {
      ಪೂರ್ಣಾಙ್ಕ i;

      ppath++;
      ಪ್ರತಿ(i = 0; ppath[i]; i++) {
        ಯದಿ(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      ಯದಿ(result) {
        failf(data, "Failed sending DICT request");
        ಗಚ್ಛ error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * DICT protocol
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_dict = {
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
