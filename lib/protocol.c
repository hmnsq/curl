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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"

#ಅನ್ತರ್ಭಾವ "protocol.h"
#ಅನ್ತರ್ಭಾವ "strcase.h"

#ಅನ್ತರ್ಭಾವ "dict.h"
#ಅನ್ತರ್ಭಾವ "file.h"
#ಅನ್ತರ್ಭಾವ "ftp.h"
#ಅನ್ತರ್ಭಾವ "gopher.h"
#ಅನ್ತರ್ಭಾವ "http.h"
#ಅನ್ತರ್ಭಾವ "imap.h"
#ಅನ್ತರ್ಭಾವ "curl_ldap.h"
#ಅನ್ತರ್ಭಾವ "mqtt.h"
#ಅನ್ತರ್ಭಾವ "pop3.h"
#ಅನ್ತರ್ಭಾವ "rtsp.h"
#ಅನ್ತರ್ಭಾವ "smtp.h"
#ಅನ್ತರ್ಭಾವ "telnet.h"
#ಅನ್ತರ್ಭಾವ "tftp.h"
#ಅನ್ತರ್ಭಾವ "ws.h"
#ಅನ್ತರ್ಭಾವ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#ಅನ್ಯಥಾ
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

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ಯದಿ defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ಯದಿ defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ಯದಿ defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ಯದಿ defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ಯದಿ defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ಯದಿ defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ಯದಿ defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  ರಿಕ್ತ,
#ಅನ್ಯಥಾ
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  ರಿಕ್ತ,
#ಅನ್ಯಥಾ
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ಯದಿ defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ಯದಿ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ಯದಿ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#ಅನ್ಯಥಾ
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *two_letter_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ಯದಿ((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    ಪ್ರತ್ಯಾಗಮ &Curl_scheme_ws;
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *three_letter_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ವರ್ಣ s0 = Curl_raw_tolower(scheme[0]);
  ವರ್ಣ s1 = Curl_raw_tolower(scheme[1]);
  ವರ್ಣ s2 = Curl_raw_tolower(scheme[2]);
  ಯದಿ(s0 == 'f') {
    ಯದಿ(s1 == 't' && s2 == 'p')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_ftp;
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 'w') {
    ಯದಿ(s1 == 's' && s2 == 's')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_wss;
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 's') {
    ಯದಿ(s1 == 'c' && s2 == 'p')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_scp;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *four_letter_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ವರ್ಣ s0 = Curl_raw_tolower(scheme[0]);
  ವರ್ಣ s1 = Curl_raw_tolower(scheme[1]);
  ವರ್ಣ s2 = Curl_raw_tolower(scheme[2]);
  ವರ್ಣ s3 = Curl_raw_tolower(scheme[3]);
  ಯದಿ(s3 == 'p') {
    ಯದಿ(s0 == 'h') {
      ಯದಿ(s1 == 't' && s2 == 't')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_http;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 'i') {
      ಯದಿ(s1 == 'm' && s2 == 'a')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_imap;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 'l') {
      ಯದಿ(s1 == 'd' && s2 == 'a')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_ldap;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 'r') {
      ಯದಿ(s1 == 't' && s2 == 's')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_rtsp;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 't') {
      ಯದಿ(s1 == 'f' && s2 == 't')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_tftp;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 's') {
      ಯದಿ(s1 == 'f' && s2 == 't')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_sftp;
      ಯದಿ(s1 == 'm' && s2 == 't')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_smtp;
    }
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 'f') {
    ಯದಿ(s1 == 't' && s2 == 'p' && s3 == 's')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_ftps;
    ಯದಿ(s1 == 'i' && s2 == 'l' && s3 == 'e')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_file;
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 'm') {
    ಯದಿ(s1 == 'q' && s2 == 't' && s3 == 't')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_mqtt;
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 'p') {
    ಯದಿ(s1 == 'o' && s2 == 'p' && s3 == '3')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_pop3;
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 'd') {
    ಯದಿ(s1 == 'i' && s2 == 'c' && s3 == 't')
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_dict;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *five_letter_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ವರ್ಣ s4 = Curl_raw_tolower(scheme[4]);
  ಯದಿ(s4 == 's') {
    ವರ್ಣ s0 = Curl_raw_tolower(scheme[0]);
    ವರ್ಣ s1 = Curl_raw_tolower(scheme[1]);
    ವರ್ಣ s2 = Curl_raw_tolower(scheme[2]);
    ವರ್ಣ s3 = Curl_raw_tolower(scheme[3]);
    ಯದಿ(s3 == 'p') {
      ವಿಕಲ್ಪ(s0) {
      ಪಕ್ಷ 'h':
        ಯದಿ(s1 == 't' && s2 == 't')
          ಪ್ರತ್ಯಾಗಮ &Curl_scheme_https;
        ಭಙ್ಗ;
      ಪಕ್ಷ 'l':
        ಯದಿ(s1 == 'd' && s2 == 'a')
          ಪ್ರತ್ಯಾಗಮ &Curl_scheme_ldaps;
        ಭಙ್ಗ;
      ಪಕ್ಷ 'i':
        ಯದಿ(s1 == 'm' && s2 == 'a')
          ಪ್ರತ್ಯಾಗಮ &Curl_scheme_imaps;
        ಭಙ್ಗ;
      ಪಕ್ಷ 's':
        ಯದಿ(s1 == 'm' && s2 == 't')
          ಪ್ರತ್ಯಾಗಮ &Curl_scheme_smtps;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        ಭಙ್ಗ;
      }
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 'p') {
      ಯದಿ(s1 == 'o' && s2 == 'p' && s3 == '3')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_pop3s;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 'm') {
      ಯದಿ(s1 == 'q' && s2 == 't' && s3 == 't')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_mqtts;
    }
    ಅನ್ಯಥಾ ಯದಿ(s0 == 's') {
      ಯದಿ(s1 == 'o' && s2 == 'c' && s3 == 'k')
        ಪ್ರತ್ಯಾಗಮ &Curl_scheme_socks;
    }
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *six_letter_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ವರ್ಣ s0 = Curl_raw_tolower(scheme[0]);
  ವಿಕಲ್ಪ(s0) {
  ಪಕ್ಷ 's':
    ಯದಿ(curl_strnequal("ocks4", &scheme[1], 5))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_socks4;
    ಯದಿ(curl_strnequal("ocks5", &scheme[1], 5))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_socks5;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'g':
    ಯದಿ(curl_strnequal("opher", &scheme[1], 5))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_gopher;
    ಭಙ್ಗ;
  ಪಕ್ಷ 't':
    ಯದಿ(curl_strnequal("elnet", &scheme[1], 5))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_telnet;
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *seven_letter_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ವರ್ಣ s0 = Curl_raw_tolower(scheme[0]);
  ಯದಿ(s0 == 's') {
    ಯದಿ(curl_strnequal("ocks4a", &scheme[1], 6))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_socks4a;
    ಯದಿ(curl_strnequal("ocks5h", &scheme[1], 6))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_socks5h;
  }
  ಅನ್ಯಥಾ ಯದಿ(s0 == 'g') {
    ಯದಿ(curl_strnequal("ophers", &scheme[1], 6))
      ಪ್ರತ್ಯಾಗಮ &Curl_scheme_gophers;
  }
  ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *Curl_getn_scheme(ಧ್ರುವ ವರ್ಣ *scheme, size_t len)
{
  ಪ್ರಕಾರನಿರ್ದೇಶ ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *(*letterfunc)(ಧ್ರುವ ವರ್ಣ *ptr);
  ಸ್ಥಿರ ಧ್ರುವ letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ಯದಿ(len < 2 || len > 7)
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

  ಪ್ರತ್ಯಾಗಮ parse[len - 2](scheme);
}

ಧ್ರುವ ಸಂರಚನಾ Curl_scheme *Curl_get_scheme(ಧ್ರುವ ವರ್ಣ *scheme)
{
  ಪ್ರತ್ಯಾಗಮ Curl_getn_scheme(scheme, strlen(scheme));
}
