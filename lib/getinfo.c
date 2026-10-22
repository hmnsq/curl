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
#समावेश "curl_setup.h"

#समावेश "urldata.h"
#समावेश "getinfo.h"
#समावेश "cfilters.h"
#समावेश "vtls/vtls.h"
#समावेश "connect.h" /* Curl_getconnectinfo() */
#समावेश "bufref.h"
#समावेश "url.h" /* Curl_uc_to_curlcode() */
#समावेश "curlx/strparse.h"

/*
 * Initialize statistical and informational data.
 *
 * This function is called in curl_easy_reset, curl_easy_duphandle and at the
 * beginning of a perform session. It must reset the session-info variables,
 * in particular all variables in struct PureInfo.
 */
व्योम Curl_initinfo(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा Progress *pro = &तथ्य->progress;
  काष्ठा PureInfo *info = &तथ्य->info;

  स_रखो(&pro->delta, 0, माप(pro->delta));
  स_रखो(&pro->योग, 0, माप(pro->योग));
  pro->startransfer_added = FALSE;

  info->httpcode = 0;
  info->httpproxycode = 0;
  info->httpversion = 0;
  info->filetime = -1; /* -1 is an illegal time and thus means unknown */
  info->timecond = FALSE;

  info->header_आकार = 0;
  info->request_आकार = 0;
  info->proxyauthavail = 0;
  info->httpauthavail = 0;
  info->proxyauthpicked = 0;
  info->httpauthpicked = 0;
  info->numconnects = 0;

  curlx_safefree(info->contenttype);
  curlx_safefree(info->wouldredirect);
  curlx_safefree(info->effective_url);

  स_रखो(&info->primary, 0, माप(info->primary));
  info->retry_after = 0;

  info->conn_scheme = 0;
  info->conn_protocol = 0;
  info->used_proxy = 0;

#अगर_घोषित USE_SSL
  Curl_ssl_free_certinfo(तथ्य);
#पूर्ण_अगर
}

अटल CURLcode effective_url(काष्ठा Curl_easy *तथ्य,
                              स्थिर अक्षर **param_charp)
{
  स्थिर अक्षर *s = Curl_bufref_ptr(&तथ्य->state.url);
  /* never return the URL unfiltered */
  *param_charp = s ? शून्य : "";
  अगर(s) {
    अक्षर *url = शून्य;
    /* Make a copy of the URL without credentials */
    CURLU *u = curl_url();
    CURLUcode uc;
    अगर(!u)
      वापस CURLE_OUT_OF_MEMORY;

    uc = curl_url_set(u, CURLUPART_URL, s,
                      CURLU_NON_SUPPORT_SCHEME |
                      (तथ्य->set.path_as_is ? CURLU_PATH_AS_IS : 0));
    अगर(!uc)
      uc = curl_url_set(u, CURLUPART_USER, शून्य, 0);
    अगर(!uc)
      uc = curl_url_set(u, CURLUPART_PASSWORD, शून्य, 0);
    अगर(!uc)
      uc = curl_url_get(u, CURLUPART_URL, &url, CURLU_GET_EMPTY);

    curl_url_cleanup(u);

    अगर(uc || !url)
      वापस uc ? Curl_uc_to_curlcode(uc) : CURLE_OUT_OF_MEMORY;
    अगर(म_भेद(url, s)) {
      /* they are different */
      curlx_free(तथ्य->info.effective_url);
      तथ्य->info.effective_url = url;
      *param_charp = url;
    }
    अन्यथा {
      curl_free(url);
      *param_charp = s;
    }
  }
  वापस CURLE_OK;
}

