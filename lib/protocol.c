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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "protocol.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "strcase.h"

#ਅਨ੍ਤਰ੍ਭਾਵ "dict.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "file.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "gopher.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "http.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "imap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_ldap.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "mqtt.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "pop3.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "rtsp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "smtp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "telnet.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "tftp.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "ws.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_DICT
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_dict,
#ਜੇ_ਅੰਤ
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  ਪੋਰਟ_DICT,                            /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FILE
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_file,
#ਜੇ_ਅੰਤ
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_FTP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_ftp,
#ਜੇ_ਅੰਤ
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  ਪੋਰਟ_FTP,                        /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ਜੇ defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_ftp,
#ਜੇ_ਅੰਤ
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  ਪੋਰਟ_FTPS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_GOPHER
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_gopher,
#ਜੇ_ਅੰਤ
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  ਪੋਰਟ_GOPHER,                          /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ਜੇ defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_gophers,
#ਜੇ_ਅੰਤ
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  ਪੋਰਟ_GOPHER,                          /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_HTTP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_http,
#ਜੇ_ਅੰਤ
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_HTTP,                            /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ਜੇ defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_http,
#ਜੇ_ਅੰਤ
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  ਪੋਰਟ_HTTPS,                           /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_IMAP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_imap,
#ਜੇ_ਅੰਤ
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  ਪੋਰਟ_IMAP,                        /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ਜੇ defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_imap,
#ਜੇ_ਅੰਤ
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_IMAPS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_LDAP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_ldap,
#ਜੇ_ਅੰਤ
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  ਪੋਰਟ_LDAP,                            /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ਜੇ defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_ldap,
#ਜੇ_ਅੰਤ
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  ਪੋਰਟ_LDAPS,                           /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_MQTT
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_mqtt,
#ਜੇ_ਅੰਤ
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  ਪੋਰਟ_MQTT,                          /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ਜੇ defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_mqtts,
#ਜੇ_ਅੰਤ
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  ਪੋਰਟ_MQTTS,                         /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_POP3
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_pop3,
#ਜੇ_ਅੰਤ
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_POP3,                        /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ਜੇ defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_pop3,
#ਜੇ_ਅੰਤ
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_POP3_S,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_RTSP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_rtsp,
#ਜੇ_ਅੰਤ
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  ਪੋਰਟ_RTSP,                            /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_sftp,
#ਜੇ_ਅੰਤ
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_SSH                              /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USE_SSH
  ਨਿਰਾ,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_scp,
#ਜੇ_ਅੰਤ
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_SSH,                             /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_SMTP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_smtp,
#ਜੇ_ਅੰਤ
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_SMTP,                        /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ਜੇ defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_smtp,
#ਜੇ_ਅੰਤ
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  ਪੋਰਟ_SMTPS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ਪੋਰਟ_SOCKS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ਪੋਰਟ_SOCKS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ਪੋਰਟ_SOCKS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ਪੋਰਟ_SOCKS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ਪੋਰਟ_SOCKS,                       /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TELNET
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_telnet,
#ਜੇ_ਅੰਤ
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  ਪੋਰਟ_TELNET,                          /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TFTP
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_tftp,
#ਜੇ_ਅੰਤ
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  ਪੋਰਟ_TFTP,                            /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ਜੇ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_ws,
#ਜੇ_ਅੰਤ
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  ਪੋਰਟ_HTTP                             /* defport */
};

ਅਚਲ ਬਣਤਰ Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ਜੇ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#ਨਹੀਂਤਾਂ
  &Curl_protocol_ws,
