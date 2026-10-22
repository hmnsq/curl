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
تهی Curl_initinfo(ساختار Curl_easy *data)
{
  ساختار Progress *pro = &data->progress;
  ساختار PureInfo *info = &data->info;

  memset(&pro->delta, 0, اندازه(pro->delta));
  memset(&pro->total, 0, اندازه(pro->total));
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

  memset(&info->primary, 0, اندازه(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

ایستا CURLcode effective_url(ساختار Curl_easy *data,
                              ثابت نویسه **param_charp)
{
  ثابت نویسه *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? پوچ : "";
  اگر(s) {
    نویسه *url = پوچ;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    اگر(!u)
      بازگشت CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    اگر(!uc)
      uc = curl_url_set(u, CURLUPART_USER, پوچ, 0);
    اگر(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, پوچ, 0);
    اگر(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    اگر(uc || !url)
      بازگشت uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    اگر(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    وگرنه {
      curl_free(url);
      *param_charp = s;
    }
  }
  بازگشت CURLE_OK;
}

ایستا CURLcode getinfo_char(ساختار Curl_easy *data, CURLINFO info,
                             ثابت نویسه **param_charp)
{
  انتخاب(info) {
  حالت CURLINFO_EFFECTIVE_URL:
    بازگشت effective_url(data, param_charp);
  حالت CURLINFO_EFFECTIVE_METHOD: {
    ثابت نویسه *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    اگر(!m) {
      اگر(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      وگرنه {
        انتخاب(data->state.httpreq) {
        حالت HTTPREQ_POST:
        حالت HTTPREQ_POST_FORM:
        حالت HTTPREQ_POST_MIME:
          m = "POST";
          بشکن;
        حالت HTTPREQ_PUT:
          m = "PUT";
          بشکن;
        پیشفرض: /* this should never happen */
        حالت HTTPREQ_GET:
          m = "GET";
          بشکن;
        حالت HTTPREQ_HEAD:
          m = "HEAD";
          بشکن;
        }
      }
#endif
    }
    *param_charp = m;
  }
    بشکن;
  حالت CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    بشکن;
  حالت CURLINFO_PRIVATE:
    *param_charp = (ثابت نویسه *)data->set.private_data;
    بشکن;
  حالت CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    بشکن;
  حالت CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    بشکن;
  حالت CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    بشکن;
  حالت CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    بشکن;
  حالت CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    بشکن;
  حالت CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#وگرنه
    *param_charp = پوچ;
#endif
    بشکن;
  حالت CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    بشکن;
  حالت CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#وگرنه
    *param_charp = پوچ;
#endif
    بشکن;
  حالت CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#وگرنه
    *param_charp = پوچ;
#endif
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode getinfo_long(ساختار Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  اجتماع {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  ثابت نویسه *envstr;

  /* use another variable for this to allow different values */
  انتخاب(info) {
  حالت CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      بازگشت CURLE_OK;
    }
    بشکن;
  حالت CURLINFO_HEADER_SIZE:
  حالت CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      بازگشت CURLE_OK;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
#endif

  انتخاب(info) {
  حالت CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    بشکن;
  حالت CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    بشکن;
  حالت CURLINFO_FILETIME:
    اگر(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#اگر !defined(MSDOS) && !defined(__AMIGA__)
    وگرنه اگر(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    وگرنه
      *param_longp = (long)data->info.filetime;
    بشکن;
  حالت CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    بشکن;
  حالت CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    بشکن;
  حالت CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    بشکن;
  حالت CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#وگرنه
    *param_longp = 0;
#endif
    بشکن;
  حالت CURLINFO_REDIRECT_شمار:
    *param_longp = data->state.followlocation;
    بشکن;
  حالت CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    بشکن;
  حالت CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    بشکن;
  حالت CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    بشکن;
  حالت CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    بشکن;
  حالت CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    بشکن;
  حالت CURLINFO_رقم_CONNECTS:
#اگر SIZEOF_LONG < SIZEOF_CURL_OFF_T
    اگر(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    وگرنه
#endif
      *param_longp = (long)data->info.numconnects;
    بشکن;
  حالت CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, پوچ);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    اگر(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    وگرنه
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    بشکن;
  حالت CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    اگر(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    وگرنه
      *param_longp = -1;
    بشکن;
  حالت CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    اگر(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    وگرنه
      *param_longp = -1;
    بشکن;
  حالت CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    بشکن;
  حالت CURLINFO_CONDITION_UNMET:
    اگر(data->info.httpcode == 304)
      *param_longp = 1L;
    وگرنه
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    بشکن;
#ifndef CURL_DISABLE_RTSP
  حالت CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    بشکن;
  حالت CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    بشکن;
  حالت CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    بشکن;
#وگرنه
  حالت CURLINFO_RTSP_CLIENT_CSEQ:
  حالت CURLINFO_RTSP_SERVER_CSEQ:
  حالت CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    بشکن;
#endif
  حالت CURLINFO_HTTP_VERSION:
    انتخاب(data->info.httpversion) {
    حالت 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      بشکن;
    حالت 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      بشکن;
    حالت 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      بشکن;
    حالت 30:
      *param_longp = CURL_HTTP_VERSION_3;
      بشکن;
    پیشفرض:
      *param_longp = CURL_HTTP_VERSION_NONE;
      بشکن;
    }
    بشکن;
  حالت CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    بشکن;
  حالت CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#وگرنه
      data->info.used_proxy
#endif
      ;
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }

  بازگشت CURLE_OK;
}

#define DOUBLE_SECS(x) ((دوگانه)(x) / 1000000)

ایستا CURLcode getinfo_offt(ساختار Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  ثابت نویسه *envstr;
  انتخاب(info) {
  حالت CURLINFO_TOTAL_TIME_T:
  حالت CURLINFO_NAMELOOKUP_TIME_T:
  حالت CURLINFO_CONNECT_TIME_T:
  حالت CURLINFO_APPCONNECT_TIME_T:
  حالت CURLINFO_PRETRANSFER_TIME_T:
  حالت CURLINFO_POSTTRANSFER_TIME_T:
  حالت CURLINFO_QUEUE_TIME_T:
  حالت CURLINFO_STARTTRANSFER_TIME_T:
  حالت CURLINFO_REDIRECT_TIME_T:
  حالت CURLINFO_SPEED_DOWNLOAD_T:
  حالت CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      بازگشت CURLE_OK;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
#endif
  انتخاب(info) {
  حالت CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    بشکن;
  حالت CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    بشکن;
  حالت CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    بشکن;
  حالت CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    بشکن;
  حالت CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    بشکن;
  حالت CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    بشکن;
  حالت CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    بشکن;
  حالت CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    بشکن;
   حالت CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    بشکن;
  حالت CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    بشکن;
  حالت CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    بشکن;
  حالت CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    بشکن;
  حالت CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    بشکن;
  حالت CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    بشکن;
  حالت CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    بشکن;
  حالت CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    بشکن;
  حالت CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    بشکن;
  حالت CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    بشکن;
  حالت CURLINFO_XFER_ID:
    *param_offt = data->id;
    بشکن;
  حالت CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    بشکن;
  حالت CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode getinfo_double(ساختار Curl_easy *data, CURLINFO info,
                               دوگانه *param_doublep)
{
#ifdef DEBUGBUILD
  ثابت نویسه *envstr;

  انتخاب(info) {
  حالت CURLINFO_TOTAL_TIME:
  حالت CURLINFO_NAMELOOKUP_TIME:
  حالت CURLINFO_CONNECT_TIME:
  حالت CURLINFO_APPCONNECT_TIME:
  حالت CURLINFO_PRETRANSFER_TIME:
  حالت CURLINFO_STARTTRANSFER_TIME:
  حالت CURLINFO_REDIRECT_TIME:
  حالت CURLINFO_SPEED_DOWNLOAD:
  حالت CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    اگر(envstr) {
      curl_off_t val;
      curlx_str_عدد(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (دوگانه)val;
      بازگشت CURLE_OK;
    }
    بشکن;
  پیشفرض:
    بشکن;
  }
#endif
  انتخاب(info) {
  حالت CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    بشکن;
  حالت CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    بشکن;
  حالت CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    بشکن;
  حالت CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    بشکن;
  حالت CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    بشکن;
  حالت CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    بشکن;
  حالت CURLINFO_SIZE_UPLOAD:
    *param_doublep = (دوگانه)data->progress.ul.cur_size;
    بشکن;
  حالت CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (دوگانه)data->progress.dl.cur_size;
    بشکن;
  حالت CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (دوگانه)data->progress.dl.speed;
    بشکن;
  حالت CURLINFO_SPEED_UPLOAD:
    *param_doublep = (دوگانه)data->progress.ul.speed;
    بشکن;
  حالت CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (دوگانه)data->progress.dl.total_size : -1;
    بشکن;
  حالت CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (دوگانه)data->progress.ul.total_size : -1;
    بشکن;
  حالت CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    بشکن;

  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode getinfo_slist(ساختار Curl_easy *data, CURLINFO info,
                              ساختار curl_slist **param_slistp)
{
  اجتماع {
    ساختار curl_certinfo *to_certinfo;
    ساختار curl_slist    *to_slist;
  } ptr;

  انتخاب(info) {
  حالت CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    بشکن;
  حالت CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    بشکن;
  حالت CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    بشکن;
  حالت CURLINFO_TLS_SESSION:
  حالت CURLINFO_TLS_SSL_PTR: {
    صحیح query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ساختار curl_tlssessioninfo **tsip = (ساختار curl_tlssessioninfo **)
      param_slistp;
    ساختار curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    اگر(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = پوچ;
    }
  }
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }

  بازگشت CURLE_OK;
}

ایستا CURLcode getinfo_socket(ساختار Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  انتخاب(info) {
  حالت CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, پوچ);
    بشکن;
  پیشفرض:
    بازگشت CURLE_UNKNOWN_OPTION;
  }

  بازگشت CURLE_OK;
}

CURLcode Curl_getinfo(ساختار Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = پوچ;
  دوگانه *param_doublep = پوچ;
  curl_off_t *param_offt = پوچ;
  ثابت نویسه **param_charp = پوچ;
  ساختار curl_slist **param_slistp = پوچ;
  curl_socket_t *param_socketp = پوچ;
  صحیح type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  اگر(!data)
    بازگشت CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (صحیح)info;
  انتخاب(type) {
  حالت CURLINFO_STRING:
    param_charp = va_arg(arg, ثابت نویسه **);
    اگر(param_charp)
      result = getinfo_char(data, info, param_charp);
    بشکن;
  حالت CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    اگر(param_longp)
      result = getinfo_long(data, info, param_longp);
    بشکن;
  حالت CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, دوگانه *);
    اگر(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    بشکن;
  حالت CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    اگر(param_offt)
      result = getinfo_offt(data, info, param_offt);
    بشکن;
  حالت CURLINFO_SLIST:
    param_slistp = va_arg(arg, ساختار curl_slist **);
    اگر(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    بشکن;
  حالت CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    اگر(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    بشکن;
  پیشفرض:
    result = CURLE_UNKNOWN_OPTION;
    بشکن;
  }

  va_end(arg);

  بازگشت result;
}
