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
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_DICT
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_dict,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  ಪೋರ್ಟ್_DICT,                            /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_FILE
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_file,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_FTP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_ftp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  ಪೋರ್ಟ್_FTP,                        /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ಆದರೆ defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_ftp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  ಪೋರ್ಟ್_FTPS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_GOPHER
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_gopher,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  ಪೋರ್ಟ್_GOPHER,                          /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ಆದರೆ defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_gophers,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  ಪೋರ್ಟ್_GOPHER,                          /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_HTTP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_http,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_HTTP,                            /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ಆದರೆ defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_http,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  ಪೋರ್ಟ್_HTTPS,                           /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_IMAP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_imap,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_IMAP,                        /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ಆದರೆ defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_imap,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_IMAPS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_LDAP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_ldap,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  ಪೋರ್ಟ್_LDAP,                            /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ಆದರೆ defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_ldap,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  ಪೋರ್ಟ್_LDAPS,                           /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_MQTT
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_mqtt,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  ಪೋರ್ಟ್_MQTT,                          /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ಆದರೆ defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_mqtts,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  ಪೋರ್ಟ್_MQTTS,                         /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_POP3
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_pop3,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_POP3,                        /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ಆದರೆ defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_pop3,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_POP3_S,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_RTSP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_rtsp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  ಪೋರ್ಟ್_RTSP,                            /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_SSH
  ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_sftp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_SSH                              /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USE_SSH
  ಖಾಲಿ,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_scp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_SSH,                             /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_SMTP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_smtp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_SMTP,                        /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ಆದರೆ defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_smtp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  ಪೋರ್ಟ್_SMTPS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ಪೋರ್ಟ್_SOCKS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ಪೋರ್ಟ್_SOCKS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ಪೋರ್ಟ್_SOCKS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ಪೋರ್ಟ್_SOCKS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  ಪೋರ್ಟ್_SOCKS,                       /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_TELNET
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_telnet,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  ಪೋರ್ಟ್_TELNET,                          /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ CURL_DISABLE_TFTP
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_tftp,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  ಪೋರ್ಟ್_TFTP,                            /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ಆದರೆ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_ws,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  ಪೋರ್ಟ್_HTTP                             /* defport */
};

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ಆದರೆ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#ಇಲ್ಲದಿದ್ದರೆ
  &Curl_protocol_ws,
