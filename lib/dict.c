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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_DICT

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_SELECT_H
#ಅನ್ತರ್ಭಾವ <sys/select.h>
#ಅಥವಾಆದರೆ defined(HAVE_UNISTD_H)
#ಅನ್ತರ್ಭಾವ <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "escape.h"

#ವ್ಯಾಖ್ಯಾನಿಸು DICT_ಹೊಂದಿಕೆ   "/MATCH:"
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_MATCH2  "/M:"
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_MATCH3  "/FIND:"
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_DEFINE  "/DEFINE:"
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_DEFINE2 "/D:"
#ವ್ಯಾಖ್ಯಾನಿಸು DICT_DEFINE3 "/LOOKUP:"

#ವ್ಯಾಖ್ಯಾನಿಸು DYN_DICT_ಶಬ್ದ 10000
ಸ್ಥಿರ ಅಕ್ಷರ *unescape_ಶಬ್ದ(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಒಳಹರಿವು)
{
  ರಚನೆ dynbuf ಹೊರಹೋಗು;
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  curlx_dyn_init(&ಹೊರಹೋಗು, DYN_DICT_ಶಬ್ದ);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  ಗಾಗಿ(ptr = ಒಳಹರಿವು; *ptr; ptr++) {
    ಅಕ್ಷರ ch = *ptr;
    ಆದರೆ((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, "\\", 1);
    ಆದರೆ(!ಫಲಿತಾಂಶ)
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&ಹೊರಹೋಗು, ptr, 1);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      ಹಿಂತಿರುಗಿ ಖಾಲಿ;
  }
  ಹಿಂತಿರುಗಿ curlx_dyn_ptr(&ಹೊರಹೋಗು);
}

/* sendf() sends formatted data to the server */
ಸ್ಥಿರ CURLcode sendf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...) CURL_PRINTF(2, 3);

