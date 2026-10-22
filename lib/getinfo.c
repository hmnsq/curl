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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "getinfo.h"
#ଅନ୍ତର୍ଭାଵ "cfilters.h"
#ଅନ୍ତର୍ଭାଵ "vtls/vtls.h"
#ଅନ୍ତର୍ଭାଵ "connect.h" /* Curl_getconnectinfo() */
#ଅନ୍ତର୍ଭାଵ "bufref.h"
#ଅନ୍ତର୍ଭାଵ "url.h" /* Curl_uc_to_curlcode() */
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
ଶୂନ୍ୟ Curl_initinfo(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା Progress *pro = &data->progress;
  ସଂରଚନା PureInfo *info = &data->info;

  memset(&pro->delta, 0, ଆକାର(pro->delta));
  memset(&pro->total, 0, ଆକାର(pro->total));
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

  memset(&info->primary, 0, ଆକାର(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

ସ୍ଥିର CURLcode effective_url(ସଂରଚନା Curl_easy *data,
                              ଅଚଳ ଅକ୍ଷର **param_charp)
{
  ଅଚଳ ଅକ୍ଷର *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ଖାଲି : "";
  ଯଦି(s) {
    ଅକ୍ଷର *url = ଖାଲି;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    ଯଦି(!u)
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    ଯଦି(!uc)
      uc = curl_url_set(u, CURLUPART_USER, ଖାଲି, 0);
    ଯଦି(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, ଖାଲି, 0);
    ଯଦି(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    ଯଦି(uc || !url)
      ଫେରନ୍ତୁ uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    ଯଦି(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    ଅନ୍ୟଥା {
      curl_free(url);
      *param_charp = s;
    }
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode getinfo_char(ସଂରଚନା Curl_easy *data, CURLINFO info,
                             ଅଚଳ ଅକ୍ଷର **param_charp)
{
  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_EFFECTIVE_URL:
    ଫେରନ୍ତୁ effective_url(data, param_charp);
  ସ୍ଥିତି CURLINFO_EFFECTIVE_METHOD: {
    ଅଚଳ ଅକ୍ଷର *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    ଯଦି(!m) {
      ଯଦି(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      ଅନ୍ୟଥା {
        ବାଛ(data->state.httpreq) {
        ସ୍ଥିତି HTTPREQ_POST:
        ସ୍ଥିତି HTTPREQ_POST_FORM:
        ସ୍ଥିତି HTTPREQ_POST_MIME:
          m = "POST";
          ଭାଙ୍ଗ;
        ସ୍ଥିତି HTTPREQ_PUT:
          m = "PUT";
          ଭାଙ୍ଗ;
        ପୂର୍ବନିର୍ଦ୍ଧାରିତ: /* this should never happen */
        ସ୍ଥିତି HTTPREQ_GET:
          m = "GET";
          ଭାଙ୍ଗ;
        ସ୍ଥିତି HTTPREQ_HEAD:
          m = "HEAD";
          ଭାଙ୍ଗ;
        }
      }
#endif
    }
    *param_charp = m;
  }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PRIVATE:
    *param_charp = (ଅଚଳ ଅକ୍ଷର *)data->set.private_data;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#ଅନ୍ୟଥା
    *param_charp = ଖାଲି;
#endif
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#ଅନ୍ୟଥା
    *param_charp = ଖାଲି;
#endif
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#ଅନ୍ୟଥା
    *param_charp = ଖାଲି;
#endif
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode getinfo_long(ସଂରଚନା Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  ସଂଘ {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  ଅଚଳ ଅକ୍ଷର *envstr;

  /* use another variable for this to allow different values */
  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    ଯଦି(envstr) {
      curl_off_t val;
      curlx_str_ସଂଖ୍ୟା(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_HEADER_SIZE:
  ସ୍ଥିତି CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    ଯଦି(envstr) {
      curl_off_t val;
      curlx_str_ସଂଖ୍ୟା(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
#endif

  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_FILETIME:
    ଯଦି(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#ଯଦି !defined(MSDOS) && !defined(__AMIGA__)
    ଅନ୍ୟଥା ଯଦି(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    ଅନ୍ୟଥା
      *param_longp = (long)data->info.filetime;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#ଅନ୍ୟଥା
    *param_longp = 0;
#endif
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_REDIRECT_ଗଣନା:
    *param_longp = data->state.followlocation;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_ଅଙ୍କ_CONNECTS:
#ଯଦି SIZEOF_LONG < SIZEOF_CURL_OFF_T
    ଯଦି(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    ଅନ୍ୟଥା
#endif
      *param_longp = (long)data->info.numconnects;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, ଖାଲି);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    ଯଦି(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    ଅନ୍ୟଥା
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    ଯଦି(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    ଅନ୍ୟଥା
      *param_longp = -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    ଯଦି(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    ଅନ୍ୟଥା
      *param_longp = -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONDITION_UNMET:
    ଯଦି(data->info.httpcode == 304)
      *param_longp = 1L;
    ଅନ୍ୟଥା
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    ଭାଙ୍ଗ;
#ifndef CURL_DISABLE_RTSP
  ସ୍ଥିତି CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    ଭାଙ୍ଗ;
#ଅନ୍ୟଥା
  ସ୍ଥିତି CURLINFO_RTSP_CLIENT_CSEQ:
  ସ୍ଥିତି CURLINFO_RTSP_SERVER_CSEQ:
  ସ୍ଥିତି CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    ଭାଙ୍ଗ;
#endif
  ସ୍ଥିତି CURLINFO_HTTP_VERSION:
    ବାଛ(data->info.httpversion) {
    ସ୍ଥିତି 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 30:
      *param_longp = CURL_HTTP_VERSION_3;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      *param_longp = CURL_HTTP_VERSION_NONE;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#ଅନ୍ୟଥା
      data->info.used_proxy
#endif
      ;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

#define DOUBLE_SECS(x) ((ଦ୍ୱିଗୁଣ)(x) / 1000000)

ସ୍ଥିର CURLcode getinfo_offt(ସଂରଚନା Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  ଅଚଳ ଅକ୍ଷର *envstr;
  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_TOTAL_TIME_T:
  ସ୍ଥିତି CURLINFO_NAMELOOKUP_TIME_T:
  ସ୍ଥିତି CURLINFO_CONNECT_TIME_T:
  ସ୍ଥିତି CURLINFO_APPCONNECT_TIME_T:
  ସ୍ଥିତି CURLINFO_PRETRANSFER_TIME_T:
  ସ୍ଥିତି CURLINFO_POSTTRANSFER_TIME_T:
  ସ୍ଥିତି CURLINFO_QUEUE_TIME_T:
  ସ୍ଥିତି CURLINFO_STARTTRANSFER_TIME_T:
  ସ୍ଥିତି CURLINFO_REDIRECT_TIME_T:
  ସ୍ଥିତି CURLINFO_SPEED_DOWNLOAD_T:
  ସ୍ଥିତି CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    ଯଦି(envstr) {
      curl_off_t val;
      curlx_str_ସଂଖ୍ୟା(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
#endif
  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    ଭାଙ୍ଗ;
   ସ୍ଥିତି CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_XFER_ID:
    *param_offt = data->id;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode getinfo_double(ସଂରଚନା Curl_easy *data, CURLINFO info,
                               ଦ୍ୱିଗୁଣ *param_doublep)
{
#ifdef DEBUGBUILD
  ଅଚଳ ଅକ୍ଷର *envstr;

  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_TOTAL_TIME:
  ସ୍ଥିତି CURLINFO_NAMELOOKUP_TIME:
  ସ୍ଥିତି CURLINFO_CONNECT_TIME:
  ସ୍ଥିତି CURLINFO_APPCONNECT_TIME:
  ସ୍ଥିତି CURLINFO_PRETRANSFER_TIME:
  ସ୍ଥିତି CURLINFO_STARTTRANSFER_TIME:
  ସ୍ଥିତି CURLINFO_REDIRECT_TIME:
  ସ୍ଥିତି CURLINFO_SPEED_DOWNLOAD:
  ସ୍ଥିତି CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    ଯଦି(envstr) {
      curl_off_t val;
      curlx_str_ସଂଖ୍ୟା(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (ଦ୍ୱିଗୁଣ)val;
      ଫେରନ୍ତୁ CURLE_OK;
    }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଭାଙ୍ଗ;
  }
#endif
  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SIZE_UPLOAD:
    *param_doublep = (ଦ୍ୱିଗୁଣ)data->progress.ul.cur_size;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (ଦ୍ୱିଗୁଣ)data->progress.dl.cur_size;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (ଦ୍ୱିଗୁଣ)data->progress.dl.speed;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SPEED_UPLOAD:
    *param_doublep = (ଦ୍ୱିଗୁଣ)data->progress.ul.speed;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (ଦ୍ୱିଗୁଣ)data->progress.dl.total_size : -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (ଦ୍ୱିଗୁଣ)data->progress.ul.total_size : -1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    ଭାଙ୍ଗ;

  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode getinfo_slist(ସଂରଚନା Curl_easy *data, CURLINFO info,
                              ସଂରଚନା curl_slist **param_slistp)
{
  ସଂଘ {
    ସଂରଚନା curl_certinfo *to_certinfo;
    ସଂରଚନା curl_slist    *to_slist;
  } ptr;

  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_TLS_SESSION:
  ସ୍ଥିତି CURLINFO_TLS_SSL_PTR: {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    ସଂରଚନା curl_tlssessioninfo **tsip = (ସଂରଚନା curl_tlssessioninfo **)
      param_slistp;
    ସଂରଚନା curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    ଯଦି(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ଖାଲି;
    }
  }
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode getinfo_socket(ସଂରଚନା Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  ବାଛ(info) {
  ସ୍ଥିତି CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, ଖାଲି);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    ଫେରନ୍ତୁ CURLE_UNKNOWN_OPTION;
  }

  ଫେରନ୍ତୁ CURLE_OK;
}

CURLcode Curl_getinfo(ସଂରଚନା Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = ଖାଲି;
  ଦ୍ୱିଗୁଣ *param_doublep = ଖାଲି;
  curl_off_t *param_offt = ଖାଲି;
  ଅଚଳ ଅକ୍ଷର **param_charp = ଖାଲି;
  ସଂରଚନା curl_slist **param_slistp = ଖାଲି;
  curl_socket_t *param_socketp = ଖାଲି;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  ଯଦି(!data)
    ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)info;
  ବାଛ(type) {
  ସ୍ଥିତି CURLINFO_STRING:
    param_charp = va_arg(arg, ଅଚଳ ଅକ୍ଷର **);
    ଯଦି(param_charp)
      result = getinfo_char(data, info, param_charp);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    ଯଦି(param_longp)
      result = getinfo_long(data, info, param_longp);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, ଦ୍ୱିଗୁଣ *);
    ଯଦି(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    ଯଦି(param_offt)
      result = getinfo_offt(data, info, param_offt);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SLIST:
    param_slistp = va_arg(arg, ସଂରଚନା curl_slist **);
    ଯଦି(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    ଯଦି(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    ଭାଙ୍ଗ;
  ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
    result = CURLE_UNKNOWN_OPTION;
    ଭାଙ୍ଗ;
  }

  va_end(arg);

  ଫେରନ୍ତୁ result;
}
