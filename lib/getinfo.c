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
فراغ Curl_initinfo(بنية Curl_easy *data)
{
  بنية Progress *pro = &data->progress;
  بنية PureInfo *info = &data->info;

  memset(&pro->delta, 0, حجم(pro->delta));
  memset(&pro->total, 0, حجم(pro->total));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->header_size = 0;
  info->request_size = 0;
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
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

ساكن CURLcode effective_url(بنية Curl_easy *data,
                              ثابت حرف **param_charp)
{
  ثابت حرف *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? عدم : "";
  إذا(s) {
    حرف *url = عدم;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    إذا(!u)
      أرجع CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    إذا(!uc)
      uc = curl_url_set(u, CURLUPART_USER, عدم, 0);
    إذا(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, عدم, 0);
    إذا(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    إذا(uc || !url)
      أرجع uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    إذا(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    وإلا {
      curl_free(url);
      *param_charp = s;
    }
  }
  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_char(بنية Curl_easy *data, CURLINFO info,
                             ثابت حرف **param_charp)
{
  بدل(info) {
  حالة CURLINFO_EFFECTIVE_URL:
    أرجع effective_url(data, param_charp);
  حالة CURLINFO_EFFECTIVE_METHOD: {
    ثابت حرف *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    إذا(!m) {
      إذا(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      وإلا {
        بدل(data->state.httpreq) {
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
        حالة HTTPREQ_HEAD:
          m = "HEAD";
          اكسر;
        }
      }
#endif
    }
    *param_charp = m;
  }
    اكسر;
  حالة CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    اكسر;
  حالة CURLINFO_PRIVATE:
    *param_charp = (ثابت حرف *)data->set.private_data;
    اكسر;
  حالة CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    اكسر;
  حالة CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    اكسر;
  حالة CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    اكسر;
  حالة CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    اكسر;
  حالة CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    اكسر;
  حالة CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#وإلا
    *param_charp = عدم;
#endif
    اكسر;
  حالة CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    اكسر;
  حالة CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#وإلا
    *param_charp = عدم;
#endif
    اكسر;
  حالة CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#وإلا
    *param_charp = عدم;
#endif
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_long(بنية Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  اتحاد {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  ثابت حرف *envstr;

  /* use another variable for this to allow different values */
  بدل(info) {
  حالة CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_رقم(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      أرجع CURLE_OK;
    }
    اكسر;
  حالة CURLINFO_HEADER_SIZE:
  حالة CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_رقم(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#endif

  بدل(info) {
  حالة CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    اكسر;
  حالة CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    اكسر;
  حالة CURLINFO_FILETIME:
    إذا(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#إذا !defined(MSDOS) && !defined(__AMIGA__)
    وإلا إذا(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    وإلا
      *param_longp = (long)data->info.filetime;
    اكسر;
  حالة CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    اكسر;
  حالة CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    اكسر;
  حالة CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    اكسر;
  حالة CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#وإلا
    *param_longp = 0;
#endif
    اكسر;
  حالة CURLINFO_REDIRECT_عدد:
    *param_longp = data->state.followlocation;
    اكسر;
  حالة CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    اكسر;
  حالة CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    اكسر;
  حالة CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    اكسر;
  حالة CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    اكسر;
  حالة CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    اكسر;
  حالة CURLINFO_رقم_CONNECTS:
#إذا SIZEOF_LONG < SIZEOF_CURL_OFF_T
    إذا(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    وإلا
#endif
      *param_longp = (long)data->info.numconnects;
    اكسر;
  حالة CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, عدم);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    إذا(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    وإلا
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    إذا(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    وإلا
      *param_longp = -1;
    اكسر;
  حالة CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    اكسر;
  حالة CURLINFO_CONDITION_UNMET:
    إذا(data->info.httpcode == 304)
      *param_longp = 1L;
    وإلا
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    اكسر;
#ifndef CURL_DISABLE_RTSP
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    اكسر;
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    اكسر;
#وإلا
  حالة CURLINFO_RTSP_CLIENT_CSEQ:
  حالة CURLINFO_RTSP_SERVER_CSEQ:
  حالة CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    اكسر;
#endif
  حالة CURLINFO_HTTP_VERSION:
    بدل(data->info.httpversion) {
    حالة 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      اكسر;
    حالة 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      اكسر;
    حالة 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      اكسر;
    حالة 30:
      *param_longp = CURL_HTTP_VERSION_3;
      اكسر;
    افتراضي:
      *param_longp = CURL_HTTP_VERSION_NONE;
      اكسر;
    }
    اكسر;
  حالة CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    اكسر;
  حالة CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#وإلا
      data->info.used_proxy
#endif
      ;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

#define DOUBLE_SECS(x) ((مزدوج)(x) / 1000000)

ساكن CURLcode getinfo_offt(بنية Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  ثابت حرف *envstr;
  بدل(info) {
  حالة CURLINFO_TOTAL_TIME_T:
  حالة CURLINFO_NAMELOOKUP_TIME_T:
  حالة CURLINFO_CONNECT_TIME_T:
  حالة CURLINFO_APPCONNECT_TIME_T:
  حالة CURLINFO_PRETRANSFER_TIME_T:
  حالة CURLINFO_POSTTRANSFER_TIME_T:
  حالة CURLINFO_QUEUE_TIME_T:
  حالة CURLINFO_STARTTRANSFER_TIME_T:
  حالة CURLINFO_REDIRECT_TIME_T:
  حالة CURLINFO_SPEED_DOWNLOAD_T:
  حالة CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_رقم(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#endif
  بدل(info) {
  حالة CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    اكسر;
  حالة CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    اكسر;
  حالة CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    اكسر;
  حالة CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    اكسر;
  حالة CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    اكسر;
  حالة CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    اكسر;
  حالة CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    اكسر;
  حالة CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    اكسر;
   حالة CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    اكسر;
  حالة CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    اكسر;
  حالة CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    اكسر;
  حالة CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    اكسر;
  حالة CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    اكسر;
  حالة CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    اكسر;
  حالة CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    اكسر;
  حالة CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    اكسر;
  حالة CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    اكسر;
  حالة CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    اكسر;
  حالة CURLINFO_XFER_ID:
    *param_offt = data->id;
    اكسر;
  حالة CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    اكسر;
  حالة CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_double(بنية Curl_easy *data, CURLINFO info,
                               مزدوج *param_doublep)
{
#ifdef DEBUGBUILD
  ثابت حرف *envstr;

  بدل(info) {
  حالة CURLINFO_TOTAL_TIME:
  حالة CURLINFO_NAMELOOKUP_TIME:
  حالة CURLINFO_CONNECT_TIME:
  حالة CURLINFO_APPCONNECT_TIME:
  حالة CURLINFO_PRETRANSFER_TIME:
  حالة CURLINFO_STARTTRANSFER_TIME:
  حالة CURLINFO_REDIRECT_TIME:
  حالة CURLINFO_SPEED_DOWNLOAD:
  حالة CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    إذا(envstr) {
      curl_off_t val;
      curlx_str_رقم(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (مزدوج)val;
      أرجع CURLE_OK;
    }
    اكسر;
  افتراضي:
    اكسر;
  }
#endif
  بدل(info) {
  حالة CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    اكسر;
  حالة CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    اكسر;
  حالة CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    اكسر;
  حالة CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    اكسر;
  حالة CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    اكسر;
  حالة CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    اكسر;
  حالة CURLINFO_SIZE_UPLOAD:
    *param_doublep = (مزدوج)data->progress.ul.cur_size;
    اكسر;
  حالة CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (مزدوج)data->progress.dl.cur_size;
    اكسر;
  حالة CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (مزدوج)data->progress.dl.speed;
    اكسر;
  حالة CURLINFO_SPEED_UPLOAD:
    *param_doublep = (مزدوج)data->progress.ul.speed;
    اكسر;
  حالة CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (مزدوج)data->progress.dl.total_size : -1;
    اكسر;
  حالة CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (مزدوج)data->progress.ul.total_size : -1;
    اكسر;
  حالة CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    اكسر;

  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

ساكن CURLcode getinfo_slist(بنية Curl_easy *data, CURLINFO info,
                              بنية curl_slist **param_slistp)
{
  اتحاد {
    بنية curl_certinfo *to_certinfo;
    بنية curl_slist    *to_slist;
  } ptr;

  بدل(info) {
  حالة CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    اكسر;
  حالة CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    اكسر;
  حالة CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    اكسر;
  حالة CURLINFO_TLS_SESSION:
  حالة CURLINFO_TLS_SSL_PTR: {
    صحيح query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    بنية curl_tlssessioninfo **tsip = (بنية curl_tlssessioninfo **)
      param_slistp;
    بنية curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    إذا(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
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

ساكن CURLcode getinfo_socket(بنية Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  بدل(info) {
  حالة CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, عدم);
    اكسر;
  افتراضي:
    أرجع CURLE_UNKNOWN_OPTION;
  }

  أرجع CURLE_OK;
}

CURLcode Curl_getinfo(بنية Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = عدم;
  مزدوج *param_doublep = عدم;
  curl_off_t *param_offt = عدم;
  ثابت حرف **param_charp = عدم;
  بنية curl_slist **param_slistp = عدم;
  curl_socket_t *param_socketp = عدم;
  صحيح type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  إذا(!data)
    أرجع CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحيح)info;
  بدل(type) {
  حالة CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت حرف **);
    إذا(param_charp)
      result = getinfo_char(data, info, param_charp);
    اكسر;
  حالة CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    إذا(param_longp)
      result = getinfo_long(data, info, param_longp);
    اكسر;
  حالة CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, مزدوج *);
    إذا(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    اكسر;
  حالة CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    إذا(param_offt)
      result = getinfo_offt(data, info, param_offt);
    اكسر;
  حالة CURLINFO_SLIST:
    param_slistp = va_arg(arg, بنية curl_slist **);
    إذا(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    اكسر;
  حالة CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    إذا(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    اكسر;
  افتراضي:
    result = CURLE_UNKNOWN_OPTION;
    اكسر;
  }

  va_end(arg);

  أرجع result;
}
