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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#অন্তর্ভাৱ <sys/select.h>
#নোচেদ্যদি defined(HAVE_UNISTD_H)
#অন্তর্ভাৱ <unistd.h>
#endif

#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
স্থির ৱর্ণ *unescape_word(ধ্রুৱ ৱর্ণ *প্ৰবেশ)
{
  সংরচনা dynbuf out;
  ধ্রুৱ ৱর্ণ *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  বাবে(ptr = প্ৰবেশ; *ptr; ptr++) {
    ৱর্ণ ch = *ptr;
    যদি((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    যদি(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    যদি(result)
      ঘূৰাই রিক্ত;
  }
  ঘূৰাই curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
স্থির CURLcode sendf(সংরচনা Curl_easy *data,
                      ধ্রুৱ ৱর্ণ *fmt, ...) CURL_PRINTF(2, 3);

স্থির CURLcode sendf(সংরচনা Curl_easy *data, ধ্রুৱ ৱর্ণ *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  পূর্ণাঙ্ক what;
  ৱর্ণ *s;
  ৱর্ণ *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  যদি(!s)
    ঘূৰাই CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  বাবে(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    যদি(result)
      ভঙ্গ;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    যদি(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    অন্যথা
      ভঙ্গ;

    timeout_ms = Curl_timeleft_ms(data);
    যদি(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
    }
    যদি(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    যদি(what < 0) {
      result = CURLE_SEND_ERROR;
      ভঙ্গ;
    }
    অন্যথা যদি(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
    }
  }

  curlx_free(s); /* free the output string */

  ঘূৰাই result;
}

স্থির CURLcode dict_do(সংরচনা Curl_easy *data, bool *done)
{
  ৱর্ণ *word;
  ৱর্ণ *eword = রিক্ত;
  ৱর্ণ *ppath;
  ৱর্ণ *database = রিক্ত;
  ৱর্ণ *strategy = রিক্ত;
  ৱর্ণ *nthdef = রিক্ত; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  ৱর্ণ *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, রিক্ত, REJECT_CTRL);
  যদি(result)
    ঘূৰাই result;

  যদি(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    যদি(word) {
      word++;
      database = strchr(word, ':');
      যদি(database) {
        *database++ = (ৱর্ণ)0;
        strategy = strchr(database, ':');
        যদি(strategy) {
          *strategy++ = (ৱর্ণ)0;
          nthdef = strchr(strategy, ':');
          যদি(nthdef) {
            *nthdef = (ৱর্ণ)0;
          }
        }
      }
    }

    যদি(!word || (*word == (ৱর্ণ)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (ৱর্ণ)0)) ? "default" : word);
    যদি(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (ৱর্ণ)0)) ? "!" : database,
                   (!strategy || (*strategy == (ৱর্ণ)0)) ? "." : strategy,
                   eword);

    যদি(result) {
      failf(data, "Failed sending DICT request");
      গচ্ছ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  অন্যথা যদি(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    যদি(word) {
      word++;
      database = strchr(word, ':');
      যদি(database) {
        *database++ = (ৱর্ণ)0;
        nthdef = strchr(database, ':');
        যদি(nthdef) {
          *nthdef = (ৱর্ণ)0;
        }
      }
    }

    যদি(!word || (*word == (ৱর্ণ)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (ৱর্ণ)0)) ? "default" : word);
    যদি(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      গচ্ছ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (ৱর্ণ)0)) ? "!" : database,
                   eword);

    যদি(result) {
      failf(data, "Failed sending DICT request");
      গচ্ছ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  অন্যথা {

    ppath = strchr(path, '/');
    যদি(ppath) {
      পূর্ণাঙ্ক i;

      ppath++;
      বাবে(i = 0; ppath[i]; i++) {
        যদি(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      যদি(result) {
        failf(data, "Failed sending DICT request");
        গচ্ছ error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  ঘূৰাই result;
}

/*
 * DICT protocol
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_dict = {
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