अटल CURLcode getinfo_char(काष्ठा Curl_easy *तथ्य, CURLINFO info,
                             स्थिर अक्षर **param_charp)
{
  चयन(info) {
  हाल CURLINFO_EFFECTIVE_URL:
    वापस effective_url(तथ्य, param_charp);
  हाल CURLINFO_EFFECTIVE_METHOD: {
    स्थिर अक्षर *m = CURL_EASY_STR(तथ्य, STRING_CUSTOMREQUEST);
    अगर(!m) {
      अगर(तथ्य->set.opt_no_body)
        m = "HEAD";
#अगर_अघोषित CURL_DISABLE_HTTP
      अन्यथा {
        चयन(तथ्य->state.httpreq) {
        हाल HTTPREQ_POST:
        हाल HTTPREQ_POST_FORM:
        हाल HTTPREQ_POST_MIME:
          m = "POST";
          अवरोध;
        हाल HTTPREQ_PUT:
          m = "PUT";
          अवरोध;
        शेष: /* this should never happen */
        हाल HTTPREQ_GET:
          m = "GET";
          अवरोध;
        हाल HTTPREQ_शीर्ष:
          m = "HEAD";
          अवरोध;
        }
      }
#पूर्ण_अगर
    }
    *param_charp = m;
  }
    अवरोध;
  हाल CURLINFO_CONTENT_TYPE:
    *param_charp = तथ्य->info.contenttype;
    अवरोध;
  हाल CURLINFO_PRIVATE:
    *param_charp = (स्थिर अक्षर *)तथ्य->set.private_तथ्य;
    अवरोध;
  हाल CURLINFO_FTP_ENTRY_PATH:
    /* Return the entrypath string from the most recent connection.
       This pointer was copied from the connectdata structure by FTP.
       The actual string may be freed by subsequent libcurl calls so
       it must be copied to a safer area before the next libcurl call.
       Callers must never free it themselves. */
    *param_charp = तथ्य->state.most_recent_ftp_entrypath;
    अवरोध;
  हाल CURLINFO_REDIRECT_URL:
    /* Return the URL this request would have been redirected to if that
       option had been enabled! */
    *param_charp = तथ्य->info.wouldredirect;
    अवरोध;
  हाल CURLINFO_REFERER:
    /* Return the referrer header for this request, or NULL if unset */
    *param_charp = Curl_bufref_ptr(&तथ्य->state.referer);
    अवरोध;
  हाल CURLINFO_PRIMARY_IP:
    /* Return the ip address of the most recent (primary) connection */
    *param_charp = तथ्य->info.primary.remote_ip;
    अवरोध;
  हाल CURLINFO_LOCAL_IP:
    /* Return the source/local ip address of the most recent (primary)
       connection */
    *param_charp = तथ्य->info.primary.local_ip;
    अवरोध;
  हाल CURLINFO_RTSP_SESSION_ID:
#अगर_अघोषित CURL_DISABLE_RTSP
    *param_charp = CURL_EASY_STR(तथ्य, STRING_RTSP_SESSION_ID);
#अन्यथा
    *param_charp = शून्य;
#पूर्ण_अगर
    अवरोध;
  हाल CURLINFO_SCHEME:
    *param_charp = तथ्य->info.conn_scheme;
    अवरोध;
  हाल CURLINFO_CAPATH:
#अगर_घोषित CURL_CA_PATH
    *param_charp = CURL_CA_PATH;
#अन्यथा
    *param_charp = शून्य;
#पूर्ण_अगर
    अवरोध;
  हाल CURLINFO_CAINFO:
#अगर_घोषित CURL_CA_BUNDLE
    *param_charp = CURL_CA_BUNDLE;
#अन्यथा
    *param_charp = शून्य;
#पूर्ण_अगर
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }

  वापस CURLE_OK;
}