#ਜੇ_ਅੰਤ
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  ਪੋਰਟ_HTTPS                            /* defport */
};

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_scheme *two_letter_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਜੇ((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    ਵਾਪਸ &Curl_scheme_ws;
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_scheme *three_letter_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਅੱਖਰ s0 = Curl_raw_tolower(scheme[0]);
  ਅੱਖਰ s1 = Curl_raw_tolower(scheme[1]);
  ਅੱਖਰ s2 = Curl_raw_tolower(scheme[2]);
  ਜੇ(s0 == 'f') {
    ਜੇ(s1 == 't' && s2 == 'p')
      ਵਾਪਸ &Curl_scheme_ftp;
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 'w') {
    ਜੇ(s1 == 's' && s2 == 's')
      ਵਾਪਸ &Curl_scheme_wss;
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 's') {
    ਜੇ(s1 == 'c' && s2 == 'p')
      ਵਾਪਸ &Curl_scheme_scp;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_scheme *four_letter_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਅੱਖਰ s0 = Curl_raw_tolower(scheme[0]);
  ਅੱਖਰ s1 = Curl_raw_tolower(scheme[1]);
  ਅੱਖਰ s2 = Curl_raw_tolower(scheme[2]);
  ਅੱਖਰ s3 = Curl_raw_tolower(scheme[3]);
  ਜੇ(s3 == 'p') {
    ਜੇ(s0 == 'h') {
      ਜੇ(s1 == 't' && s2 == 't')
        ਵਾਪਸ &Curl_scheme_http;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 'i') {
      ਜੇ(s1 == 'm' && s2 == 'a')
        ਵਾਪਸ &Curl_scheme_imap;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 'l') {
      ਜੇ(s1 == 'd' && s2 == 'a')
        ਵਾਪਸ &Curl_scheme_ldap;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 'r') {
      ਜੇ(s1 == 't' && s2 == 's')
        ਵਾਪਸ &Curl_scheme_rtsp;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 't') {
      ਜੇ(s1 == 'f' && s2 == 't')
        ਵਾਪਸ &Curl_scheme_tftp;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 's') {
      ਜੇ(s1 == 'f' && s2 == 't')
        ਵਾਪਸ &Curl_scheme_sftp;
      ਜੇ(s1 == 'm' && s2 == 't')
        ਵਾਪਸ &Curl_scheme_smtp;
    }
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 'f') {
    ਜੇ(s1 == 't' && s2 == 'p' && s3 == 's')
      ਵਾਪਸ &Curl_scheme_ftps;
    ਜੇ(s1 == 'i' && s2 == 'l' && s3 == 'e')
      ਵਾਪਸ &Curl_scheme_file;
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 'm') {
    ਜੇ(s1 == 'q' && s2 == 't' && s3 == 't')
      ਵਾਪਸ &Curl_scheme_mqtt;
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 'p') {
    ਜੇ(s1 == 'o' && s2 == 'p' && s3 == '3')
      ਵਾਪਸ &Curl_scheme_pop3;
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 'd') {
    ਜੇ(s1 == 'i' && s2 == 'c' && s3 == 't')
      ਵਾਪਸ &Curl_scheme_dict;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_scheme *five_letter_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਅੱਖਰ s4 = Curl_raw_tolower(scheme[4]);
  ਜੇ(s4 == 's') {
    ਅੱਖਰ s0 = Curl_raw_tolower(scheme[0]);
    ਅੱਖਰ s1 = Curl_raw_tolower(scheme[1]);
    ਅੱਖਰ s2 = Curl_raw_tolower(scheme[2]);
    ਅੱਖਰ s3 = Curl_raw_tolower(scheme[3]);
    ਜੇ(s3 == 'p') {
      ਚੋਣ(s0) {
      ਹਾਲਤ 'h':
        ਜੇ(s1 == 't' && s2 == 't')
          ਵਾਪਸ &Curl_scheme_https;
        ਰੋਕੋ;
      ਹਾਲਤ 'l':
        ਜੇ(s1 == 'd' && s2 == 'a')
          ਵਾਪਸ &Curl_scheme_ldaps;
        ਰੋਕੋ;
      ਹਾਲਤ 'i':
        ਜੇ(s1 == 'm' && s2 == 'a')
          ਵਾਪਸ &Curl_scheme_imaps;
        ਰੋਕੋ;
      ਹਾਲਤ 's':
        ਜੇ(s1 == 'm' && s2 == 't')
          ਵਾਪਸ &Curl_scheme_smtps;
        ਰੋਕੋ;
      ਮੂਲ:
        ਰੋਕੋ;
      }
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 'p') {
      ਜੇ(s1 == 'o' && s2 == 'p' && s3 == '3')
        ਵਾਪਸ &Curl_scheme_pop3s;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 'm') {
      ਜੇ(s1 == 'q' && s2 == 't' && s3 == 't')
        ਵਾਪਸ &Curl_scheme_mqtts;
    }
    ਨਹੀਂਤਾਂ ਜੇ(s0 == 's') {
      ਜੇ(s1 == 'o' && s2 == 'c' && s3 == 'k')
        ਵਾਪਸ &Curl_scheme_socks;
    }
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_scheme *six_letter_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਅੱਖਰ s0 = Curl_raw_tolower(scheme[0]);
  ਚੋਣ(s0) {
  ਹਾਲਤ 's':
    ਜੇ(curl_strnequal("ocks4", &scheme[1], 5))
      ਵਾਪਸ &Curl_scheme_socks4;
    ਜੇ(curl_strnequal("ocks5", &scheme[1], 5))
      ਵਾਪਸ &Curl_scheme_socks5;
    ਰੋਕੋ;
  ਹਾਲਤ 'g':
    ਜੇ(curl_strnequal("opher", &scheme[1], 5))
      ਵਾਪਸ &Curl_scheme_gopher;
    ਰੋਕੋ;
  ਹਾਲਤ 't':
    ਜੇ(curl_strnequal("elnet", &scheme[1], 5))
      ਵਾਪਸ &Curl_scheme_telnet;
    ਰੋਕੋ;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

ਸਥਿਰ ਅਚਲ ਬਣਤਰ Curl_scheme *seven_letter_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਅੱਖਰ s0 = Curl_raw_tolower(scheme[0]);
  ਜੇ(s0 == 's') {
    ਜੇ(curl_strnequal("ocks4a", &scheme[1], 6))
      ਵਾਪਸ &Curl_scheme_socks4a;
    ਜੇ(curl_strnequal("ocks5h", &scheme[1], 6))
      ਵਾਪਸ &Curl_scheme_socks5h;
  }
  ਨਹੀਂਤਾਂ ਜੇ(s0 == 'g') {
    ਜੇ(curl_strnequal("ophers", &scheme[1], 6))
      ਵਾਪਸ &Curl_scheme_gophers;
  }
  ਵਾਪਸ ਨਿਰਾ;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ਅਚਲ ਬਣਤਰ Curl_scheme *Curl_getn_scheme(ਅਚਲ ਅੱਖਰ *scheme, size_t len)
{
  ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਅਚਲ ਬਣਤਰ Curl_scheme *(*letterfunc)(ਅਚਲ ਅੱਖਰ *ptr);
  ਸਥਿਰ ਅਚਲ letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ਜੇ(len < 2 || len > 7)
    ਵਾਪਸ ਨਿਰਾ;

  ਵਾਪਸ parse[len - 2](scheme);
}

ਅਚਲ ਬਣਤਰ Curl_scheme *Curl_get_scheme(ਅਚਲ ਅੱਖਰ *scheme)
{
  ਵਾਪਸ Curl_getn_scheme(scheme, ਲੜੀ_ਲੰਬਾਈ(scheme));
}
