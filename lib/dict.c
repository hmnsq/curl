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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "dict.h"

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_DICT

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#جيڪڏهن_پورو

#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "escape.h"

#وصف_ڪريو DICT_ميلاپ   "/MATCH:"
#وصف_ڪريو DICT_MATCH2  "/M:"
#وصف_ڪريو DICT_MATCH3  "/FIND:"
#وصف_ڪريو DICT_DEFINE  "/DEFINE:"
#وصف_ڪريو DICT_DEFINE2 "/D:"
#وصف_ڪريو DICT_DEFINE3 "/LOOKUP:"

#وصف_ڪريو DYN_DICT_لفظ 10000
ساكن حرف *unescape_لفظ(ثابت حرف *داخلا)
{
  بنية dynbuf ٻاهريون;
  ثابت حرف *ptr;
  CURLcode نتيجو = CURLE_OK;
  curlx_dyn_init(&ٻاهريون, DYN_DICT_لفظ);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  لكل(ptr = داخلا; *ptr; ptr++) {
    حرف ch = *ptr;
    إذا((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      نتيجو = curlx_dyn_addn(&ٻاهريون, "\\", 1);
    إذا(!نتيجو)
      نتيجو = curlx_dyn_addn(&ٻاهريون, ptr, 1);
    إذا(نتيجو)
      أرجع عدم;
  }
  أرجع curlx_dyn_ptr(&ٻاهريون);
}

/* sendf() sends formatted data to the server */
ساكن CURLcode sendf(بنية Curl_easy *معلومات,
                      ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

ساكن CURLcode sendf(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  curl_socket_t sockfd = معلومات->conn->sock[FIRSTSOCKET];
  size_t بائيٽون_written;
  size_t write_len;
  CURLcode نتيجو = CURLE_OK;
  timediff_t timeout_ms;
  صحيح what;
  حرف *s;
  حرف *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  إذا(!s)
    أرجع CURLE_ٻاهريون_جو_يادگيري; /* failure */

  بائيٽون_written = 0;
  write_len = لڙي_ڊيگهه(s);
  sptr = s;

  لكل(;;) {
    /* Write the buffer to the socket */
    نتيجو = Curl_xfer_send(معلومات, sptr, write_len, FALSE, &بائيٽون_written);

    إذا(نتيجو)
      اكسر;

    Curl_debug(معلومات, CURLINFO_معلومات_ٻاهريون, sptr, بائيٽون_written);

    إذا(بائيٽون_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= بائيٽون_written;
      sptr += بائيٽون_written;
    }
    وإلا
      اكسر;

    timeout_ms = Curl_timeleft_ms(معلومات);
    إذا(timeout_ms < 0) {
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
    إذا(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    إذا(what < 0) {
      نتيجو = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  curlx_free(s); /* free the output string */

  أرجع نتيجو;
}

ساكن CURLcode dict_do(بنية Curl_easy *معلومات, bool *پورو)
{
  حرف *لفظ;
  حرف *eword = عدم;
  حرف *ppath;
  حرف *database = عدم;
  حرف *strategy = عدم;
  حرف *nthdef = عدم; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode نتيجو;

  حرف *رستو;

  *پورو = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  نتيجو = Curl_urldecode(معلومات->state.مٿي.رستو, 0, &رستو, عدم, REJECT_CTRL);
  إذا(نتيجو)
    أرجع نتيجو;

  إذا(curl_strnequal(رستو, DICT_ميلاپ, CURL_CSTRLEN(DICT_ميلاپ)) ||
     curl_strnequal(رستو, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(رستو, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    لفظ = strchr(رستو, ':');
    إذا(لفظ) {
      لفظ++;
      database = strchr(لفظ, ':');
      إذا(database) {
        *database++ = (حرف)0;
        strategy = strchr(database, ':');
        إذا(strategy) {
          *strategy++ = (حرف)0;
          nthdef = strchr(strategy, ':');
          إذا(nthdef) {
            *nthdef = (حرف)0;
          }
        }
      }
    }

    إذا(!لفظ || (*لفظ == (حرف)0)) {
      infof(معلومات, "lookup word is missing");
    }
    eword = unescape_لفظ((!لفظ || (*لفظ == (حرف)0)) ? "default" : لفظ);
    إذا(!eword) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب error;
    }

    نتيجو = sendf(معلومات,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخو "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*لفظ*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   (!strategy || (*strategy == (حرف)0)) ? "." : strategy,
                   eword);

    إذا(نتيجو) {
      failf(معلومات, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_تياري_recv(معلومات, FIRSTSOCKET, -1);
  }
  وإلا إذا(curl_strnequal(رستو, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(رستو, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(رستو, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    لفظ = strchr(رستو, ':');
    إذا(لفظ) {
      لفظ++;
      database = strchr(لفظ, ':');
      إذا(database) {
        *database++ = (حرف)0;
        nthdef = strchr(database, ':');
        إذا(nthdef) {
          *nthdef = (حرف)0;
        }
      }
    }

    إذا(!لفظ || (*لفظ == (حرف)0)) {
      infof(معلومات, "lookup word is missing");
    }
    eword = unescape_لفظ((!لفظ || (*لفظ == (حرف)0)) ? "default" : لفظ);
    إذا(!eword) {
      نتيجو = CURLE_ٻاهريون_جو_يادگيري;
      اذهب error;
    }

    نتيجو = sendf(معلومات,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخو "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*لفظ*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   eword);

    إذا(نتيجو) {
      failf(معلومات, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_تياري_recv(معلومات, FIRSTSOCKET, -1);
  }
  وإلا {

    ppath = strchr(رستو, '/');
    إذا(ppath) {
      صحيح i;

      ppath++;
      لكل(i = 0; ppath[i]; i++) {
        إذا(ppath[i] == ':')
          ppath[i] = ' ';
      }
      نتيجو = sendf(معلومات,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_نسخو "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      إذا(نتيجو) {
        failf(معلومات, "Failed sending DICT request");
        اذهب error;
      }

      Curl_xfer_تياري_recv(معلومات, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(رستو);
  أرجع نتيجو;
}

/*
 * DICT protocol
 */
ثابت بنية Curl_protocol Curl_protocol_dict = {
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

#جيڪڏهن_پورو /* CURL_DISABLE_DICT */