अटल CURLcode getinfo_long(काष्ठा Curl_easy *तथ्य, CURLINFO info,
                             दीर्घ *param_longp)
{
  curl_socket_t sockfd;

  जोड़ {
    अचिन्हित दीर्घ *to_ulong;
    दीर्घ          *to_long;
  } lptr;

#अगर_घोषित DEBUGBUILD
  स्थिर अक्षर *envstr;

  /* use another variable for this to allow different values */
  चयन(info) {
  हाल CURLINFO_LOCAL_PORT:
    envstr = दो_पर्या("CURL_TIME");
    अगर(envstr) {
      curl_off_t val;
      curlx_str_संख्या(&envstr, &val, TIME_T_MAX);
      *param_longp = (दीर्घ)val;
      वापस CURLE_OK;
    }
    अवरोध;
  हाल CURLINFO_HEADER_आकार:
  हाल CURLINFO_REQUEST_आकार:
    envstr = दो_पर्या("CURL_DEBUG_SIZE");
    अगर(envstr) {
      curl_off_t val;
      curlx_str_संख्या(&envstr, &val, दीर्घ_उच्च);
      *param_longp = (दीर्घ)val;
      वापस CURLE_OK;
    }
    अवरोध;
  शेष:
    अवरोध;
  }
#पूर्ण_अगर

  चयन(info) {
  हाल CURLINFO_RESPONSE_CODE:
    *param_longp = तथ्य->info.httpcode;
    अवरोध;
  हाल CURLINFO_HTTP_CONNECTCODE:
    *param_longp = तथ्य->info.httpproxycode;
    अवरोध;
  हाल CURLINFO_FILETIME:
    अगर(तथ्य->info.filetime > दीर्घ_उच्च)
      *param_longp = दीर्घ_उच्च;
#अगर !defined(MSDOS) && !defined(__AMIGA__)
    अन्यथा अगर(तथ्य->info.filetime < दीर्घ_न्यून)
      *param_longp = दीर्घ_न्यून;
#पूर्ण_अगर
    अन्यथा
      *param_longp = (दीर्घ)तथ्य->info.filetime;
    अवरोध;
  हाल CURLINFO_HEADER_आकार:
    *param_longp = (दीर्घ)तथ्य->info.header_आकार;
    अवरोध;
  हाल CURLINFO_REQUEST_आकार:
    *param_longp = (दीर्घ)तथ्य->info.request_आकार;
    अवरोध;
  हाल CURLINFO_SSL_VERIFYRESULT:
    *param_longp = तथ्य->set.ssl.certverifyresult;
    अवरोध;
  हाल CURLINFO_PROXY_SSL_VERIFYRESULT:
#अगर_अघोषित CURL_DISABLE_PROXY
    *param_longp = तथ्य->set.proxy_ssl.certverifyresult;
#अन्यथा
    *param_longp = 0;
#पूर्ण_अगर
    अवरोध;
  हाल CURLINFO_REDIRECT_गिनती:
    *param_longp = तथ्य->state.followlocation;
    अवरोध;
  हाल CURLINFO_HTTPAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = तथ्य->info.httpauthavail;
    अवरोध;
  हाल CURLINFO_PROXYAUTH_AVAIL:
    lptr.to_long = param_longp;
    *lptr.to_ulong = तथ्य->info.proxyauthavail;
    अवरोध;
  हाल CURLINFO_HTTPAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = तथ्य->info.httpauthpicked;
    अवरोध;
  हाल CURLINFO_PROXYAUTH_USED:
    lptr.to_long = param_longp;
    *lptr.to_ulong = तथ्य->info.proxyauthpicked;
    अवरोध;
  हाल CURLINFO_OS_ERRNO:
    *param_longp = तथ्य->state.os_errno;
    अवरोध;
  हाल CURLINFO_अंक_CONNECTS:
#अगर SIZEOF_LONG < SIZEOF_CURL_OFF_T
    अगर(तथ्य->info.numconnects > दीर्घ_उच्च)
      *param_longp = दीर्घ_उच्च;
    अन्यथा
#पूर्ण_अगर
      *param_longp = (दीर्घ)तथ्य->info.numconnects;
    अवरोध;
  हाल CURLINFO_LASTSOCKET:
    sockfd = Curl_getconnectinfo(तथ्य, शून्य);

    /* note: this is not a good conversion for systems with 64-bit sockets and
       32-bit longs */
    अगर(sockfd != CURL_SOCKET_BAD)
      *param_longp = (दीर्घ)sockfd;
    अन्यथा
      /* this interface is documented to return -1 in case of badness, which
         may not be the same as the CURL_SOCKET_BAD value */
      *param_longp = -1;
    अवरोध;
  हाल CURLINFO_PRIMARY_PORT:
    /* Return the (remote) port of the most recent (primary) connection */
    अगर(CUR_IP_QUAD_HAS_PORTS(&तथ्य->info.primary))
      *param_longp = तथ्य->info.primary.remote_port;
    अन्यथा
      *param_longp = -1;
    अवरोध;
  हाल CURLINFO_LOCAL_PORT:
    /* Return the local port of the most recent (primary) connection */
    अगर(CUR_IP_QUAD_HAS_PORTS(&तथ्य->info.primary))
      *param_longp = तथ्य->info.primary.local_port;
    अन्यथा
      *param_longp = -1;
    अवरोध;
  हाल CURLINFO_PROXY_ERROR:
    *param_longp = (दीर्घ)तथ्य->info.pxcode;
    अवरोध;
  हाल CURLINFO_CONDITION_UNMET:
    अगर(तथ्य->info.httpcode == 304)
      *param_longp = 1L;
    अन्यथा
      /* return if the condition prevented the document to get transferred */
      *param_longp = तथ्य->info.timecond ? 1L : 0L;
    अवरोध;
#अगर_अघोषित CURL_DISABLE_RTSP
  हाल CURLINFO_RTSP_CLIENT_CSEQ:
    *param_longp = तथ्य->state.rtsp_अग्र_client_CSeq;
    अवरोध;
  हाल CURLINFO_RTSP_SERVER_CSEQ:
    *param_longp = तथ्य->state.rtsp_अग्र_server_CSeq;
    अवरोध;
  हाल CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = तथ्य->state.rtsp_CSeq_recv;
    अवरोध;
#अन्यथा
  हाल CURLINFO_RTSP_CLIENT_CSEQ:
  हाल CURLINFO_RTSP_SERVER_CSEQ:
  हाल CURLINFO_RTSP_CSEQ_RECV:
    *param_longp = 0;
    अवरोध;
#पूर्ण_अगर
  हाल CURLINFO_HTTP_VERSION:
    चयन(तथ्य->info.httpversion) {
    हाल 10:
      *param_longp = CURL_HTTP_VERSION_1_0;
      अवरोध;
    हाल 11:
      *param_longp = CURL_HTTP_VERSION_1_1;
      अवरोध;
    हाल 20:
      *param_longp = CURL_HTTP_VERSION_2_0;
      अवरोध;
    हाल 30:
      *param_longp = CURL_HTTP_VERSION_3;
      अवरोध;
    शेष:
      *param_longp = CURL_HTTP_VERSION_NONE;
      अवरोध;
    }
    अवरोध;
  हाल CURLINFO_PROTOCOL:
    *param_longp = (दीर्घ)तथ्य->info.conn_protocol;
    अवरोध;
  हाल CURLINFO_USED_PROXY:
    *param_longp =
#अगर_घोषित CURL_DISABLE_PROXY
      0
#अन्यथा
      तथ्य->info.used_proxy
#पूर्ण_अगर
      ;
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }

  वापस CURLE_OK;
}

