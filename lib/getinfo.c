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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "getinfo.h"
#অন্তর্ভাব "cfilters.h"
#অন্তর্ভাব "vtls/vtls.h"
#অন্তর্ভাব "connect.h" /* Curl_getconnectinfo() */
#অন্তর্ভাব "bufref.h"
#অন্তর্ভাব "url.h" /* Curl_uc_to_curlcode() */
#অন্তর্ভাব "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
শূন্য Curl_initinfo(সংরচনা Curl_easy *data)
{
  সংরচনা Progress *pro = &data->progress;
  সংরচনা PureInfo *info = &data->info;

  memset(&pro->delta, 0, পরিমাণ(pro->delta));
  memset(&pro->total, 0, পরিমাণ(pro->total));
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

  memset(&info->primary, 0, পরিমাণ(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

স্থির CURLcode effective_url(সংরচনা Curl_easy *data,
                              ধ্রুব বর্ণ **param_charp)
{
  ধ্রুব বর্ণ *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? রিক্ত : "";
  যদি(s) {
    বর্ণ *url = রিক্ত;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    যদি(!u)
      প্রত্যাগম CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    যদি(!uc)
      uc = curl_url_set(u, CURLUPART_USER, রিক্ত, 0);
    যদি(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, রিক্ত, 0);
    যদি(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    যদি(uc || !url)
      প্রত্যাগম uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    যদি(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    অন্যথা {
      curl_free(url);
      *param_charp = s;
    }
  }
  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode getinfo_char(সংরচনা Curl_easy *data, CURLINFO info,
                             ধ্রুব বর্ণ **param_charp)
{
  বিকল্প(info) {
  পক্ষ CURLINFO_EFFECTIVE_URL:
    প্রত্যাগম effective_url(data, param_charp);
  পক্ষ CURLINFO_EFFECTIVE_METHOD: {
    ধ্রুব বর্ণ *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    যদি(!m) {
      যদি(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      অন্যথা {
        বিকল্প(data->state.httpreq) {
        পক্ষ HTTPREQ_POST:
        পক্ষ HTTPREQ_POST_FORM:
        পক্ষ HTTPREQ_POST_MIME:
          m = "POST";
          ভঙ্গ;
        পক্ষ HTTPREQ_PUT:
          m = "PUT";
          ভঙ্গ;
        স্বভাব: /* this should never happen */
        পক্ষ HTTPREQ_GET:
          m = "GET";
          ভঙ্গ;
        পক্ষ HTTPREQ_HEAD:
          m = "HEAD";
          ভঙ্গ;
        }
      }
#endif
    }
    *param_charp = m;
  }
    ভঙ্গ;
  পক্ষ CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    ভঙ্গ;
  পক্ষ CURLINFO_PRIVATE:
    *param_charp = (ধ্রুব বর্ণ *)data->set.private_data;
    ভঙ্গ;
  পক্ষ CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    ভঙ্গ;
  পক্ষ CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    ভঙ্গ;
  পক্ষ CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    ভঙ্গ;
  পক্ষ CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    ভঙ্গ;
  পক্ষ CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    ভঙ্গ;
  পক্ষ CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#অন্যথা
    *param_charp = রিক্ত;
#endif
    ভঙ্গ;
  পক্ষ CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    ভঙ্গ;
  পক্ষ CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#অন্যথা
    *param_charp = রিক্ত;
#endif
    ভঙ্গ;
  পক্ষ CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#অন্যথা
    *param_charp = রিক্ত;
#endif
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode getinfo_long(সংরচনা Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  সঙ্ঘ {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  ধ্রুব বর্ণ *envstr;

  /* use another variable for this to allow different values */
  বিকল্প(info) {
  পক্ষ CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  পক্ষ CURLINFO_HEADER_SIZE:
  পক্ষ CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
#endif

  বিকল্প(info) {
  পক্ষ CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    ভঙ্গ;
  পক্ষ CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    ভঙ্গ;
  পক্ষ CURLINFO_FILETIME:
    যদি(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#যদি !defined(MSDOS) && !defined(__AMIGA__)
    অন্যথা যদি(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    অন্যথা
      *param_longp = (long)data->info.filetime;
    ভঙ্গ;
  পক্ষ CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    ভঙ্গ;
  পক্ষ CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    ভঙ্গ;
  পক্ষ CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    ভঙ্গ;
  পক্ষ CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#অন্যথা
    *param_longp = 0;
#endif
    ভঙ্গ;
  পক্ষ CURLINFO_REDIRECT_গুনতি:
    *param_longp = data->state.followlocation;
    ভঙ্গ;
  পক্ষ CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    ভঙ্গ;
  পক্ষ CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    ভঙ্গ;
  পক্ষ CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    ভঙ্গ;
  পক্ষ CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    ভঙ্গ;
  পক্ষ CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    ভঙ্গ;
  পক্ষ CURLINFO_অঙ্ক_CONNECTS:
#যদি SIZEOF_LONG < SIZEOF_CURL_OFF_T
    যদি(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    অন্যথা
#endif
      *param_longp = (long)data->info.numconnects;
    ভঙ্গ;
  পক্ষ CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, রিক্ত);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    যদি(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    অন্যথা
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    ভঙ্গ;
  পক্ষ CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    যদি(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    অন্যথা
      *param_longp = -1;
    ভঙ্গ;
  পক্ষ CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    যদি(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    অন্যথা
      *param_longp = -1;
    ভঙ্গ;
  পক্ষ CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    ভঙ্গ;
  পক্ষ CURLINFO_CONDITION_UNMET:
    যদি(data->info.httpcode == 304)
      *param_longp = 1L;
    অন্যথা
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    ভঙ্গ;
#ifndef CURL_DISABLE_RTSP
  পক্ষ CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    ভঙ্গ;
  পক্ষ CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    ভঙ্গ;
  পক্ষ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    ভঙ্গ;
#অন্যথা
  পক্ষ CURLINFO_RTSP_CLIENT_CSEQ:
  পক্ষ CURLINFO_RTSP_SERVER_CSEQ:
  পক্ষ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    ভঙ্গ;
#endif
  পক্ষ CURLINFO_HTTP_VERSION:
    বিকল্প(data->info.httpversion) {
    পক্ষ 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      ভঙ্গ;
    পক্ষ 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      ভঙ্গ;
    পক্ষ 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      ভঙ্গ;
    পক্ষ 30:
      *param_longp = CURL_HTTP_VERSION_3;
      ভঙ্গ;
    স্বভাব:
      *param_longp = CURL_HTTP_VERSION_NONE;
      ভঙ্গ;
    }
    ভঙ্গ;
  পক্ষ CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    ভঙ্গ;
  পক্ষ CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#অন্যথা
      data->info.used_proxy
#endif
      ;
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }

  প্রত্যাগম CURLE_OK;
}

#define DOUBLE_SECS(x) ((দ্বিগুণ)(x) / 1000000)

স্থির CURLcode getinfo_offt(সংরচনা Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  ধ্রুব বর্ণ *envstr;
  বিকল্প(info) {
  পক্ষ CURLINFO_TOTAL_TIME_T:
  পক্ষ CURLINFO_NAMELOOKUP_TIME_T:
  পক্ষ CURLINFO_CONNECT_TIME_T:
  পক্ষ CURLINFO_APPCONNECT_TIME_T:
  পক্ষ CURLINFO_PRETRANSFER_TIME_T:
  পক্ষ CURLINFO_POSTTRANSFER_TIME_T:
  পক্ষ CURLINFO_QUEUE_TIME_T:
  পক্ষ CURLINFO_STARTTRANSFER_TIME_T:
  পক্ষ CURLINFO_REDIRECT_TIME_T:
  পক্ষ CURLINFO_SPEED_DOWNLOAD_T:
  পক্ষ CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
#endif
  বিকল্প(info) {
  পক্ষ CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    ভঙ্গ;
  পক্ষ CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    ভঙ্গ;
  পক্ষ CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    ভঙ্গ;
  পক্ষ CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    ভঙ্গ;
  পক্ষ CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    ভঙ্গ;
  পক্ষ CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    ভঙ্গ;
  পক্ষ CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    ভঙ্গ;
  পক্ষ CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    ভঙ্গ;
   পক্ষ CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    ভঙ্গ;
  পক্ষ CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    ভঙ্গ;
  পক্ষ CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    ভঙ্গ;
  পক্ষ CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    ভঙ্গ;
  পক্ষ CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    ভঙ্গ;
  পক্ষ CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    ভঙ্গ;
  পক্ষ CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    ভঙ্গ;
  পক্ষ CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    ভঙ্গ;
  পক্ষ CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    ভঙ্গ;
  পক্ষ CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    ভঙ্গ;
  পক্ষ CURLINFO_XFER_ID:
    *param_offt = data->id;
    ভঙ্গ;
  পক্ষ CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    ভঙ্গ;
  পক্ষ CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode getinfo_double(সংরচনা Curl_easy *data, CURLINFO info,
                               দ্বিগুণ *param_doublep)
{
#ifdef DEBUGBUILD
  ধ্রুব বর্ণ *envstr;

  বিকল্প(info) {
  পক্ষ CURLINFO_TOTAL_TIME:
  পক্ষ CURLINFO_NAMELOOKUP_TIME:
  পক্ষ CURLINFO_CONNECT_TIME:
  পক্ষ CURLINFO_APPCONNECT_TIME:
  পক্ষ CURLINFO_PRETRANSFER_TIME:
  পক্ষ CURLINFO_STARTTRANSFER_TIME:
  পক্ষ CURLINFO_REDIRECT_TIME:
  পক্ষ CURLINFO_SPEED_DOWNLOAD:
  পক্ষ CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    যদি(envstr) {
      curl_off_t val;
      curlx_str_সংখ্যা(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (দ্বিগুণ)val;
      প্রত্যাগম CURLE_OK;
    }
    ভঙ্গ;
  স্বভাব:
    ভঙ্গ;
  }
#endif
  বিকল্প(info) {
  পক্ষ CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    ভঙ্গ;
  পক্ষ CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    ভঙ্গ;
  পক্ষ CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    ভঙ্গ;
  পক্ষ CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    ভঙ্গ;
  পক্ষ CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    ভঙ্গ;
  পক্ষ CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    ভঙ্গ;
  পক্ষ CURLINFO_SIZE_UPLOAD:
    *param_doublep = (দ্বিগুণ)data->progress.ul.cur_size;
    ভঙ্গ;
  পক্ষ CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (দ্বিগুণ)data->progress.dl.cur_size;
    ভঙ্গ;
  পক্ষ CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (দ্বিগুণ)data->progress.dl.speed;
    ভঙ্গ;
  পক্ষ CURLINFO_SPEED_UPLOAD:
    *param_doublep = (দ্বিগুণ)data->progress.ul.speed;
    ভঙ্গ;
  পক্ষ CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (দ্বিগুণ)data->progress.dl.total_size : -1;
    ভঙ্গ;
  পক্ষ CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (দ্বিগুণ)data->progress.ul.total_size : -1;
    ভঙ্গ;
  পক্ষ CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    ভঙ্গ;

  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode getinfo_slist(সংরচনা Curl_easy *data, CURLINFO info,
                              সংরচনা curl_slist **param_slistp)
{
  সঙ্ঘ {
    সংরচনা curl_certinfo *to_certinfo;
    সংরচনা curl_slist    *to_slist;
  } ptr;

  বিকল্প(info) {
  পক্ষ CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    ভঙ্গ;
  পক্ষ CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    ভঙ্গ;
  পক্ষ CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    ভঙ্গ;
  পক্ষ CURLINFO_TLS_SESSION:
  পক্ষ CURLINFO_TLS_SSL_PTR: {
    পূর্ণাঙ্ক query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    সংরচনা curl_tlssessioninfo **tsip = (সংরচনা curl_tlssessioninfo **)
      param_slistp;
    সংরচনা curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    যদি(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = রিক্ত;
    }
  }
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }

  প্রত্যাগম CURLE_OK;
}

স্থির CURLcode getinfo_socket(সংরচনা Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  বিকল্প(info) {
  পক্ষ CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, রিক্ত);
    ভঙ্গ;
  স্বভাব:
    প্রত্যাগম CURLE_UNKNOWN_OPTION;
  }

  প্রত্যাগম CURLE_OK;
}

CURLcode Curl_getinfo(সংরচনা Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = রিক্ত;
  দ্বিগুণ *param_doublep = রিক্ত;
  curl_off_t *param_offt = রিক্ত;
  ধ্রুব বর্ণ **param_charp = রিক্ত;
  সংরচনা curl_slist **param_slistp = রিক্ত;
  curl_socket_t *param_socketp = রিক্ত;
  পূর্ণাঙ্ক type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  যদি(!data)
    প্রত্যাগম CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (পূর্ণাঙ্ক)info;
  বিকল্প(type) {
  পক্ষ CURLINFO_STRING:
    param_charp = va_arg(arg, ধ্রুব বর্ণ **);
    যদি(param_charp)
      result = getinfo_char(data, info, param_charp);
    ভঙ্গ;
  পক্ষ CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    যদি(param_longp)
      result = getinfo_long(data, info, param_longp);
    ভঙ্গ;
  পক্ষ CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, দ্বিগুণ *);
    যদি(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    ভঙ্গ;
  পক্ষ CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    যদি(param_offt)
      result = getinfo_offt(data, info, param_offt);
    ভঙ্গ;
  পক্ষ CURLINFO_SLIST:
    param_slistp = va_arg(arg, সংরচনা curl_slist **);
    যদি(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    ভঙ্গ;
  পক্ষ CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    যদি(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    ভঙ্গ;
  স্বভাব:
    result = CURLE_UNKNOWN_OPTION;
    ভঙ্গ;
  }

  va_end(arg);

  প্রত্যাগম result;
}
