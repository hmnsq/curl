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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "dict.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_DICT

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_SELECT_H
#شامل <sys/select.h>
#وگرنہ_اگر defined(HAVE_UNISTD_H)
#شامل <unistd.h>
#اگر_ختم

#شامل "transfer.h"
#شامل "curl_trc.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "escape.h"

#تعریف_کرو DICT_مطابقت   "/MATCH:"
#تعریف_کرو DICT_MATCH2  "/M:"
#تعریف_کرو DICT_MATCH3  "/FIND:"
#تعریف_کرو DICT_DEFINE  "/DEFINE:"
#تعریف_کرو DICT_DEFINE2 "/D:"
#تعریف_کرو DICT_DEFINE3 "/LOOKUP:"

#تعریف_کرو DYN_DICT_لفظ 10000
ساکن حرف *unescape_لفظ(ثابت حرف *داخلہ)
{
  ساخت dynbuf برآمد;
  ثابت حرف *ptr;
  CURLcode نتیجہ = CURLE_OK;
  curlx_dyn_init(&برآمد, DYN_DICT_لفظ);

  /* According to RFC2229 section 2.2, these letters need to be escaped with
     \[letter] */
  برائے(ptr = داخلہ; *ptr; ptr++) {
    حرف ch = *ptr;
    اگر((ch <= 32) || (ch == 127) ||
       (ch == '\'') || (ch == '\"') || (ch == '\\'))
      نتیجہ = curlx_dyn_addn(&برآمد, "\\", 1);
    اگر(!نتیجہ)
      نتیجہ = curlx_dyn_addn(&برآمد, ptr, 1);
    اگر(نتیجہ)
      واپس تہی;
  }
  واپس curlx_dyn_ptr(&برآمد);
}

/* sendf() sends formatted data to the server */
ساکن CURLcode sendf(ساخت Curl_easy *ڈیٹا,
                      ثابت حرف *fmt, ...) CURL_PRINTF(2, 3);

ساکن CURLcode sendf(ساخت Curl_easy *ڈیٹا, ثابت حرف *fmt, ...)
{
  curl_socket_t sockfd = ڈیٹا->conn->sock[FIRSTSOCKET];
  size_t بائٹیں_written;
  size_t write_len;
  CURLcode نتیجہ = CURLE_OK;
  timediff_t timeout_ms;
  صحیح what;
  حرف *s;
  حرف *sptr;
  va_list ap;
  va_start(ap, fmt);
  s = curl_mvaprintf(fmt, ap); /* returns an allocated string */
  va_end(ap);
  اگر(!s)
    واپس CURLE_برآمد_کا_حافظہ; /* failure */

  بائٹیں_written = 0;
  write_len = لڑی_لمبائی(s);
  sptr = s;

  برائے(;;) {
    /* Write the buffer to the socket */
    نتیجہ = Curl_xfer_send(ڈیٹا, sptr, write_len, FALSE, &بائٹیں_written);

    اگر(نتیجہ)
      توڑو;

    Curl_debug(ڈیٹا, CURLINFO_ڈیٹا_برآمد, sptr, بائٹیں_written);

    اگر(بائٹیں_written != write_len) {
      /* if not all was written at once, we must advance the pointer, decrease
         the size left and try again! */
      write_len -= بائٹیں_written;
      sptr += بائٹیں_written;
    }
    ورنہ
      توڑو;

    timeout_ms = Curl_timeleft_ms(ڈیٹا);
    اگر(timeout_ms < 0) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
    }
    اگر(!timeout_ms)
      timeout_ms = TIMEDIFF_T_MAX;

    /* Do not busyloop. The entire loop thing is a workaround as it causes a
       BLOCKING behavior which is a NO-NO. This function should rather be
       split up in a do and a doing piece where the pieces that are not
       possible to send now will be sent in the doing function repeatedly
       until the entire request is sent. */
    what = SOCKET_WRITABLE(sockfd, timeout_ms);
    اگر(what < 0) {
      نتیجہ = CURLE_SEND_ERROR;
      توڑو;
    }
    ورنہ اگر(!what) {
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
    }
  }

  curlx_free(s); /* free the output string */

  واپس نتیجہ;
}

