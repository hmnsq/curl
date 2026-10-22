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
#અન્તર્ભાવ "curl_setup.h"
#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#અન્તર્ભાવ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#અન્તર્ભાવ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#અન્તર્ભાવ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#અન્તર્ભાવ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#અન્તર્ભાવ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#અન્તર્ભાવ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#અન્તર્ભાવ <sys/select.h>
#અથવાજો defined(HAVE_UNISTD_H)
#અન્તર્ભાવ <unistd.h>
#endif

#અન્તર્ભાવ "transfer.h"
#અન્તર્ભાવ "curl_trc.h"
#અન્તર્ભાવ "connect.h"
#અન્તર્ભાવ "select.h"
#અન્તર્ભાવ "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
સ્થિર અક્ષર *unescape_word(અચલ અક્ષર *આગત)
{
  સંરચના dynbuf out;
  અચલ અક્ષર *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  માટે(ptr = આગત; *ptr; ptr++) {
    અક્ષર ch = *ptr;
    જો((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    જો(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    જો(result)
      પાછા ખાલી;
  }
  પાછા curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
સ્થિર CURLcode sendf(સંરચના Curl_easy *data,
                      અચલ અક્ષર *fmt, ...) CURL_PRINTF(2, 3);

સ્થિર CURLcode sendf(સંરચના Curl_easy *data, અચલ અક્ષર *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  પૂર્ણાંક what;
  અક્ષર *s;
  અક્ષર *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  જો(!s)
    પાછા CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  માટે(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    જો(result)
      તોડો;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    જો(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    નહીંતર
      તોડો;

    timeout_ms = Curl_timeleft_ms(data);
    જો(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    }
    જો(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    જો(what < 0) {
      result = CURLE_SEND_ERROR;
      તોડો;
    }
    નહીંતર જો(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      તોડો;
    }
  }

  curlx_free(s); /* free the output string */

  પાછા result;
}

સ્થિર CURLcode dict_do(સંરચના Curl_easy *data, bool *done)
{
  અક્ષર *word;
  અક્ષર *eword = ખાલી;
  અક્ષર *ppath;
  અક્ષર *database = ખાલી;
  અક્ષર *strategy = ખાલી;
  અક્ષર *nthdef = ખાલી; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  અક્ષર *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, ખાલી, REJECT_CTRL);
  જો(result)
    પાછા result;

  જો(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    જો(word) {
      word++;
      database = strchr(word, ':');
      જો(database) {
        *database++ = (અક્ષર)0;
        strategy = strchr(database, ':');
        જો(strategy) {
          *strategy++ = (અક્ષર)0;
          nthdef = strchr(strategy, ':');
          જો(nthdef) {
            *nthdef = (અક્ષર)0;
          }
        }
      }
    }

    જો(!word || (*word == (અક્ષર)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (અક્ષર)0)) ? "default" : word);
    જો(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (અક્ષર)0)) ? "!" : database,
                   (!strategy || (*strategy == (અક્ષર)0)) ? "." : strategy,
                   eword);

    જો(result) {
      failf(data, "Failed sending DICT request");
      જાઓ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  નહીંતર જો(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    જો(word) {
      word++;
      database = strchr(word, ':');
      જો(database) {
        *database++ = (અક્ષર)0;
        nthdef = strchr(database, ':');
        જો(nthdef) {
          *nthdef = (અક્ષર)0;
        }
      }
    }

    જો(!word || (*word == (અક્ષર)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (અક્ષર)0)) ? "default" : word);
    જો(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      જાઓ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (અક્ષર)0)) ? "!" : database,
                   eword);

    જો(result) {
      failf(data, "Failed sending DICT request");
      જાઓ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  નહીંતર {

    ppath = strchr(path, '/');
    જો(ppath) {
      પૂર્ણાંક i;

      ppath++;
      માટે(i = 0; ppath[i]; i++) {
        જો(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      જો(result) {
        failf(data, "Failed sending DICT request");
        જાઓ error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  પાછા result;
}

/*
 * DICT protocol
 */
અચલ સંરચના Curl_protocol Curl_protocol_dict = {
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
