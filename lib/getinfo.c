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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "urldata.h"
#अन्तर्भाव "getinfo.h"
#अन्तर्भाव "cfilters.h"
#अन्तर्भाव "vtls/vtls.h"
#अन्तर्भाव "connect.h" /* Curl_getconnectinfo() */
#अन्तर्भाव "bufref.h"
#अन्तर्भाव "url.h" /* Curl_uc_to_curlcode() */
#अन्तर्भाव "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
शून्य Curl_initinfo(संरचना Curl_easy *data)
{
  संरचना Progress *pro = &data->progress;
  संरचना PureInfo *info = &data->info;

  स_रखो(&pro->delta, 0, परिमाण(pro->delta));
  स_रखो(&pro->total, 0, परिमाण(pro->total));
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

  स_रखो(&info->primary, 0, परिमाण(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#ifdef USE_SSL
  Curl_ssl_free_certinfo(data);
#endif
}

स्थिर CURLcode effective_url(संरचना Curl_easy *data,
                              अचर वर्ण **param_charp)
{
  अचर वर्ण *s = Curl_bufref_ptr(&data->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? रिक्त : "";
  यदि(s) {
    वर्ण *url = रिक्त;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    यदि(!u)
      फर्काऊ CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (data->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    यदि(!uc)
      uc = curl_url_set(u, CURLUPART_USER, रिक्त, 0);
    यदि(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, रिक्त, 0);
    यदि(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    यदि(uc || !url)
      फर्काऊ uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    यदि(म_भेद(url, s)) {
      /* they are different */
      curlx_free(data->info.effective_url);
      data->info.effective_url = url;
      *param_charp = url;
    }
    अन्यथा {
      curl_free(url);
      *param_charp = s;
    }
  }
  फर्काऊ CURLE_OK;
}

स्थिर CURLcode getinfo_char(संरचना Curl_easy *data, CURLINFO info,
                             अचर वर्ण **param_charp)
{
  विकल्प(info) {
  पक्ष CURLINFO_EFFECTIVE_URL:
    फर्काऊ effective_url(data, param_charp);
  पक्ष CURLINFO_EFFECTIVE_METHOD: {
    अचर वर्ण *m = CURL_EASY_STR(data, STRING_CUSTOMREQUEST);
    यदि(!m) {
      यदि(data->set.opt_no_body)
        m = "HEAD";
#ifndef CURL_DISABLE_HTTP
      अन्यथा {
        विकल्प(data->state.httpreq) {
        पक्ष HTTPREQ_POST:
        पक्ष HTTPREQ_POST_FORM:
        पक्ष HTTPREQ_POST_MIME:
          m = "POST";
          रोक;
        पक्ष HTTPREQ_PUT:
          m = "PUT";
          रोक;
        स्वभाव: /* this should never happen */
        पक्ष HTTPREQ_GET:
          m = "GET";
          रोक;
        पक्ष HTTPREQ_HEAD:
          m = "HEAD";
          रोक;
        }
      }
#endif
    }
    *param_charp = m;
  }
    रोक;
  पक्ष CURLINFO_CONTENT_TYPE:
    *param_charp = data->info.contenttype;
    रोक;
  पक्ष CURLINFO_PRIVATE:
    *param_charp = (अचर वर्ण *)data->set.private_data;
    रोक;
  पक्ष CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = data->state.most_recent_ftp_entrypath;
    रोक;
  पक्ष CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = data->info.wouldredirect;
    रोक;
  पक्ष CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&data->state.referer);
    रोक;
  पक्ष CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = data->info.primary.remote_ip;
    रोक;
  पक्ष CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = data->info.primary.local_ip;
    रोक;
  पक्ष CURLINFO_RTSP_SESSION_ID:
#ifndef CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(data, STRING_RTSP_SESSION_ID);
#अन्यथा
    *param_charp = रिक्त;
#endif
    रोक;
  पक्ष CURLINFO_SCHEME:
    *param_charp = data->info.conn_scheme;
    रोक;
  पक्ष CURLINFO_CAPATH:
#ifdef CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#अन्यथा
    *param_charp = रिक्त;
#endif
    रोक;
  पक्ष CURLINFO_CAINFO:
#ifdef CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#अन्यथा
    *param_charp = रिक्त;
#endif
    रोक;
  स्वभाव:
    फर्काऊ CURLE_UNKNOWN_OPTION;
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode getinfo_long(संरचना Curl_easy *data, CURLINFO info,
                             दीर्घ *param_longp)
{
  curl_socket_t sockfd;

  सङ्घ {
    अचिन्हित दीर्घ *to_ulong;
    दीर्घ          *to_long;
  } lptr;

#ifdef DEBUGBUILD
  अचर वर्ण *envstr;

  /* use another variable for this to allow different values */
  विकल्प(info) {
  पक्ष CURLINFO_LOCAL_PORT:
    envstr = दो_पर्या("CURL_TIME");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, TIME_T_MAX);
      *param_longp = (दीर्घ)val;
      फर्काऊ CURLE_OK;
    }
    रोक;
  पक्ष CURLINFO_HEADER_SIZE:
  पक्ष CURLINFO_REQUEST_SIZE:
    envstr = दो_पर्या("CURL_DEBUG_SIZE");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, दीर्घ_उच्च);
      *param_longp = (दीर्घ)val;
      फर्काऊ CURLE_OK;
    }
    रोक;
  स्वभाव:
    रोक;
  }
#endif

  विकल्प(info) {
  पक्ष CURLINFO_RESPONSE_CODE:
    *param_longp = data->info.httpcode;
    रोक;
  पक्ष CURLINFO_HTTP_CONNECTCODE:
    *param_longp = data->info.httpproxycode;
    रोक;
  पक्ष CURLINFO_FILETIME:
    यदि(data->info.filetime > दीर्घ_उच्च)
      *param_longp = दीर्घ_उच्च;
#यदि !defined(MSDOS) && !defined(__AMIGA__)
    अन्यथा यदि(data->info.filetime < दीर्घ_न्यून)
      *param_longp = दीर्घ_न्यून;
#endif
    अन्यथा
      *param_longp = (दीर्घ)data->info.filetime;
    रोक;
  पक्ष CURLINFO_HEADER_SIZE:
    *param_longp = (दीर्घ)data->info.header_size;
    रोक;
  पक्ष CURLINFO_REQUEST_SIZE:
    *param_longp = (दीर्घ)data->info.request_size;
    रोक;
  पक्ष CURLINFO_SSL_VERIFYRESULT:
    *param_longp = data->set.ssl.certverifyresult;
    रोक;
  पक्ष CURLINFO_PROXY_SSL_VERIFYRESULT:
#ifndef CURL_DISABLE_PROXY
    *param_longp = data->set.proxy_ssl.certverifyresult;
#अन्यथा
    *param_longp = 0;
#endif
    रोक;
  पक्ष CURLINFO_REDIRECT_गन्ती:
    *param_longp = data->state.followlocation;
    रोक;
  पक्ष CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthavail;
    रोक;
  पक्ष CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthavail;
    रोक;
  पक्ष CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.httpauthpicked;
    रोक;
  पक्ष CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = data->info.proxyauthpicked;
    रोक;
  पक्ष CURLINFO_OS_ERRNO:
    *param_longp = data->state.os_errno;
    रोक;
  पक्ष CURLINFO_अङ्क_CONNECTS:
#यदि SIZEOF_LONG < SIZEOF_CURL_OFF_T
    यदि(data->info.numconnects > दीर्घ_उच्च)
      *param_longp = दीर्घ_उच्च;
    अन्यथा
#endif
      *param_longp = (दीर्घ)data->info.numconnects;
    रोक;
  पक्ष CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(data, रिक्त);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    यदि(sockfd != CURL_SOCKET_BAD)
      *param_longp = (दीर्घ)sockfd;
    अन्यथा
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    रोक;
  पक्ष CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    यदि(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.remote_port;
    अन्यथा
      *param_longp = -1;
    रोक;
  पक्ष CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    यदि(CUR_IP_QUAD_HAS_PORTS(&data->info.primary))
      *param_longp = data->info.primary.local_port;
    अन्यथा
      *param_longp = -1;
    रोक;
  पक्ष CURLINFO_PROXY_ERROR:
    *param_longp = (दीर्घ)data->info.pxcode;
    रोक;
  पक्ष CURLINFO_CONDITION_UNMET:
    यदि(data->info.httpcode == 304)
      *param_longp = 1L;
    अन्यथा
      /* return if the condition prevented the document to get transferred */
      *param_longp = data->info.timecond ? 1L : 0L;
    रोक;
#ifndef CURL_DISABLE_RTSP
  पक्ष CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = data->state.rtsp_next_client_CSeq;
    रोक;
  पक्ष CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = data->state.rtsp_next_server_CSeq;
    रोक;
  पक्ष CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = data->state.rtsp_CSeq_recv;
    रोक;
#अन्यथा
  पक्ष CURLINFO_RTSP_CLIENT_CSEQ:
  पक्ष CURLINFO_RTSP_SERVER_CSEQ:
  पक्ष CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    रोक;
#endif
  पक्ष CURLINFO_HTTP_VERSION:
    विकल्प(data->info.httpversion) {
    पक्ष 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      रोक;
    पक्ष 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      रोक;
    पक्ष 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      रोक;
    पक्ष 30:
      *param_longp = CURL_HTTP_VERSION_3;
      रोक;
    स्वभाव:
      *param_longp = CURL_HTTP_VERSION_NONE;
      रोक;
    }
    रोक;
  पक्ष CURLINFO_PROTOCOL:
    *param_longp = (दीर्घ)data->info.conn_protocol;
    रोक;
  पक्ष CURLINFO_USED_PROXY:
    *param_longp =
#ifdef CURL_DISABLE_PROXY
      0
#अन्यथा
      data->info.used_proxy
#endif
      ;
    रोक;
  स्वभाव:
    फर्काऊ CURLE_UNKNOWN_OPTION;
  }

  फर्काऊ CURLE_OK;
}

#define DOUBLE_SECS(x) ((द्विगुण)(x) / 1000000)

स्थिर CURLcode getinfo_offt(संरचना Curl_easy *data, CURLINFO info,
                             curl_off_t *param_offt)
{
#ifdef DEBUGBUILD
  अचर वर्ण *envstr;
  विकल्प(info) {
  पक्ष CURLINFO_TOTAL_TIME_T:
  पक्ष CURLINFO_NAMELOOKUP_TIME_T:
  पक्ष CURLINFO_CONNECT_TIME_T:
  पक्ष CURLINFO_APPCONNECT_TIME_T:
  पक्ष CURLINFO_PRETRANSFER_TIME_T:
  पक्ष CURLINFO_POSTTRANSFER_TIME_T:
  पक्ष CURLINFO_QUEUE_TIME_T:
  पक्ष CURLINFO_STARTTRANSFER_TIME_T:
  पक्ष CURLINFO_REDIRECT_TIME_T:
  पक्ष CURLINFO_SPEED_DOWNLOAD_T:
  पक्ष CURLINFO_SPEED_UPLOAD_T:
    envstr = दो_पर्या("CURL_TIME");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      फर्काऊ CURLE_OK;
    }
    रोक;
  स्वभाव:
    रोक;
  }
#endif
  विकल्प(info) {
  पक्ष CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)data->info.filetime;
    रोक;
  पक्ष CURLINFO_SIZE_DELIVERED:
    *param_offt = data->progress.deliver;
    रोक;
  पक्ष CURLINFO_SIZE_UPLOAD_T:
    *param_offt = data->progress.ul.cur_size;
    रोक;
  पक्ष CURLINFO_SIZE_DOWNLOAD_T:
    *param_offt = data->progress.dl.cur_size;
    रोक;
  पक्ष CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = data->progress.dl.speed;
    रोक;
  पक्ष CURLINFO_SPEED_UPLOAD_T:
    *param_offt = data->progress.ul.speed;
    रोक;
  पक्ष CURLINFO_CONTENT_LENGTH_DOWNLOAD_T:
    *param_offt = data->progress.dl_size_known ?
      data->progress.dl.total_size : -1;
    रोक;
  पक्ष CURLINFO_CONTENT_LENGTH_UPLOAD_T:
    *param_offt = data->progress.ul_size_known ?
      data->progress.ul.total_size : -1;
    रोक;
   पक्ष CURLINFO_TOTAL_TIME_T:
    *param_offt = data->progress.total.spent_us;
    रोक;
  पक्ष CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = data->progress.total.nslookup_us;
    रोक;
  पक्ष CURLINFO_CONNECT_TIME_T:
    *param_offt = data->progress.total.connect_us;
    रोक;
  पक्ष CURLINFO_APPCONNECT_TIME_T:
    *param_offt = data->progress.total.appconnect_us;
    रोक;
  पक्ष CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = data->progress.total.pretransfer_us;
    रोक;
  पक्ष CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = data->progress.total.posttransfer_us;
    रोक;
  पक्ष CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = data->progress.total.starttransfer_us;
    रोक;
  पक्ष CURLINFO_QUEUE_TIME_T:
    *param_offt = data->progress.total.queued_us;
    रोक;
  पक्ष CURLINFO_REDIRECT_TIME_T:
    *param_offt = data->progress.delta.startredirect_us;
    रोक;
  पक्ष CURLINFO_RETRY_AFTER:
    *param_offt = data->info.retry_after;
    रोक;
  पक्ष CURLINFO_XFER_ID:
    *param_offt = data->id;
    रोक;
  पक्ष CURLINFO_CONN_ID:
    *param_offt = data->state.last_conn_id;
    रोक;
  पक्ष CURLINFO_EARLYDATA_SENT_T:
    *param_offt = data->progress.earlydata_sent;
    रोक;
  स्वभाव:
    फर्काऊ CURLE_UNKNOWN_OPTION;
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode getinfo_double(संरचना Curl_easy *data, CURLINFO info,
                               द्विगुण *param_doublep)
{
#ifdef DEBUGBUILD
  अचर वर्ण *envstr;

  विकल्प(info) {
  पक्ष CURLINFO_TOTAL_TIME:
  पक्ष CURLINFO_NAMELOOKUP_TIME:
  पक्ष CURLINFO_CONNECT_TIME:
  पक्ष CURLINFO_APPCONNECT_TIME:
  पक्ष CURLINFO_PRETRANSFER_TIME:
  पक्ष CURLINFO_STARTTRANSFER_TIME:
  पक्ष CURLINFO_REDIRECT_TIME:
  पक्ष CURLINFO_SPEED_DOWNLOAD:
  पक्ष CURLINFO_SPEED_UPLOAD:
    envstr = दो_पर्या("CURL_TIME");
    यदि(envstr) {
      curl_off_t val;
      curlx_str_सङ्ख्या(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (द्विगुण)val;
      फर्काऊ CURLE_OK;
    }
    रोक;
  स्वभाव:
    रोक;
  }
#endif
  विकल्प(info) {
  पक्ष CURLINFO_TOTAL_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.spent_us);
    रोक;
  पक्ष CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.nslookup_us);
    रोक;
  पक्ष CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.connect_us);
    रोक;
  पक्ष CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.appconnect_us);
    रोक;
  पक्ष CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.pretransfer_us);
    रोक;
  पक्ष CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.total.starttransfer_us);
    रोक;
  पक्ष CURLINFO_SIZE_UPLOAD:
    *param_doublep = (द्विगुण)data->progress.ul.cur_size;
    रोक;
  पक्ष CURLINFO_SIZE_DOWNLOAD:
    *param_doublep = (द्विगुण)data->progress.dl.cur_size;
    रोक;
  पक्ष CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (द्विगुण)data->progress.dl.speed;
    रोक;
  पक्ष CURLINFO_SPEED_UPLOAD:
    *param_doublep = (द्विगुण)data->progress.ul.speed;
    रोक;
  पक्ष CURLINFO_CONTENT_LENGTH_DOWNLOAD:
    *param_doublep = data->progress.dl_size_known ?
      (द्विगुण)data->progress.dl.total_size : -1;
    रोक;
  पक्ष CURLINFO_CONTENT_LENGTH_UPLOAD:
    *param_doublep = data->progress.ul_size_known ?
      (द्विगुण)data->progress.ul.total_size : -1;
    रोक;
  पक्ष CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(data->progress.delta.startredirect_us);
    रोक;

  स्वभाव:
    फर्काऊ CURLE_UNKNOWN_OPTION;
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode getinfo_slist(संरचना Curl_easy *data, CURLINFO info,
                              संरचना curl_slist **param_slistp)
{
  सङ्घ {
    संरचना curl_certinfo *to_certinfo;
    संरचना curl_slist    *to_slist;
  } ptr;

  विकल्प(info) {
  पक्ष CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(data);
    रोक;
  पक्ष CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(data);
    रोक;
  पक्ष CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &data->info.certs;
    *param_slistp = ptr.to_slist;
    रोक;
  पक्ष CURLINFO_TLS_SESSION:
  पक्ष CURLINFO_TLS_SSL_PTR: {
    पूर्णाङ्क query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    संरचना curl_tlssessioninfo **tsip = (संरचना curl_tlssessioninfo **)
      param_slistp;
    संरचना curl_tlssessioninfo *tsi = &data->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    यदि(!Curl_conn_get_ssl_info(data, data->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = रिक्त;
    }
  }
    रोक;
  स्वभाव:
    फर्काऊ CURLE_UNKNOWN_OPTION;
  }

  फर्काऊ CURLE_OK;
}

स्थिर CURLcode getinfo_socket(संरचना Curl_easy *data, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  विकल्प(info) {
  पक्ष CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(data, रिक्त);
    रोक;
  स्वभाव:
    फर्काऊ CURLE_UNKNOWN_OPTION;
  }

  फर्काऊ CURLE_OK;
}

CURLcode Curl_getinfo(संरचना Curl_easy *data, CURLINFO info, ...)
{
  बहु_सूची arg;
  दीर्घ *param_longp = रिक्त;
  द्विगुण *param_doublep = रिक्त;
  curl_off_t *param_offt = रिक्त;
  अचर वर्ण **param_charp = रिक्त;
  संरचना curl_slist **param_slistp = रिक्त;
  curl_socket_t *param_socketp = रिक्त;
  पूर्णाङ्क type;
  CURLcode result = CURLE_BAD_FUNCTION_ARGUMENT;

  यदि(!data)
    फर्काऊ CURLE_BAD_FUNCTION_ARGUMENT;

  बहु_शुरू(arg, info);

  type = CURLINFO_TYPEMASK & (पूर्णाङ्क)info;
  विकल्प(type) {
  पक्ष CURLINFO_STRING:
    param_charp = बहु_तर्क(arg, अचर वर्ण **);
    यदि(param_charp)
      result = getinfo_char(data, info, param_charp);
    रोक;
  पक्ष CURLINFO_LONG:
    param_longp = बहु_तर्क(arg, दीर्घ *);
    यदि(param_longp)
      result = getinfo_long(data, info, param_longp);
    रोक;
  पक्ष CURLINFO_DOUBLE:
    param_doublep = बहु_तर्क(arg, द्विगुण *);
    यदि(param_doublep)
      result = getinfo_double(data, info, param_doublep);
    रोक;
  पक्ष CURLINFO_OFF_T:
    param_offt = बहु_तर्क(arg, curl_off_t *);
    यदि(param_offt)
      result = getinfo_offt(data, info, param_offt);
    रोक;
  पक्ष CURLINFO_SLIST:
    param_slistp = बहु_तर्क(arg, संरचना curl_slist **);
    यदि(param_slistp)
      result = getinfo_slist(data, info, param_slistp);
    रोक;
  पक्ष CURLINFO_SOCKET:
    param_socketp = बहु_तर्क(arg, curl_socket_t *);
    यदि(param_socketp)
      result = getinfo_socket(data, info, param_socketp);
    रोक;
  स्वभाव:
    result = CURLE_UNKNOWN_OPTION;
    रोक;
  }

  बहु_पूर्ण(arg);

  फर्काऊ result;
}
