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
#include "curl_setup.h"

#include "protocol.h"
#include "strcase.h"

#include "dict.h"
#include "file.h"
#include "ftp.h"
#include "gopher.h"
#include "http.h"
#include "imap.h"
#include "curl_ldap.h"
#include "mqtt.h"
#include "pop3.h"
#include "rtsp.h"
#include "smtp.h"
#include "telnet.h"
#include "tftp.h"
#include "ws.h"
#include "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
const sa_nracanaa Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#ᬮᬶᬬ
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

const sa_nracanaa Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  NULL,
#ᬮᬶᬬ
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  NULL,
#ᬮᬶᬬ
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

const sa_nracanaa Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ᬬᬾᬦ᭄ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#ᬮᬶᬬ
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

static const sa_nracanaa Curl_scheme *two_letter_scheme(const var_na *scheme)
{
  ᬬᬾᬦ᭄((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    ᬯᬮᬶ &Curl_scheme_ws;
  ᬯᬮᬶ NULL;
}

static const sa_nracanaa Curl_scheme *three_letter_scheme(const var_na *scheme)
{
  var_na s0 = Curl_raw_tolower(scheme[0]);
  var_na s1 = Curl_raw_tolower(scheme[1]);
  var_na s2 = Curl_raw_tolower(scheme[2]);
  ᬬᬾᬦ᭄(s0 == 'f') {
    ᬬᬾᬦ᭄(s1 == 't' && s2 == 'p')
      ᬯᬮᬶ &Curl_scheme_ftp;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'w') {
    ᬬᬾᬦ᭄(s1 == 's' && s2 == 's')
      ᬯᬮᬶ &Curl_scheme_wss;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 's') {
    ᬬᬾᬦ᭄(s1 == 'c' && s2 == 'p')
      ᬯᬮᬶ &Curl_scheme_scp;
  }
  ᬯᬮᬶ NULL;
}

static const sa_nracanaa Curl_scheme *four_letter_scheme(const var_na *scheme)
{
  var_na s0 = Curl_raw_tolower(scheme[0]);
  var_na s1 = Curl_raw_tolower(scheme[1]);
  var_na s2 = Curl_raw_tolower(scheme[2]);
  var_na s3 = Curl_raw_tolower(scheme[3]);
  ᬬᬾᬦ᭄(s3 == 'p') {
    ᬬᬾᬦ᭄(s0 == 'h') {
      ᬬᬾᬦ᭄(s1 == 't' && s2 == 't')
        ᬯᬮᬶ &Curl_scheme_http;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'i') {
      ᬬᬾᬦ᭄(s1 == 'm' && s2 == 'a')
        ᬯᬮᬶ &Curl_scheme_imap;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'l') {
      ᬬᬾᬦ᭄(s1 == 'd' && s2 == 'a')
        ᬯᬮᬶ &Curl_scheme_ldap;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'r') {
      ᬬᬾᬦ᭄(s1 == 't' && s2 == 's')
        ᬯᬮᬶ &Curl_scheme_rtsp;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 't') {
      ᬬᬾᬦ᭄(s1 == 'f' && s2 == 't')
        ᬯᬮᬶ &Curl_scheme_tftp;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 's') {
      ᬬᬾᬦ᭄(s1 == 'f' && s2 == 't')
        ᬯᬮᬶ &Curl_scheme_sftp;
      ᬬᬾᬦ᭄(s1 == 'm' && s2 == 't')
        ᬯᬮᬶ &Curl_scheme_smtp;
    }
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'f') {
    ᬬᬾᬦ᭄(s1 == 't' && s2 == 'p' && s3 == 's')
      ᬯᬮᬶ &Curl_scheme_ftps;
    ᬬᬾᬦ᭄(s1 == 'i' && s2 == 'l' && s3 == 'e')
      ᬯᬮᬶ &Curl_scheme_file;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'm') {
    ᬬᬾᬦ᭄(s1 == 'q' && s2 == 't' && s3 == 't')
      ᬯᬮᬶ &Curl_scheme_mqtt;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'p') {
    ᬬᬾᬦ᭄(s1 == 'o' && s2 == 'p' && s3 == '3')
      ᬯᬮᬶ &Curl_scheme_pop3;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'd') {
    ᬬᬾᬦ᭄(s1 == 'i' && s2 == 'c' && s3 == 't')
      ᬯᬮᬶ &Curl_scheme_dict;
  }
  ᬯᬮᬶ NULL;
}

static const sa_nracanaa Curl_scheme *five_letter_scheme(const var_na *scheme)
{
  var_na s4 = Curl_raw_tolower(scheme[4]);
  ᬬᬾᬦ᭄(s4 == 's') {
    var_na s0 = Curl_raw_tolower(scheme[0]);
    var_na s1 = Curl_raw_tolower(scheme[1]);
    var_na s2 = Curl_raw_tolower(scheme[2]);
    var_na s3 = Curl_raw_tolower(scheme[3]);
    ᬬᬾᬦ᭄(s3 == 'p') {
      switch(s0) {
      pak_sa 'h':
        ᬬᬾᬦ᭄(s1 == 't' && s2 == 't')
          ᬯᬮᬶ &Curl_scheme_https;
        bha_nga;
      pak_sa 'l':
        ᬬᬾᬦ᭄(s1 == 'd' && s2 == 'a')
          ᬯᬮᬶ &Curl_scheme_ldaps;
        bha_nga;
      pak_sa 'i':
        ᬬᬾᬦ᭄(s1 == 'm' && s2 == 'a')
          ᬯᬮᬶ &Curl_scheme_imaps;
        bha_nga;
      pak_sa 's':
        ᬬᬾᬦ᭄(s1 == 'm' && s2 == 't')
          ᬯᬮᬶ &Curl_scheme_smtps;
        bha_nga;
      default:
        bha_nga;
      }
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'p') {
      ᬬᬾᬦ᭄(s1 == 'o' && s2 == 'p' && s3 == '3')
        ᬯᬮᬶ &Curl_scheme_pop3s;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'm') {
      ᬬᬾᬦ᭄(s1 == 'q' && s2 == 't' && s3 == 't')
        ᬯᬮᬶ &Curl_scheme_mqtts;
    }
    ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 's') {
      ᬬᬾᬦ᭄(s1 == 'o' && s2 == 'c' && s3 == 'k')
        ᬯᬮᬶ &Curl_scheme_socks;
    }
  }
  ᬯᬮᬶ NULL;
}

static const sa_nracanaa Curl_scheme *six_letter_scheme(const var_na *scheme)
{
  var_na s0 = Curl_raw_tolower(scheme[0]);
  switch(s0) {
  pak_sa 's':
    ᬬᬾᬦ᭄(curl_strnequal("ocks4", &scheme[1], 5))
      ᬯᬮᬶ &Curl_scheme_socks4;
    ᬬᬾᬦ᭄(curl_strnequal("ocks5", &scheme[1], 5))
      ᬯᬮᬶ &Curl_scheme_socks5;
    bha_nga;
  pak_sa 'g':
    ᬬᬾᬦ᭄(curl_strnequal("opher", &scheme[1], 5))
      ᬯᬮᬶ &Curl_scheme_gopher;
    bha_nga;
  pak_sa 't':
    ᬬᬾᬦ᭄(curl_strnequal("elnet", &scheme[1], 5))
      ᬯᬮᬶ &Curl_scheme_telnet;
    bha_nga;
  }
  ᬯᬮᬶ NULL;
}

static const sa_nracanaa Curl_scheme *seven_letter_scheme(const var_na *scheme)
{
  var_na s0 = Curl_raw_tolower(scheme[0]);
  ᬬᬾᬦ᭄(s0 == 's') {
    ᬬᬾᬦ᭄(curl_strnequal("ocks4a", &scheme[1], 6))
      ᬯᬮᬶ &Curl_scheme_socks4a;
    ᬬᬾᬦ᭄(curl_strnequal("ocks5h", &scheme[1], 6))
      ᬯᬮᬶ &Curl_scheme_socks5h;
  }
  ᬮᬶᬬ ᬬᬾᬦ᭄(s0 == 'g') {
    ᬬᬾᬦ᭄(curl_strnequal("ophers", &scheme[1], 6))
      ᬯᬮᬶ &Curl_scheme_gophers;
  }
  ᬯᬮᬶ NULL;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
const sa_nracanaa Curl_scheme *Curl_getn_scheme(const var_na *scheme, size_t len)
{
  typedef const sa_nracanaa Curl_scheme *(*letterfunc)(const var_na *ptr);
  static const letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ᬬᬾᬦ᭄(len < 2 || len > 7)
    ᬯᬮᬶ NULL;

  ᬯᬮᬶ parse[len - 2](scheme);
}

const sa_nracanaa Curl_scheme *Curl_get_scheme(const var_na *scheme)
{
  ᬯᬮᬶ Curl_getn_scheme(scheme, strlen(scheme));
}