#घोषणा DOUBLE_SECS(x) ((द्विगुन)(x) / 1000000)

अटल CURLcode getinfo_offt(काष्ठा Curl_easy *तथ्य, CURLINFO info,
                             curl_off_t *param_offt)
{
#अगर_घोषित DEBUGBUILD
  स्थिर अक्षर *envstr;
  चयन(info) {
  हाल CURLINFO_योग_TIME_T:
  हाल CURLINFO_NAMELOOKUP_TIME_T:
  हाल CURLINFO_CONNECT_TIME_T:
  हाल CURLINFO_APPCONNECT_TIME_T:
  हाल CURLINFO_PRETRANSFER_TIME_T:
  हाल CURLINFO_POSTTRANSFER_TIME_T:
  हाल CURLINFO_QUEUE_TIME_T:
  हाल CURLINFO_STARTTRANSFER_TIME_T:
  हाल CURLINFO_REDIRECT_TIME_T:
  हाल CURLINFO_SPEED_DOWNLOAD_T:
  हाल CURLINFO_SPEED_UPLOAD_T:
    envstr = दो_पर्या("CURL_TIME");
    अगर(envstr) {
      curl_off_t val;
      curlx_str_संख्या(&envstr, &val, CURL_OFF_T_MAX);
      *param_offt = (curl_off_t)val;
      वापस CURLE_OK;
    }
    अवरोध;
  शेष:
    अवरोध;
  }
#पूर्ण_अगर
  चयन(info) {
  हाल CURLINFO_FILETIME_T:
    *param_offt = (curl_off_t)तथ्य->info.filetime;
    अवरोध;
  हाल CURLINFO_आकार_DELIVERED:
    *param_offt = तथ्य->progress.deliver;
    अवरोध;
  हाल CURLINFO_आकार_UPLOAD_T:
    *param_offt = तथ्य->progress.ul.cur_आकार;
    अवरोध;
  हाल CURLINFO_आकार_DOWNLOAD_T:
    *param_offt = तथ्य->progress.dl.cur_आकार;
    अवरोध;
  हाल CURLINFO_SPEED_DOWNLOAD_T:
    *param_offt = तथ्य->progress.dl.speed;
    अवरोध;
  हाल CURLINFO_SPEED_UPLOAD_T:
    *param_offt = तथ्य->progress.ul.speed;
    अवरोध;
  हाल CURLINFO_CONTENT_लंबाई_DOWNLOAD_T:
    *param_offt = तथ्य->progress.dl_आकार_known ?
      तथ्य->progress.dl.योग_आकार : -1;
    अवरोध;
  हाल CURLINFO_CONTENT_लंबाई_UPLOAD_T:
    *param_offt = तथ्य->progress.ul_आकार_known ?
      तथ्य->progress.ul.योग_आकार : -1;
    अवरोध;
   हाल CURLINFO_योग_TIME_T:
    *param_offt = तथ्य->progress.योग.spent_us;
    अवरोध;
  हाल CURLINFO_NAMELOOKUP_TIME_T:
    *param_offt = तथ्य->progress.योग.nslookup_us;
    अवरोध;
  हाल CURLINFO_CONNECT_TIME_T:
    *param_offt = तथ्य->progress.योग.connect_us;
    अवरोध;
  हाल CURLINFO_APPCONNECT_TIME_T:
    *param_offt = तथ्य->progress.योग.appconnect_us;
    अवरोध;
  हाल CURLINFO_PRETRANSFER_TIME_T:
    *param_offt = तथ्य->progress.योग.pretransfer_us;
    अवरोध;
  हाल CURLINFO_POSTTRANSFER_TIME_T:
    *param_offt = तथ्य->progress.योग.posttransfer_us;
    अवरोध;
  हाल CURLINFO_STARTTRANSFER_TIME_T:
    *param_offt = तथ्य->progress.योग.starttransfer_us;
    अवरोध;
  हाल CURLINFO_QUEUE_TIME_T:
    *param_offt = तथ्य->progress.योग.queued_us;
    अवरोध;
  हाल CURLINFO_REDIRECT_TIME_T:
    *param_offt = तथ्य->progress.delta.startredirect_us;
    अवरोध;
  हाल CURLINFO_RETRY_AFTER:
    *param_offt = तथ्य->info.retry_after;
    अवरोध;
  हाल CURLINFO_XFER_ID:
    *param_offt = तथ्य->id;
    अवरोध;
  हाल CURLINFO_CONN_ID:
    *param_offt = तथ्य->state.last_conn_id;
    अवरोध;
  हाल CURLINFO_EARLYDATA_SENT_T:
    *param_offt = तथ्य->progress.earlydata_sent;
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }

  वापस CURLE_OK;
}