#ಆದರೆ_ಅಂತ್ಯ
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  ಪೋರ್ಟ್_HTTPS                            /* defport */
};

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *two_letter_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಆದರೆ((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    ಹಿಂತಿರುಗಿ &Curl_scheme_ws;
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *three_letter_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಅಕ್ಷರ s0 = Curl_raw_tolower(scheme[0]);
  ಅಕ್ಷರ s1 = Curl_raw_tolower(scheme[1]);
  ಅಕ್ಷರ s2 = Curl_raw_tolower(scheme[2]);
  ಆದರೆ(s0 == 'f') {
    ಆದರೆ(s1 == 't' && s2 == 'p')
      ಹಿಂತಿರುಗಿ &Curl_scheme_ftp;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'w') {
    ಆದರೆ(s1 == 's' && s2 == 's')
      ಹಿಂತಿರುಗಿ &Curl_scheme_wss;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 's') {
    ಆದರೆ(s1 == 'c' && s2 == 'p')
      ಹಿಂತಿರುಗಿ &Curl_scheme_scp;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *four_letter_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಅಕ್ಷರ s0 = Curl_raw_tolower(scheme[0]);
  ಅಕ್ಷರ s1 = Curl_raw_tolower(scheme[1]);
  ಅಕ್ಷರ s2 = Curl_raw_tolower(scheme[2]);
  ಅಕ್ಷರ s3 = Curl_raw_tolower(scheme[3]);
  ಆದರೆ(s3 == 'p') {
    ಆದರೆ(s0 == 'h') {
      ಆದರೆ(s1 == 't' && s2 == 't')
        ಹಿಂತಿರುಗಿ &Curl_scheme_http;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'i') {
      ಆದರೆ(s1 == 'm' && s2 == 'a')
        ಹಿಂತಿರುಗಿ &Curl_scheme_imap;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'l') {
      ಆದರೆ(s1 == 'd' && s2 == 'a')
        ಹಿಂತಿರುಗಿ &Curl_scheme_ldap;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'r') {
      ಆದರೆ(s1 == 't' && s2 == 's')
        ಹಿಂತಿರುಗಿ &Curl_scheme_rtsp;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 't') {
      ಆದರೆ(s1 == 'f' && s2 == 't')
        ಹಿಂತಿರುಗಿ &Curl_scheme_tftp;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 's') {
      ಆದರೆ(s1 == 'f' && s2 == 't')
        ಹಿಂತಿರುಗಿ &Curl_scheme_sftp;
      ಆದರೆ(s1 == 'm' && s2 == 't')
        ಹಿಂತಿರುಗಿ &Curl_scheme_smtp;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'f') {
    ಆದರೆ(s1 == 't' && s2 == 'p' && s3 == 's')
      ಹಿಂತಿರುಗಿ &Curl_scheme_ftps;
    ಆದರೆ(s1 == 'i' && s2 == 'l' && s3 == 'e')
      ಹಿಂತಿರುಗಿ &Curl_scheme_file;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'm') {
    ಆದರೆ(s1 == 'q' && s2 == 't' && s3 == 't')
      ಹಿಂತಿರುಗಿ &Curl_scheme_mqtt;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'p') {
    ಆದರೆ(s1 == 'o' && s2 == 'p' && s3 == '3')
      ಹಿಂತಿರುಗಿ &Curl_scheme_pop3;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'd') {
    ಆದರೆ(s1 == 'i' && s2 == 'c' && s3 == 't')
      ಹಿಂತಿರುಗಿ &Curl_scheme_dict;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *five_letter_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಅಕ್ಷರ s4 = Curl_raw_tolower(scheme[4]);
  ಆದರೆ(s4 == 's') {
    ಅಕ್ಷರ s0 = Curl_raw_tolower(scheme[0]);
    ಅಕ್ಷರ s1 = Curl_raw_tolower(scheme[1]);
    ಅಕ್ಷರ s2 = Curl_raw_tolower(scheme[2]);
    ಅಕ್ಷರ s3 = Curl_raw_tolower(scheme[3]);
    ಆದರೆ(s3 == 'p') {
      ಆಯ್ಕೆ(s0) {
      ಸ್ಥಿತಿ 'h':
        ಆದರೆ(s1 == 't' && s2 == 't')
          ಹಿಂತಿರುಗಿ &Curl_scheme_https;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'l':
        ಆದರೆ(s1 == 'd' && s2 == 'a')
          ಹಿಂತಿರುಗಿ &Curl_scheme_ldaps;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 'i':
        ಆದರೆ(s1 == 'm' && s2 == 'a')
          ಹಿಂತಿರುಗಿ &Curl_scheme_imaps;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ 's':
        ಆದರೆ(s1 == 'm' && s2 == 't')
          ಹಿಂತಿರುಗಿ &Curl_scheme_smtps;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        ನಿಲ್ಲಿಸು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'p') {
      ಆದರೆ(s1 == 'o' && s2 == 'p' && s3 == '3')
        ಹಿಂತಿರುಗಿ &Curl_scheme_pop3s;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'm') {
      ಆದರೆ(s1 == 'q' && s2 == 't' && s3 == 't')
        ಹಿಂತಿರುಗಿ &Curl_scheme_mqtts;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 's') {
      ಆದರೆ(s1 == 'o' && s2 == 'c' && s3 == 'k')
        ಹಿಂತಿರುಗಿ &Curl_scheme_socks;
    }
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *six_letter_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಅಕ್ಷರ s0 = Curl_raw_tolower(scheme[0]);
  ಆಯ್ಕೆ(s0) {
  ಸ್ಥಿತಿ 's':
    ಆದರೆ(curl_strnequal("ocks4", &scheme[1], 5))
      ಹಿಂತಿರುಗಿ &Curl_scheme_socks4;
    ಆದರೆ(curl_strnequal("ocks5", &scheme[1], 5))
      ಹಿಂತಿರುಗಿ &Curl_scheme_socks5;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 'g':
    ಆದರೆ(curl_strnequal("opher", &scheme[1], 5))
      ಹಿಂತಿರುಗಿ &Curl_scheme_gopher;
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ 't':
    ಆದರೆ(curl_strnequal("elnet", &scheme[1], 5))
      ಹಿಂತಿರುಗಿ &Curl_scheme_telnet;
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *seven_letter_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಅಕ್ಷರ s0 = Curl_raw_tolower(scheme[0]);
  ಆದರೆ(s0 == 's') {
    ಆದರೆ(curl_strnequal("ocks4a", &scheme[1], 6))
      ಹಿಂತಿರುಗಿ &Curl_scheme_socks4a;
    ಆದರೆ(curl_strnequal("ocks5h", &scheme[1], 6))
      ಹಿಂತಿರುಗಿ &Curl_scheme_socks5h;
  }
  ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(s0 == 'g') {
    ಆದರೆ(curl_strnequal("ophers", &scheme[1], 6))
      ಹಿಂತಿರುಗಿ &Curl_scheme_gophers;
  }
  ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *Curl_getn_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme, size_t len)
{
  ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *(*letterfunc)(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ptr);
  ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ಆದರೆ(len < 2 || len > 7)
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

  ಹಿಂತಿರುಗಿ parse[len - 2](scheme);
}

ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_scheme *Curl_get_scheme(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *scheme)
{
  ಹಿಂತಿರುಗಿ Curl_getn_scheme(scheme, ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(scheme));
}
