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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#অন্তর্ভাব <sys/select.h>
#নতুবা_যদি defined(HAVE_UNISTD_H)
#অন্তর্ভাব <unistd.h>
#endif

#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
স্থির অক্ষর *unescape_word(ধ্রুবক অক্ষর *প্রবেশ)
{
  কাঠামো dynbuf out;
  ধ্রুবক অক্ষর *ptr;
  CURLcode ফলাফল = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  জন্য(ptr = প্রবেশ; *ptr; ptr++) {
    অক্ষর ch = *ptr;
    যদি((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      ফলাফল = curlx_dyn_addn(&out, "\\", 1);
    যদি(!ফলাফল)
      ফলাফল = curlx_dyn_addn(&out, ptr, 1);
    যদি(ফলাফল)
      ফেরত ফাঁকা;
  }
  ফেরত curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
স্থির CURLcode sendf(কাঠামো Curl_easy *তথ্য,
                      ধ্রুবক অক্ষর *fmt, ...) CURL_PRINTF(2, 3);

স্থির CURLcode sendf(কাঠামো Curl_easy *তথ্য, ধ্রুবক অক্ষর *fmt, ...)
{
  curl_socket_t sockfd = তথ্য->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode ফলাফল = CURLE_OK;
  timediff_t timeout_ms;
  পূর্ণসংখ্যা what;
  অক্ষর *s;
  অক্ষর *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  যদি(!s)
    ফেরত CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  জন্য(;;) {
    /* Write the buffer to the socket */
    ফলাফল = Curl_xfer_send(তথ্য, sptr, write_len, FALSE, &bytes_written);

    যদি(ফলাফল)
      বিরতি;

    Curl_debug(তথ্য, CURLINFO_তথ্য_OUT, sptr, bytes_written);

    যদি(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    অন্যথা
      বিরতি;

    timeout_ms = Curl_timeleft_ms(তথ্য);
    যদি(timeout_ms < 0) {
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      বিরতি;
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
      ফলাফল = CURLE_SEND_ERROR;
      বিরতি;
    }
    অন্যথা যদি(!what) {
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      বিরতি;
    }
  }

  curlx_free(s); /* free the output string */

  ফেরত ফলাফল;
}

স্থির CURLcode dict_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  অক্ষর *word;
  অক্ষর *eword = ফাঁকা;
  অক্ষর *ppath;
  অক্ষর *database = ফাঁকা;
  অক্ষর *strategy = ফাঁকা;
  অক্ষর *nthdef = ফাঁকা; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode ফলাফল;

  অক্ষর *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  ফলাফল = Curl_urldecode(তথ্য->state.up.path, 0, &path, ফাঁকা, REJECT_CTRL);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  যদি(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    যদি(word) {
      word++;
      database = strchr(word, ':');
      যদি(database) {
        *database++ = (অক্ষর)0;
        strategy = strchr(database, ':');
        যদি(strategy) {
          *strategy++ = (অক্ষর)0;
          nthdef = strchr(strategy, ':');
          যদি(nthdef) {
            *nthdef = (অক্ষর)0;
          }
        }
      }
    }

    যদি(!word || (*word == (অক্ষর)0)) {
      infof(তথ্য, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (অক্ষর)0)) ? "default" : word);
    যদি(!eword) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও error;
    }

    ফলাফল = sendf(তথ্য,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (অক্ষর)0)) ? "!" : database,
                   (!strategy || (*strategy == (অক্ষর)0)) ? "." : strategy,
                   eword);

    যদি(ফলাফল) {
      failf(তথ্য, "Failed sending DICT request");
      যাও error;
    }
    Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, -1);
  }
  অন্যথা যদি(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    যদি(word) {
      word++;
      database = strchr(word, ':');
      যদি(database) {
        *database++ = (অক্ষর)0;
        nthdef = strchr(database, ':');
        যদি(nthdef) {
          *nthdef = (অক্ষর)0;
        }
      }
    }

    যদি(!word || (*word == (অক্ষর)0)) {
      infof(তথ্য, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (অক্ষর)0)) ? "default" : word);
    যদি(!eword) {
      ফলাফল = CURLE_OUT_OF_MEMORY;
      যাও error;
    }

    ফলাফল = sendf(তথ্য,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (অক্ষর)0)) ? "!" : database,
                   eword);

    যদি(ফলাফল) {
      failf(তথ্য, "Failed sending DICT request");
      যাও error;
    }
    Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, -1);
  }
  অন্যথা {

    ppath = strchr(path, '/');
    যদি(ppath) {
      পূর্ণসংখ্যা i;

      ppath++;
      জন্য(i = 0; ppath[i]; i++) {
        যদি(ppath[i] == ':')
          ppath[i] = ' ';
      }
      ফলাফল = sendf(তথ্য,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      যদি(ফলাফল) {
        failf(তথ্য, "Failed sending DICT request");
        যাও error;
      }

      Curl_xfer_setup_recv(তথ্য, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  ফেরত ফলাফল;
}

/*
 * DICT protocol
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_dict = {
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
