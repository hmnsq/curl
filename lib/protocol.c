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

#ଅନ୍ତର୍ଭାଵ "protocol.h"
#ଅନ୍ତର୍ଭାଵ "strcase.h"

#ଅନ୍ତର୍ଭାଵ "dict.h"
#ଅନ୍ତର୍ଭାଵ "file.h"
#ଅନ୍ତର୍ଭାଵ "ftp.h"
#ଅନ୍ତର୍ଭାଵ "gopher.h"
#ଅନ୍ତର୍ଭାଵ "http.h"
#ଅନ୍ତର୍ଭାଵ "imap.h"
#ଅନ୍ତର୍ଭାଵ "curl_ldap.h"
#ଅନ୍ତର୍ଭାଵ "mqtt.h"
#ଅନ୍ତର୍ଭାଵ "pop3.h"
#ଅନ୍ତର୍ଭାଵ "rtsp.h"
#ଅନ୍ତର୍ଭାଵ "smtp.h"
#ଅନ୍ତର୍ଭାଵ "telnet.h"
#ଅନ୍ତର୍ଭାଵ "tftp.h"
#ଅନ୍ତର୍ଭାଵ "ws.h"
#ଅନ୍ତର୍ଭାଵ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTP,                        /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ଯଦି defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ଯଦି defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ଯଦି defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ଯଦି defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ଯଦି defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ଯଦି defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ଯଦି defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  ଖାଲି,
#ଅନ୍ୟଥା
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  ଖାଲି,
#ଅନ୍ୟଥା
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ଯଦି defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ଯଦି defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

