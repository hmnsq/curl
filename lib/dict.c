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

#यदि_अपरिभाषितम् CURL_DISABLE_DICT

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_SELECT_H
#अन्तर्भाव <sys/select.h>
#नोचेद्यदि defined(HAVE_UNISTD_H)
#अन्तर्भाव <unistd.h>
#यदि_अन्तः

#अन्तर्भाव "transfer.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "escape.h"

#परिभाषय DICT_मेलनम्   "/MATCH:"
#परिभाषय DICT_MATCH2  "/M:"
#परिभाषय DICT_MATCH3  "/FIND:"
#परिभाषय DICT_DEFINE  "/DEFINE:"
#परिभाषय DICT_DEFINE2 "/D:"
#परिभाषय DICT_DEFINE3 "/LOOKUP:"

#परिभाषय DYN_DICT_WORD 10000
स्थिर वर्ण *unescape_word(ध्रुव वर्ण *निवेशः)
{
  संरचना dynbuf बहिर्गमनम्;
  ध्रुव वर्ण *ptr;
  CURLcode परिणाम = CURLE_OK;
  curlx_dyn_init(&बहिर्गमनम्, DYN_DICT_WORD);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  प्रति(ptr = निवेशः; *ptr; ptr++) {
    वर्ण ch = *ptr;
    यदि((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, "\\", 1);
    यदि(!परिणाम)
      परिणाम = curlx_dyn_addn(&बहिर्गमनम्, ptr, 1);
    यदि(परिणाम)
      प्रत्यागम रिक्त;
  }
  प्रत्यागम curlx_dyn_ptr(&बहिर्गमनम्);
}

/* sendf() sends formatted data to the server */
स्थिर CURLcode sendf(संरचना Curl_easy *दत्त,
                      ध्रुव वर्ण *fmt, ...) CURL_PRINTF(2, 3);

स्थिर CURLcode sendf(संरचना Curl_easy *दत्त, ध्रुव वर्ण *fmt, ...)
{
  curl_socket_t sockfd = दत्त->conn->sock[FIRSTSOCKET];
  size_t अष्टकानि_written;
  size_t write_len;
  CURLcode परिणाम = CURLE_OK;
  timediff_t timeout_ms;
  पूर्णाङ्क what;
  वर्ण *s;
  वर्ण *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  यदि(!s)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः; /* failure */

  अष्टकानि_written = 0;
  write_len = वाक्यदैर्घ्यम्(s);
  sptr = s;

  प्रति(;;) {
    /* Write the buffer to the socket */
    परिणाम = Curl_xfer_send(दत्त, sptr, write_len, FALSE, &अष्टकानि_written);

    यदि(परिणाम)
      भङ्ग;

    Curl_debug(दत्त, CURLINFO_दत्त_बहिर्गमनम्, sptr, अष्टकानि_written);

    यदि(अष्टकानि_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= अष्टकानि_written;
      sptr += अष्टकानि_written;
    }
    अन्यथा
      भङ्ग;

    timeout_ms = Curl_timeleft_ms(दत्त);
    यदि(timeout_ms < 0) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }
    यदि(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    यदि(what < 0) {
      परिणाम = CURLE_SEND_ERROR;
      भङ्ग;
    }
    अन्यथा यदि(!what) {
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    }
  }

  curlx_free(s); /* free the output string */

  प्रत्यागम परिणाम;
}

स्थिर CURLcode dict_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  वर्ण *word;
  वर्ण *eword = रिक्त;
  वर्ण *ppath;
  वर्ण *database = रिक्त;
  वर्ण *strategy = रिक्त;
  वर्ण *nthdef = रिक्त; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode परिणाम;

  वर्ण *मार्गः;

  *समाप्तम् = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  परिणाम = Curl_urldecode(दत्त->state.ऊर्ध्वम्.मार्गः, 0, &मार्गः, रिक्त, REJECT_CTRL);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  यदि(curl_strnequal(मार्गः, DICT_मेलनम्, CURL_CSTRLEN(DICT_मेलनम्)) ||
     curl_strnequal(मार्गः, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(मार्गः, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    word = strchr(मार्गः, ':');
    यदि(word) {
      word++;
      database = strchr(word, ':');
      यदि(database) {
        *database++ = (वर्ण)0;
        strategy = strchr(database, ':');
        यदि(strategy) {
          *strategy++ = (वर्ण)0;
          nthdef = strchr(strategy, ':');
          यदि(nthdef) {
            *nthdef = (वर्ण)0;
          }
        }
      }
    }

    यदि(!word || (*word == (वर्ण)0)) {
      infof(दत्त, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (वर्ण)0)) ? "default" : word);
    यदि(!eword) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ error;
    }

    परिणाम = sendf(दत्त,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_संस्करणम् "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /* word */
                   "QUIT\r\n",
                   (!database || (*database == (वर्ण)0)) ? "!" : database,
                   (!strategy || (*strategy == (वर्ण)0)) ? "." : strategy,
                   eword);

    यदि(परिणाम) {
      failf(दत्त, "Failed sending DICT request");
      गच्छ error;
    }
    Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);
  }
  अन्यथा यदि(curl_strnequal(मार्गः, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(मार्गः, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(मार्गः, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    word = strchr(मार्गः, ':');
    यदि(word) {
      word++;
      database = strchr(word, ':');
      यदि(database) {
        *database++ = (वर्ण)0;
        nthdef = strchr(database, ':');
        यदि(nthdef) {
          *nthdef = (वर्ण)0;
        }
      }
    }

    यदि(!word || (*word == (वर्ण)0)) {
      infof(दत्त, "lookup word is missing");
    }
    eword = unescape_word((!word || (*word == (वर्ण)0)) ? "default" : word);
    यदि(!eword) {
      परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
      गच्छ error;
    }

    परिणाम = sendf(दत्त,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_संस्करणम् "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /* word */
                   "QUIT\r\n",
                   (!database || (*database == (वर्ण)0)) ? "!" : database,
                   eword);

    यदि(परिणाम) {
      failf(दत्त, "Failed sending DICT request");
      गच्छ error;
    }
    Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);
  }
  अन्यथा {

    ppath = strchr(मार्गः, '/');
    यदि(ppath) {
      पूर्णाङ्क i;

      ppath++;
      प्रति(i = 0; ppath[i]; i++) {
        यदि(ppath[i] == ':')
          ppath[i] = ' ';
      }
      परिणाम = sendf(दत्त,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_संस्करणम् "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      यदि(परिणाम) {
        failf(दत्त, "Failed sending DICT request");
        गच्छ error;
      }

      Curl_xfer_सज्जता_recv(दत्त, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(मार्गः);
  प्रत्यागम परिणाम;
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

#यदि_अन्तः /* CURL_DISABLE_DICT */
