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

#که_نه_تعریف_شوی CURL_DISABLE_DICT

#که_تعریف_شوی HAVE_NETINET_دننه_H
#ضمن <netinet/in.h>
#د_که_پای
#که_تعریف_شوی HAVE_NETDB_H
#ضمن <netdb.h>
#د_که_پای
#که_تعریف_شوی HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#د_که_پای
#که_تعریف_شوی HAVE_NET_IF_H
#ضمن <net/if.h>
#د_که_پای
#که_تعریف_شوی HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#د_که_پای

#که_تعریف_شوی HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#د_که_پای

#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "escape.h"

#تعریف_کړه DICT_سمون   "/MATCH:"
#تعریف_کړه DICT_MATCH2  "/M:"
#تعریف_کړه DICT_MATCH3  "/FIND:"
#تعریف_کړه DICT_DEFINE  "/DEFINE:"
#تعریف_کړه DICT_DEFINE2 "/D:"
#تعریف_کړه DICT_DEFINE3 "/LOOKUP:"

#تعریف_کړه DYN_DICT_کلمه 10000
ساكن حرف *unescape_کلمه(ثابت حرف *ننوت)
{
  بنية dynbuf بهر;
  ثابت حرف *ptr;
  CURLcode پایله = CURLE_OK;
  curlx_dyn_init(&بهر, DYN_DICT_کلمه);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  لكل(ptr = ننوت; *ptr; ptr++) {
    حرف ch = *ptr;
    إذا((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      پایله = curlx_dyn_addn(&بهر, "\\", 1);
    إذا(!پایله)
      پایله = curlx_dyn_addn(&بهر, ptr, 1);
    إذا(پایله)
      أرجع عدم;
  }
  أرجع curlx_dyn_ptr(&بهر);
}

/* sendf() sends formatted data to the server */
ساكن CURLcode sendf(بنية Curl_easy *معلومات,
                      ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

ساكن CURLcode sendf(بنية Curl_easy *معلومات, ثابت حرف *fmt, ...)
{
  curl_socket_t sockfd = معلومات->conn->sock[FIRSTSOCKET];
  size_t بایټونه_written;
  size_t write_len;
  CURLcode پایله = CURLE_OK;
  timediff_t timeout_ms;
  صحيح what;
  حرف *s;
  حرف *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  إذا(!s)
    أرجع CURLE_بهر_د_حافظه; /* failure */

  بایټونه_written = 0;
  write_len = مزي_اوږدوالی(s);
  sptr = s;

  لكل(;;) {
    /* Write the buffer to the socket */
    پایله = Curl_xfer_send(معلومات, sptr, write_len, FALSE, &بایټونه_written);

    إذا(پایله)
      اكسر;

    Curl_debug(معلومات, CURLINFO_معلومات_بهر, sptr, بایټونه_written);

    إذا(بایټونه_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= بایټونه_written;
      sptr += بایټونه_written;
    }
    وإلا
      اكسر;

    timeout_ms = Curl_timeleft_ms(معلومات);
    إذا(timeout_ms < 0) {
      پایله = CURLE_OPERATION_TIMEDOUT;
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
      پایله = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      پایله = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  curlx_free(s); /* free the output string */

  أرجع پایله;
}

ساكن CURLcode dict_do(بنية Curl_easy *معلومات, bool *بشپړ)
{
  حرف *کلمه;
  حرف *eword = عدم;
  حرف *ppath;
  حرف *database = عدم;
  حرف *strategy = عدم;
  حرف *nthdef = عدم; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode پایله;

  حرف *لار;

  *بشپړ = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  پایله = Curl_urldecode(معلومات->state.پورته.لار, 0, &لار, عدم, REJECT_CTRL);
  إذا(پایله)
    أرجع پایله;

  إذا(curl_strnequal(لار, DICT_سمون, CURL_CSTRLEN(DICT_سمون)) ||
     curl_strnequal(لار, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(لار, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    کلمه = strchr(لار, ':');
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
      infof(معلومات, "lookup word is missing");
    }
    eword = unescape_کلمه((!کلمه || (*کلمه == (حرف)0)) ? "default" : کلمه);
    إذا(!eword) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب error;
    }

    پایله = sendf(معلومات,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخه "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*کلمه*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   (!strategy || (*strategy == (حرف)0)) ? "." : strategy,
                   eword);

    إذا(پایله) {
      failf(معلومات, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, -1);
  }
  وإلا إذا(curl_strnequal(لار, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(لار, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(لار, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    کلمه = strchr(لار, ':');
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
      infof(معلومات, "lookup word is missing");
    }
    eword = unescape_کلمه((!کلمه || (*کلمه == (حرف)0)) ? "default" : کلمه);
    إذا(!eword) {
      پایله = CURLE_بهر_د_حافظه;
      اذهب error;
    }

    پایله = sendf(معلومات,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخه "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*کلمه*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   eword);

    إذا(پایله) {
      failf(معلومات, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, -1);
  }
  وإلا {

    ppath = strchr(لار, '/');
    إذا(ppath) {
      صحيح i;

      ppath++;
      لكل(i = 0; ppath[i]; i++) {
        إذا(ppath[i] == ':')
          ppath[i] = ' ';
      }
      پایله = sendf(معلومات,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_نسخه "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      إذا(پایله) {
        failf(معلومات, "Failed sending DICT request");
        اذهب error;
      }

      Curl_xfer_تیاری_recv(معلومات, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(لار);
  أرجع پایله;
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

#د_که_پای /* CURL_DISABLE_DICT */