ಸ್ಥಿರ CURLcode sendf(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt, ...)
{
  curl_socket_t sockfd = ದತ್ತಾಂಶ->conn->sock[FIRSTSOCKET];
  size_t ಬೈಟುಗಳು_written;
  size_t write_len;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  timediff_t timeout_ms;
  ಪೂರ್ಣಾಂಕ what;
  ಅಕ್ಷರ *s;
  ಅಕ್ಷರ *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  ಆದರೆ(!s)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ; /* failure */

  ಬೈಟುಗಳು_written = 0;
  write_len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(s);
  sptr = s;

  ಗಾಗಿ(;;) {
    /* Write the buffer to the socket */
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, sptr, write_len, FALSE, &ಬೈಟುಗಳು_written);

    ಆದರೆ(ಫಲಿತಾಂಶ)
      ನಿಲ್ಲಿಸು;

    Curl_debug(ದತ್ತಾಂಶ, CURLINFO_ದತ್ತಾಂಶ_ಹೊರಹೋಗು, sptr, ಬೈಟುಗಳು_written);

    ಆದರೆ(ಬೈಟುಗಳು_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= ಬೈಟುಗಳು_written;
      sptr += ಬೈಟುಗಳು_written;
    }
    ಇಲ್ಲದಿದ್ದರೆ
      ನಿಲ್ಲಿಸು;

    timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
    ಆದರೆ(timeout_ms < 0) {
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    }
    ಆದರೆ(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    ಆದರೆ(what < 0) {
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ನಿಲ್ಲಿಸು;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!what) {
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    }
  }

  curlx_free(s); /* free the output string */

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

ಸ್ಥಿರ CURLcode dict_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  ಅಕ್ಷರ *ಶಬ್ದ;
  ಅಕ್ಷರ *eword = ಖಾಲಿ;
  ಅಕ್ಷರ *ppath;
  ಅಕ್ಷರ *database = ಖಾಲಿ;
  ಅಕ್ಷರ *strategy = ಖಾಲಿ;
  ಅಕ್ಷರ *nthdef = ಖಾಲಿ; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode ಫಲಿತಾಂಶ;

  ಅಕ್ಷರ *ಮಾರ್ಗ;

  *ಮುಗಿದ = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  ಫಲಿತಾಂಶ = Curl_urldecode(ದತ್ತಾಂಶ->state.ಮೇಲೆ.ಮಾರ್ಗ, 0, &ಮಾರ್ಗ, ಖಾಲಿ, REJECT_CTRL);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  ಆದರೆ(curl_strnequal(ಮಾರ್ಗ, DICT_ಹೊಂದಿಕೆ, CURL_CSTRLEN(DICT_ಹೊಂದಿಕೆ)) ||
     curl_strnequal(ಮಾರ್ಗ, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(ಮಾರ್ಗ, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    ಶಬ್ದ = strchr(ಮಾರ್ಗ, ':');
    ಆದರೆ(ಶಬ್ದ) {
      ಶಬ್ದ++;
      database = strchr(ಶಬ್ದ, ':');
      ಆದರೆ(database) {
        *database++ = (ಅಕ್ಷರ)0;
        strategy = strchr(database, ':');
        ಆದರೆ(strategy) {
          *strategy++ = (ಅಕ್ಷರ)0;
          nthdef = strchr(strategy, ':');
          ಆದರೆ(nthdef) {
            *nthdef = (ಅಕ್ಷರ)0;
          }
        }
      }
    }

    ಆದರೆ(!ಶಬ್ದ || (*ಶಬ್ದ == (ಅಕ್ಷರ)0)) {
      infof(ದತ್ತಾಂಶ, "lookup word is missing");
    }
    eword = unescape_ಶಬ್ದ((!ಶಬ್ದ || (*ಶಬ್ದ == (ಅಕ್ಷರ)0)) ? "default" : ಶಬ್ದ);
    ಆದರೆ(!eword) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು error;
    }

    ಫಲಿತಾಂಶ = sendf(ದತ್ತಾಂಶ,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_ಆವೃತ್ತಿ "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*ಶಬ್ದ*/
                   "QUIT\r\n",
                   (!database || (*database == (ಅಕ್ಷರ)0)) ? "!" : database,
                   (!strategy || (*strategy == (ಅಕ್ಷರ)0)) ? "." : strategy,
                   eword);

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Failed sending DICT request");
      ಹೋಗು error;
    }
    Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(curl_strnequal(ಮಾರ್ಗ, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(ಮಾರ್ಗ, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(ಮಾರ್ಗ, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    ಶಬ್ದ = strchr(ಮಾರ್ಗ, ':');
    ಆದರೆ(ಶಬ್ದ) {
      ಶಬ್ದ++;
      database = strchr(ಶಬ್ದ, ':');
      ಆದರೆ(database) {
        *database++ = (ಅಕ್ಷರ)0;
        nthdef = strchr(database, ':');
        ಆದರೆ(nthdef) {
          *nthdef = (ಅಕ್ಷರ)0;
        }
      }
    }

    ಆದರೆ(!ಶಬ್ದ || (*ಶಬ್ದ == (ಅಕ್ಷರ)0)) {
      infof(ದತ್ತಾಂಶ, "lookup word is missing");
    }
    eword = unescape_ಶಬ್ದ((!ಶಬ್ದ || (*ಶಬ್ದ == (ಅಕ್ಷರ)0)) ? "default" : ಶಬ್ದ);
    ಆದರೆ(!eword) {
      ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
      ಹೋಗು error;
    }

    ಫಲಿತಾಂಶ = sendf(ದತ್ತಾಂಶ,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_ಆವೃತ್ತಿ "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*ಶಬ್ದ*/
                   "QUIT\r\n",
                   (!database || (*database == (ಅಕ್ಷರ)0)) ? "!" : database,
                   eword);

    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Failed sending DICT request");
      ಹೋಗು error;
    }
    Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
  }
  ಇಲ್ಲದಿದ್ದರೆ {

    ppath = strchr(ಮಾರ್ಗ, '/');
    ಆದರೆ(ppath) {
      ಪೂರ್ಣಾಂಕ i;

      ppath++;
      ಗಾಗಿ(i = 0; ppath[i]; i++) {
        ಆದರೆ(ppath[i] == ':')
          ppath[i] = ' ';
      }
      ಫಲಿತಾಂಶ = sendf(ದತ್ತಾಂಶ,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_ಆವೃತ್ತಿ "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        failf(ದತ್ತಾಂಶ, "Failed sending DICT request");
        ಹೋಗು error;
      }

      Curl_xfer_ಸಿದ್ಧತೆ_recv(ದತ್ತಾಂಶ, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(ಮಾರ್ಗ);
  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * DICT protocol
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_dict = {
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

#ಆದರೆ_ಅಂತ್ಯ /* CURL_DISABLE_DICT */
