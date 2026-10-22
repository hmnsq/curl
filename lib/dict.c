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
#अन्तर्भाव "curl_setup.h"
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "dict.h"

#ifndef CURL_DISABLE_DICT

#ifdef HAVE_NETINET_IN_H
#अन्तर्भाव <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#endif

#ifdef HAVE_SYS_SELECT_H
#अन्तर्भाव <sys/select.h>
#नोचेद्यदि defined(HAVE_UNISTD_H)
#अन्तर्भाव <unistd.h>
#endif

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "escape.h"

#define DICT_MATCH   "/MATCH:"
#define DICT_MATCH2  "/M:"
#define DICT_MATCH3  "/FIND:"
#define DICT_DEFINE  "/DEFINE:"
#define DICT_DEFINE2 "/D:"
#define DICT_DEFINE3 "/LOOKUP:"

#define DYN_DICT_WORD 10000
स्थिर वर्ण *unescape_word(ध्रुव वर्ण *निवेशः)
{
  संरचना dynbuf out;
  ध्रुव वर्ण *ptr;
  CURLcode result = CURLE_OK;
  curlx_dyn_init(&out, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  अत्थाय(ptr = निवेशः; *ptr; ptr++) {
    वर्ण ch = *ptr;
    सचे((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      result = curlx_dyn_addn(&out, "\\", 1);
    सचे(!result)
      result = curlx_dyn_addn(&out, ptr, 1);
    सचे(result)
      पटिनिवत्त रिक्त;
  }
  पटिनिवत्त curlx_dyn_ptr(&out);
}

/* sendf() sends formatted data to the server */
स्थिर CURLcode sendf(संरचना Curl_easy *data,
                      ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);

स्थिर CURLcode sendf(संरचना Curl_easy *data, ध्रुव वर्ण *fmt, ...)
{
  curl_socket_t sockfd = data->conn->sock[FIRSTSOCKET];
  size_t bytes_written;
  size_t write_len;
  CURLcode result = CURLE_OK;
  timediff_t timeout_ms;
  पूर्णाङ्क what;
  वर्ण *s;
  वर्ण *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  सचे(!s)
    पटिनिवत्त CURLE_OUT_OF_MEMORY; /* failure */

  bytes_written = 0;
  write_len = strlen(s);
  sptr = s;

  अत्थाय(;;) {
    /* Write the buffer to the socket */
    result = Curl_xfer_send(data, sptr, write_len, FALSE, &bytes_written);

    सचे(result)
      भङ्ग;

    Curl_debug(data, CURLINFO_DATA_OUT, sptr, bytes_written);

    सचे(bytes_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= bytes_written;
      sptr += bytes_written;
    }
    अञ्ञथा
      भङ्ग;

    timeout_ms = Curl_timeleft_ms(data);
    सचे(timeout_ms < 0) {
      result = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }
    सचे(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    सचे(what < 0) {
      result = CURLE_SEND_ERROR;
      भङ्ग;
    }
    अञ्ञथा सचे(!what) {
      result = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }
  }

  curlx_free(s); /* free the output string */

  पटिनिवत्त result;
}

स्थिर CURLcode dict_do(संरचना Curl_easy *data, bool *done)
{
  वर्ण *word;
  वर्ण *eword = रिक्त;
  वर्ण *ppath;
  वर्ण *database = रिक्त;
  वर्ण *strategy = रिक्त;
  वर्ण *nthdef = रिक्त; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode result;

  वर्ण *path;

  *done = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  result = Curl_urldecode(data->state.up.path, 0, &path, रिक्त, REJECT_CTRL);
  सचे(result)
    पटिनिवत्त result;

  सचे(curl_strnequal(path, DICT_MATCH, CURL_CSTRLEN(DICT_MATCH)) ||
     curl_strnequal(path, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(path, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(path, ':');
    सचे(word) {
      word++;
      database = strchr(word, ':');
      सचे(database) {
        *database++ = (वर्ण)0;
        strategy = strchr(database, ':');
        सचे(strategy) {
          *strategy++ = (वर्ण)0;
          nthdef = strchr(strategy, ':');
          सचे(nthdef) {
            *nthdef = (वर्ण)0;
          }
        }
      }
    }

    सचे(!word || (*word == (वर्ण)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (वर्ण)0)) ? "default" : word);
    सचे(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (वर्ण)0)) ? "!" : database,
                   (!strategy || (*strategy == (वर्ण)0)) ? "." : strategy,
                   eword);

    सचे(result) {
      failf(data, "Failed sending DICT request");
      गच्छ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  अञ्ञथा सचे(curl_strnequal(path, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(path, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(path, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(path, ':');
    सचे(word) {
      word++;
      database = strchr(word, ':');
      सचे(database) {
        *database++ = (वर्ण)0;
        nthdef = strchr(database, ':');
        सचे(nthdef) {
          *nthdef = (वर्ण)0;
        }
      }
    }

    सचे(!word || (*word == (वर्ण)0)) {
      infof(data, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (वर्ण)0)) ? "default" : word);
    सचे(!eword) {
      result = CURLE_OUT_OF_MEMORY;
      गच्छ error;
    }

    result = sendf(data,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (वर्ण)0)) ? "!" : database,
                   eword);

    सचे(result) {
      failf(data, "Failed sending DICT request");
      गच्छ error;
    }
    Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
  }
  अञ्ञथा {

    ppath = strchr(path, '/');
    सचे(ppath) {
      पूर्णाङ्क i;

      ppath++;
      अत्थाय(i = 0; ppath[i]; i++) {
        सचे(ppath[i] == ':')
          ppath[i] = ' ';
      }
      result = sendf(data,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_VERSION "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      सचे(result) {
        failf(data, "Failed sending DICT request");
        गच्छ error;
      }

      Curl_xfer_setup_recv(data, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(path);
  पटिनिवत्त result;
}

/*
 * DICT protocol
 */
ध्रुव संरचना Curl_protocol Curl_protocol_dict = {
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