ଅଚଳ ସଂରଚନା Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ଯଦି defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#ଅନ୍ୟଥା
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_scheme *two_letter_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଯଦି((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    ଫେରନ୍ତୁ &Curl_scheme_ws;
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_scheme *three_letter_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଅକ୍ଷର s0 = Curl_raw_tolower(scheme[0]);
  ଅକ୍ଷର s1 = Curl_raw_tolower(scheme[1]);
  ଅକ୍ଷର s2 = Curl_raw_tolower(scheme[2]);
  ଯଦି(s0 == 'f') {
    ଯଦି(s1 == 't' && s2 == 'p')
      ଫେରନ୍ତୁ &Curl_scheme_ftp;
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 'w') {
    ଯଦି(s1 == 's' && s2 == 's')
      ଫେରନ୍ତୁ &Curl_scheme_wss;
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 's') {
    ଯଦି(s1 == 'c' && s2 == 'p')
      ଫେରନ୍ତୁ &Curl_scheme_scp;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_scheme *four_letter_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଅକ୍ଷର s0 = Curl_raw_tolower(scheme[0]);
  ଅକ୍ଷର s1 = Curl_raw_tolower(scheme[1]);
  ଅକ୍ଷର s2 = Curl_raw_tolower(scheme[2]);
  ଅକ୍ଷର s3 = Curl_raw_tolower(scheme[3]);
  ଯଦି(s3 == 'p') {
    ଯଦି(s0 == 'h') {
      ଯଦି(s1 == 't' && s2 == 't')
        ଫେରନ୍ତୁ &Curl_scheme_http;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 'i') {
      ଯଦି(s1 == 'm' && s2 == 'a')
        ଫେରନ୍ତୁ &Curl_scheme_imap;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 'l') {
      ଯଦି(s1 == 'd' && s2 == 'a')
        ଫେରନ୍ତୁ &Curl_scheme_ldap;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 'r') {
      ଯଦି(s1 == 't' && s2 == 's')
        ଫେରନ୍ତୁ &Curl_scheme_rtsp;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 't') {
      ଯଦି(s1 == 'f' && s2 == 't')
        ଫେରନ୍ତୁ &Curl_scheme_tftp;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 's') {
      ଯଦି(s1 == 'f' && s2 == 't')
        ଫେରନ୍ତୁ &Curl_scheme_sftp;
      ଯଦି(s1 == 'm' && s2 == 't')
        ଫେରନ୍ତୁ &Curl_scheme_smtp;
    }
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 'f') {
    ଯଦି(s1 == 't' && s2 == 'p' && s3 == 's')
      ଫେରନ୍ତୁ &Curl_scheme_ftps;
    ଯଦି(s1 == 'i' && s2 == 'l' && s3 == 'e')
      ଫେରନ୍ତୁ &Curl_scheme_file;
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 'm') {
    ଯଦି(s1 == 'q' && s2 == 't' && s3 == 't')
      ଫେରନ୍ତୁ &Curl_scheme_mqtt;
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 'p') {
    ଯଦି(s1 == 'o' && s2 == 'p' && s3 == '3')
      ଫେରନ୍ତୁ &Curl_scheme_pop3;
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 'd') {
    ଯଦି(s1 == 'i' && s2 == 'c' && s3 == 't')
      ଫେରନ୍ତୁ &Curl_scheme_dict;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_scheme *five_letter_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଅକ୍ଷର s4 = Curl_raw_tolower(scheme[4]);
  ଯଦି(s4 == 's') {
    ଅକ୍ଷର s0 = Curl_raw_tolower(scheme[0]);
    ଅକ୍ଷର s1 = Curl_raw_tolower(scheme[1]);
    ଅକ୍ଷର s2 = Curl_raw_tolower(scheme[2]);
    ଅକ୍ଷର s3 = Curl_raw_tolower(scheme[3]);
    ଯଦି(s3 == 'p') {
      ବାଛ(s0) {
      ସ୍ଥିତି 'h':
        ଯଦି(s1 == 't' && s2 == 't')
          ଫେରନ୍ତୁ &Curl_scheme_https;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 'l':
        ଯଦି(s1 == 'd' && s2 == 'a')
          ଫେରନ୍ତୁ &Curl_scheme_ldaps;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 'i':
        ଯଦି(s1 == 'm' && s2 == 'a')
          ଫେରନ୍ତୁ &Curl_scheme_imaps;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି 's':
        ଯଦି(s1 == 'm' && s2 == 't')
          ଫେରନ୍ତୁ &Curl_scheme_smtps;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ଭାଙ୍ଗ;
      }
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 'p') {
      ଯଦି(s1 == 'o' && s2 == 'p' && s3 == '3')
        ଫେରନ୍ତୁ &Curl_scheme_pop3s;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 'm') {
      ଯଦି(s1 == 'q' && s2 == 't' && s3 == 't')
        ଫେରନ୍ତୁ &Curl_scheme_mqtts;
    }
    ଅନ୍ୟଥା ଯଦି(s0 == 's') {
      ଯଦି(s1 == 'o' && s2 == 'c' && s3 == 'k')
        ଫେରନ୍ତୁ &Curl_scheme_socks;
    }
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_scheme *six_letter_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଅକ୍ଷର s0 = Curl_raw_tolower(scheme[0]);
  ବାଛ(s0) {
  ସ୍ଥିତି 's':
    ଯଦି(curl_strnequal("ocks4", &scheme[1], 5))
      ଫେରନ୍ତୁ &Curl_scheme_socks4;
    ଯଦି(curl_strnequal("ocks5", &scheme[1], 5))
      ଫେରନ୍ତୁ &Curl_scheme_socks5;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'g':
    ଯଦି(curl_strnequal("opher", &scheme[1], 5))
      ଫେରନ୍ତୁ &Curl_scheme_gopher;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 't':
    ଯଦି(curl_strnequal("elnet", &scheme[1], 5))
      ଫେରନ୍ତୁ &Curl_scheme_telnet;
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

ସ୍ଥିର ଅଚଳ ସଂରଚନା Curl_scheme *seven_letter_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଅକ୍ଷର s0 = Curl_raw_tolower(scheme[0]);
  ଯଦି(s0 == 's') {
    ଯଦି(curl_strnequal("ocks4a", &scheme[1], 6))
      ଫେରନ୍ତୁ &Curl_scheme_socks4a;
    ଯଦି(curl_strnequal("ocks5h", &scheme[1], 6))
      ଫେରନ୍ତୁ &Curl_scheme_socks5h;
  }
  ଅନ୍ୟଥା ଯଦି(s0 == 'g') {
    ଯଦି(curl_strnequal("ophers", &scheme[1], 6))
      ଫେରନ୍ତୁ &Curl_scheme_gophers;
  }
  ଫେରନ୍ତୁ ଖାଲି;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ଅଚଳ ସଂରଚନା Curl_scheme *Curl_getn_scheme(ଅଚଳ ଅକ୍ଷର *scheme, size_t len)
{
  ପ୍ରକାରବ୍ୟାଖ୍ୟା ଅଚଳ ସଂରଚନା Curl_scheme *(*letterfunc)(ଅଚଳ ଅକ୍ଷର *ptr);
  ସ୍ଥିର ଅଚଳ letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ଯଦି(len < 2 || len > 7)
    ଫେରନ୍ତୁ ଖାଲି;

  ଫେରନ୍ତୁ parse[len - 2](scheme);
}

ଅଚଳ ସଂରଚନା Curl_scheme *Curl_get_scheme(ଅଚଳ ଅକ୍ଷର *scheme)
{
  ଫେରନ୍ତୁ Curl_getn_scheme(scheme, strlen(scheme));
}
