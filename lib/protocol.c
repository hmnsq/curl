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
#அந்தர்பாவ "curl_setup.h"

#அந்தர்பாவ "protocol.h"
#அந்தர்பாவ "strcase.h"

#அந்தர்பாவ "dict.h"
#அந்தர்பாவ "file.h"
#அந்தர்பாவ "ftp.h"
#அந்தர்பாவ "gopher.h"
#அந்தர்பாவ "http.h"
#அந்தர்பாவ "imap.h"
#அந்தர்பாவ "curl_ldap.h"
#அந்தர்பாவ "mqtt.h"
#அந்தர்பாவ "pop3.h"
#அந்தர்பாவ "rtsp.h"
#அந்தர்பாவ "smtp.h"
#அந்தர்பாவ "telnet.h"
#அந்தர்பாவ "tftp.h"
#அந்தர்பாவ "ws.h"
#அந்தர்பாவ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#இல்லையெனில்
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

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#எனில் defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#எனில் defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#எனில் defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#எனில் defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#எனில் defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#எனில் defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#எனில் defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  வெற்று,
#இல்லையெனில்
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  வெற்று,
#இல்லையெனில்
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#எனில் defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#எனில் defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

மாறிலி கட்டமைப்பு Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#எனில் defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#இல்லையெனில்
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

