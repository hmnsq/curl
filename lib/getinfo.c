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
فراغ Curl_initinfo(بنية Curl_easy *معلومات)
{
  بنية پرمختګ_2 *pro = &معلومات->پرمختګ;
  بنية خالص_Info *info = &معلومات->info;

  memset(&pro->delta, 0, حجم(pro->delta));
  memset(&pro->ټول, 0, حجم(pro->ټول));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->سرلیک_اندازه = 0;
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
  info->کارول_شوی_proxy = 0;

#که_تعریف_شوی USE_SSL
  Curl_ssl_free_certinfo(معلومات);
#د_که_پای
}

ساكن CURLcode effective_url(بنية Curl_easy *معلومات,
                              ثابت حرف **param_charp)
{
  ثابت حرف *s = Curl_bufref_ptr(&معلومات->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? عدم : "";
  إذا(s) {
    حرف *url = عدم;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    إذا(!u)
      أرجع CURLE_بهر_د_حافظه;

    uc = curl_url_سېټ(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (معلومات->سېټ.لار_as_is ? CURLU_لار_AS_IS : 0));
    إذا(!uc)
      uc = curl_url_سېټ(u, CURLUPART_USER, عدم, 0);
    إذا(!uc)
      uc = curl_url_سېټ(u, CURLUPART_PASSWORD, عدم, 0);
    إذا(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_تشه);

    curl_url_cleanup(u);

    إذا(uc || !url)
      أرجع uc ? Curl_uc_منزل_curlcode(uc) : CURLE_بهر_د_حافظه;
    إذا(strcmp(url, s)) {
      /* they are different */
      curlx_free(معلومات->info.effective_url);
      معلومات->info.effective_url = url;
      *param_charp = url;
    }
    وإلا {
      curl_free(url);
      *param_charp = s;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_char(بنية Curl_easy *معلومات, CURLINFO info,
                             ثابت حرف **param_charp)
{
  بدل(info) {
  حالة CURLINFO_EFFECTIVE_URL:
    أرجع effective_url(معلومات, param_charp);
  حالة CURLINFO_EFFECTIVE_لاره: {
    ثابت حرف *m = CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST);
    إذا(!m) {
      إذا(معلومات->سېټ.opt_no_body)
        m = "HEAD";
#که_نه_تعریف_شوی CURL_DISABLE_HTTP
      وإلا {
        بدل(معلومات->state.httpreq) {
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
#د_که_پای
    }
    *param_charp = m;
  }
    اكسر;
  حالة CURLINFO_CONTENT_TYPE:
    *param_charp = معلومات->info.contenttype;
    اكسر;
  حالة CURLINFO_شخصي:
    *param_charp = (ثابت حرف *)معلومات->سېټ.شخصي_معلومات;
    اكسر;
  حالة CURLINFO_FTP_ثبت_لار:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = معلومات->state.most_recent_ftp_entrypath;
    اكسر;
  حالة CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = معلومات->info.wouldredirect;
    اكسر;
  حالة CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&معلومات->state.referer);
    اكسر;
  حالة CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = معلومات->info.primary.remote_ip;
    اكسر;
  حالة CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = معلومات->info.primary.local_ip;
    اكسر;
  حالة CURLINFO_RTSP_SESSION_ID:
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(معلومات, STRING_RTSP_SESSION_ID);
#وإلا
    *param_charp = عدم;
#د_که_پای
    اكسر;
  حالة CURLINFO_SCHEME:
    *param_charp = معلومات->info.conn_scheme;
    اكسر;
  حالة CURLINFO_CAPATH:
#که_تعریف_شوی CURL_CA_لار
    *param_charp = CURL_CA_لار;
#وإلا
    *param_charp = عدم;
#د_که_پای
    اكسر;
  حالة CURLINFO_CAINFO:
#که_تعریف_شوی CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#وإلا
    *param_charp = عدم;
#د_که_پای
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_long(بنية Curl_easy *معلومات, CURLINFO info,
                             اوږد *param_longp)
{
  curl_socket_t sockfd;

  اتحاد {
    بینښه اوږد *منزل_ulong;
    اوږد          *منزل_long;
  } lptr;

#که_تعریف_شوی DEBUGBUILD
  ثابت حرف *envstr;

  /* use another variable for this to allow different values */
  بدل(info) {
  حالة CURLINFO_LOCAL_پورټ:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_شمېره(&envstr, &val, وخت_T_MAX);
      *param_longp = (اوږد)val;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CURLINFO_سرلیک_اندازه:
  حالة CURLINFO_REQUEST_اندازه:
    envstr = getenv("CURL_DEBUG_SIZE");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_شمېره(&envstr, &val, LONG_MAX);
      *param_longp = (اوږد)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#د_که_پای

  بدل(info) {
  حالة CURLINFO_RESPONSE_کوډ:
    *param_longp = معلومات->info.httpcode;
    اكسر;
  حالة CURLINFO_HTTP_CONNECTCODE:
    *param_longp = معلومات->info.httpproxycode;
    اكسر;
  حالة CURLINFO_FILETIME:
    إذا(معلومات->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#إذا !defined(MSDOS) && !defined(__AMIGA__)
    وإلا إذا(معلومات->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#د_که_پای
    وإلا
      *param_longp = (اوږد)معلومات->info.filetime;
    اكسر;
  حالة CURLINFO_سرلیک_اندازه:
    *param_longp = (اوږد)معلومات->info.سرلیک_اندازه;
    اكسر;
  حالة CURLINFO_REQUEST_اندازه:
    *param_longp = (اوږد)معلومات->info.request_اندازه;
    اكسر;
  حالة CURLINFO_SSL_VERIFYRESULT:
    *param_longp = معلومات->سېټ.ssl.certverifyresult;
    اكسر;
  حالة CURLINFO_PROXY_SSL_VERIFYRESULT:
#که_نه_تعریف_شوی CURL_DISABLE_PROXY
    *param_longp = معلومات->سېټ.proxy_ssl.certverifyresult;
#وإلا
    *param_longp = 0;
#د_که_پای
    اكسر;
  حالة CURLINFO_REDIRECT_شمېر:
    *param_longp = معلومات->state.followlocation;
    اكسر;
  حالة CURLINFO_HTTPAUTH_AVAIL:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = معلومات->info.httpauthavail;
    اكسر;
  حالة CURLINFO_PROXYAUTH_AVAIL:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = معلومات->info.proxyauthavail;
    اكسر;
  حالة CURLINFO_HTTPAUTH_کارول_شوی:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = معلومات->info.httpauthpicked;
    اكسر;
  حالة CURLINFO_PROXYAUTH_کارول_شوی:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = معلومات->info.proxyauthpicked;
    اكسر;
  حالة CURLINFO_OS_ERRNO:
    *param_longp = معلومات->state.os_errno;
    اكسر;
  حالة CURLINFO_عدد_CONNECTS:
#إذا SIZEOF_LONG < SIZEOF_CURL_OFF_T
    إذا(معلومات->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    وإلا
#د_که_پای
      *param_longp = (اوږد)معلومات->info.numconnects;
    اكسر;
  حالة CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(معلومات, عدم);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    إذا(sockfd != CURL_SOCKET_BAD)
      *param_longp = (اوږد)sockfd;
    وإلا
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PRIMARY_پورټ:
    /* Return the (remote) port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&معلومات->info.primary))
      *param_longp = معلومات->info.primary.remote_پورټ;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_LOCAL_پورټ:
    /* Return the local port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&معلومات->info.primary))
      *param_longp = معلومات->info.primary.local_پورټ;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PROXY_ERROR:
    *param_longp = (اوږد)معلومات->info.pxcode;
    اكسر;
  حالة CURLINFO_CONDITION_UNMET:
    إذا(معلومات->info.httpcode == 304)
      *param_longp = 1L;
    وإلا
      /* return if the condition prevented the document to get transferred */
      *param_longp = معلومات->info.timecond ? 1L : 0L;
    اكسر;
#که_نه_تعریف_شوی CURL_DISABLE_RTSP
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = معلومات->state.rtsp_بل_client_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = معلومات->state.rtsp_بل_server_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = معلومات->state.rtsp_CSeq_recv;
    اكسر;
#وإلا
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
  حالة CURLINFO_RTSP_SERVER_CSEQ:
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    اكسر;
#د_که_پای
  حالة CURLINFO_HTTP_نسخه:
    بدل(معلومات->info.httpversion) {
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
    *param_longp = (اوږد)معلومات->info.conn_protocol;
    اكسر;
  حالة CURLINFO_کارول_شوی_PROXY:
    *param_longp =
#که_تعریف_شوی CURL_DISABLE_PROXY
      0
#وإلا
      معلومات->info.کارول_شوی_proxy
#د_که_پای
      ;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

#تعریف_کړه DOUBLE_SECS(x) ((مزدوج)(x) / 1000000)

ساكن CURLcode getinfo_offt(بنية Curl_easy *معلومات, CURLINFO info,
                             curl_off_t *param_offt)
{
#که_تعریف_شوی DEBUGBUILD
  ثابت حرف *envstr;
  بدل(info) {
  حالة CURLINFO_ټول_وخت_T:
  حالة CURLINFO_NAMELOOKUP_وخت_T:
  حالة CURLINFO_CONNECT_وخت_T:
  حالة CURLINFO_APPCONNECT_وخت_T:
  حالة CURLINFO_PRETRANSFER_وخت_T:
  حالة CURLINFO_POSTTRANSFER_وخت_T:
  حالة CURLINFO_کتار_وخت_T:
  حالة CURLINFO_STARTTRANSFER_وخت_T:
  حالة CURLINFO_REDIRECT_وخت_T:
  حالة CURLINFO_چټکتیا_DOWNLOAD_T:
  حالة CURLINFO_چټکتیا_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_شمېره(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#د_که_پای
  بدل(info) {
  حالة CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)معلومات->info.filetime;
    اكسر;
  حالة CURLINFO_اندازه_DELIVERED:
    *param_offt = معلومات->پرمختګ.deliver;
    اكسر;
  حالة CURLINFO_اندازه_UPLOAD_T:
    *param_offt = معلومات->پرمختګ.ul.cur_اندازه;
    اكسر;
  حالة CURLINFO_اندازه_DOWNLOAD_T:
    *param_offt = معلومات->پرمختګ.dl.cur_اندازه;
    اكسر;
  حالة CURLINFO_چټکتیا_DOWNLOAD_T:
    *param_offt = معلومات->پرمختګ.dl.چټکتیا;
    اكسر;
  حالة CURLINFO_چټکتیا_UPLOAD_T:
    *param_offt = معلومات->پرمختګ.ul.چټکتیا;
    اكسر;
  حالة CURLINFO_CONTENT_اوږدتیا_DOWNLOAD_T:
    *param_offt = معلومات->پرمختګ.dl_اندازه_معلوم ?
      معلومات->پرمختګ.dl.ټول_اندازه : -1;
    اكسر;
  حالة CURLINFO_CONTENT_اوږدتیا_UPLOAD_T:
    *param_offt = معلومات->پرمختګ.ul_اندازه_معلوم ?
      معلومات->پرمختګ.ul.ټول_اندازه : -1;
    اكسر;
   حالة CURLINFO_ټول_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.spent_us;
    اكسر;
  حالة CURLINFO_NAMELOOKUP_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.nslookup_us;
    اكسر;
  حالة CURLINFO_CONNECT_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.connect_us;
    اكسر;
  حالة CURLINFO_APPCONNECT_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.appconnect_us;
    اكسر;
  حالة CURLINFO_PRETRANSFER_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.pretransfer_us;
    اكسر;
  حالة CURLINFO_POSTTRANSFER_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.posttransfer_us;
    اكسر;
  حالة CURLINFO_STARTTRANSFER_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.starttransfer_us;
    اكسر;
  حالة CURLINFO_کتار_وخت_T:
    *param_offt = معلومات->پرمختګ.ټول.queued_us;
    اكسر;
  حالة CURLINFO_REDIRECT_وخت_T:
    *param_offt = معلومات->پرمختګ.delta.startredirect_us;
    اكسر;
  حالة CURLINFO_RETRY_AFTER:
    *param_offt = معلومات->info.retry_after;
    اكسر;
  حالة CURLINFO_XFER_ID:
    *param_offt = معلومات->id;
    اكسر;
  حالة CURLINFO_CONN_ID:
    *param_offt = معلومات->state.وروستی_conn_id;
    اكسر;
  حالة CURLINFO_EARLYDATA_لېږل_شوی_T:
    *param_offt = معلومات->پرمختګ.earlydata_لېږل_شوی;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_double(بنية Curl_easy *معلومات, CURLINFO info,
                               مزدوج *param_doublep)
{
#که_تعریف_شوی DEBUGBUILD
  ثابت حرف *envstr;

  بدل(info) {
  حالة CURLINFO_ټول_وخت:
  حالة CURLINFO_NAMELOOKUP_وخت:
  حالة CURLINFO_CONNECT_وخت:
  حالة CURLINFO_APPCONNECT_وخت:
  حالة CURLINFO_PRETRANSFER_وخت:
  حالة CURLINFO_STARTTRANSFER_وخت:
  حالة CURLINFO_REDIRECT_وخت:
  حالة CURLINFO_چټکتیا_DOWNLOAD:
  حالة CURLINFO_چټکتیا_UPLOAD:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_شمېره(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (مزدوج)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#د_که_پای
  بدل(info) {
  حالة CURLINFO_ټول_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.ټول.spent_us);
    اكسر;
  حالة CURLINFO_NAMELOOKUP_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.ټول.nslookup_us);
    اكسر;
  حالة CURLINFO_CONNECT_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.ټول.connect_us);
    اكسر;
  حالة CURLINFO_APPCONNECT_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.ټول.appconnect_us);
    اكسر;
  حالة CURLINFO_PRETRANSFER_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.ټول.pretransfer_us);
    اكسر;
  حالة CURLINFO_STARTTRANSFER_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.ټول.starttransfer_us);
    اكسر;
  حالة CURLINFO_اندازه_UPLOAD:
    *param_doublep = (مزدوج)معلومات->پرمختګ.ul.cur_اندازه;
    اكسر;
  حالة CURLINFO_اندازه_DOWNLOAD:
    *param_doublep = (مزدوج)معلومات->پرمختګ.dl.cur_اندازه;
    اكسر;
  حالة CURLINFO_چټکتیا_DOWNLOAD:
    *param_doublep = (مزدوج)معلومات->پرمختګ.dl.چټکتیا;
    اكسر;
  حالة CURLINFO_چټکتیا_UPLOAD:
    *param_doublep = (مزدوج)معلومات->پرمختګ.ul.چټکتیا;
    اكسر;
  حالة CURLINFO_CONTENT_اوږدتیا_DOWNLOAD:
    *param_doublep = معلومات->پرمختګ.dl_اندازه_معلوم ?
      (مزدوج)معلومات->پرمختګ.dl.ټول_اندازه : -1;
    اكسر;
  حالة CURLINFO_CONTENT_اوږدتیا_UPLOAD:
    *param_doublep = معلومات->پرمختګ.ul_اندازه_معلوم ?
      (مزدوج)معلومات->پرمختګ.ul.ټول_اندازه : -1;
    اكسر;
  حالة CURLINFO_REDIRECT_وخت:
    *param_doublep = DOUBLE_SECS(معلومات->پرمختګ.delta.startredirect_us);
    اكسر;

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_slist(بنية Curl_easy *معلومات, CURLINFO info,
                              بنية curl_slist **param_slistp)
{
  اتحاد {
    بنية curl_certinfo *منزل_certinfo;
    بنية curl_slist    *منزل_slist;
  } ptr;

  بدل(info) {
  حالة CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(معلومات);
    اكسر;
  حالة CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(معلومات);
    اكسر;
  حالة CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.منزل_certinfo = &معلومات->info.certs;
    *param_slistp = ptr.منزل_slist;
    اكسر;
  حالة CURLINFO_TLS_SESSION:
  حالة CURLINFO_TLS_SSL_PTR: {
    صحيح query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    بنية curl_tlssessioninfo **tsip = (بنية curl_tlssessioninfo **)
      param_slistp;
    بنية curl_tlssessioninfo *tsi = &معلومات->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    إذا(!Curl_conn_get_ssl_info(معلومات, معلومات->conn, FIRSTSOCKET, query, tsi)) {
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

ساكن CURLcode getinfo_socket(بنية Curl_easy *معلومات, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  بدل(info) {
  حالة CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(معلومات, عدم);
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_getinfo(بنية Curl_easy *معلومات, CURLINFO info, ...)
{
  va_list arg;
  اوږد *param_longp = عدم;
  مزدوج *param_doublep = عدم;
  curl_off_t *param_offt = عدم;
  ثابت حرف **param_charp = عدم;
  بنية curl_slist **param_slistp = عدم;
  curl_socket_t *param_socketp = عدم;
  صحيح type;
  CURLcode پایله = CURLE_BAD_FUNCTION_دلیل;

  إذا(!معلومات)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحيح)info;
  بدل(type) {
  حالة CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت حرف **);
    إذا(param_charp)
      پایله = getinfo_char(معلومات, info, param_charp);
    اكسر;
  حالة CURLINFO_LONG:
    param_longp = va_arg(arg, اوږد *);
    إذا(param_longp)
      پایله = getinfo_long(معلومات, info, param_longp);
    اكسر;
  حالة CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, مزدوج *);
    إذا(param_doublep)
      پایله = getinfo_double(معلومات, info, param_doublep);
    اكسر;
  حالة CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    إذا(param_offt)
      پایله = getinfo_offt(معلومات, info, param_offt);
    اكسر;
  حالة CURLINFO_SLIST:
    param_slistp = va_arg(arg, بنية curl_slist **);
    إذا(param_slistp)
      پایله = getinfo_slist(معلومات, info, param_slistp);
    اكسر;
  حالة CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    إذا(param_socketp)
      پایله = getinfo_socket(معلومات, info, param_socketp);
    اكسر;
  افتراضي:
    پایله = CURLE_UNKNOWN_OPTION;
    اكسر;
  }

  va_end(arg);

  أرجع پایله;
}
