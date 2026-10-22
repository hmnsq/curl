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
  بنية ترقي_2 *pro = &معلومات->ترقي;
  بنية خالص_Info *info = &معلومات->info;

  memset(&pro->delta, 0, حجم(pro->delta));
  memset(&pro->ڪل, 0, حجم(pro->ڪل));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->سرو_سائيز = 0;
  info->request_سائيز = 0;
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
  info->استعمال_ٿيل_proxy = 0;

#جيڪڏهن_وصف_ٿيل USE_SSL
  Curl_ssl_free_certinfo(معلومات);
#جيڪڏهن_پورو
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
      أرجع CURLE_ٻاهريون_جو_يادگيري;

    uc = curl_url_سيٽ(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (معلومات->سيٽ.رستو_as_is ? CURLU_رستو_AS_IS : 0));
    إذا(!uc)
      uc = curl_url_سيٽ(u, CURLUPART_USER, عدم, 0);
    إذا(!uc)
      uc = curl_url_سيٽ(u, CURLUPART_PASSWORD, عدم, 0);
    إذا(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_خالي_جايون);

    curl_url_cleanup(u);

    إذا(uc || !url)
      أرجع uc ? Curl_uc_منزل_curlcode(uc) : CURLE_ٻاهريون_جو_يادگيري;
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
  حالة CURLINFO_EFFECTIVE_طريقو: {
    ثابت حرف *m = CURL_EASY_STR(معلومات, STRING_CUSTOMREQUEST);
    إذا(!m) {
      إذا(معلومات->سيٽ.opt_no_body)
        m = "HEAD";
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_HTTP
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
#جيڪڏهن_پورو
    }
    *param_charp = m;
  }
    اكسر;
  حالة CURLINFO_CONTENT_TYPE:
    *param_charp = معلومات->info.contenttype;
    اكسر;
  حالة CURLINFO_ذاتي:
    *param_charp = (ثابت حرف *)معلومات->سيٽ.ذاتي_معلومات;
    اكسر;
  حالة CURLINFO_FTP_داخلا_رستو:
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
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(معلومات, STRING_RTSP_SESSION_ID);
#وإلا
    *param_charp = عدم;
#جيڪڏهن_پورو
    اكسر;
  حالة CURLINFO_SCHEME:
    *param_charp = معلومات->info.conn_scheme;
    اكسر;
  حالة CURLINFO_CAPATH:
#جيڪڏهن_وصف_ٿيل CURL_CA_رستو
    *param_charp = CURL_CA_رستو;
#وإلا
    *param_charp = عدم;
#جيڪڏهن_پورو
    اكسر;
  حالة CURLINFO_CAINFO:
#جيڪڏهن_وصف_ٿيل CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#وإلا
    *param_charp = عدم;
