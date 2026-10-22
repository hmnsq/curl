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
#شامل "getinfo.h"
#شامل "cfilters.h"
#شامل "vtls/vtls.h"
#شامل "connect.h" /* Curl_getconnectinfo() */
#شامل "bufref.h"
#شامل "url.h" /* Curl_uc_to_curlcode() */
#شامل "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
خالی Curl_initinfo(ساخت Curl_easy *ڈیٹا)
{
  ساخت پیش_رفت_2 *pro = &ڈیٹا->پیش_رفت;
  ساخت خالص_Info *info = &ڈیٹا->info;

  memset(&pro->delta, 0, حجم(pro->delta));
  memset(&pro->کل, 0, حجم(pro->کل));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->سرخی_سائز = 0;
  info->request_سائز = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  memset(&info->primary, 0, حجم(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->مستعمل_proxy = 0;

#اگر_تعریف_شدہ USE_SSL
  Curl_ssl_free_certinfo(ڈیٹا);
#اگر_ختم
}

ساکن CURLcode effective_url(ساخت Curl_easy *ڈیٹا,
                              ثابت حرف **param_charp)
{
  ثابت حرف *s = Curl_bufref_ptr(&ڈیٹا->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? تہی : "";
  اگر(s) {
    حرف *url = تہی;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    اگر(!u)
      واپس CURLE_برآمد_کا_حافظہ;

    uc = curl_url_سیٹ(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (ڈیٹا->سیٹ.راستہ_as_is ? CURLU_راستہ_AS_IS : 0));
    اگر(!uc)
      uc = curl_url_سیٹ(u, CURLUPART_USER, تہی, 0);
    اگر(!uc)
      uc = curl_url_سیٹ(u, CURLUPART_PASSWORD, تہی, 0);
    اگر(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_خالی_خانے);

    curl_url_cleanup(u);

    اگر(uc || !url)
      واپس uc ? Curl_uc_مقصد_curlcode(uc) : CURLE_برآمد_کا_حافظہ;
    اگر(strcmp(url, s)) {
      /* they are different */
      curlx_free(ڈیٹا->info.effective_url);
      ڈیٹا->info.effective_url = url;
      *param_charp = url;
    }
    ورنہ {
      curl_free(url);
      *param_charp = s;
    }
  }
  واپس CURLE_OK;
}

ساکن CURLcode getinfo_char(ساخت Curl_easy *ڈیٹا, CURLINFO info,
                             ثابت حرف **param_charp)
{
  انتخاب(info) {
  حالت CURLINFO_EFFECTIVE_URL:
    واپس effective_url(ڈیٹا, param_charp);
  حالت CURLINFO_EFFECTIVE_طریقہ: {
    ثابت حرف *m = CURL_EASY_STR(ڈیٹا, STRING_CUSTOMREQUEST);
    اگر(!m) {
      اگر(ڈیٹا->سیٹ.opt_no_body)
        m = "HEAD";
#اگر_غیر_تعریف_شدہ CURL_DISABLE_HTTP
      ورنہ {
        انتخاب(ڈیٹا->state.httpreq) {
        حالت HTTPREQ_POST:
        حالت HTTPREQ_POST_FORM:
        حالت HTTPREQ_POST_MIME:
          m = "POST";
          توڑو;
        حالت HTTPREQ_PUT:
          m = "PUT";
          توڑو;
        طےشدہ: /* this should never happen */
        حالت HTTPREQ_GET:
          m = "GET";
          توڑو;
        حالت HTTPREQ_سر:
          m = "HEAD";
          توڑو;
        }
      }
#اگر_ختم
    }
    *param_charp = m;
  }
    توڑو;
  حالت CURLINFO_CONTENT_TYPE:
    *param_charp = ڈیٹا->info.contenttype;
    توڑو;
  حالت CURLINFO_نجی:
    *param_charp = (ثابت حرف *)ڈیٹا->سیٹ.نجی_ڈیٹا;
    توڑو;
  حالت CURLINFO_FTP_اندراج_راستہ:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = ڈیٹا->state.most_recent_ftp_entrypath;
    توڑو;
  حالت CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = ڈیٹا->info.wouldredirect;
    توڑو;
  حالت CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&ڈیٹا->state.referer);
    توڑو;
  حالت CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = ڈیٹا->info.primary.remote_ip;
    توڑو;
  حالت CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = ڈیٹا->info.primary.local_ip;
    توڑو;
  حالت CURLINFO_RTSP_SESSION_ID:
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(ڈیٹا, STRING_RTSP_SESSION_ID);
#ورنہ
    *param_charp = تہی;
#اگر_ختم
    توڑو;
  حالت CURLINFO_SCHEME:
    *param_charp = ڈیٹا->info.conn_scheme;
    توڑو;
  حالت CURLINFO_CAPATH:
#اگر_تعریف_شدہ CURL_CA_راستہ
    *param_charp = CURL_CA_راستہ;
#ورنہ
    *param_charp = تہی;
#اگر_ختم
    توڑو;
  حالت CURLINFO_CAINFO:
#اگر_تعریف_شدہ CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#ورنہ
    *param_charp = تہی;
#اگر_ختم
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }

  واپس CURLE_OK;
}

ساکن CURLcode getinfo_long(ساخت Curl_easy *ڈیٹا, CURLINFO info,
                             طویل *param_longp)
{
  curl_socket_t sockfd;

  اتحاد {
    غیرعلامتی طویل *مقصد_ulong;
    طویل          *مقصد_long;
  } lptr;

#اگر_تعریف_شدہ DEBUGBUILD
  ثابت حرف *envstr;

  /* use another variable for this to allow different values */
  انتخاب(info) {
  حالت CURLINFO_LOCAL_پورٹ:
    envstr = getenv("CURL_TIME");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, وقت_T_MAX);
      *param_longp = (طویل)val;
      واپس CURLE_OK;
    }
    توڑو;
  حالت CURLINFO_سرخی_سائز:
  حالت CURLINFO_REQUEST_سائز:
    envstr = getenv("CURL_DEBUG_SIZE");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, LONG_MAX);
      *param_longp = (طویل)val;
      واپس CURLE_OK;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
#اگر_ختم

  انتخاب(info) {
  حالت CURLINFO_RESPONSE_رمز:
    *param_longp = ڈیٹا->info.httpcode;
    توڑو;
  حالت CURLINFO_HTTP_CONNECTCODE:
    *param_longp = ڈیٹا->info.httpproxycode;
    توڑو;
  حالت CURLINFO_FILETIME:
    اگر(ڈیٹا->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#اگر !defined(MSDOS) && !defined(__AMIGA__)
    ورنہ اگر(ڈیٹا->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#اگر_ختم
    ورنہ
      *param_longp = (طویل)ڈیٹا->info.filetime;
    توڑو;
  حالت CURLINFO_سرخی_سائز:
    *param_longp = (طویل)ڈیٹا->info.سرخی_سائز;
    توڑو;
  حالت CURLINFO_REQUEST_سائز:
    *param_longp = (طویل)ڈیٹا->info.request_سائز;
    توڑو;
  حالت CURLINFO_SSL_VERIFYRESULT:
    *param_longp = ڈیٹا->سیٹ.ssl.certverifyresult;
    توڑو;
  حالت CURLINFO_PROXY_SSL_VERIFYRESULT:
#اگر_غیر_تعریف_شدہ CURL_DISABLE_PROXY
    *param_longp = ڈیٹا->سیٹ.proxy_ssl.certverifyresult;
#ورنہ
    *param_longp = 0;
#اگر_ختم
    توڑو;
  حالت CURLINFO_REDIRECT_گنتی:
    *param_longp = ڈیٹا->state.followlocation;
    توڑو;
  حالت CURLINFO_HTTPAUTH_AVAIL:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = ڈیٹا->info.httpauthavail;
    توڑو;
  حالت CURLINFO_PROXYAUTH_AVAIL:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = ڈیٹا->info.proxyauthavail;
    توڑو;
  حالت CURLINFO_HTTPAUTH_مستعمل:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = ڈیٹا->info.httpauthpicked;
    توڑو;
  حالت CURLINFO_PROXYAUTH_مستعمل:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = ڈیٹا->info.proxyauthpicked;
    توڑو;
  حالت CURLINFO_OS_ERRNO:
    *param_longp = ڈیٹا->state.os_errno;
    توڑو;
  حالت CURLINFO_عدد_CONNECTS:
#اگر SIZEOF_LONG < SIZEOF_CURL_OFF_T
    اگر(ڈیٹا->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    ورنہ
#اگر_ختم
      *param_longp = (طویل)ڈیٹا->info.numconnects;
    توڑو;
  حالت CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(ڈیٹا, تہی);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    اگر(sockfd != CURL_SOCKET_BAD)
      *param_longp = (طویل)sockfd;
    ورنہ
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    توڑو;
  حالت CURLINFO_PRIMARY_پورٹ:
    /* Return the (remote) port of the most recent (primary) connection */
    اگر(CUR_IP_QUAD_HAS_PORTS(&ڈیٹا->info.primary))
      *param_longp = ڈیٹا->info.primary.remote_پورٹ;
    ورنہ
      *param_longp = -1;
    توڑو;
  حالت CURLINFO_LOCAL_پورٹ:
    /* Return the local port of the most recent (primary) connection */
    اگر(CUR_IP_QUAD_HAS_PORTS(&ڈیٹا->info.primary))
      *param_longp = ڈیٹا->info.primary.local_پورٹ;
    ورنہ
      *param_longp = -1;
    توڑو;
  حالت CURLINFO_PROXY_ERROR:
    *param_longp = (طویل)ڈیٹا->info.pxcode;
    توڑو;
  حالت CURLINFO_CONDITION_UNMET:
    اگر(ڈیٹا->info.httpcode == 304)
      *param_longp = 1L;
    ورنہ
      /* return if the condition prevented the document to get transferred */
      *param_longp = ڈیٹا->info.timecond ? 1L : 0L;
    توڑو;
#اگر_غیر_تعریف_شدہ CURL_DISABLE_RTSP
  حالت CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = ڈیٹا->state.rtsp_اگلا_client_CSeq;
    توڑو;
  حالت CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = ڈیٹا->state.rtsp_اگلا_server_CSeq;
    توڑو;
  حالت CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = ڈیٹا->state.rtsp_CSeq_recv;
    توڑو;
#ورنہ
  حالت CURLINFO_RTSP_CLIENT_CSEQ:
  حالت CURLINFO_RTSP_SERVER_CSEQ:
  حالت CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    توڑو;
#اگر_ختم
  حالت CURLINFO_HTTP_نسخہ:
    انتخاب(ڈیٹا->info.httpversion) {
    حالت 10:
      *param_longp = CURL_HTTP_نسخہ_1_0;
      توڑو;
    حالت 11:
      *param_longp = CURL_HTTP_نسخہ_1_1;
      توڑو;
    حالت 20:
      *param_longp = CURL_HTTP_نسخہ_2_0;
      توڑو;
    حالت 30:
      *param_longp = CURL_HTTP_نسخہ_3;
      توڑو;
    طےشدہ:
      *param_longp = CURL_HTTP_نسخہ_NONE;
      توڑو;
    }
    توڑو;
  حالت CURLINFO_PROTOCOL:
    *param_longp = (طویل)ڈیٹا->info.conn_protocol;
    توڑو;
  حالت CURLINFO_مستعمل_PROXY:
    *param_longp =
#اگر_تعریف_شدہ CURL_DISABLE_PROXY
      0
#ورنہ
      ڈیٹا->info.مستعمل_proxy
#اگر_ختم
      ;
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }

  واپس CURLE_OK;
}

#تعریف_کرو DOUBLE_SECS(x) ((دوہرا)(x) / 1000000)

ساکن CURLcode getinfo_offt(ساخت Curl_easy *ڈیٹا, CURLINFO info,
                             curl_off_t *param_offt)
{
#اگر_تعریف_شدہ DEBUGBUILD
  ثابت حرف *envstr;
  انتخاب(info) {
  حالت CURLINFO_کل_وقت_T:
  حالت CURLINFO_NAMELOOKUP_وقت_T:
  حالت CURLINFO_CONNECT_وقت_T:
  حالت CURLINFO_APPCONNECT_وقت_T:
  حالت CURLINFO_PRETRANSFER_وقت_T:
  حالت CURLINFO_POSTTRANSFER_وقت_T:
  حالت CURLINFO_قطار_وقت_T:
  حالت CURLINFO_STARTTRANSFER_وقت_T:
  حالت CURLINFO_REDIRECT_وقت_T:
  حالت CURLINFO_رفتار_DOWNLOAD_T:
  حالت CURLINFO_رفتار_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      واپس CURLE_OK;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
#اگر_ختم
  انتخاب(info) {
  حالت CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)ڈیٹا->info.filetime;
    توڑو;
  حالت CURLINFO_سائز_DELIVERED:
    *param_offt = ڈیٹا->پیش_رفت.deliver;
    توڑو;
  حالت CURLINFO_سائز_UPLOAD_T:
    *param_offt = ڈیٹا->پیش_رفت.ul.cur_سائز;
    توڑو;
  حالت CURLINFO_سائز_DOWNLOAD_T:
    *param_offt = ڈیٹا->پیش_رفت.dl.cur_سائز;
    توڑو;
  حالت CURLINFO_رفتار_DOWNLOAD_T:
    *param_offt = ڈیٹا->پیش_رفت.dl.رفتار;
    توڑو;
  حالت CURLINFO_رفتار_UPLOAD_T:
    *param_offt = ڈیٹا->پیش_رفت.ul.رفتار;
    توڑو;
  حالت CURLINFO_CONTENT_لمبائی_DOWNLOAD_T:
    *param_offt = ڈیٹا->پیش_رفت.dl_سائز_معلوم ?
      ڈیٹا->پیش_رفت.dl.کل_سائز : -1;
    توڑو;
  حالت CURLINFO_CONTENT_لمبائی_UPLOAD_T:
    *param_offt = ڈیٹا->پیش_رفت.ul_سائز_معلوم ?
      ڈیٹا->پیش_رفت.ul.کل_سائز : -1;
    توڑو;
   حالت CURLINFO_کل_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.spent_us;
    توڑو;
  حالت CURLINFO_NAMELOOKUP_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.nslookup_us;
    توڑو;
  حالت CURLINFO_CONNECT_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.connect_us;
    توڑو;
  حالت CURLINFO_APPCONNECT_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.appconnect_us;
    توڑو;
  حالت CURLINFO_PRETRANSFER_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.pretransfer_us;
    توڑو;
  حالت CURLINFO_POSTTRANSFER_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.posttransfer_us;
    توڑو;
  حالت CURLINFO_STARTTRANSFER_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.starttransfer_us;
    توڑو;
  حالت CURLINFO_قطار_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.کل.queued_us;
    توڑو;
  حالت CURLINFO_REDIRECT_وقت_T:
    *param_offt = ڈیٹا->پیش_رفت.delta.startredirect_us;
    توڑو;
  حالت CURLINFO_RETRY_AFTER:
    *param_offt = ڈیٹا->info.retry_after;
    توڑو;
  حالت CURLINFO_XFER_ID:
    *param_offt = ڈیٹا->id;
    توڑو;
  حالت CURLINFO_CONN_ID:
    *param_offt = ڈیٹا->state.آخری_conn_id;
    توڑو;
  حالت CURLINFO_EARLYDATA_بھیجا_T:
    *param_offt = ڈیٹا->پیش_رفت.earlydata_بھیجا;
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }

  واپس CURLE_OK;
}

ساکن CURLcode getinfo_double(ساخت Curl_easy *ڈیٹا, CURLINFO info,
                               دوہرا *param_doublep)
{
#اگر_تعریف_شدہ DEBUGBUILD
  ثابت حرف *envstr;

  انتخاب(info) {
  حالت CURLINFO_کل_وقت:
  حالت CURLINFO_NAMELOOKUP_وقت:
  حالت CURLINFO_CONNECT_وقت:
  حالت CURLINFO_APPCONNECT_وقت:
  حالت CURLINFO_PRETRANSFER_وقت:
  حالت CURLINFO_STARTTRANSFER_وقت:
  حالت CURLINFO_REDIRECT_وقت:
  حالت CURLINFO_رفتار_DOWNLOAD:
  حالت CURLINFO_رفتار_UPLOAD:
    envstr = getenv("CURL_TIME");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (دوہرا)val;
      واپس CURLE_OK;
    }
    توڑو;
  طےشدہ:
    توڑو;
  }
#اگر_ختم
  انتخاب(info) {
  حالت CURLINFO_کل_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.کل.spent_us);
    توڑو;
  حالت CURLINFO_NAMELOOKUP_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.کل.nslookup_us);
    توڑو;
  حالت CURLINFO_CONNECT_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.کل.connect_us);
    توڑو;
  حالت CURLINFO_APPCONNECT_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.کل.appconnect_us);
    توڑو;
  حالت CURLINFO_PRETRANSFER_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.کل.pretransfer_us);
    توڑو;
  حالت CURLINFO_STARTTRANSFER_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.کل.starttransfer_us);
    توڑو;
  حالت CURLINFO_سائز_UPLOAD:
    *param_doublep = (دوہرا)ڈیٹا->پیش_رفت.ul.cur_سائز;
    توڑو;
  حالت CURLINFO_سائز_DOWNLOAD:
    *param_doublep = (دوہرا)ڈیٹا->پیش_رفت.dl.cur_سائز;
    توڑو;
  حالت CURLINFO_رفتار_DOWNLOAD:
    *param_doublep = (دوہرا)ڈیٹا->پیش_رفت.dl.رفتار;
    توڑو;
  حالت CURLINFO_رفتار_UPLOAD:
    *param_doublep = (دوہرا)ڈیٹا->پیش_رفت.ul.رفتار;
    توڑو;
  حالت CURLINFO_CONTENT_لمبائی_DOWNLOAD:
    *param_doublep = ڈیٹا->پیش_رفت.dl_سائز_معلوم ?
      (دوہرا)ڈیٹا->پیش_رفت.dl.کل_سائز : -1;
    توڑو;
  حالت CURLINFO_CONTENT_لمبائی_UPLOAD:
    *param_doublep = ڈیٹا->پیش_رفت.ul_سائز_معلوم ?
      (دوہرا)ڈیٹا->پیش_رفت.ul.کل_سائز : -1;
    توڑو;
  حالت CURLINFO_REDIRECT_وقت:
    *param_doublep = DOUBLE_SECS(ڈیٹا->پیش_رفت.delta.startredirect_us);
    توڑو;

  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }

  واپس CURLE_OK;
}

