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

#جے_بنا_تعریف CURL_DISABLE_DICT

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_SELECT_H
#ضمن <sys/select.h>
#وإلاإذا defined(HAVE_UNISTD_H)
#ضمن <unistd.h>
#جے_انت

#ضمن "transfer.h"
#ضمن "curl_trc.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "escape.h"

#تعریف_کرو DICT_میل   "/MATCH:"
#تعریف_کرو DICT_MATCH2  "/M:"
#تعریف_کرو DICT_MATCH3  "/FIND:"
#تعریف_کرو DICT_DEFINE  "/DEFINE:"
#تعریف_کرو DICT_DEFINE2 "/D:"
#تعریف_کرو DICT_DEFINE3 "/LOOKUP:"

#تعریف_کرو DYN_DICT_لفظ 10000
ساكن حرف *unescape_لفظ(ثابت حرف *داخلہ)
{
  بنية dynbuf باہرلا;
  ثابت حرف *ptr;
  CURLcode نتیجہ = CURLE_OK;
  curlx_dyn_init(&باہرلا, DYN_DICT_لفظ);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  لكل(ptr = داخلہ; *ptr; ptr++) {
    حرف ch = *ptr;
    إذا((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      نتیجہ = curlx_dyn_addn(&باہرلا, "\\", 1);
    إذا(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&باہرلا, ptr, 1);
    إذا(نتیجہ)
      أرجع عدم;
  }
  أرجع curlx_dyn_ptr(&باہرلا);
}

/* sendf() sends formatted data to the server */
ساكن CURLcode sendf(بنية Curl_easy *انکڑے,
                      ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

ساكن CURLcode sendf(بنية Curl_easy *انکڑے, ثابت حرف *fmt, ...)
{
  curl_socket_t sockfd = انکڑے->conn->sock[FIRSTSOCKET];
  size_t بائٹاں_written;
  size_t write_len;
  CURLcode نتیجہ = CURLE_OK;
  timediff_t timeout_ms;
  صحيح what;
  حرف *s;
  حرف *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  إذا(!s)
    أرجع CURLE_باہرلا_دا_حافظہ; /* failure */

  بائٹاں_written = 0;
  write_len = لڑی_لمبائی(s);
  sptr = s;

  لكل(;;) {
    /* Write the buffer to the socket */
    نتیجہ = Curl_xfer_send(انکڑے, sptr, write_len, FALSE, &بائٹاں_written);

    إذا(نتیجہ)
      اكسر;

    Curl_debug(انکڑے, CURLINFO_انکڑے_باہرلا, sptr, بائٹاں_written);

    إذا(بائٹاں_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= بائٹاں_written;
      sptr += بائٹاں_written;
    }
    وإلا
      اكسر;

    timeout_ms = Curl_timeleft_ms(انکڑے);
    إذا(timeout_ms < 0) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
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
      نتیجہ = CURLE_SEND_ERROR;
      اكسر;
    }
    وإلا إذا(!what) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    }
  }

  curlx_free(s); /* free the output string */

  أرجع نتیجہ;
}

ساكن CURLcode dict_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  حرف *لفظ;
  حرف *eword = عدم;
  حرف *ppath;
  حرف *database = عدم;
  حرف *strategy = عدم;
  حرف *nthdef = عدم; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode نتیجہ;

  حرف *رستہ;

  *پورا = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  نتیجہ = Curl_urldecode(انکڑے->state.اتے.رستہ, 0, &رستہ, عدم, REJECT_CTRL);
  إذا(نتیجہ)
    أرجع نتیجہ;

  إذا(curl_strnequal(رستہ, DICT_میل, CURL_CSTRLEN(DICT_میل)) ||
     curl_strnequal(رستہ, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(رستہ, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    لفظ = strchr(رستہ, ':');
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
      infof(انکڑے, "lookup word is missing");
    }
    eword = unescape_لفظ((!لفظ || (*لفظ == (حرف)0)) ? "default" : لفظ);
    إذا(!eword) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب error;
    }

    نتیجہ = sendf(انکڑے,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخہ "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*لفظ*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   (!strategy || (*strategy == (حرف)0)) ? "." : strategy,
                   eword);

    إذا(نتیجہ) {
      failf(انکڑے, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, -1);
  }
  وإلا إذا(curl_strnequal(رستہ, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(رستہ, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(رستہ, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    لفظ = strchr(رستہ, ':');
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
      infof(انکڑے, "lookup word is missing");
    }
    eword = unescape_لفظ((!لفظ || (*لفظ == (حرف)0)) ? "default" : لفظ);
    إذا(!eword) {
      نتیجہ = CURLE_باہرلا_دا_حافظہ;
      اذهب error;
    }

    نتیجہ = sendf(انکڑے,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخہ "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*لفظ*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   eword);

    إذا(نتیجہ) {
      failf(انکڑے, "Failed sending DICT request");
      اذهب error;
    }
    Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, -1);
  }
  وإلا {

    ppath = strchr(رستہ, '/');
    إذا(ppath) {
      صحيح i;

      ppath++;
      لكل(i = 0; ppath[i]; i++) {
        إذا(ppath[i] == ':')
          ppath[i] = ' ';
      }
      نتیجہ = sendf(انکڑے,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_نسخہ "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      إذا(نتیجہ) {
        failf(انکڑے, "Failed sending DICT request");
        اذهب error;
      }

      Curl_xfer_تیاری_recv(انکڑے, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(رستہ);
  أرجع نتیجہ;
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

#جے_انت /* CURL_DISABLE_DICT */
