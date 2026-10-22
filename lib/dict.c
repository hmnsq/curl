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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "dict.h"

#अगर_अघोषित CURL_DISABLE_DICT

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_SELECT_H
#समावेश <sys/select.h>
#या_अगर defined(HAVE_UNISTD_H)
#समावेश <unistd.h>
#पूर्ण_अगर

#समावेश "transfer.h"
#समावेश "curl_trc.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "escape.h"

#घोषणा DICT_MATCH   "/MATCH:"
#घोषणा DICT_MATCH2  "/M:"
#घोषणा DICT_MATCH3  "/FIND:"
#घोषणा DICT_DEFINE  "/DEFINE:"
#घोषणा DICT_DEFINE2 "/D:"
#घोषणा DICT_DEFINE3 "/LOOKUP:"

#घोषणा DYN_DICT_WORD 10000
अटल अक्षर *unescape_word(स्थिर अक्षर *निवेश)
{
  काष्ठा dynbuf out;
  स्थिर अक्षर *ptr;
  CURLcode परिणाम = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  क्रम(ptr = निवेश; *ptr; ptr++) {
    अक्षर ch = *ptr;
    अगर((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      परिणाम = curlx_dyn_addn(&out, "\\", 1);
    अगर(!परिणाम)
      परिणाम = curlx_dyn_addn(&out, ptr, 1);
    अगर(परिणाम)
      वापस शून्य;
  }
  वापस curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
अटल CURLcode sendf(काष्ठा Curl_easy *तथ्य,
                      स्थिर अक्षर *fmt, ...) CURL_PRINTF(2, 3);

अटल CURLcode sendf(काष्ठा Curl_easy *तथ्य, स्थिर अक्षर *fmt, ...)
{
  curl_socket_t sockfd = तथ्य->conn->sock[FIRSTSOCKET];
  माप_प्रकार bytes_written;
  माप_प्रकार write_len;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;
  पूर्णांक what;
  अक्षर *s;
  अक्षर *sptr;
  बहु_सूची ap;
  बहु_शुरू(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  बहु_पूर्ण(ap);
  अगर(!s)
    वापस CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = म_माप(s);
  sptr = s;

  क्रम(;;) {
    /* Write the buffer to the socket */
    परिणाम = Curl_xfer_send(तथ्य, sptr, write_len, FALSE, &bytes_written);

    अगर(परिणाम)
      अवरोध;

    Curl_debug(तथ्य, CURLINFO_तथ्य_OUT, sptr, bytes_written);

    अगर(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    अन्यथा
      अवरोध;

    timeout_ms = Curl_timeleft_ms(तथ्य);
    अगर(timeout_ms < 0) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    }
    अगर(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    अगर(what < 0) {
      परिणाम = CURLE_SEND_ERROR;
      अवरोध;
    }
    अन्यथा अगर(!what) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    }
  }

  curlx_free(s); /* free the output string */

  वापस परिणाम;
}

अटल CURLcode dict_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  अक्षर *word;
  अक्षर *eword = शून्य;
  अक्षर *ppath;
  अक्षर *database = शून्य;
  अक्षर *strategy = शून्य;
  अक्षर *nthdef = शून्य; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode परिणाम;

  अक्षर *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  परिणाम = Curl_urldecode(तथ्य->state.up.path, 0, &path, शून्य, REJECT_CTRL);
  अगर(परिणाम)
    वापस परिणाम;

  अगर(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = म_अक्षर(path, ':');
    अगर(word) {
      word++;
      database = म_अक्षर(word, ':');
      अगर(database) {
        *database++ = (अक्षर)0;
        strategy = म_अक्षर(database, ':');
        अगर(strategy) {
          *strategy++ = (अक्षर)0;
          nthdef = म_अक्षर(strategy, ':');
          अगर(nthdef) {
            *nthdef = (अक्षर)0;
          }
        }
      }
    }

    अगर(!word || (*word == (अक्षर)0)) {
      infof(तथ्य, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (अक्षर)0)) ? "default" : word);
    अगर(!eword) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ error;
    }

    परिणाम = sendf(तथ्य,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (अक्षर)0)) ? "!" : database,
                   (!strategy || (*strategy == (अक्षर)0)) ? "." : strategy,
                   eword);

    अगर(परिणाम) {
      failf(तथ्य, "Failed sending DICT request");
      जाओ error;
    }
    Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);
  }
  अन्यथा अगर(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = म_अक्षर(path, ':');
    अगर(word) {
      word++;
      database = म_अक्षर(word, ':');
      अगर(database) {
        *database++ = (अक्षर)0;
        nthdef = म_अक्षर(database, ':');
        अगर(nthdef) {
          *nthdef = (अक्षर)0;
        }
      }
    }

    अगर(!word || (*word == (अक्षर)0)) {
      infof(तथ्य, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (अक्षर)0)) ? "default" : word);
    अगर(!eword) {
      परिणाम = CURLE_OUT_OF_MEMORY;
      जाओ error;
    }

    परिणाम = sendf(तथ्य,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (अक्षर)0)) ? "!" : database,
                   eword);

    अगर(परिणाम) {
      failf(तथ्य, "Failed sending DICT request");
      जाओ error;
    }
    Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);
  }
  अन्यथा {

    ppath = म_अक्षर(path, '/');
    अगर(ppath) {
      पूर्णांक i;

      ppath++;
      क्रम(i = 0; ppath[i]; i++) {
        अगर(ppath[i] == ':')
          ppath[i] = ' ';
      }
      परिणाम = sendf(तथ्य,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      अगर(परिणाम) {
        failf(तथ्य, "Failed sending DICT request");
        जाओ error;
      }

      Curl_xfer_setup_recv(तथ्य, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  वापस परिणाम;
}

/*
 * DICT protocol
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_dict = {
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

#पूर्ण_अगर /* CURL_DISABLE_DICT */