अटल CURLcode getinfo_double(काष्ठा Curl_easy *तथ्य, CURLINFO info,
                               द्विगुन *param_doublep)
{
#अगर_घोषित DEBUGBUILD
  स्थिर अक्षर *envstr;

  चयन(info) {
  हाल CURLINFO_योग_TIME:
  हाल CURLINFO_NAMELOOKUP_TIME:
  हाल CURLINFO_CONNECT_TIME:
  हाल CURLINFO_APPCONNECT_TIME:
  हाल CURLINFO_PRETRANSFER_TIME:
  हाल CURLINFO_STARTTRANSFER_TIME:
  हाल CURLINFO_REDIRECT_TIME:
  हाल CURLINFO_SPEED_DOWNLOAD:
  हाल CURLINFO_SPEED_UPLOAD:
    envstr = दो_पर्या("CURL_TIME");
    अगर(envstr) {
      curl_off_t val;
      curlx_str_संख्या(&envstr, &val, CURL_OFF_T_MAX);
      *param_doublep = (द्विगुन)val;
      वापस CURLE_OK;
    }
    अवरोध;
  शेष:
    अवरोध;
  }
#पूर्ण_अगर
  चयन(info) {
  हाल CURLINFO_योग_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.योग.spent_us);
    अवरोध;
  हाल CURLINFO_NAMELOOKUP_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.योग.nslookup_us);
    अवरोध;
  हाल CURLINFO_CONNECT_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.योग.connect_us);
    अवरोध;
  हाल CURLINFO_APPCONNECT_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.योग.appconnect_us);
    अवरोध;
  हाल CURLINFO_PRETRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.योग.pretransfer_us);
    अवरोध;
  हाल CURLINFO_STARTTRANSFER_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.योग.starttransfer_us);
    अवरोध;
  हाल CURLINFO_आकार_UPLOAD:
    *param_doublep = (द्विगुन)तथ्य->progress.ul.cur_आकार;
    अवरोध;
  हाल CURLINFO_आकार_DOWNLOAD:
    *param_doublep = (द्विगुन)तथ्य->progress.dl.cur_आकार;
    अवरोध;
  हाल CURLINFO_SPEED_DOWNLOAD:
    *param_doublep = (द्विगुन)तथ्य->progress.dl.speed;
    अवरोध;
  हाल CURLINFO_SPEED_UPLOAD:
    *param_doublep = (द्विगुन)तथ्य->progress.ul.speed;
    अवरोध;
  हाल CURLINFO_CONTENT_लंबाई_DOWNLOAD:
    *param_doublep = तथ्य->progress.dl_आकार_known ?
      (द्विगुन)तथ्य->progress.dl.योग_आकार : -1;
    अवरोध;
  हाल CURLINFO_CONTENT_लंबाई_UPLOAD:
    *param_doublep = तथ्य->progress.ul_आकार_known ?
      (द्विगुन)तथ्य->progress.ul.योग_आकार : -1;
    अवरोध;
  हाल CURLINFO_REDIRECT_TIME:
    *param_doublep = DOUBLE_SECS(तथ्य->progress.delta.startredirect_us);
    अवरोध;

  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }

  वापस CURLE_OK;
}

