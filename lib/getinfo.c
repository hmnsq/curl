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
#ضمن "getinfo.h"
#ضمن "cfilters.h"
#ضمن "vtls/vtls.h"
#ضمن "connect.h" /* Curl_getconnectinfo() */
#ضمن "bufref.h"
#ضمن "url.h" /* Curl_uc_to_curlcode() */
#ضمن "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
فراغ Curl_initinfo(بنية Curl_easy *داده)
{
  بنية پیشرفت_2 *pro = &داده->پیشرفت;
  بنية خالص_Info *info = &داده->info;

  memset(&pro->delta, 0, حجم(pro->delta));
  memset(&pro->مجموع, 0, حجم(pro->مجموع));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->سرآیند_اندازه = 0;
  info->request_اندازه = 0;
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
  info->استفاده_شده_proxy = 0;

#اگر_تعریف_شده USE_SSL
  Curl_ssl_free_certinfo(داده);
#پایان_اگر
}

ساكن CURLcode effective_url(بنية Curl_easy *داده,
                              ثابت حرف **param_charp)
{
  ثابت حرف *s = Curl_bufref_ptr(&داده->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? عدم : "";
  إذا(s) {
    حرف *url = عدم;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    إذا(!u)
      أرجع CURLE_بیرون_از_حافظه;

    uc = curl_url_مجموعه(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (داده->مجموعه.مسیر_as_is ? CURLU_مسیر_AS_IS : 0));
    إذا(!uc)
      uc = curl_url_مجموعه(u, CURLUPART_USER, عدم, 0);
    إذا(!uc)
      uc = curl_url_مجموعه(u, CURLUPART_PASSWORD, عدم, 0);
    إذا(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_خالی);

    curl_url_cleanup(u);

    إذا(uc || !url)
      أرجع uc ? Curl_uc_مقصد_curlcode(uc) : CURLE_بیرون_از_حافظه;
    إذا(strcmp(url, s)) {
      /* they are different */
      curlx_free(داده->info.effective_url);
      داده->info.effective_url = url;
      *param_charp = url;
    }
    وإلا {
      curl_free(url);
      *param_charp = s;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_char(بنية Curl_easy *داده, CURLINFO info,
                             ثابت حرف **param_charp)
{
  بدل(info) {
  حالة CURLINFO_EFFECTIVE_URL:
    أرجع effective_url(داده, param_charp);
  حالة CURLINFO_EFFECTIVE_روش: {
    ثابت حرف *m = CURL_EASY_STR(داده, STRING_CUSTOMREQUEST);
    إذا(!m) {
      إذا(داده->مجموعه.opt_no_body)
        m = "HEAD";
#اگر_تعریف_نشده CURL_DISABLE_HTTP
      وإلا {
        بدل(داده->state.httpreq) {
        حالة HTTPREQ_POST:
        حالة HTTPREQ_POST_FORM:
        حالة HTTPREQ_POST_MIME:
          m = "POST";
          اكسر;
        حالة HTTPREQ_PUT:
          m = "PUT";
          اكسر;
        افتراضي: /* this should never happen */
        حالة HTTPREQ_GET:
          m = "GET";
          اكسر;
        حالة HTTPREQ_سر:
          m = "HEAD";
          اكسر;
        }
      }
#پایان_اگر
    }
    *param_charp = m;
  }
    اكسر;
  حالة CURLINFO_CONTENT_TYPE:
    *param_charp = داده->info.contenttype;
    اكسر;
  حالة CURLINFO_خصوصی:
    *param_charp = (ثابت حرف *)داده->مجموعه.خصوصی_داده;
    اكسر;
  حالة CURLINFO_FTP_مدخل_مسیر:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = داده->state.most_recent_ftp_entrypath;
    اكسر;
  حالة CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = داده->info.wouldredirect;
    اكسر;
  حالة CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&داده->state.referer);
    اكسر;
  حالة CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = داده->info.primary.remote_ip;
    اكسر;
  حالة CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = داده->info.primary.local_ip;
    اكسر;
  حالة CURLINFO_RTSP_SESSION_ID:
#اگر_تعریف_نشده CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(داده, STRING_RTSP_SESSION_ID);
#وإلا
    *param_charp = عدم;
#پایان_اگر
    اكسر;
  حالة CURLINFO_SCHEME:
    *param_charp = داده->info.conn_scheme;
    اكسر;
  حالة CURLINFO_CAPATH:
#اگر_تعریف_شده CURL_CA_مسیر
    *param_charp = CURL_CA_مسیر;
#وإلا
    *param_charp = عدم;
#پایان_اگر
    اكسر;
  حالة CURLINFO_CAINFO:
#اگر_تعریف_شده CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#وإلا
    *param_charp = عدم;
#پایان_اگر
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_long(بنية Curl_easy *داده, CURLINFO info,
                             دراز *param_longp)
{
  curl_socket_t sockfd;

  اتحاد {
    بیعلامت دراز *مقصد_ulong;
    دراز          *مقصد_long;
  } lptr;

#اگر_تعریف_شده DEBUGBUILD
  ثابت حرف *envstr;

  /* use another variable for this to allow different values */
  بدل(info) {
  حالة CURLINFO_LOCAL_پورت:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, زمان_T_MAX);
      *param_longp = (دراز)val;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CURLINFO_سرآیند_اندازه:
  حالة CURLINFO_REQUEST_اندازه:
    envstr = getenv("CURL_DEBUG_SIZE");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, LONG_MAX);
      *param_longp = (دراز)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#پایان_اگر

  بدل(info) {
  حالة CURLINFO_RESPONSE_کد:
    *param_longp = داده->info.httpcode;
    اكسر;
  حالة CURLINFO_HTTP_CONNECTCODE:
    *param_longp = داده->info.httpproxycode;
    اكسر;
  حالة CURLINFO_FILETIME:
    إذا(داده->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#إذا !defined(MSDOS) && !defined(__AMIGA__)
    وإلا إذا(داده->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#پایان_اگر
    وإلا
      *param_longp = (دراز)داده->info.filetime;
    اكسر;
  حالة CURLINFO_سرآیند_اندازه:
    *param_longp = (دراز)داده->info.سرآیند_اندازه;
    اكسر;
  حالة CURLINFO_REQUEST_اندازه:
    *param_longp = (دراز)داده->info.request_اندازه;
    اكسر;
  حالة CURLINFO_SSL_VERIFYRESULT:
    *param_longp = داده->مجموعه.ssl.certverifyresult;
    اكسر;
  حالة CURLINFO_PROXY_SSL_VERIFYRESULT:
#اگر_تعریف_نشده CURL_DISABLE_PROXY
    *param_longp = داده->مجموعه.proxy_ssl.certverifyresult;
#وإلا
    *param_longp = 0;
#پایان_اگر
    اكسر;
  حالة CURLINFO_REDIRECT_شمار:
    *param_longp = داده->state.followlocation;
    اكسر;
  حالة CURLINFO_HTTPAUTH_AVAIL:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = داده->info.httpauthavail;
    اكسر;
  حالة CURLINFO_PROXYAUTH_AVAIL:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = داده->info.proxyauthavail;
    اكسر;
  حالة CURLINFO_HTTPAUTH_استفاده_شده:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = داده->info.httpauthpicked;
    اكسر;
  حالة CURLINFO_PROXYAUTH_استفاده_شده:
    lptr.مقصد_long = param_longp;
    *lptr.مقصد_ulong = داده->info.proxyauthpicked;
    اكسر;
  حالة CURLINFO_OS_ERRNO:
    *param_longp = داده->state.os_errno;
    اكسر;
  حالة CURLINFO_رقم_CONNECTS:
#إذا SIZEOF_LONG < SIZEOF_CURL_OFF_T
    إذا(داده->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    وإلا
#پایان_اگر
      *param_longp = (دراز)داده->info.numconnects;
    اكسر;
  حالة CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(داده, عدم);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    إذا(sockfd != CURL_SOCKET_BAD)
      *param_longp = (دراز)sockfd;
    وإلا
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PRIMARY_پورت:
    /* Return the (remote) port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&داده->info.primary))
      *param_longp = داده->info.primary.remote_پورت;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_LOCAL_پورت:
    /* Return the local port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&داده->info.primary))
      *param_longp = داده->info.primary.local_پورت;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PROXY_ERROR:
    *param_longp = (دراز)داده->info.pxcode;
    اكسر;
  حالة CURLINFO_CONDITION_UNMET:
    إذا(داده->info.httpcode == 304)
      *param_longp = 1L;
    وإلا
      /* return if the condition prevented the document to get transferred */
      *param_longp = داده->info.timecond ? 1L : 0L;
    اكسر;
#اگر_تعریف_نشده CURL_DISABLE_RTSP
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = داده->state.rtsp_بعدی_client_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = داده->state.rtsp_بعدی_server_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = داده->state.rtsp_CSeq_recv;
    اكسر;
#وإلا
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
  حالة CURLINFO_RTSP_SERVER_CSEQ:
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    اكسر;
#پایان_اگر
  حالة CURLINFO_HTTP_نسخه:
    بدل(داده->info.httpversion) {
    حالة 10:
      *param_longp = CURL_HTTP_نسخه_1_0;
      اكسر;
    حالة 11:
      *param_longp = CURL_HTTP_نسخه_1_1;
      اكسر;
    حالة 20:
      *param_longp = CURL_HTTP_نسخه_2_0;
      اكسر;
    حالة 30:
      *param_longp = CURL_HTTP_نسخه_3;
      اكسر;
    افتراضي:
      *param_longp = CURL_HTTP_نسخه_NONE;
      اكسر;
    }
    اكسر;
  حالة CURLINFO_PROTOCOL:
    *param_longp = (دراز)داده->info.conn_protocol;
    اكسر;
  حالة CURLINFO_استفاده_شده_PROXY:
    *param_longp =
#اگر_تعریف_شده CURL_DISABLE_PROXY
      0
#وإلا
      داده->info.استفاده_شده_proxy
#پایان_اگر
      ;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

#تعریف_کن DOUBLE_SECS(x) ((مزدوج)(x) / 1000000)

ساكن CURLcode getinfo_offt(بنية Curl_easy *داده, CURLINFO info,
                             curl_off_t *param_offt)
{
#اگر_تعریف_شده DEBUGBUILD
  ثابت حرف *envstr;
  بدل(info) {
  حالة CURLINFO_مجموع_زمان_T:
  حالة CURLINFO_NAMELOOKUP_زمان_T:
  حالة CURLINFO_CONNECT_زمان_T:
  حالة CURLINFO_APPCONNECT_زمان_T:
  حالة CURLINFO_PRETRANSFER_زمان_T:
  حالة CURLINFO_POSTTRANSFER_زمان_T:
  حالة CURLINFO_صف_زمان_T:
  حالة CURLINFO_STARTTRANSFER_زمان_T:
  حالة CURLINFO_REDIRECT_زمان_T:
  حالة CURLINFO_سرعت_DOWNLOAD_T:
  حالة CURLINFO_سرعت_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#پایان_اگر
  بدل(info) {
  حالة CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)داده->info.filetime;
    اكسر;
  حالة CURLINFO_اندازه_DELIVERED:
    *param_offt = داده->پیشرفت.deliver;
    اكسر;
  حالة CURLINFO_اندازه_UPLOAD_T:
    *param_offt = داده->پیشرفت.ul.cur_اندازه;
    اكسر;
  حالة CURLINFO_اندازه_DOWNLOAD_T:
    *param_offt = داده->پیشرفت.dl.cur_اندازه;
    اكسر;
  حالة CURLINFO_سرعت_DOWNLOAD_T:
    *param_offt = داده->پیشرفت.dl.سرعت;
    اكسر;
  حالة CURLINFO_سرعت_UPLOAD_T:
    *param_offt = داده->پیشرفت.ul.سرعت;
    اكسر;
  حالة CURLINFO_CONTENT_درازی_DOWNLOAD_T:
    *param_offt = داده->پیشرفت.dl_اندازه_معلوم ?
      داده->پیشرفت.dl.مجموع_اندازه : -1;
    اكسر;
  حالة CURLINFO_CONTENT_درازی_UPLOAD_T:
    *param_offt = داده->پیشرفت.ul_اندازه_معلوم ?
      داده->پیشرفت.ul.مجموع_اندازه : -1;
    اكسر;
   حالة CURLINFO_مجموع_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.spent_us;
    اكسر;
  حالة CURLINFO_NAMELOOKUP_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.nslookup_us;
    اكسر;
  حالة CURLINFO_CONNECT_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.connect_us;
    اكسر;
  حالة CURLINFO_APPCONNECT_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.appconnect_us;
    اكسر;
  حالة CURLINFO_PRETRANSFER_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.pretransfer_us;
    اكسر;
  حالة CURLINFO_POSTTRANSFER_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.posttransfer_us;
    اكسر;
  حالة CURLINFO_STARTTRANSFER_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.starttransfer_us;
    اكسر;
  حالة CURLINFO_صف_زمان_T:
    *param_offt = داده->پیشرفت.مجموع.queued_us;
    اكسر;
  حالة CURLINFO_REDIRECT_زمان_T:
    *param_offt = داده->پیشرفت.delta.startredirect_us;
    اكسر;
  حالة CURLINFO_RETRY_AFTER:
    *param_offt = داده->info.retry_after;
    اكسر;
  حالة CURLINFO_XFER_ID:
    *param_offt = داده->id;
    اكسر;
  حالة CURLINFO_CONN_ID:
    *param_offt = داده->state.آخرین_conn_id;
    اكسر;
  حالة CURLINFO_EARLYDATA_فرستاده_T:
    *param_offt = داده->پیشرفت.earlydata_فرستاده;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_double(بنية Curl_easy *داده, CURLINFO info,
                               مزدوج *param_doublep)
{
#اگر_تعریف_شده DEBUGBUILD
  ثابت حرف *envstr;

  بدل(info) {
  حالة CURLINFO_مجموع_زمان:
  حالة CURLINFO_NAMELOOKUP_زمان:
  حالة CURLINFO_CONNECT_زمان:
  حالة CURLINFO_APPCONNECT_زمان:
  حالة CURLINFO_PRETRANSFER_زمان:
  حالة CURLINFO_STARTTRANSFER_زمان:
  حالة CURLINFO_REDIRECT_زمان:
  حالة CURLINFO_سرعت_DOWNLOAD:
  حالة CURLINFO_سرعت_UPLOAD:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (مزدوج)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#پایان_اگر
  بدل(info) {
  حالة CURLINFO_مجموع_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.مجموع.spent_us);
    اكسر;
  حالة CURLINFO_NAMELOOKUP_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.مجموع.nslookup_us);
    اكسر;
  حالة CURLINFO_CONNECT_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.مجموع.connect_us);
    اكسر;
  حالة CURLINFO_APPCONNECT_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.مجموع.appconnect_us);
    اكسر;
  حالة CURLINFO_PRETRANSFER_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.مجموع.pretransfer_us);
    اكسر;
  حالة CURLINFO_STARTTRANSFER_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.مجموع.starttransfer_us);
    اكسر;
  حالة CURLINFO_اندازه_UPLOAD:
    *param_doublep = (مزدوج)داده->پیشرفت.ul.cur_اندازه;
    اكسر;
  حالة CURLINFO_اندازه_DOWNLOAD:
    *param_doublep = (مزدوج)داده->پیشرفت.dl.cur_اندازه;
    اكسر;
  حالة CURLINFO_سرعت_DOWNLOAD:
    *param_doublep = (مزدوج)داده->پیشرفت.dl.سرعت;
    اكسر;
  حالة CURLINFO_سرعت_UPLOAD:
    *param_doublep = (مزدوج)داده->پیشرفت.ul.سرعت;
    اكسر;
  حالة CURLINFO_CONTENT_درازی_DOWNLOAD:
    *param_doublep = داده->پیشرفت.dl_اندازه_معلوم ?
      (مزدوج)داده->پیشرفت.dl.مجموع_اندازه : -1;
    اكسر;
  حالة CURLINFO_CONTENT_درازی_UPLOAD:
    *param_doublep = داده->پیشرفت.ul_اندازه_معلوم ?
      (مزدوج)داده->پیشرفت.ul.مجموع_اندازه : -1;
    اكسر;
  حالة CURLINFO_REDIRECT_زمان:
    *param_doublep = DOUBLE_SECS(داده->پیشرفت.delta.startredirect_us);
    اكسر;

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_slist(بنية Curl_easy *داده, CURLINFO info,
                              بنية curl_slist **param_slistp)
{
  اتحاد {
    بنية curl_certinfo *مقصد_certinfo;
    بنية curl_slist    *مقصد_slist;
  } ptr;

  بدل(info) {
  حالة CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(داده);
    اكسر;
  حالة CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(داده);
    اكسر;
  حالة CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.مقصد_certinfo = &داده->info.certs;
    *param_slistp = ptr.مقصد_slist;
    اكسر;
  حالة CURLINFO_TLS_SESSION:
  حالة CURLINFO_TLS_SSL_PTR: {
    صحيح query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    بنية curl_tlssessioninfo **tsip = (بنية curl_tlssessioninfo **)
      param_slistp;
    بنية curl_tlssessioninfo *tsi = &داده->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    إذا(!Curl_conn_get_ssl_info(داده, داده->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = عدم;
    }
  }
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_socket(بنية Curl_easy *داده, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  بدل(info) {
  حالة CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(داده, عدم);
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_getinfo(بنية Curl_easy *داده, CURLINFO info, ...)
{
  va_list arg;
  دراز *param_longp = عدم;
  مزدوج *param_doublep = عدم;
  curl_off_t *param_offt = عدم;
  ثابت حرف **param_charp = عدم;
  بنية curl_slist **param_slistp = عدم;
  curl_socket_t *param_socketp = عدم;
  صحيح type;
  CURLcode نتیجه = CURLE_BAD_FUNCTION_آرگومان;

  إذا(!داده)
    أرجع CURLE_BAD_FUNCTION_آرگومان;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحيح)info;
  بدل(type) {
  حالة CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت حرف **);
    إذا(param_charp)
      نتیجه = getinfo_char(داده, info, param_charp);
    اكسر;
  حالة CURLINFO_LONG:
    param_longp = va_arg(arg, دراز *);
    إذا(param_longp)
      نتیجه = getinfo_long(داده, info, param_longp);
    اكسر;
  حالة CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, مزدوج *);
    إذا(param_doublep)
      نتیجه = getinfo_double(داده, info, param_doublep);
    اكسر;
  حالة CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    إذا(param_offt)
      نتیجه = getinfo_offt(داده, info, param_offt);
    اكسر;
  حالة CURLINFO_SLIST:
    param_slistp = va_arg(arg, بنية curl_slist **);
    إذا(param_slistp)
      نتیجه = getinfo_slist(داده, info, param_slistp);
    اكسر;
  حالة CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    إذا(param_socketp)
      نتیجه = getinfo_socket(داده, info, param_socketp);
    اكسر;
  افتراضي:
    نتیجه = CURLE_UNKNOWN_OPTION;
    اكسر;
  }

  va_end(arg);

  أرجع نتیجه;
}