ساکن CURLcode getinfo_slist(ساخت Curl_easy *ڈیٹا, CURLINFO info,
                              ساخت curl_slist **param_slistp)
{
  اتحاد {
    ساخت curl_certinfo *مقصد_certinfo;
    ساخت curl_slist    *مقصد_slist;
  } ptr;

  انتخاب(info) {
  حالت CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(ڈیٹا);
    توڑو;
  حالت CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(ڈیٹا);
    توڑو;
  حالت CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.مقصد_certinfo = &ڈیٹا->info.certs;
    *param_slistp = ptr.مقصد_slist;
    توڑو;
  حالت CURLINFO_TLS_SESSION:
  حالت CURLINFO_TLS_SSL_PTR: {
    صحیح query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ساخت curl_tlssessioninfo **tsip = (ساخت curl_tlssessioninfo **)
      param_slistp;
    ساخت curl_tlssessioninfo *tsi = &ڈیٹا->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    اگر(!Curl_conn_get_ssl_info(ڈیٹا, ڈیٹا->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = تہی;
    }
  }
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }

  واپس CURLE_OK;
}

ساکن CURLcode getinfo_socket(ساخت Curl_easy *ڈیٹا, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  انتخاب(info) {
  حالت CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(ڈیٹا, تہی);
    توڑو;
  طےشدہ:
    واپس CURLE_UNKNOWN_OPTION;
  }

  واپس CURLE_OK;
}

CURLcode Curl_getinfo(ساخت Curl_easy *ڈیٹا, CURLINFO info, ...)
{
  va_list arg;
  طویل *param_longp = تہی;
  دوہرا *param_doublep = تہی;
  curl_off_t *param_offt = تہی;
  ثابت حرف **param_charp = تہی;
  ساخت curl_slist **param_slistp = تہی;
  curl_socket_t *param_socketp = تہی;
  صحیح type;
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;

  اگر(!ڈیٹا)
    واپس CURLE_BAD_FUNCTION_دلیل;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحیح)info;
  انتخاب(type) {
  حالت CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت حرف **);
    اگر(param_charp)
      نتیجہ = getinfo_char(ڈیٹا, info, param_charp);
    توڑو;
  حالت CURLINFO_LONG:
    param_longp = va_arg(arg, طویل *);
    اگر(param_longp)
      نتیجہ = getinfo_long(ڈیٹا, info, param_longp);
    توڑو;
  حالت CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, دوہرا *);
    اگر(param_doublep)
      نتیجہ = getinfo_double(ڈیٹا, info, param_doublep);
    توڑو;
  حالت CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    اگر(param_offt)
      نتیجہ = getinfo_offt(ڈیٹا, info, param_offt);
    توڑو;
  حالت CURLINFO_SLIST:
    param_slistp = va_arg(arg, ساخت curl_slist **);
    اگر(param_slistp)
      نتیجہ = getinfo_slist(ڈیٹا, info, param_slistp);
    توڑو;
  حالت CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    اگر(param_socketp)
      نتیجہ = getinfo_socket(ڈیٹا, info, param_socketp);
    توڑو;
  طےشدہ:
    نتیجہ = CURLE_UNKNOWN_OPTION;
    توڑو;
  }

  va_end(arg);

  واپس نتیجہ;
}