நிலையான மாறிலி கட்டமைப்பு Curl_scheme *two_letter_scheme(மாறிலி எழுத்து *scheme)
{
  எனில்((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    திரும்பு &Curl_scheme_ws;
  திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு Curl_scheme *three_letter_scheme(மாறிலி எழுத்து *scheme)
{
  எழுத்து s0 = Curl_raw_tolower(scheme[0]);
  எழுத்து s1 = Curl_raw_tolower(scheme[1]);
  எழுத்து s2 = Curl_raw_tolower(scheme[2]);
  எனில்(s0 == 'f') {
    எனில்(s1 == 't' && s2 == 'p')
      திரும்பு &Curl_scheme_ftp;
  }
  இல்லையெனில் எனில்(s0 == 'w') {
    எனில்(s1 == 's' && s2 == 's')
      திரும்பு &Curl_scheme_wss;
  }
  இல்லையெனில் எனில்(s0 == 's') {
    எனில்(s1 == 'c' && s2 == 'p')
      திரும்பு &Curl_scheme_scp;
  }
  திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு Curl_scheme *four_letter_scheme(மாறிலி எழுத்து *scheme)
{
  எழுத்து s0 = Curl_raw_tolower(scheme[0]);
  எழுத்து s1 = Curl_raw_tolower(scheme[1]);
  எழுத்து s2 = Curl_raw_tolower(scheme[2]);
  எழுத்து s3 = Curl_raw_tolower(scheme[3]);
  எனில்(s3 == 'p') {
    எனில்(s0 == 'h') {
      எனில்(s1 == 't' && s2 == 't')
        திரும்பு &Curl_scheme_http;
    }
    இல்லையெனில் எனில்(s0 == 'i') {
      எனில்(s1 == 'm' && s2 == 'a')
        திரும்பு &Curl_scheme_imap;
    }
    இல்லையெனில் எனில்(s0 == 'l') {
      எனில்(s1 == 'd' && s2 == 'a')
        திரும்பு &Curl_scheme_ldap;
    }
    இல்லையெனில் எனில்(s0 == 'r') {
      எனில்(s1 == 't' && s2 == 's')
        திரும்பு &Curl_scheme_rtsp;
    }
    இல்லையெனில் எனில்(s0 == 't') {
      எனில்(s1 == 'f' && s2 == 't')
        திரும்பு &Curl_scheme_tftp;
    }
    இல்லையெனில் எனில்(s0 == 's') {
      எனில்(s1 == 'f' && s2 == 't')
        திரும்பு &Curl_scheme_sftp;
      எனில்(s1 == 'm' && s2 == 't')
        திரும்பு &Curl_scheme_smtp;
    }
  }
  இல்லையெனில் எனில்(s0 == 'f') {
    எனில்(s1 == 't' && s2 == 'p' && s3 == 's')
      திரும்பு &Curl_scheme_ftps;
    எனில்(s1 == 'i' && s2 == 'l' && s3 == 'e')
      திரும்பு &Curl_scheme_file;
  }
  இல்லையெனில் எனில்(s0 == 'm') {
    எனில்(s1 == 'q' && s2 == 't' && s3 == 't')
      திரும்பு &Curl_scheme_mqtt;
  }
  இல்லையெனில் எனில்(s0 == 'p') {
    எனில்(s1 == 'o' && s2 == 'p' && s3 == '3')
      திரும்பு &Curl_scheme_pop3;
  }
  இல்லையெனில் எனில்(s0 == 'd') {
    எனில்(s1 == 'i' && s2 == 'c' && s3 == 't')
      திரும்பு &Curl_scheme_dict;
  }
  திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு Curl_scheme *five_letter_scheme(மாறிலி எழுத்து *scheme)
{
  எழுத்து s4 = Curl_raw_tolower(scheme[4]);
  எனில்(s4 == 's') {
    எழுத்து s0 = Curl_raw_tolower(scheme[0]);
    எழுத்து s1 = Curl_raw_tolower(scheme[1]);
    எழுத்து s2 = Curl_raw_tolower(scheme[2]);
    எழுத்து s3 = Curl_raw_tolower(scheme[3]);
    எனில்(s3 == 'p') {
      தேர்வு(s0) {
      நிலை 'h':
        எனில்(s1 == 't' && s2 == 't')
          திரும்பு &Curl_scheme_https;
        நிறுத்து;
      நிலை 'l':
        எனில்(s1 == 'd' && s2 == 'a')
          திரும்பு &Curl_scheme_ldaps;
        நிறுத்து;
      நிலை 'i':
        எனில்(s1 == 'm' && s2 == 'a')
          திரும்பு &Curl_scheme_imaps;
        நிறுத்து;
      நிலை 's':
        எனில்(s1 == 'm' && s2 == 't')
          திரும்பு &Curl_scheme_smtps;
        நிறுத்து;
      இயல்பு:
        நிறுத்து;
      }
    }
    இல்லையெனில் எனில்(s0 == 'p') {
      எனில்(s1 == 'o' && s2 == 'p' && s3 == '3')
        திரும்பு &Curl_scheme_pop3s;
    }
    இல்லையெனில் எனில்(s0 == 'm') {
      எனில்(s1 == 'q' && s2 == 't' && s3 == 't')
        திரும்பு &Curl_scheme_mqtts;
    }
    இல்லையெனில் எனில்(s0 == 's') {
      எனில்(s1 == 'o' && s2 == 'c' && s3 == 'k')
        திரும்பு &Curl_scheme_socks;
    }
  }
  திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு Curl_scheme *six_letter_scheme(மாறிலி எழுத்து *scheme)
{
  எழுத்து s0 = Curl_raw_tolower(scheme[0]);
  தேர்வு(s0) {
  நிலை 's':
    எனில்(curl_strnequal("ocks4", &scheme[1], 5))
      திரும்பு &Curl_scheme_socks4;
    எனில்(curl_strnequal("ocks5", &scheme[1], 5))
      திரும்பு &Curl_scheme_socks5;
    நிறுத்து;
  நிலை 'g':
    எனில்(curl_strnequal("opher", &scheme[1], 5))
      திரும்பு &Curl_scheme_gopher;
    நிறுத்து;
  நிலை 't':
    எனில்(curl_strnequal("elnet", &scheme[1], 5))
      திரும்பு &Curl_scheme_telnet;
    நிறுத்து;
  }
  திரும்பு வெற்று;
}

நிலையான மாறிலி கட்டமைப்பு Curl_scheme *seven_letter_scheme(மாறிலி எழுத்து *scheme)
{
  எழுத்து s0 = Curl_raw_tolower(scheme[0]);
  எனில்(s0 == 's') {
    எனில்(curl_strnequal("ocks4a", &scheme[1], 6))
      திரும்பு &Curl_scheme_socks4a;
    எனில்(curl_strnequal("ocks5h", &scheme[1], 6))
      திரும்பு &Curl_scheme_socks5h;
  }
  இல்லையெனில் எனில்(s0 == 'g') {
    எனில்(curl_strnequal("ophers", &scheme[1], 6))
      திரும்பு &Curl_scheme_gophers;
  }
  திரும்பு வெற்று;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
மாறிலி கட்டமைப்பு Curl_scheme *Curl_getn_scheme(மாறிலி எழுத்து *scheme, size_t len)
{
  வகைவரை மாறிலி கட்டமைப்பு Curl_scheme *(*letterfunc)(மாறிலி எழுத்து *ptr);
  நிலையான மாறிலி letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  எனில்(len < 2 || len > 7)
    திரும்பு வெற்று;

  திரும்பு parse[len - 2](scheme);
}

மாறிலி கட்டமைப்பு Curl_scheme *Curl_get_scheme(மாறிலி எழுத்து *scheme)
{
  திரும்பு Curl_getn_scheme(scheme, strlen(scheme));
}
