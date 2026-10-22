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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "urldata.h"
#அந்தர்பாவ "getinfo.h"
#அந்தர்பாவ "cfilters.h"
#அந்தர்பாவ "vtls/vtls.h"
#அந்தர்பாவ "connect.h" /* Curl_getconnectinfo() */
#அந்தர்பாவ "bufref.h"
#அந்தர்பாவ "url.h" /* Curl_uc_to_curlcode() */
#அந்தர்பாவ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
வெறுமை Curl_initinfo(கட்டமைப்பு Curl_easy *data)
{
  கட்டமைப்பு Progress *pro = &data->progress;
  கட்டமைப்பு PureInfo *info = &data->info;

  memset(&pro->delta, 0, அளவு(pro->delta));
  memset(&pro->total, 0, அளவு(pro->total));
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

  memset(&info->primary, 0, அளவு(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

நிலையான CURLcode effective_url(கட்டமைப்பு Curl_easy *data,
                              மாறிலி எழுத்து **param_charp)
{
  மாறிலி எழுத்து *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? வெற்று : "";
  எனில்(s) {
    எழுத்து *url = வெற்று;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    எனில்(!u)
      திரும்பு CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    எனில்(!uc)
      uc = curl_url_set(u, CURLUPART_USER, வெற்று, 0);
    எனில்(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, வெற்று, 0);
    எனில்(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    எனில்(uc || !url)
      திரும்பு uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    எனில்(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    இல்லையெனில் {
      curl_free(url);
      *param_charp = s;
    }
  }
  திரும்பு CURLE_OK;
}

நிலையான CURLcode getinfo_char(கட்டமைப்பு Curl_easy *data, CURLINFO info,
                             மாறிலி எழுத்து **param_charp)
{
  தேர்வு(info) {
  நிலை CURLINFO_EFFECTIVE_URL:
    திரும்பு effective_url(data, param_charp);
  நிலை CURLINFO_EFFECTIVE_METHOD: {
    மாறிலி எழுத்து *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    எனில்(!m) {
      எனில்(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      இல்லையெனில் {
        தேர்வு(data->state.httpreq) {
        நிலை HTTPREQ_POST:
        நிலை HTTPREQ_POST_FORM:
        நிலை HTTPREQ_POST_MIME:
          m = "POST";
          நிறுத்து;
        நிலை HTTPREQ_PUT:
          m = "PUT";
          நிறுத்து;
        இயல்பு: /* this should never happen */
        நிலை HTTPREQ_GET:
          m = "GET";
          நிறுத்து;
        நிலை HTTPREQ_HEAD:
          m = "HEAD";
          நிறுத்து;
        }
      }
#endif
    }
    *param_charp = m;
  }
    நிறுத்து;
  நிலை CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    நிறுத்து;
  நிலை CURLINFO_PRIVATE:
    *param_charp = (மாறிலி எழுத்து *)data->set.private_data;
    நிறுத்து;
  நிலை CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    நிறுத்து;
  நிலை CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    நிறுத்து;
  நிலை CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    நிறுத்து;
  நிலை CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    நிறுத்து;
  நிலை CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    நிறுத்து;
  நிலை CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#இல்லையெனில்
    *param_charp = வெற்று;
#endif
    நிறுத்து;
  நிலை CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    நிறுத்து;
  நிலை CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#இல்லையெனில்
    *param_charp = வெற்று;
#endif
    நிறுத்து;
  நிலை CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#இல்லையெனில்
    *param_charp = வெற்று;
#endif
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode getinfo_long(கட்டமைப்பு Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  ஒன்றிப்பு {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  மாறிலி எழுத்து *envstr;

  /* use another variable for this to allow different values */
  தேர்வு(info) {
  நிலை CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    எனில்(envstr) {
      curl_off_t val;
      curlx_str_எண்(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  நிலை CURLINFO_HEADER_SIZE:
  நிலை CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    எனில்(envstr) {
      curl_off_t val;
      curlx_str_எண்(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
#endif

  தேர்வு(info) {
  நிலை CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    நிறுத்து;
  நிலை CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    நிறுத்து;
  நிலை CURLINFO_FILETIME:
    எனில்(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#எனில் !defined(MSDOS) && !defined(__AMIGA__)
    இல்லையெனில் எனில்(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    இல்லையெனில்
      *param_longp = (long)data->info.filetime;
    நிறுத்து;
  நிலை CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    நிறுத்து;
  நிலை CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    நிறுத்து;
  நிலை CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    நிறுத்து;
  நிலை CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#இல்லையெனில்
    *param_longp = 0;
#endif
    நிறுத்து;
  நிலை CURLINFO_REDIRECT_மொத்தம்:
    *param_longp = data->state.followlocation;
    நிறுத்து;
  நிலை CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    நிறுத்து;
  நிலை CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    நிறுத்து;
  நிலை CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    நிறுத்து;
  நிலை CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    நிறுத்து;
  நிலை CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    நிறுத்து;
  நிலை CURLINFO_இலக்கம்_CONNECTS:
#எனில் SIZEOF_LONG < SIZEOF_CURL_OFF_T
    எனில்(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    இல்லையெனில்
#endif
      *param_longp = (long)data->info.numconnects;
    நிறுத்து;
  நிலை CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, வெற்று);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    எனில்(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    இல்லையெனில்
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    நிறுத்து;
  நிலை CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    எனில்(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    இல்லையெனில்
      *param_longp = -1;
    நிறுத்து;
  நிலை CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    எனில்(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    இல்லையெனில்
      *param_longp = -1;
    நிறுத்து;
  நிலை CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    நிறுத்து;
  நிலை CURLINFO_CONDITION_UNMET:
    எனில்(data->info.httpcode == 304)
      *param_longp = 1L;
    இல்லையெனில்
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    நிறுத்து;
#ifndef CURL_DISABLE_RTSP
  நிலை CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    நிறுத்து;
  நிலை CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    நிறுத்து;
  நிலை CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    நிறுத்து;
#இல்லையெனில்
  நிலை CURLINFO_RTSP_CLIENT_CSEQ:
  நிலை CURLINFO_RTSP_SERVER_CSEQ:
  நிலை CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    நிறுத்து;
#endif
  நிலை CURLINFO_HTTP_VERSION:
    தேர்வு(data->info.httpversion) {
    நிலை 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      நிறுத்து;
    நிலை 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      நிறுத்து;
    நிலை 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      நிறுத்து;
    நிலை 30:
      *param_longp = CURL_HTTP_VERSION_3;
      நிறுத்து;
    இயல்பு:
      *param_longp = CURL_HTTP_VERSION_NONE;
      நிறுத்து;
    }
    நிறுத்து;
  நிலை CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    நிறுத்து;
  நிலை CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#இல்லையெனில்
      data->info.used_proxy
#endif
      ;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }

  திரும்பு CURLE_OK;
}

#define DOUBLE_SECS(x) ((இரட்டை)(x) / 1000000)

நிலையான CURLcode getinfo_offt(கட்டமைப்பு Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  மாறிலி எழுத்து *envstr;
  தேர்வு(info) {
  நிலை CURLINFO_TOTAL_TIME_T:
  நிலை CURLINFO_NAMELOOKUP_TIME_T:
  நிலை CURLINFO_CONNECT_TIME_T:
  நிலை CURLINFO_APPCONNECT_TIME_T:
  நிலை CURLINFO_PRETRANSFER_TIME_T:
  நிலை CURLINFO_POSTTRANSFER_TIME_T:
  நிலை CURLINFO_QUEUE_TIME_T:
  நிலை CURLINFO_STARTTRANSFER_TIME_T:
  நிலை CURLINFO_REDIRECT_TIME_T:
  நிலை CURLINFO_SPEED_DOWNLOAD_T:
  நிலை CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    எனில்(envstr) {
      curl_off_t val;
      curlx_str_எண்(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
#endif
  தேர்வு(info) {
  நிலை CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    நிறுத்து;
  நிலை CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    நிறுத்து;
  நிலை CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    நிறுத்து;
  நிலை CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    நிறுத்து;
  நிலை CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    நிறுத்து;
  நிலை CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    நிறுத்து;
  நிலை CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    நிறுத்து;
  நிலை CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    நிறுத்து;
   நிலை CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    நிறுத்து;
  நிலை CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    நிறுத்து;
  நிலை CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    நிறுத்து;
  நிலை CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    நிறுத்து;
  நிலை CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    நிறுத்து;
  நிலை CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    நிறுத்து;
  நிலை CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    நிறுத்து;
  நிலை CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    நிறுத்து;
  நிலை CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    நிறுத்து;
  நிலை CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    நிறுத்து;
  நிலை CURLINFO_XFER_ID:
    *param_offt = data->id;
    நிறுத்து;
  நிலை CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    நிறுத்து;
  நிலை CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode getinfo_double(கட்டமைப்பு Curl_easy *data, CURLINFO info,
                               இரட்டை *param_doublep)
{
#ifdef DEBUGBUILD
  மாறிலி எழுத்து *envstr;

  தேர்வு(info) {
  நிலை CURLINFO_TOTAL_TIME:
  நிலை CURLINFO_NAMELOOKUP_TIME:
  நிலை CURLINFO_CONNECT_TIME:
  நிலை CURLINFO_APPCONNECT_TIME:
  நிலை CURLINFO_PRETRANSFER_TIME:
  நிலை CURLINFO_STARTTRANSFER_TIME:
  நிலை CURLINFO_REDIRECT_TIME:
  நிலை CURLINFO_SPEED_DOWNLOAD:
  நிலை CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    எனில்(envstr) {
      curl_off_t val;
      curlx_str_எண்(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (இரட்டை)val;
      திரும்பு CURLE_OK;
    }
    நிறுத்து;
  இயல்பு:
    நிறுத்து;
  }
#endif
  தேர்வு(info) {
  நிலை CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    நிறுத்து;
  நிலை CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    நிறுத்து;
  நிலை CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    நிறுத்து;
  நிலை CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    நிறுத்து;
  நிலை CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    நிறுத்து;
  நிலை CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    நிறுத்து;
  நிலை CURLINFO_SIZE_UPLOAD:
    *param_doublep = (இரட்டை)data->progress.ul.cur_size;
    நிறுத்து;
  நிலை CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (இரட்டை)data->progress.dl.cur_size;
    நிறுத்து;
  நிலை CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (இரட்டை)data->progress.dl.speed;
    நிறுத்து;
  நிலை CURLINFO_SPEED_UPLOAD:
    *param_doublep = (இரட்டை)data->progress.ul.speed;
    நிறுத்து;
  நிலை CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (இரட்டை)data->progress.dl.total_size : -1;
    நிறுத்து;
  நிலை CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (இரட்டை)data->progress.ul.total_size : -1;
    நிறுத்து;
  நிலை CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    நிறுத்து;

  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode getinfo_slist(கட்டமைப்பு Curl_easy *data, CURLINFO info,
                              கட்டமைப்பு curl_slist **param_slistp)
{
  ஒன்றிப்பு {
    கட்டமைப்பு curl_certinfo *to_certinfo;
    கட்டமைப்பு curl_slist    *to_slist;
  } ptr;

  தேர்வு(info) {
  நிலை CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    நிறுத்து;
  நிலை CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    நிறுத்து;
  நிலை CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    நிறுத்து;
  நிலை CURLINFO_TLS_SESSION:
  நிலை CURLINFO_TLS_SSL_PTR: {
    முழு query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    கட்டமைப்பு curl_tlssessioninfo **tsip = (கட்டமைப்பு curl_tlssessioninfo **)
      param_slistp;
    கட்டமைப்பு curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    எனில்(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = வெற்று;
    }
  }
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }

  திரும்பு CURLE_OK;
}

நிலையான CURLcode getinfo_socket(கட்டமைப்பு Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  தேர்வு(info) {
  நிலை CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, வெற்று);
    நிறுத்து;
  இயல்பு:
    திரும்பு CURLE_UNKNOWN_OPTION;
  }

  திரும்பு CURLE_OK;
}

CURLcode Curl_getinfo(கட்டமைப்பு Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = வெற்று;
  இரட்டை *param_doublep = வெற்று;
  curl_off_t *param_offt = வெற்று;
  மாறிலி எழுத்து **param_charp = வெற்று;
  கட்டமைப்பு curl_slist **param_slistp = வெற்று;
  curl_socket_t *param_socketp = வெற்று;
  முழு type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  எனில்(!data)
    திரும்பு CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (முழு)info;
  தேர்வு(type) {
  நிலை CURLINFO_STRING:
    param_charp = va_arg(arg, மாறிலி எழுத்து **);
    எனில்(param_charp)
      result = getinfo_char(data, info, param_charp);
    நிறுத்து;
  நிலை CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    எனில்(param_longp)
      result = getinfo_long(data, info, param_longp);
    நிறுத்து;
  நிலை CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, இரட்டை *);
    எனில்(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    நிறுத்து;
  நிலை CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    எனில்(param_offt)
      result = getinfo_offt(data, info, param_offt);
    நிறுத்து;
  நிலை CURLINFO_SLIST:
    param_slistp = va_arg(arg, கட்டமைப்பு curl_slist **);
    எனில்(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    நிறுத்து;
  நிலை CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    எனில்(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    நிறுத்து;
  இயல்பு:
    result = CURLE_UNKNOWN_OPTION;
    நிறுத்து;
  }

  va_end(arg);

  திரும்பு result;
}