#جيڪڏهن_پورو
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_long(بنية Curl_easy *معلومات, CURLINFO info,
                             ڊگهو *param_longp)
{
  curl_socket_t sockfd;

  اتحاد {
    بنانشان ڊگهو *منزل_ulong;
    ڊگهو          *منزل_long;
  } lptr;

#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  ثابت حرف *envstr;

  /* use another variable for this to allow different values */
  بدل(info) {
  حالة CURLINFO_LOCAL_پورٽ:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, وقت_T_MAX);
      *param_longp = (ڊگهو)val;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CURLINFO_سرو_سائيز:
  حالة CURLINFO_REQUEST_سائيز:
    envstr = getenv("CURL_DEBUG_SIZE");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, LONG_MAX);
      *param_longp = (ڊگهو)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#جيڪڏهن_پورو

  بدل(info) {
  حالة CURLINFO_RESPONSE_ڪوڊ:
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
#جيڪڏهن_پورو
    وإلا
      *param_longp = (ڊگهو)معلومات->info.filetime;
    اكسر;
  حالة CURLINFO_سرو_سائيز:
    *param_longp = (ڊگهو)معلومات->info.سرو_سائيز;
    اكسر;
  حالة CURLINFO_REQUEST_سائيز:
    *param_longp = (ڊگهو)معلومات->info.request_سائيز;
    اكسر;
  حالة CURLINFO_SSL_VERIFYRESULT:
    *param_longp = معلومات->سيٽ.ssl.certverifyresult;
    اكسر;
  حالة CURLINFO_PROXY_SSL_VERIFYRESULT:
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_PROXY
    *param_longp = معلومات->سيٽ.proxy_ssl.certverifyresult;
#وإلا
    *param_longp = 0;
#جيڪڏهن_پورو
    اكسر;
  حالة CURLINFO_REDIRECT_ڳڻپ:
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
  حالة CURLINFO_HTTPAUTH_استعمال_ٿيل:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = معلومات->info.httpauthpicked;
    اكسر;
  حالة CURLINFO_PROXYAUTH_استعمال_ٿيل:
    lptr.منزل_long = param_longp;
    *lptr.منزل_ulong = معلومات->info.proxyauthpicked;
    اكسر;
  حالة CURLINFO_OS_ERRNO:
    *param_longp = معلومات->state.os_errno;
    اكسر;
  حالة CURLINFO_انگ_CONNECTS:
#إذا SIZEOF_LONG < SIZEOF_CURL_OFF_T
    إذا(معلومات->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    وإلا
#جيڪڏهن_پورو
      *param_longp = (ڊگهو)معلومات->info.numconnects;
    اكسر;
  حالة CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(معلومات, عدم);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    إذا(sockfd != CURL_SOCKET_BAD)
      *param_longp = (ڊگهو)sockfd;
    وإلا
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PRIMARY_پورٽ:
    /* Return the (remote) port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&معلومات->info.primary))
      *param_longp = معلومات->info.primary.remote_پورٽ;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_LOCAL_پورٽ:
    /* Return the local port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&معلومات->info.primary))
      *param_longp = معلومات->info.primary.local_پورٽ;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PROXY_ERROR:
    *param_longp = (ڊگهو)معلومات->info.pxcode;
    اكسر;
  حالة CURLINFO_CONDITION_UNMET:
    إذا(معلومات->info.httpcode == 304)
      *param_longp = 1L;
    وإلا
      /* return if the condition prevented the document to get transferred */
      *param_longp = معلومات->info.timecond ? 1L : 0L;
    اكسر;
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_RTSP
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = معلومات->state.rtsp_ايندڙ_client_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = معلومات->state.rtsp_ايندڙ_server_CSeq;
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
#جيڪڏهن_پورو
  حالة CURLINFO_HTTP_نسخو:
    بدل(معلومات->info.httpversion) {
    حالة 10:
      *param_longp = CURL_HTTP_نسخو_1_0;
      اكسر;
    حالة 11:
      *param_longp = CURL_HTTP_نسخو_1_1;
      اكسر;
    حالة 20:
      *param_longp = CURL_HTTP_نسخو_2_0;
      اكسر;
    حالة 30:
      *param_longp = CURL_HTTP_نسخو_3;
      اكسر;
    افتراضي:
      *param_longp = CURL_HTTP_نسخو_NONE;
      اكسر;
    }
    اكسر;
  حالة CURLINFO_PROTOCOL:
    *param_longp = (ڊگهو)معلومات->info.conn_protocol;
    اكسر;
  حالة CURLINFO_استعمال_ٿيل_PROXY:
    *param_longp =
#جيڪڏهن_وصف_ٿيل CURL_DISABLE_PROXY
      0
#وإلا
      معلومات->info.استعمال_ٿيل_proxy
#جيڪڏهن_پورو
      ;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

#وصف_ڪريو DOUBLE_SECS(x) ((مزدوج)(x) / 1000000)

ساكن CURLcode getinfo_offt(بنية Curl_easy *معلومات, CURLINFO info,
                             curl_off_t *param_offt)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  ثابت حرف *envstr;
  بدل(info) {
  حالة CURLINFO_ڪل_وقت_T:
  حالة CURLINFO_NAMELOOKUP_وقت_T:
  حالة CURLINFO_CONNECT_وقت_T:
  حالة CURLINFO_APPCONNECT_وقت_T:
  حالة CURLINFO_PRETRANSFER_وقت_T:
  حالة CURLINFO_POSTTRANSFER_وقت_T:
  حالة CURLINFO_قطار_وقت_T:
  حالة CURLINFO_STARTTRANSFER_وقت_T:
  حالة CURLINFO_REDIRECT_وقت_T:
  حالة CURLINFO_رفتار_DOWNLOAD_T:
  حالة CURLINFO_رفتار_UPLOAD_T:
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
#جيڪڏهن_پورو
  بدل(info) {
  حالة CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)معلومات->info.filetime;
    اكسر;
  حالة CURLINFO_سائيز_DELIVERED:
    *param_offt = معلومات->ترقي.deliver;
    اكسر;
  حالة CURLINFO_سائيز_UPLOAD_T:
    *param_offt = معلومات->ترقي.ul.cur_سائيز;
    اكسر;
  حالة CURLINFO_سائيز_DOWNLOAD_T:
    *param_offt = معلومات->ترقي.dl.cur_سائيز;
    اكسر;
  حالة CURLINFO_رفتار_DOWNLOAD_T:
    *param_offt = معلومات->ترقي.dl.رفتار;
    اكسر;
  حالة CURLINFO_رفتار_UPLOAD_T:
    *param_offt = معلومات->ترقي.ul.رفتار;
    اكسر;
  حالة CURLINFO_CONTENT_ڊيگهائي_DOWNLOAD_T:
    *param_offt = معلومات->ترقي.dl_سائيز_معلوم ?
      معلومات->ترقي.dl.ڪل_سائيز : -1;
    اكسر;
  حالة CURLINFO_CONTENT_ڊيگهائي_UPLOAD_T:
    *param_offt = معلومات->ترقي.ul_سائيز_معلوم ?
      معلومات->ترقي.ul.ڪل_سائيز : -1;
    اكسر;
   حالة CURLINFO_ڪل_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.spent_us;
    اكسر;
  حالة CURLINFO_NAMELOOKUP_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.nslookup_us;
    اكسر;
  حالة CURLINFO_CONNECT_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.connect_us;
    اكسر;
  حالة CURLINFO_APPCONNECT_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.appconnect_us;
    اكسر;
  حالة CURLINFO_PRETRANSFER_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.pretransfer_us;
    اكسر;
  حالة CURLINFO_POSTTRANSFER_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.posttransfer_us;
    اكسر;
  حالة CURLINFO_STARTTRANSFER_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.starttransfer_us;
    اكسر;
  حالة CURLINFO_قطار_وقت_T:
    *param_offt = معلومات->ترقي.ڪل.queued_us;
    اكسر;
  حالة CURLINFO_REDIRECT_وقت_T:
    *param_offt = معلومات->ترقي.delta.startredirect_us;
    اكسر;
  حالة CURLINFO_RETRY_AFTER:
    *param_offt = معلومات->info.retry_after;
    اكسر;
  حالة CURLINFO_XFER_ID:
    *param_offt = معلومات->id;
    اكسر;
  حالة CURLINFO_CONN_ID:
    *param_offt = معلومات->state.آخري_conn_id;
    اكسر;
  حالة CURLINFO_EARLYDATA_موڪليل_T:
    *param_offt = معلومات->ترقي.earlydata_موڪليل;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_double(بنية Curl_easy *معلومات, CURLINFO info,
                               مزدوج *param_doublep)
{
#جيڪڏهن_وصف_ٿيل DEBUGBUILD
  ثابت حرف *envstr;

  بدل(info) {
  حالة CURLINFO_ڪل_وقت:
  حالة CURLINFO_NAMELOOKUP_وقت:
  حالة CURLINFO_CONNECT_وقت:
  حالة CURLINFO_APPCONNECT_وقت:
  حالة CURLINFO_PRETRANSFER_وقت:
  حالة CURLINFO_STARTTRANSFER_وقت:
  حالة CURLINFO_REDIRECT_وقت:
  حالة CURLINFO_رفتار_DOWNLOAD:
  حالة CURLINFO_رفتار_UPLOAD:
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
#جيڪڏهن_پورو
  بدل(info) {
  حالة CURLINFO_ڪل_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.ڪل.spent_us);
    اكسر;
  حالة CURLINFO_NAMELOOKUP_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.ڪل.nslookup_us);
    اكسر;
  حالة CURLINFO_CONNECT_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.ڪل.connect_us);
    اكسر;
  حالة CURLINFO_APPCONNECT_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.ڪل.appconnect_us);
    اكسر;
  حالة CURLINFO_PRETRANSFER_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.ڪل.pretransfer_us);
    اكسر;
  حالة CURLINFO_STARTTRANSFER_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.ڪل.starttransfer_us);
    اكسر;
  حالة CURLINFO_سائيز_UPLOAD:
    *param_doublep = (مزدوج)معلومات->ترقي.ul.cur_سائيز;
    اكسر;
  حالة CURLINFO_سائيز_DOWNLOAD:
    *param_doublep = (مزدوج)معلومات->ترقي.dl.cur_سائيز;
    اكسر;
  حالة CURLINFO_رفتار_DOWNLOAD:
    *param_doublep = (مزدوج)معلومات->ترقي.dl.رفتار;
    اكسر;
  حالة CURLINFO_رفتار_UPLOAD:
    *param_doublep = (مزدوج)معلومات->ترقي.ul.رفتار;
    اكسر;
  حالة CURLINFO_CONTENT_ڊيگهائي_DOWNLOAD:
    *param_doublep = معلومات->ترقي.dl_سائيز_معلوم ?
      (مزدوج)معلومات->ترقي.dl.ڪل_سائيز : -1;
    اكسر;
  حالة CURLINFO_CONTENT_ڊيگهائي_UPLOAD:
    *param_doublep = معلومات->ترقي.ul_سائيز_معلوم ?
      (مزدوج)معلومات->ترقي.ul.ڪل_سائيز : -1;
    اكسر;
  حالة CURLINFO_REDIRECT_وقت:
    *param_doublep = DOUBLE_SECS(معلومات->ترقي.delta.startredirect_us);
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
  ڊگهو *param_longp = عدم;
  مزدوج *param_doublep = عدم;
  curl_off_t *param_offt = عدم;
  ثابت حرف **param_charp = عدم;
  بنية curl_slist **param_slistp = عدم;
  curl_socket_t *param_socketp = عدم;
  صحيح type;
  CURLcode نتيجو = CURLE_BAD_FUNCTION_دليل;

  إذا(!معلومات)
    أرجع CURLE_BAD_FUNCTION_دليل;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحيح)info;
  بدل(type) {
  حالة CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت حرف **);
    إذا(param_charp)
      نتيجو = getinfo_char(معلومات, info, param_charp);
    اكسر;
  حالة CURLINFO_LONG:
    param_longp = va_arg(arg, ڊگهو *);
    إذا(param_longp)
      نتيجو = getinfo_long(معلومات, info, param_longp);
    اكسر;
  حالة CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, مزدوج *);
    إذا(param_doublep)
      نتيجو = getinfo_double(معلومات, info, param_doublep);
    اكسر;
  حالة CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    إذا(param_offt)
      نتيجو = getinfo_offt(معلومات, info, param_offt);
    اكسر;
  حالة CURLINFO_SLIST:
    param_slistp = va_arg(arg, بنية curl_slist **);
    إذا(param_slistp)
      نتيجو = getinfo_slist(معلومات, info, param_slistp);
    اكسر;
  حالة CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    إذا(param_socketp)
      نتيجو = getinfo_socket(معلومات, info, param_socketp);
    اكسر;
  افتراضي:
    نتيجو = CURLE_UNKNOWN_OPTION;
    اكسر;
  }

  va_end(arg);

  أرجع نتيجو;
}