अटल CURLcode getinfo_slist(काष्ठा Curl_easy *तथ्य, CURLINFO info,
                              काष्ठा curl_slist **param_slistp)
{
  जोड़ {
    काष्ठा curl_certinfo *to_certinfo;
    काष्ठा curl_slist    *to_slist;
  } ptr;

  चयन(info) {
  हाल CURLINFO_SSL_ENGINES:
    *param_slistp = Curl_ssl_engines_list(तथ्य);
    अवरोध;
  हाल CURLINFO_COOKIELIST:
    *param_slistp = Curl_cookie_list(तथ्य);
    अवरोध;
  हाल CURLINFO_CERTINFO:
    /* Return the a pointer to the certinfo struct. Not really an slist
       pointer but we can pretend it is here */
    ptr.to_certinfo = &तथ्य->info.certs;
    *param_slistp = ptr.to_slist;
    अवरोध;
  हाल CURLINFO_TLS_SESSION:
  हाल CURLINFO_TLS_SSL_PTR: {
    पूर्णांक query = (info == CURLINFO_TLS_SSL_PTR) ?
      CF_QUERY_SSL_INFO : CF_QUERY_SSL_CTX_INFO;
    काष्ठा curl_tlssessioninfo **tsip = (काष्ठा curl_tlssessioninfo **)
      param_slistp;
    काष्ठा curl_tlssessioninfo *tsi = &तथ्य->tsi;

    /* we are exposing a pointer to internal memory with unknown
     * lifetime here. */
    *tsip = tsi;
    अगर(!Curl_conn_get_ssl_info(तथ्य, तथ्य->conn, FIRSTSOCKET, query, tsi)) {
      tsi->backend = Curl_ssl_backend();
      tsi->internals = शून्य;
    }
  }
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }

  वापस CURLE_OK;
}

