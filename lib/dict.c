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
#అన్తర్భావ "curl_setup.h"
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "dict.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_DICT

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_SELECT_H
#అన్తర్భావ <sys/select.h>
#లేదావేళ defined(HAVE_UNISTD_H)
#అన్తర్భావ <unistd.h>
#అయితే_ముగింపు

#అన్తర్భావ "transfer.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "escape.h"

#నిర్వచించు DICT_సరిపోలిక   "/MATCH:"
#నిర్వచించు DICT_MATCH2  "/M:"
#నిర్వచించు DICT_MATCH3  "/FIND:"
#నిర్వచించు DICT_DEFINE  "/DEFINE:"
#నిర్వచించు DICT_DEFINE2 "/D:"
#నిర్వచించు DICT_DEFINE3 "/LOOKUP:"

#నిర్వచించు DYN_DICT_మాట 10000
స్థిర అక్షరం *unescape_మాట(స్థిరరాశి అక్షరం *ఆదానం)
{
  నిర్మాణం dynbuf బహిర్గతం;
  స్థిరరాశి అక్షరం *ptr;
  CURLcode ఫలితం = CURLE_OK;
  curlx_dyn_init(&బహిర్గతం, DYN_DICT_మాట);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  కొరకు(ptr = ఆదానం; *ptr; ptr++) {
    అక్షరం ch = *ptr;
    ఒకవేళ((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, "\\", 1);
    ఒకవేళ(!ఫలితం)
      ఫలితం = curlx_dyn_addn(&బహిర్గతం, ptr, 1);
    ఒకవేళ(ఫలితం)
      తిరిగి ఖాళీ;
  }
  తిరిగి curlx_dyn_ptr(&బహిర్గతం);
}

/* sendf() sends formatted data to the server */
స్థిర CURLcode sendf(నిర్మాణం Curl_easy *దత్తాంశం,
                      స్థిరరాశి అక్షరం *fmt, ...) CURL_PRINTF(2, 3);

స్థిర CURLcode sendf(నిర్మాణం Curl_easy *దత్తాంశం, స్థిరరాశి అక్షరం *fmt, ...)
{
  curl_socket_t sockfd = దత్తాంశం->conn->sock[FIRSTSOCKET];
  size_t బైట్లు_written;
  size_t write_len;
  CURLcode ఫలితం = CURLE_OK;
  timediff_t timeout_ms;
  పూర్ణాంకం what;
  అక్షరం *s;
  అక్షరం *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  ఒకవేళ(!s)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం; /* failure */

  బైట్లు_written = 0;
  write_len = అక్షరశ్రేణి_పొడవు(s);
  sptr = s;

  కొరకు(;;) {
    /* Write the buffer to the socket */
    ఫలితం = Curl_xfer_send(దత్తాంశం, sptr, write_len, FALSE, &బైట్లు_written);

    ఒకవేళ(ఫలితం)
      ఆపు;

    Curl_debug(దత్తాంశం, CURLINFO_దత్తాంశం_బహిర్గతం, sptr, బైట్లు_written);

    ఒకవేళ(బైట్లు_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= బైట్లు_written;
      sptr += బైట్లు_written;
    }
    లేకపోతే
      ఆపు;

    timeout_ms = Curl_timeleft_ms(దత్తాంశం);
    ఒకవేళ(timeout_ms < 0) {
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    }
    ఒకవేళ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ఒకవేళ(what < 0) {
      ఫలితం = CURLE_SEND_ERROR;
      ఆపు;
    }
    లేకపోతే ఒకవేళ(!what) {
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    }
  }

  curlx_free(s); /* free the output string */

  తిరిగి ఫలితం;
}

స్థిర CURLcode dict_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  అక్షరం *మాట;
  అక్షరం *eword = ఖాళీ;
  అక్షరం *ppath;
  అక్షరం *database = ఖాళీ;
  అక్షరం *strategy = ఖాళీ;
  అక్షరం *nthdef = ఖాళీ; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode ఫలితం;

  అక్షరం *మార్గం;

  *పూర్తి = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  ఫలితం = Curl_urldecode(దత్తాంశం->state.పైకి.మార్గం, 0, &మార్గం, ఖాళీ, REJECT_CTRL);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  ఒకవేళ(curl_strnequal(మార్గం, DICT_సరిపోలిక, CURL_CSTRLEN(DICT_సరిపోలిక)) ||
     curl_strnequal(మార్గం, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(మార్గం, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    మాట = strchr(మార్గం, ':');
    ఒకవేళ(మాట) {
      మాట++;
      database = strchr(మాట, ':');
      ఒకవేళ(database) {
        *database++ = (అక్షరం)0;
        strategy = strchr(database, ':');
        ఒకవేళ(strategy) {
          *strategy++ = (అక్షరం)0;
          nthdef = strchr(strategy, ':');
          ఒకవేళ(nthdef) {
            *nthdef = (అక్షరం)0;
          }
        }
      }
    }

    ఒకవేళ(!మాట || (*మాట == (అక్షరం)0)) {
      infof(దత్తాంశం, "lookup word is missing");
    }
    eword = unescape_మాట((!మాట || (*మాట == (అక్షరం)0)) ? "default" : మాట);
    ఒకవేళ(!eword) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు error;
    }

    ఫలితం = sendf(దత్తాంశం,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_సంస్కరణ "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*మాట*/
                   "QUIT\r\n",
                   (!database || (*database == (అక్షరం)0)) ? "!" : database,
                   (!strategy || (*strategy == (అక్షరం)0)) ? "." : strategy,
                   eword);

    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Failed sending DICT request");
      వెళ్ళు error;
    }
    Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);
  }
  లేకపోతే ఒకవేళ(curl_strnequal(మార్గం, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(మార్గం, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(మార్గం, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    మాట = strchr(మార్గం, ':');
    ఒకవేళ(మాట) {
      మాట++;
      database = strchr(మాట, ':');
      ఒకవేళ(database) {
        *database++ = (అక్షరం)0;
        nthdef = strchr(database, ':');
        ఒకవేళ(nthdef) {
          *nthdef = (అక్షరం)0;
        }
      }
    }

    ఒకవేళ(!మాట || (*మాట == (అక్షరం)0)) {
      infof(దత్తాంశం, "lookup word is missing");
    }
    eword = unescape_మాట((!మాట || (*మాట == (అక్షరం)0)) ? "default" : మాట);
    ఒకవేళ(!eword) {
      ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
      వెళ్ళు error;
    }

    ఫలితం = sendf(దత్తాంశం,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_సంస్కరణ "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*మాట*/
                   "QUIT\r\n",
                   (!database || (*database == (అక్షరం)0)) ? "!" : database,
                   eword);

    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Failed sending DICT request");
      వెళ్ళు error;
    }
    Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);
  }
  లేకపోతే {

    ppath = strchr(మార్గం, '/');
    ఒకవేళ(ppath) {
      పూర్ణాంకం i;

      ppath++;
      కొరకు(i = 0; ppath[i]; i++) {
        ఒకవేళ(ppath[i] == ':')
          ppath[i] = ' ';
      }
      ఫలితం = sendf(దత్తాంశం,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_సంస్కరణ "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      ఒకవేళ(ఫలితం) {
        failf(దత్తాంశం, "Failed sending DICT request");
        వెళ్ళు error;
      }

      Curl_xfer_అమరిక_recv(దత్తాంశం, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(మార్గం);
  తిరిగి ఫలితం;
}

/*
 * DICT protocol
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_dict = {
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

#అయితే_ముగింపు /* CURL_DISABLE_DICT */
