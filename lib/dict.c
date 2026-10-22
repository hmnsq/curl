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
#include "curl_setup.h"
#include "urldata.h"
#include "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#include <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#include <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#include <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#include <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#include <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#elif defined(HAVE_UNISTD_H)
#include <unistd.h>
#endif

#include "transfer.h"
#include "curl_trc.h"
#include "connect.h"
#include "select.h"
#include "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
static var_na *unescape_word(const var_na *input)
{
  sa_nracanaa dynbuf out;
  const var_na *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  ꦏꦁꦒꦺꦴ(ptr = input; *ptr; ptr++) {
    var_na ch = *ptr;
    ꦪꦼꦤ꧀((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    ꦪꦼꦤ꧀(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    ꦪꦼꦤ꧀(result)
      ꦮꦭꦶ NULL;
  }
  ꦮꦭꦶ curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
static CURLcode sendf(sa_nracanaa Curl_easy *data,
                      const var_na *fmt, ...) CURL_PRINTF(2, 3);

static CURLcode sendf(sa_nracanaa Curl_easy *data, const var_na *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  puur_naa_nka what;
  var_na *s;
  var_na *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  ꦪꦼꦤ꧀(!s)
    ꦮꦭꦶ CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  ꦏꦁꦒꦺꦴ(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    ꦪꦼꦤ꧀(result)
      bha_nga;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    ꦪꦼꦤ꧀(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    ꦭꦶꦪ
      bha_nga;

    timeout_ms = Curl_timeleft_ms(data);
    ꦪꦼꦤ꧀(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      bha_nga;
    }
    ꦪꦼꦤ꧀(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ꦪꦼꦤ꧀(what < 0) {
      result = CURLE_SEND_ERROR;
      bha_nga;
    }
    ꦭꦶꦪ ꦪꦼꦤ꧀(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      bha_nga;
    }
  }

  curlx_free(s); /* free the output string */

  ꦮꦭꦶ result;
}

static CURLcode dict_do(sa_nracanaa Curl_easy *data, bool *done)
{
  var_na *word;
  var_na *eword = NULL;
  var_na *ppath;
  var_na *database = NULL;
  var_na *strategy = NULL;
  var_na *nthdef = NULL; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  var_na *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, NULL, REJECT_CTRL);
  ꦪꦼꦤ꧀(result)
    ꦮꦭꦶ result;

  ꦪꦼꦤ꧀(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    ꦪꦼꦤ꧀(word) {
      word++;
      database = strchr(word, ':');
      ꦪꦼꦤ꧀(database) {
        *database++ = (var_na)0;
        strategy = strchr(database, ':');
        ꦪꦼꦤ꧀(strategy) {
          *strategy++ = (var_na)0;
          nthdef = strchr(strategy, ':');
          ꦪꦼꦤ꧀(nthdef) {
            *nthdef = (var_na)0;
          }
        }
      }
    }

    ꦪꦼꦤ꧀(!word || (*word == (var_na)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (var_na)0)) ? "default" : word);
    ꦪꦼꦤ꧀(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      goto error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (var_na)0)) ? "!" : database,
                   (!strategy || (*strategy == (var_na)0)) ? "." : strategy,
                   eword);

    ꦪꦼꦤ꧀(result) {
      failf(data, "Failed sending DICT request");
      goto error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  ꦭꦶꦪ ꦪꦼꦤ꧀(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    ꦪꦼꦤ꧀(word) {
      word++;
      database = strchr(word, ':');
      ꦪꦼꦤ꧀(database) {
        *database++ = (var_na)0;
        nthdef = strchr(database, ':');
        ꦪꦼꦤ꧀(nthdef) {
          *nthdef = (var_na)0;
        }
      }
    }

    ꦪꦼꦤ꧀(!word || (*word == (var_na)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (var_na)0)) ? "default" : word);
    ꦪꦼꦤ꧀(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      goto error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (var_na)0)) ? "!" : database,
                   eword);

    ꦪꦼꦤ꧀(result) {
      failf(data, "Failed sending DICT request");
      goto error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  ꦭꦶꦪ {

    ppath = strchr(path, '/');
    ꦪꦼꦤ꧀(ppath) {
      puur_naa_nka i;

      ppath++;
      ꦏꦁꦒꦺꦴ(i = 0; ppath[i]; i++) {
        ꦪꦼꦤ꧀(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      ꦪꦼꦤ꧀(result) {
        failf(data, "Failed sending DICT request");
        goto error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  ꦮꦭꦶ result;
}

/*
 * DICT protocol
 */
const sa_nracanaa Curl_protocol Curl_protocol_dict = {
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
