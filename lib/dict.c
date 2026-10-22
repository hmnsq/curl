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

#اگر_تعریف_نشده CURL_DISABLE_DICT

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#پایان_اگر

#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "escape.h"

#تعریف_کن DICT_تطابق   "/MATCH:"
#تعریف_کن DICT_MATCH2  "/M:"
#تعریف_کن DICT_MATCH3  "/FIND:"
#تعریف_کن DICT_DEFINE  "/DEFINE:"
#تعریف_کن DICT_DEFINE2 "/D:"
#تعریف_کن DICT_DEFINE3 "/LOOKUP:"

#تعریف_کن DYN_DICT_کلمه 10000
ساكن حرف *unescape_کلمه(ثابت حرف *درون_داد)
{
  بنية dynbuf بیرون;
  ثابت حرف *ptr;
  CURLcode نتیجه = CURLE_OK;
  curlx_dyn_init(&بیرون, DYN_DICT_کلمه);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  لكل(ptr = درون_داد; *ptr; ptr++) {
    حرف ch = *ptr;
    إذا((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      نتیجه = curlx_dyn_addn(&بیرون, "\\", 1);
    إذا(!نتیجه)
      نتیجه = curlx_dyn_addn(&بیرون, ptr, 1);
    إذا(نتیجه)
      أرجع عدم;
  }
  أرجع curlx_dyn_ptr(&بیرون);
}

/* sendf() sends formatted data to the server */
ساكن CURLcode sendf(بنية Curl_easy *داده,
                      ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

ساكن CURLcode sendf(بنية Curl_easy *داده, ثابت حرف *fmt, ...)
{
  curl_socket_t sockfd = داده->conn->sock[FIRSTSOCKET];
  size_t بایتها_written;
  size_t write_len;
  CURLcode نتیجه = CURLE_OK;
  timediff_t timeout_ms;
  صحيح what;
  حرف *s;
  حرف *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  إذا(!s)
    أرجع CURLE_بیرون_از_حافظه; /* failure */

  بایتها_written = 0;
  write_len = طول_رشته(s);
  sptr = s;

  لكل(;;) {
    /* Write the buffer to the socket */
    نتیجه = Curl_xfer_send(داده, sptr, write_len, FALSE, &بایتها_written);

    إذا(نتیجه)
      اكسر;

    Curl_debug(داده, CURLINFO_داده_بیرون, sptr, بایتها_written);

    إذا(بایتها_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= بایتها_written;
      sptr += بایتها_written;
    }
    وإلا
      اكسر;

    timeout_ms = Curl_timeleft_ms(داده);
    إذا(timeout_ms < 0) {
      نتیجه = CURLE_OPERATION_TIMEDOUT;
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
      نتیجه = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  curlx_free(s); /* free the output string */

  أرجع نتیجه;
}

ساكن CURLcode dict_do(بنية Curl_easy *داده, bool *انجامشده)
{
  حرف *کلمه;
  حرف *eword = عدم;
  حرف *ppath;
  حرف *database = عدم;
  حرف *strategy = عدم;
  حرف *nthdef = عدم; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode نتیجه;

  حرف *مسیر;

  *انجامشده = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  نتیجه = Curl_urldecode(داده->state.به_بالا.مسیر, 0, &مسیر, عدم, REJECT_CTRL);
  إذا(نتیجه)
    أرجع نتیجه;

  إذا(curl_strnequal(مسیر, DICT_تطابق, CURL_CSTRLEN(DICT_تطابق)) ||
     curl_strnequal(مسیر, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(مسیر, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    کلمه = strchr(مسیر, ':');
    إذا(کلمه) {
      کلمه++;
      database = strchr(کلمه, ':');
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

    إذا(!کلمه || (*کلمه == (حرف)0)) {
      infof(داده, "lookup word is missing");
    }
    eword = unescape_کلمه((!کلمه || (*کلمه == (حرف)0)) ? "default" : کلمه);
    إذا(!eword) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب error;
    }

    نتیجه = sendf(داده,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخه "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*کلمه*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   (!strategy || (*strategy == (حرف)0)) ? "." : strategy,
                   eword);

    إذا(نتیجه) {
      failf(داده, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, -1);
  }
  وإلا إذا(curl_strnequal(مسیر, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(مسیر, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(مسیر, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    کلمه = strchr(مسیر, ':');
    إذا(کلمه) {
      کلمه++;
      database = strchr(کلمه, ':');
      إذا(database) {
        *database++ = (حرف)0;
        nthdef = strchr(database, ':');
        إذا(nthdef) {
          *nthdef = (حرف)0;
        }
      }
    }

    إذا(!کلمه || (*کلمه == (حرف)0)) {
      infof(داده, "lookup word is missing");
    }
    eword = unescape_کلمه((!کلمه || (*کلمه == (حرف)0)) ? "default" : کلمه);
    إذا(!eword) {
      نتیجه = CURLE_بیرون_از_حافظه;
      اذهب error;
    }

    نتیجه = sendf(داده,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخه "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*کلمه*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   eword);

    إذا(نتیجه) {
      failf(داده, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, -1);
  }
  وإلا {

    ppath = strchr(مسیر, '/');
    إذا(ppath) {
      صحيح i;

      ppath++;
      لكل(i = 0; ppath[i]; i++) {
        إذا(ppath[i] == ':')
          ppath[i] = ' ';
      }
      نتیجه = sendf(داده,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_نسخه "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      إذا(نتیجه) {
        failf(داده, "Failed sending DICT request");
        اذهب error;
      }

      Curl_xfer_راهاندازی_recv(داده, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(مسیر);
  أرجع نتیجه;
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

#پایان_اگر /* CURL_DISABLE_DICT */
