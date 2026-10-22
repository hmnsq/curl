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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "urldata.h"
#અન્તર્ભાવ "getinfo.h"
#અન્તર્ભાવ "cfilters.h"
#અન્તર્ભાવ "vtls/vtls.h"
#અન્તર્ભાવ "connect.h" /* Curl_getconnectinfo() */
#અન્તર્ભાવ "bufref.h"
#અન્તર્ભાવ "url.h" /* Curl_uc_to_curlcode() */
#અન્તર્ભાવ "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
શૂન્ય Curl_initinfo(સંરચના Curl_easy *data)
{
  સંરચના Progress *pro = &data->progress;
  સંરચના PureInfo *info = &data->info;

  memset(&pro->delta, 0, કદ(pro->delta));
  memset(&pro->total, 0, કદ(pro->total));
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

  memset(&info->primary, 0, કદ(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

સ્થિર CURLcode effective_url(સંરચના Curl_easy *data,
                              અચલ અક્ષર **param_charp)
{
  અચલ અક્ષર *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? ખાલી : "";
  જો(s) {
    અક્ષર *url = ખાલી;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    જો(!u)
      પાછા CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    જો(!uc)
      uc = curl_url_set(u, CURLUPART_USER, ખાલી, 0);
    જો(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, ખાલી, 0);
    જો(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    જો(uc || !url)
      પાછા uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    જો(strcmp(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    નહીંતર {
      curl_free(url);
      *param_charp = s;
    }
  }
  પાછા CURLE_OK;
}

સ્થિર CURLcode getinfo_char(સંરચના Curl_easy *data, CURLINFO info,
                             અચલ અક્ષર **param_charp)
{
  પસંદગી(info) {
  સ્થિતિ CURLINFO_EFFECTIVE_URL:
    પાછા effective_url(data, param_charp);
  સ્થિતિ CURLINFO_EFFECTIVE_METHOD: {
    અચલ અક્ષર *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    જો(!m) {
      જો(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      નહીંતર {
        પસંદગી(data->state.httpreq) {
        સ્થિતિ HTTPREQ_POST:
        સ્થિતિ HTTPREQ_POST_FORM:
        સ્થિતિ HTTPREQ_POST_MIME:
          m = "POST";
          તોડો;
        સ્થિતિ HTTPREQ_PUT:
          m = "PUT";
          તોડો;
        મૂળભૂત: /* this should never happen */
        સ્થિતિ HTTPREQ_GET:
          m = "GET";
          તોડો;
        સ્થિતિ HTTPREQ_HEAD:
          m = "HEAD";
          તોડો;
        }
      }
#endif
    }
    *param_charp = m;
  }
    તોડો;
  સ્થિતિ CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    તોડો;
  સ્થિતિ CURLINFO_PRIVATE:
    *param_charp = (અચલ અક્ષર *)data->set.private_data;
    તોડો;
  સ્થિતિ CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    તોડો;
  સ્થિતિ CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    તોડો;
  સ્થિતિ CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    તોડો;
  સ્થિતિ CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    તોડો;
  સ્થિતિ CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    તોડો;
  સ્થિતિ CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#નહીંતર
    *param_charp = ખાલી;
#endif
    તોડો;
  સ્થિતિ CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    તોડો;
  સ્થિતિ CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#નહીંતર
    *param_charp = ખાલી;
#endif
    તોડો;
  સ્થિતિ CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#નહીંતર
    *param_charp = ખાલી;
#endif
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode getinfo_long(સંરચના Curl_easy *data, CURLINFO info,
                             long *param_longp)
{
  curl_socket_t sockfd;

  સંઘ {
    unsigned long *to_ulong;
    long          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  અચલ અક્ષર *envstr;

  /* use another variable for this to allow different values */
  પસંદગી(info) {
  સ્થિતિ CURLINFO_LOCAL_PORT:
    envstr = getenv("CURL_TIME");
    જો(envstr) {
      curl_off_t val;
      curlx_str_સંખ્યા(&envstr, &val, TIME_T_MAX);
      *param_longp = (long)val;
      પાછા CURLE_OK;
    }
    તોડો;
  સ્થિતિ CURLINFO_HEADER_SIZE:
  સ્થિતિ CURLINFO_REQUEST_SIZE:
    envstr = getenv("CURL_DEBUG_SIZE");
    જો(envstr) {
      curl_off_t val;
      curlx_str_સંખ્યા(&envstr, &val, LONG_MAX);
      *param_longp = (long)val;
      પાછા CURLE_OK;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
#endif

  પસંદગી(info) {
  સ્થિતિ CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    તોડો;
  સ્થિતિ CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    તોડો;
  સ્થિતિ CURLINFO_FILETIME:
    જો(data->info.filetime > LONG_MAX)
      *param_longp = LONG_MAX;
#જો !defined(MSDOS) && !defined(__AMIGA__)
    નહીંતર જો(data->info.filetime < LONG_MIN)
      *param_longp = LONG_MIN;
#endif
    નહીંતર
      *param_longp = (long)data->info.filetime;
    તોડો;
  સ્થિતિ CURLINFO_HEADER_SIZE:
    *param_longp = (long)data->info.header_size;
    તોડો;
  સ્થિતિ CURLINFO_REQUEST_SIZE:
    *param_longp = (long)data->info.request_size;
    તોડો;
  સ્થિતિ CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    તોડો;
  સ્થિતિ CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#નહીંતર
    *param_longp = 0;
#endif
    તોડો;
  સ્થિતિ CURLINFO_REDIRECT_ગણતરી:
    *param_longp = data->state.followlocation;
    તોડો;
  સ્થિતિ CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    તોડો;
  સ્થિતિ CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    તોડો;
  સ્થિતિ CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    તોડો;
  સ્થિતિ CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    તોડો;
  સ્થિતિ CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    તોડો;
  સ્થિતિ CURLINFO_અંક_CONNECTS:
#જો SIZEOF_LONG < SIZEOF_CURL_OFF_T
    જો(data->info.numconnects > LONG_MAX)
      *param_longp = LONG_MAX;
    નહીંતર
#endif
      *param_longp = (long)data->info.numconnects;
    તોડો;
  સ્થિતિ CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, ખાલી);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    જો(sockfd != CURL_SOCKET_BAD)
      *param_longp = (long)sockfd;
    નહીંતર
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    તોડો;
  સ્થિતિ CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    જો(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    નહીંતર
      *param_longp = -1;
    તોડો;
  સ્થિતિ CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    જો(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    નહીંતર
      *param_longp = -1;
    તોડો;
  સ્થિતિ CURLINFO_PROXY_ERROR:
    *param_longp = (long)data->info.pxcode;
    તોડો;
  સ્થિતિ CURLINFO_CONDITION_UNMET:
    જો(data->info.httpcode == 304)
      *param_longp = 1L;
    નહીંતર
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    તોડો;
#ifndef CURL_DISABLE_RTSP
  સ્થિતિ CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    તોડો;
  સ્થિતિ CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    તોડો;
  સ્થિતિ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    તોડો;
#નહીંતર
  સ્થિતિ CURLINFO_RTSP_CLIENT_CSEQ:
  સ્થિતિ CURLINFO_RTSP_SERVER_CSEQ:
  સ્થિતિ CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    તોડો;
#endif
  સ્થિતિ CURLINFO_HTTP_VERSION:
    પસંદગી(data->info.httpversion) {
    સ્થિતિ 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      તોડો;
    સ્થિતિ 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      તોડો;
    સ્થિતિ 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      તોડો;
    સ્થિતિ 30:
      *param_longp = CURL_HTTP_VERSION_3;
      તોડો;
    મૂળભૂત:
      *param_longp = CURL_HTTP_VERSION_NONE;
      તોડો;
    }
    તોડો;
  સ્થિતિ CURLINFO_PROTOCOL:
    *param_longp = (long)data->info.conn_protocol;
    તોડો;
  સ્થિતિ CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#નહીંતર
      data->info.used_proxy
#endif
      ;
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }

  પાછા CURLE_OK;
}

#define DOUBLE_SECS(x) ((દ્વિગુણ)(x) / 1000000)

સ્થિર CURLcode getinfo_offt(સંરચના Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  અચલ અક્ષર *envstr;
  પસંદગી(info) {
  સ્થિતિ CURLINFO_TOTAL_TIME_T:
  સ્થિતિ CURLINFO_NAMELOOKUP_TIME_T:
  સ્થિતિ CURLINFO_CONNECT_TIME_T:
  સ્થિતિ CURLINFO_APPCONNECT_TIME_T:
  સ્થિતિ CURLINFO_PRETRANSFER_TIME_T:
  સ્થિતિ CURLINFO_POSTTRANSFER_TIME_T:
  સ્થિતિ CURLINFO_QUEUE_TIME_T:
  સ્થિતિ CURLINFO_STARTTRANSFER_TIME_T:
  સ્થિતિ CURLINFO_REDIRECT_TIME_T:
  સ્થિતિ CURLINFO_SPEED_DOWNLOAD_T:
  સ્થિતિ CURLINFO_SPEED_UPLOAD_T:
    envstr = getenv("CURL_TIME");
    જો(envstr) {
      curl_off_t val;
      curlx_str_સંખ્યા(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      પાછા CURLE_OK;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
#endif
  પસંદગી(info) {
  સ્થિતિ CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    તોડો;
  સ્થિતિ CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    તોડો;
  સ્થિતિ CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    તોડો;
  સ્થિતિ CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    તોડો;
  સ્થિતિ CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    તોડો;
  સ્થિતિ CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    તોડો;
  સ્થિતિ CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    તોડો;
  સ્થિતિ CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    તોડો;
   સ્થિતિ CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    તોડો;
  સ્થિતિ CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    તોડો;
  સ્થિતિ CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    તોડો;
  સ્થિતિ CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    તોડો;
  સ્થિતિ CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    તોડો;
  સ્થિતિ CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    તોડો;
  સ્થિતિ CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    તોડો;
  સ્થિતિ CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    તોડો;
  સ્થિતિ CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    તોડો;
  સ્થિતિ CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    તોડો;
  સ્થિતિ CURLINFO_XFER_ID:
    *param_offt = data->id;
    તોડો;
  સ્થિતિ CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    તોડો;
  સ્થિતિ CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode getinfo_double(સંરચના Curl_easy *data, CURLINFO info,
                               દ્વિગુણ *param_doublep)
{
#ifdef DEBUGBUILD
  અચલ અક્ષર *envstr;

  પસંદગી(info) {
  સ્થિતિ CURLINFO_TOTAL_TIME:
  સ્થિતિ CURLINFO_NAMELOOKUP_TIME:
  સ્થિતિ CURLINFO_CONNECT_TIME:
  સ્થિતિ CURLINFO_APPCONNECT_TIME:
  સ્થિતિ CURLINFO_PRETRANSFER_TIME:
  સ્થિતિ CURLINFO_STARTTRANSFER_TIME:
  સ્થિતિ CURLINFO_REDIRECT_TIME:
  સ્થિતિ CURLINFO_SPEED_DOWNLOAD:
  સ્થિતિ CURLINFO_SPEED_UPLOAD:
    envstr = getenv("CURL_TIME");
    જો(envstr) {
      curl_off_t val;
      curlx_str_સંખ્યા(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (દ્વિગુણ)val;
      પાછા CURLE_OK;
    }
    તોડો;
  મૂળભૂત:
    તોડો;
  }
#endif
  પસંદગી(info) {
  સ્થિતિ CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    તોડો;
  સ્થિતિ CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    તોડો;
  સ્થિતિ CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    તોડો;
  સ્થિતિ CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    તોડો;
  સ્થિતિ CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    તોડો;
  સ્થિતિ CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    તોડો;
  સ્થિતિ CURLINFO_SIZE_UPLOAD:
    *param_doublep = (દ્વિગુણ)data->progress.ul.cur_size;
    તોડો;
  સ્થિતિ CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (દ્વિગુણ)data->progress.dl.cur_size;
    તોડો;
  સ્થિતિ CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (દ્વિગુણ)data->progress.dl.speed;
    તોડો;
  સ્થિતિ CURLINFO_SPEED_UPLOAD:
    *param_doublep = (દ્વિગુણ)data->progress.ul.speed;
    તોડો;
  સ્થિતિ CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (દ્વિગુણ)data->progress.dl.total_size : -1;
    તોડો;
  સ્થિતિ CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (દ્વિગુણ)data->progress.ul.total_size : -1;
    તોડો;
  સ્થિતિ CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    તોડો;

  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode getinfo_slist(સંરચના Curl_easy *data, CURLINFO info,
                              સંરચના curl_slist **param_slistp)
{
  સંઘ {
    સંરચના curl_certinfo *to_certinfo;
    સંરચના curl_slist    *to_slist;
  } ptr;

  પસંદગી(info) {
  સ્થિતિ CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    તોડો;
  સ્થિતિ CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    તોડો;
  સ્થિતિ CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    તોડો;
  સ્થિતિ CURLINFO_TLS_SESSION:
  સ્થિતિ CURLINFO_TLS_SSL_PTR: {
    પૂર્ણાંક query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    સંરચના curl_tlssessioninfo **tsip = (સંરચના curl_tlssessioninfo **)
      param_slistp;
    સંરચના curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    જો(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = ખાલી;
    }
  }
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }

  પાછા CURLE_OK;
}

સ્થિર CURLcode getinfo_socket(સંરચના Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  પસંદગી(info) {
  સ્થિતિ CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, ખાલી);
    તોડો;
  મૂળભૂત:
    પાછા CURLE_UNKNOWN_OPTION;
  }

  પાછા CURLE_OK;
}

CURLcode Curl_getinfo(સંરચના Curl_easy *data, CURLINFO info, ...)
{
  va_list arg;
  long *param_longp = ખાલી;
  દ્વિગુણ *param_doublep = ખાલી;
  curl_off_t *param_offt = ખાલી;
  અચલ અક્ષર **param_charp = ખાલી;
  સંરચના curl_slist **param_slistp = ખાલી;
  curl_socket_t *param_socketp = ખાલી;
  પૂર્ણાંક type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  જો(!data)
    પાછા CURLE_BAD_FUNCTION_ARGUMENT;

  va_start(arg, info);

  type = CURLINFO_TYPEMASK & (પૂર્ણાંક)info;
  પસંદગી(type) {
  સ્થિતિ CURLINFO_STRING:
    param_charp = va_arg(arg, અચલ અક્ષર **);
    જો(param_charp)
      result = getinfo_char(data, info, param_charp);
    તોડો;
  સ્થિતિ CURLINFO_LONG:
    param_longp = va_arg(arg, long *);
    જો(param_longp)
      result = getinfo_long(data, info, param_longp);
    તોડો;
  સ્થિતિ CURLINFO_DOUBLE:
    param_doublep = va_arg(arg, દ્વિગુણ *);
    જો(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    તોડો;
  સ્થિતિ CURLINFO_OFF_T:
    param_offt = va_arg(arg, curl_off_t *);
    જો(param_offt)
      result = getinfo_offt(data, info, param_offt);
    તોડો;
  સ્થિતિ CURLINFO_SLIST:
    param_slistp = va_arg(arg, સંરચના curl_slist **);
    જો(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    તોડો;
  સ્થિતિ CURLINFO_SOCKET:
    param_socketp = va_arg(arg, curl_socket_t *);
    જો(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    તોડો;
  મૂળભૂત:
    result = CURLE_UNKNOWN_OPTION;
    તોડો;
  }

  va_end(arg);

  પાછા result;
}
