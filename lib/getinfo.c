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
فراغ Curl_initinfo(بنية Curl_easy *انکڑے)
{
  بنية ترقی_2 *pro = &انکڑے->ترقی;
  بنية خالص_Info *info = &انکڑے->info;

  memset(&pro->delta, 0, حجم(pro->delta));
  memset(&pro->کل, 0, حجم(pro->کل));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->سرخی_ناپ = 0;
  info->request_ناپ = 0;
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
  info->ورتیا_proxy = 0;

#جے_تعریف_شدہ USE_SSL
  Curl_ssl_free_certinfo(انکڑے);
#جے_انت
}

ساكن CURLcode effective_url(بنية Curl_easy *انکڑے,
                              ثابت حرف **param_charp)
{
  ثابت حرف *s = Curl_bufref_ptr(&انکڑے->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? عدم : "";
  إذا(s) {
    حرف *url = عدم;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    إذا(!u)
      أرجع CURLE_باہرلا_دا_حافظہ;

    uc = curl_url_سیٹ(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (انکڑے->سیٹ.رستہ_as_is ? CURLU_رستہ_AS_IS : 0));
    إذا(!uc)
      uc = curl_url_سیٹ(u, CURLUPART_USER, عدم, 0);
    إذا(!uc)
      uc = curl_url_سیٹ(u, CURLUPART_PASSWORD, عدم, 0);
    إذا(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_خالی_تھاواں);

    curl_url_cleanup(u);

    إذا(uc || !url)
      أرجع uc ? Curl_uc_منزل_curlcode(uc) : CURLE_باہرلا_دا_حافظہ;
    إذا(strcmp(url, s)) {
      /* they are different */
      curlx_free(انکڑے->info.effective_url);
      انکڑے->info.effective_url = url;
      *param_charp = url;
    }
    وإلا {
      curl_free(url);
      *param_charp = s;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_char(بنية Curl_easy *انکڑے, CURLINFO info,
                             ثابت حرف **param_charp)
{
  بدل(info) {
  حالة CURLINFO_EFFECTIVE_URL:
    أرجع effective_url(انکڑے, param_charp);
  حالة CURLINFO_EFFECTIVE_طریقہ: {
    ثابت حرف *m = CURL_EASY_STR(انکڑے, STRING_CUSTOMREQUEST);
    إذا(!m) {
      إذا(انکڑے->سیٹ.opt_no_body)
        m = "HEAD";
#جے_بنا_تعریف CURL_DISABLE_HTTP
      وإلا {
        بدل(انکڑے->state.httpreq) {
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
#جے_انت
    }
    *param_charp = m;
  }
    اكسر;
  حالة CURLINFO_CONTENT_TYPE:
    *param_charp = انکڑے->info.contenttype;
    اكسر;
  حالة CURLINFO_نجی:
    *param_charp = (ثابت حرف *)انکڑے->سیٹ.نجی_انکڑے;
    اكسر;
  حالة CURLINFO_FTP_اندراج_رستہ:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = انکڑے->state.most_recent_ftp_entrypath;
    اكسر;
  حالة CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = انکڑے->info.wouldredirect;
    اكسر;
  حالة CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&انکڑے->state.referer);
    اكسر;
  حالة CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = انکڑے->info.primary.remote_ip;
    اكسر;
  حالة CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = انکڑے->info.primary.local_ip;
    اكسر;
  حالة CURLINFO_RTSP_SESSION_ID:
#جے_بنا_تعریف CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(انکڑے, STRING_RTSP_SESSION_ID);
#وإلا
    *param_charp = عدم;
#جے_انت
    اكسر;
  حالة CURLINFO_SCHEME:
    *param_charp = انکڑے->info.conn_scheme;
    اكسر;
  حالة CURLINFO_CAPATH:
#جے_تعریف_شدہ CURL_CA_رستہ
    *param_charp = CURL_CA_رستہ;
#وإلا
    *param_charp = عدم;
#جے_انت
    اكسر;
  حالة CURLINFO_CAINFO:
#جے_تعریف_شدہ CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#وإلا
    *param_charp = عدم;
#جے_انت
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_long(بنية Curl_easy *انکڑے, CURLINFO info,
                             لما *param_longp)
{
  curl_socket_t sockfd;

  اتحاد {
    بنانشانی لما *منزل_ulong;
    لما          *منزل_long;
  } lptr;

#جے_تعریف_شدہ DEBUGBUILD
  ثابت حرف *envstr;

  /* use another variable for this to allow different values */
  بدل(info) {
  حالة CURLINFO_LOCAL_پورٹ:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, ویلا_T_MAX);
      *param_longp = (لما)val;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CURLINFO_سرخی_ناپ:
  حالة CURLINFO_REQUEST_ناپ:
    envstr = getenv("CURL_DEBUG_SIZE");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, LONG_MAX);
      *param_longp = (لما)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#جے_انت

  بدل(info) {
  حالة CURLINFO_RESPONSE_کوڈ:
    *param_longp = انکڑے->info.httpcode;
    اكسر;
  حالة CURLINFO_HTTP_CONNECTCODE:
    *param_longp = انکڑے->info.httpproxycode;
    اكسر;
  حالة CURLINFO_FILETIME:
    إذا(انکڑے->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#إذا !defined(MSDOS) && !defined(__AMIGA__)
    وإلا إذا(انکڑے->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#جے_انت
    وإلا
      *param_longp = (لما)انکڑے->info.filetime;
    اكسر;
  حالة CURLINFO_سرخی_ناپ:
    *param_longp = (لما)انکڑے->info.سرخی_ناپ;
    اكسر;
  حالة CURLINFO_REQUEST_ناپ:
    *param_longp = (لما)انکڑے->info.request_ناپ;
    اكسر;
  حالة CURLINFO_SSL_VERIFYRESULT:
    *param_longp = انکڑے->سیٹ.ssl.certverifyresult;
    اكسر;
  حالة CURLINFO_PROXY_SSL_VERIFYRESULT:
#جے_بنا_تعریف CURL_DISABLE_PROXY
    *param_longp = انکڑے->سیٹ.proxy_ssl.certverifyresult;
#وإلا
    *param_longp = 0;
#جے_انت
    اكسر;
  حالة CURLINFO_REDIRECT_گنتی:
    *param_longp = انکڑے->state.followlocation;
    اكسر;
  حالة CURLINFO_HTTPAUTH_AVAIL:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = انکڑے->info.httpauthavail;
    اكسر;
  حالة CURLINFO_PROXYAUTH_AVAIL:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = انکڑے->info.proxyauthavail;
    اكسر;
  حالة CURLINFO_HTTPAUTH_ورتیا:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = انکڑے->info.httpauthpicked;
    اكسر;
  حالة CURLINFO_PROXYAUTH_ورتیا:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = انکڑے->info.proxyauthpicked;
    اكسر;
  حالة CURLINFO_OS_ERRNO:
    *param_longp = انکڑے->state.os_errno;
    اكسر;
  حالة CURLINFO_عدد_CONNECTS:
#إذا SIZEOF_LONG < SIZEOF_CURL_OFF_T
    إذا(انکڑے->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    وإلا
#جے_انت
      *param_longp = (لما)انکڑے->info.numconnects;
    اكسر;
  حالة CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(انکڑے, عدم);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    إذا(sockfd != CURL_SOCKET_BAD)
      *param_longp = (لما)sockfd;
    وإلا
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PRIMARY_پورٹ:
    /* Return the (remote) port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&انکڑے->info.primary))
      *param_longp = انکڑے->info.primary.remote_پورٹ;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_LOCAL_پورٹ:
    /* Return the local port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&انکڑے->info.primary))
      *param_longp = انکڑے->info.primary.local_پورٹ;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PROXY_ERROR:
    *param_longp = (لما)انکڑے->info.pxcode;
    اكسر;
  حالة CURLINFO_CONDITION_UNMET:
    إذا(انکڑے->info.httpcode == 304)
      *param_longp = 1L;
    وإلا
      /* return if the condition prevented the document to get transferred */
      *param_longp = انکڑے->info.timecond ? 1L : 0L;
    اكسر;
#جے_بنا_تعریف CURL_DISABLE_RTSP
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = انکڑے->state.rtsp_اگلا_client_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = انکڑے->state.rtsp_اگلا_server_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = انکڑے->state.rtsp_CSeq_recv;
    اكسر;
#وإلا
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
  حالة CURLINFO_RTSP_SERVER_CSEQ:
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    اكسر;
#جے_انت
  حالة CURLINFO_HTTP_نسخہ:
    بدل(انکڑے->info.httpversion) {
    حالة 10:
      *param_longp = CURL_HTTP_نسخہ_1_0;
      اكسر;
    حالة 11:
      *param_longp = CURL_HTTP_نسخہ_1_1;
      اكسر;
    حالة 20:
      *param_longp = CURL_HTTP_نسخہ_2_0;
      اكسر;
    حالة 30:
      *param_longp = CURL_HTTP_نسخہ_3;
      اكسر;
    افتراضي:
      *param_longp = CURL_HTTP_نسخہ_NONE;
      اكسر;
    }
    اكسر;
  حالة CURLINFO_PROTOCOL:
    *param_longp = (لما)انکڑے->info.conn_protocol;
    اكسر;
  حالة CURLINFO_ورتیا_PROXY:
    *param_longp =
#جے_تعریف_شدہ CURL_DISABLE_PROXY
      0
#وإلا
      انکڑے->info.ورتیا_proxy
#جے_انت
      ;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

#تعریف_کرو DOUBLE_SECS(x) ((مزدوج)(x) / 1000000)

ساكن CURLcode getinfo_offt(بنية Curl_easy *انکڑے, CURLINFO info,
                             curl_off_t *param_offt)
{
#جے_تعریف_شدہ DEBUGBUILD
  ثابت حرف *envstr;
  بدل(info) {
  حالة CURLINFO_کل_ویلا_T:
  حالة CURLINFO_NAMELOOKUP_ویلا_T:
  حالة CURLINFO_CONNECT_ویلا_T:
  حالة CURLINFO_APPCONNECT_ویلا_T:
  حالة CURLINFO_PRETRANSFER_ویلا_T:
  حالة CURLINFO_POSTTRANSFER_ویلا_T:
  حالة CURLINFO_قطار_ویلا_T:
  حالة CURLINFO_STARTTRANSFER_ویلا_T:
  حالة CURLINFO_REDIRECT_ویلا_T:
  حالة CURLINFO_رفتار_DOWNLOAD_T:
  حالة CURLINFO_رفتار_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#جے_انت
  بدل(info) {
  حالة CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)انکڑے->info.filetime;
    اكسر;
  حالة CURLINFO_ناپ_DELIVERED:
    *param_offt = انکڑے->ترقی.deliver;
    اكسر;
  حالة CURLINFO_ناپ_UPLOAD_T:
    *param_offt = انکڑے->ترقی.ul.cur_ناپ;
    اكسر;
  حالة CURLINFO_ناپ_DOWNLOAD_T:
    *param_offt = انکڑے->ترقی.dl.cur_ناپ;
    اكسر;
  حالة CURLINFO_رفتار_DOWNLOAD_T:
    *param_offt = انکڑے->ترقی.dl.رفتار;
    اكسر;
  حالة CURLINFO_رفتار_UPLOAD_T:
    *param_offt = انکڑے->ترقی.ul.رفتار;
    اكسر;
  حالة CURLINFO_CONTENT_لمائی_DOWNLOAD_T:
    *param_offt = انکڑے->ترقی.dl_ناپ_معلوم ?
      انکڑے->ترقی.dl.کل_ناپ : -1;
    اكسر;
  حالة CURLINFO_CONTENT_لمائی_UPLOAD_T:
    *param_offt = انکڑے->ترقی.ul_ناپ_معلوم ?
      انکڑے->ترقی.ul.کل_ناپ : -1;
    اكسر;
   حالة CURLINFO_کل_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.spent_us;
    اكسر;
  حالة CURLINFO_NAMELOOKUP_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.nslookup_us;
    اكسر;
  حالة CURLINFO_CONNECT_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.connect_us;
    اكسر;
  حالة CURLINFO_APPCONNECT_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.appconnect_us;
    اكسر;
  حالة CURLINFO_PRETRANSFER_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.pretransfer_us;
    اكسر;
  حالة CURLINFO_POSTTRANSFER_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.posttransfer_us;
    اكسر;
  حالة CURLINFO_STARTTRANSFER_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.starttransfer_us;
    اكسر;
  حالة CURLINFO_قطار_ویلا_T:
    *param_offt = انکڑے->ترقی.کل.queued_us;
    اكسر;
  حالة CURLINFO_REDIRECT_ویلا_T:
    *param_offt = انکڑے->ترقی.delta.startredirect_us;
    اكسر;
  حالة CURLINFO_RETRY_AFTER:
    *param_offt = انکڑے->info.retry_after;
    اكسر;
  حالة CURLINFO_XFER_ID:
    *param_offt = انکڑے->id;
    اكسر;
  حالة CURLINFO_CONN_ID:
    *param_offt = انکڑے->state.آخری_conn_id;
    اكسر;
  حالة CURLINFO_EARLYDATA_گھلیا_T:
    *param_offt = انکڑے->ترقی.earlydata_گھلیا;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_double(بنية Curl_easy *انکڑے, CURLINFO info,
                               مزدوج *param_doublep)
{
#جے_تعریف_شدہ DEBUGBUILD
  ثابت حرف *envstr;

  بدل(info) {
  حالة CURLINFO_کل_ویلا:
  حالة CURLINFO_NAMELOOKUP_ویلا:
  حالة CURLINFO_CONNECT_ویلا:
  حالة CURLINFO_APPCONNECT_ویلا:
  حالة CURLINFO_PRETRANSFER_ویلا:
  حالة CURLINFO_STARTTRANSFER_ویلا:
  حالة CURLINFO_REDIRECT_ویلا:
  حالة CURLINFO_رفتار_DOWNLOAD:
  حالة CURLINFO_رفتار_UPLOAD:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_نمبر(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (مزدوج)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#جے_انت
  بدل(info) {
  حالة CURLINFO_کل_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.کل.spent_us);
    اكسر;
  حالة CURLINFO_NAMELOOKUP_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.کل.nslookup_us);
    اكسر;
  حالة CURLINFO_CONNECT_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.کل.connect_us);
    اكسر;
  حالة CURLINFO_APPCONNECT_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.کل.appconnect_us);
    اكسر;
  حالة CURLINFO_PRETRANSFER_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.کل.pretransfer_us);
    اكسر;
  حالة CURLINFO_STARTTRANSFER_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.کل.starttransfer_us);
    اكسر;
  حالة CURLINFO_ناپ_UPLOAD:
    *param_doublep = (مزدوج)انکڑے->ترقی.ul.cur_ناپ;
    اكسر;
  حالة CURLINFO_ناپ_DOWNLOAD:
    *param_doublep = (مزدوج)انکڑے->ترقی.dl.cur_ناپ;
    اكسر;
  حالة CURLINFO_رفتار_DOWNLOAD:
    *param_doublep = (مزدوج)انکڑے->ترقی.dl.رفتار;
    اكسر;
  حالة CURLINFO_رفتار_UPLOAD:
    *param_doublep = (مزدوج)انکڑے->ترقی.ul.رفتار;
    اكسر;
  حالة CURLINFO_CONTENT_لمائی_DOWNLOAD:
    *param_doublep = انکڑے->ترقی.dl_ناپ_معلوم ?
      (مزدوج)انکڑے->ترقی.dl.کل_ناپ : -1;
    اكسر;
  حالة CURLINFO_CONTENT_لمائی_UPLOAD:
    *param_doublep = انکڑے->ترقی.ul_ناپ_معلوم ?
      (مزدوج)انکڑے->ترقی.ul.کل_ناپ : -1;
    اكسر;
  حالة CURLINFO_REDIRECT_ویلا:
    *param_doublep = DOUBLE_SECS(انکڑے->ترقی.delta.startredirect_us);
    اكسر;

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_slist(بنية Curl_easy *انکڑے, CURLINFO info,
                              بنية curl_slist **param_slistp)
{
  اتحاد {
    بنية curl_certinfo *منزل_certinfo;
    بنية curl_slist    *منزل_slist;
  } ptr;

  بدل(info) {
  حالة CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(انکڑے);
    اكسر;
  حالة CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(انکڑے);
    اكسر;
  حالة CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.منزل_certinfo = &انکڑے->info.certs;
    *param_slistp = ptr.منزل_slist;
    اكسر;
  حالة CURLINFO_TLS_SESSION:
  حالة CURLINFO_TLS_SSL_PTR: {
    صحيح query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    بنية curl_tlssessioninfo **tsip = (بنية curl_tlssessioninfo **)
      param_slistp;
    بنية curl_tlssessioninfo *tsi = &انکڑے->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    إذا(!Curl_conn_get_ssl_info(انکڑے, انکڑے->conn, FIRSTSOCKET, query, tsi)) {
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

ساكن CURLcode getinfo_socket(بنية Curl_easy *انکڑے, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  بدل(info) {
  حالة CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(انکڑے, عدم);
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_getinfo(بنية Curl_easy *انکڑے, CURLINFO info, ...)
{
  va_list arg;
  لما *param_longp = عدم;
  مزدوج *param_doublep = عدم;
  curl_off_t *param_offt = عدم;
  ثابت حرف **param_charp = عدم;
  بنية curl_slist **param_slistp = عدم;
  curl_socket_t *param_socketp = عدم;
  صحيح type;
  CURLcode نتیجہ = CURLE_BAD_FUNCTION_دلیل;

  إذا(!انکڑے)
    أرجع CURLE_BAD_FUNCTION_دلیل;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحيح)info;
  بدل(type) {
  حالة CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت حرف **);
    إذا(param_charp)
      نتیجہ = getinfo_char(انکڑے, info, param_charp);
    اكسر;
  حالة CURLINFO_LONG:
    param_longp = va_arg(arg, لما *);
    إذا(param_longp)
      نتیجہ = getinfo_long(انکڑے, info, param_longp);
    اكسر;
  حالة CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, مزدوج *);
    إذا(param_doublep)
      نتیجہ = getinfo_double(انکڑے, info, param_doublep);
    اكسر;
  حالة CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    إذا(param_offt)
      نتیجہ = getinfo_offt(انکڑے, info, param_offt);
    اكسر;
  حالة CURLINFO_SLIST:
    param_slistp = va_arg(arg, بنية curl_slist **);
    إذا(param_slistp)
      نتیجہ = getinfo_slist(انکڑے, info, param_slistp);
    اكسر;
  حالة CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    إذا(param_socketp)
      نتیجہ = getinfo_socket(انکڑے, info, param_socketp);
    اكسر;
  افتراضي:
    نتیجہ = CURLE_UNKNOWN_OPTION;
    اكسر;
  }

  va_end(arg);

  أرجع نتیجہ;
}
