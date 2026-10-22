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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "protocol.h"
#അന്തര്ഭാവ "strcase.h"

#അന്തര്ഭാവ "dict.h"
#അന്തര്ഭാവ "file.h"
#അന്തര്ഭാവ "ftp.h"
#അന്തര്ഭാവ "gopher.h"
#അന്തര്ഭാവ "http.h"
#അന്തര്ഭാവ "imap.h"
#അന്തര്ഭാവ "curl_ldap.h"
#അന്തര്ഭാവ "mqtt.h"
#അന്തര്ഭാവ "pop3.h"
#അന്തര്ഭാവ "rtsp.h"
#അന്തര്ഭാവ "smtp.h"
#അന്തര്ഭാവ "telnet.h"
#അന്തര്ഭാവ "tftp.h"
#അന്തര്ഭാവ "ws.h"
#അന്തര്ഭാവ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
സ്ഥിരം ഘടന Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#അല്ലെങ്കിൽ
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

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  ശൂന്യ,
#അല്ലെങ്കിൽ
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  ശൂന്യ,
#അല്ലെങ്കിൽ
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

സ്ഥിരം ഘടന Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#എങ്കിൽ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#അല്ലെങ്കിൽ
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

സ്ഥിതം സ്ഥിരം ഘടന Curl_scheme *two_letter_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  എങ്കിൽ((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    തിരികെ &Curl_scheme_ws;
  തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_scheme *three_letter_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  അക്ഷരം s0 = Curl_raw_tolower(scheme[0]);
  അക്ഷരം s1 = Curl_raw_tolower(scheme[1]);
  അക്ഷരം s2 = Curl_raw_tolower(scheme[2]);
  എങ്കിൽ(s0 == 'f') {
    എങ്കിൽ(s1 == 't' && s2 == 'p')
      തിരികെ &Curl_scheme_ftp;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'w') {
    എങ്കിൽ(s1 == 's' && s2 == 's')
      തിരികെ &Curl_scheme_wss;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 's') {
    എങ്കിൽ(s1 == 'c' && s2 == 'p')
      തിരികെ &Curl_scheme_scp;
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_scheme *four_letter_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  അക്ഷരം s0 = Curl_raw_tolower(scheme[0]);
  അക്ഷരം s1 = Curl_raw_tolower(scheme[1]);
  അക്ഷരം s2 = Curl_raw_tolower(scheme[2]);
  അക്ഷരം s3 = Curl_raw_tolower(scheme[3]);
  എങ്കിൽ(s3 == 'p') {
    എങ്കിൽ(s0 == 'h') {
      എങ്കിൽ(s1 == 't' && s2 == 't')
        തിരികെ &Curl_scheme_http;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'i') {
      എങ്കിൽ(s1 == 'm' && s2 == 'a')
        തിരികെ &Curl_scheme_imap;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'l') {
      എങ്കിൽ(s1 == 'd' && s2 == 'a')
        തിരികെ &Curl_scheme_ldap;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'r') {
      എങ്കിൽ(s1 == 't' && s2 == 's')
        തിരികെ &Curl_scheme_rtsp;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 't') {
      എങ്കിൽ(s1 == 'f' && s2 == 't')
        തിരികെ &Curl_scheme_tftp;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 's') {
      എങ്കിൽ(s1 == 'f' && s2 == 't')
        തിരികെ &Curl_scheme_sftp;
      എങ്കിൽ(s1 == 'm' && s2 == 't')
        തിരികെ &Curl_scheme_smtp;
    }
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'f') {
    എങ്കിൽ(s1 == 't' && s2 == 'p' && s3 == 's')
      തിരികെ &Curl_scheme_ftps;
    എങ്കിൽ(s1 == 'i' && s2 == 'l' && s3 == 'e')
      തിരികെ &Curl_scheme_file;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'm') {
    എങ്കിൽ(s1 == 'q' && s2 == 't' && s3 == 't')
      തിരികെ &Curl_scheme_mqtt;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'p') {
    എങ്കിൽ(s1 == 'o' && s2 == 'p' && s3 == '3')
      തിരികെ &Curl_scheme_pop3;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'd') {
    എങ്കിൽ(s1 == 'i' && s2 == 'c' && s3 == 't')
      തിരികെ &Curl_scheme_dict;
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_scheme *five_letter_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  അക്ഷരം s4 = Curl_raw_tolower(scheme[4]);
  എങ്കിൽ(s4 == 's') {
    അക്ഷരം s0 = Curl_raw_tolower(scheme[0]);
    അക്ഷരം s1 = Curl_raw_tolower(scheme[1]);
    അക്ഷരം s2 = Curl_raw_tolower(scheme[2]);
    അക്ഷരം s3 = Curl_raw_tolower(scheme[3]);
    എങ്കിൽ(s3 == 'p') {
      തിരഞ്ഞെടുപ്പ്(s0) {
      അവസ്ഥ 'h':
        എങ്കിൽ(s1 == 't' && s2 == 't')
          തിരികെ &Curl_scheme_https;
        നിർത്തുക;
      അവസ്ഥ 'l':
        എങ്കിൽ(s1 == 'd' && s2 == 'a')
          തിരികെ &Curl_scheme_ldaps;
        നിർത്തുക;
      അവസ്ഥ 'i':
        എങ്കിൽ(s1 == 'm' && s2 == 'a')
          തിരികെ &Curl_scheme_imaps;
        നിർത്തുക;
      അവസ്ഥ 's':
        എങ്കിൽ(s1 == 'm' && s2 == 't')
          തിരികെ &Curl_scheme_smtps;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        നിർത്തുക;
      }
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'p') {
      എങ്കിൽ(s1 == 'o' && s2 == 'p' && s3 == '3')
        തിരികെ &Curl_scheme_pop3s;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'm') {
      എങ്കിൽ(s1 == 'q' && s2 == 't' && s3 == 't')
        തിരികെ &Curl_scheme_mqtts;
    }
    അല്ലെങ്കിൽ എങ്കിൽ(s0 == 's') {
      എങ്കിൽ(s1 == 'o' && s2 == 'c' && s3 == 'k')
        തിരികെ &Curl_scheme_socks;
    }
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_scheme *six_letter_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  അക്ഷരം s0 = Curl_raw_tolower(scheme[0]);
  തിരഞ്ഞെടുപ്പ്(s0) {
  അവസ്ഥ 's':
    എങ്കിൽ(curl_strnequal("ocks4", &scheme[1], 5))
      തിരികെ &Curl_scheme_socks4;
    എങ്കിൽ(curl_strnequal("ocks5", &scheme[1], 5))
      തിരികെ &Curl_scheme_socks5;
    നിർത്തുക;
  അവസ്ഥ 'g':
    എങ്കിൽ(curl_strnequal("opher", &scheme[1], 5))
      തിരികെ &Curl_scheme_gopher;
    നിർത്തുക;
  അവസ്ഥ 't':
    എങ്കിൽ(curl_strnequal("elnet", &scheme[1], 5))
      തിരികെ &Curl_scheme_telnet;
    നിർത്തുക;
  }
  തിരികെ ശൂന്യ;
}

സ്ഥിതം സ്ഥിരം ഘടന Curl_scheme *seven_letter_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  അക്ഷരം s0 = Curl_raw_tolower(scheme[0]);
  എങ്കിൽ(s0 == 's') {
    എങ്കിൽ(curl_strnequal("ocks4a", &scheme[1], 6))
      തിരികെ &Curl_scheme_socks4a;
    എങ്കിൽ(curl_strnequal("ocks5h", &scheme[1], 6))
      തിരികെ &Curl_scheme_socks5h;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(s0 == 'g') {
    എങ്കിൽ(curl_strnequal("ophers", &scheme[1], 6))
      തിരികെ &Curl_scheme_gophers;
  }
  തിരികെ ശൂന്യ;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
സ്ഥിരം ഘടന Curl_scheme *Curl_getn_scheme(സ്ഥിരം അക്ഷരം *scheme, size_t len)
{
  തരനിർവചനം സ്ഥിരം ഘടന Curl_scheme *(*letterfunc)(സ്ഥിരം അക്ഷരം *ptr);
  സ്ഥിതം സ്ഥിരം letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  എങ്കിൽ(len < 2 || len > 7)
    തിരികെ ശൂന്യ;

  തിരികെ parse[len - 2](scheme);
}

സ്ഥിരം ഘടന Curl_scheme *Curl_get_scheme(സ്ഥിരം അക്ഷരം *scheme)
{
  തിരികെ Curl_getn_scheme(scheme, strlen(scheme));
}
