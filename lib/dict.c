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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#അന്തര്ഭാവ <sys/select.h>
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_UNISTD_H)
#അന്തര്ഭാവ <unistd.h>
#endif

#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
സ്ഥിതം അക്ഷരം *unescape_word(സ്ഥിരം അക്ഷരം *നിവേശം)
{
  ഘടന dynbuf out;
  സ്ഥിരം അക്ഷരം *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  വേണ്ടി(ptr = നിവേശം; *ptr; ptr++) {
    അക്ഷരം ch = *ptr;
    എങ്കിൽ((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    എങ്കിൽ(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    എങ്കിൽ(result)
      തിരികെ ശൂന്യ;
  }
  തിരികെ curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
സ്ഥിതം CURLcode sendf(ഘടന Curl_easy *data,
                      സ്ഥിരം അക്ഷരം *fmt, ...) CURL_PRINTF(2, 3);

സ്ഥിതം CURLcode sendf(ഘടന Curl_easy *data, സ്ഥിരം അക്ഷരം *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  പൂർണ്ണസംഖ്യ what;
  അക്ഷരം *s;
  അക്ഷരം *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  എങ്കിൽ(!s)
    തിരികെ CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  വേണ്ടി(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    എങ്കിൽ(result)
      നിർത്തുക;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    എങ്കിൽ(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    അല്ലെങ്കിൽ
      നിർത്തുക;

    timeout_ms = Curl_timeleft_ms(data);
    എങ്കിൽ(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    }
    എങ്കിൽ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    എങ്കിൽ(what < 0) {
      result = CURLE_SEND_ERROR;
      നിർത്തുക;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    }
  }

  curlx_free(s); /* free the output string */

  തിരികെ result;
}

സ്ഥിതം CURLcode dict_do(ഘടന Curl_easy *data, bool *done)
{
  അക്ഷരം *word;
  അക്ഷരം *eword = ശൂന്യ;
  അക്ഷരം *ppath;
  അക്ഷരം *database = ശൂന്യ;
  അക്ഷരം *strategy = ശൂന്യ;
  അക്ഷരം *nthdef = ശൂന്യ; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  അക്ഷരം *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, ശൂന്യ, REJECT_CTRL);
  എങ്കിൽ(result)
    തിരികെ result;

  എങ്കിൽ(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    എങ്കിൽ(word) {
      word++;
      database = strchr(word, ':');
      എങ്കിൽ(database) {
        *database++ = (അക്ഷരം)0;
        strategy = strchr(database, ':');
        എങ്കിൽ(strategy) {
          *strategy++ = (അക്ഷരം)0;
          nthdef = strchr(strategy, ':');
          എങ്കിൽ(nthdef) {
            *nthdef = (അക്ഷരം)0;
          }
        }
      }
    }

    എങ്കിൽ(!word || (*word == (അക്ഷരം)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (അക്ഷരം)0)) ? "default" : word);
    എങ്കിൽ(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (അക്ഷരം)0)) ? "!" : database,
                   (!strategy || (*strategy == (അക്ഷരം)0)) ? "." : strategy,
                   eword);

    എങ്കിൽ(result) {
      failf(data, "Failed sending DICT request");
      പോകുക error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  അല്ലെങ്കിൽ എങ്കിൽ(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    എങ്കിൽ(word) {
      word++;
      database = strchr(word, ':');
      എങ്കിൽ(database) {
        *database++ = (അക്ഷരം)0;
        nthdef = strchr(database, ':');
        എങ്കിൽ(nthdef) {
          *nthdef = (അക്ഷരം)0;
        }
      }
    }

    എങ്കിൽ(!word || (*word == (അക്ഷരം)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (അക്ഷരം)0)) ? "default" : word);
    എങ്കിൽ(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      പോകുക error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (അക്ഷരം)0)) ? "!" : database,
                   eword);

    എങ്കിൽ(result) {
      failf(data, "Failed sending DICT request");
      പോകുക error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  അല്ലെങ്കിൽ {

    ppath = strchr(path, '/');
    എങ്കിൽ(ppath) {
      പൂർണ്ണസംഖ്യ i;

      ppath++;
      വേണ്ടി(i = 0; ppath[i]; i++) {
        എങ്കിൽ(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      എങ്കിൽ(result) {
        failf(data, "Failed sending DICT request");
        പോകുക error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  തിരികെ result;
}

/*
 * DICT protocol
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_dict = {
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