अटल CURLcode getinfo_socket(काष्ठा Curl_easy *तथ्य, CURLINFO info,
                               curl_socket_t *param_socketp)
{
  चयन(info) {
  हाल CURLINFO_ACTIVESOCKET:
    *param_socketp = Curl_getconnectinfo(तथ्य, शून्य);
    अवरोध;
  शेष:
    वापस CURLE_UNKNOWN_OPTION;
  }

  वापस CURLE_OK;
}

CURLcode Curl_getinfo(काष्ठा Curl_easy *तथ्य, CURLINFO info, ...)
{
  बहु_सूची arg;
  दीर्घ *param_longp = शून्य;
  द्विगुन *param_doublep = शून्य;
  curl_off_t *param_offt = शून्य;
  स्थिर अक्षर **param_charp = शून्य;
  काष्ठा curl_slist **param_slistp = शून्य;
  curl_socket_t *param_socketp = शून्य;
  पूर्णांक type;
  CURLcode परिणाम = CURLE_BAD_FUNCTION_ARGUMENT;

  अगर(!तथ्य)
    वापस CURLE_BAD_FUNCTION_ARGUMENT;

  बहु_शुरू(arg, info);

  type = CURLINFO_TYPEMASK & (पूर्णांक)info;
  चयन(type) {
  हाल CURLINFO_STRING:
    param_charp = बहु_तर्क(arg, स्थिर अक्षर **);
    अगर(param_charp)
      परिणाम = getinfo_char(तथ्य, info, param_charp);
    अवरोध;
  हाल CURLINFO_LONG:
    param_longp = बहु_तर्क(arg, दीर्घ *);
    अगर(param_longp)
      परिणाम = getinfo_long(तथ्य, info, param_longp);
    अवरोध;
  हाल CURLINFO_DOUBLE:
    param_doublep = बहु_तर्क(arg, द्विगुन *);
    अगर(param_doublep)
      परिणाम = getinfo_double(तथ्य, info, param_doublep);
    अवरोध;
  हाल CURLINFO_OFF_T:
    param_offt = बहु_तर्क(arg, curl_off_t *);
    अगर(param_offt)
      परिणाम = getinfo_offt(तथ्य, info, param_offt);
    अवरोध;
  हाल CURLINFO_SLIST:
    param_slistp = बहु_तर्क(arg, काष्ठा curl_slist **);
    अगर(param_slistp)
      परिणाम = getinfo_slist(तथ्य, info, param_slistp);
    अवरोध;
  हाल CURLINFO_SOCKET:
    param_socketp = बहु_तर्क(arg, curl_socket_t *);
    अगर(param_socketp)
      परिणाम = getinfo_socket(तथ्य, info, param_socketp);
    अवरोध;
  शेष:
    परिणाम = CURLE_UNKNOWN_OPTION;
    अवरोध;
  }

  बहु_पूर्ण(arg);

  वापस परिणाम;
}