ساکن CURLcode dict_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  حرف *لفظ;
  حرف *eword = تہی;
  حرف *ppath;
  حرف *database = تہی;
  حرف *strategy = تہی;
  حرف *nthdef = تہی; /* This is not part of the protocol, but required
                          by RFC 2229 */
  CURLcode نتیجہ;

  حرف *راستہ;

  *مکمل = TRUE; /* unconditionally */

  /* URL-decode path before further evaluation */
  نتیجہ = Curl_urldecode(ڈیٹا->state.اوپر.راستہ, 0, &راستہ, تہی, REJECT_CTRL);
  اگر(نتیجہ)
    واپس نتیجہ;

  اگر(curl_strnequal(راستہ, DICT_مطابقت, CURL_CSTRLEN(DICT_مطابقت)) ||
     curl_strnequal(راستہ, DICT_MATCH2, CURL_CSTRLEN(DICT_MATCH2)) ||
     curl_strnequal(راستہ, DICT_MATCH3, CURL_CSTRLEN(DICT_MATCH3))) {

    لفظ = strchr(راستہ, ':');
    اگر(لفظ) {
      لفظ++;
      database = strchr(لفظ, ':');
      اگر(database) {
        *database++ = (حرف)0;
        strategy = strchr(database, ':');
        اگر(strategy) {
          *strategy++ = (حرف)0;
          nthdef = strchr(strategy, ':');
          اگر(nthdef) {
            *nthdef = (حرف)0;
          }
        }
      }
    }

    اگر(!لفظ || (*لفظ == (حرف)0)) {
      infof(ڈیٹا, "lookup word is missing");
    }
    eword = unescape_لفظ((!لفظ || (*لفظ == (حرف)0)) ? "default" : لفظ);
    اگر(!eword) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ error;
    }

    نتیجہ = sendf(ڈیٹا,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخہ "\r\n"
                   "MATCH "
                   "%s "    /* database */
                   "%s "    /* strategy */
                   "%s\r\n" /*لفظ*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   (!strategy || (*strategy == (حرف)0)) ? "." : strategy,
                   eword);

    اگر(نتیجہ) {
      failf(ڈیٹا, "Failed sending DICT request");
      جاؤ error;
    }
    Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);
  }
  ورنہ اگر(curl_strnequal(راستہ, DICT_DEFINE, CURL_CSTRLEN(DICT_DEFINE)) ||
          curl_strnequal(راستہ, DICT_DEFINE2, CURL_CSTRLEN(DICT_DEFINE2)) ||
          curl_strnequal(راستہ, DICT_DEFINE3, CURL_CSTRLEN(DICT_DEFINE3))) {

    لفظ = strchr(راستہ, ':');
    اگر(لفظ) {
      لفظ++;
      database = strchr(لفظ, ':');
      اگر(database) {
        *database++ = (حرف)0;
        nthdef = strchr(database, ':');
        اگر(nthdef) {
          *nthdef = (حرف)0;
        }
      }
    }

    اگر(!لفظ || (*لفظ == (حرف)0)) {
      infof(ڈیٹا, "lookup word is missing");
    }
    eword = unescape_لفظ((!لفظ || (*لفظ == (حرف)0)) ? "default" : لفظ);
    اگر(!eword) {
      نتیجہ = CURLE_برآمد_کا_حافظہ;
      جاؤ error;
    }

    نتیجہ = sendf(ڈیٹا,
                   "CLIENT " LIBCURL_NAME " " LIBCURL_نسخہ "\r\n"
                   "DEFINE "
                   "%s "     /* database */
                   "%s\r\n"  /*لفظ*/
                   "QUIT\r\n",
                   (!database || (*database == (حرف)0)) ? "!" : database,
                   eword);

    اگر(نتیجہ) {
      failf(ڈیٹا, "Failed sending DICT request");
      جاؤ error;
    }
    Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);
  }
  ورنہ {

    ppath = strchr(راستہ, '/');
    اگر(ppath) {
      صحیح i;

      ppath++;
      برائے(i = 0; ppath[i]; i++) {
        اگر(ppath[i] == ':')
          ppath[i] = ' ';
      }
      نتیجہ = sendf(ڈیٹا,
                     "CLIENT " LIBCURL_NAME " " LIBCURL_نسخہ "\r\n"
                     "%s\r\n"
                     "QUIT\r\n", ppath);
      اگر(نتیجہ) {
        failf(ڈیٹا, "Failed sending DICT request");
        جاؤ error;
      }

      Curl_xfer_تیاری_recv(ڈیٹا, FIRSTSOCKET, -1);
    }
  }

error:
  curlx_free(eword);
  curlx_free(راستہ);
  واپس نتیجہ;
}

/*
 * DICT protocol
 */
ثابت ساخت Curl_protocol Curl_protocol_dict = {
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

#اگر_ختم /* CURL_DISABLE_DICT */
