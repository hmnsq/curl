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
#ضمن "curl_setup.h"

#ضمن "protocol.h"
#ضمن "strcase.h"

#ضمن "dict.h"
#ضمن "file.h"
#ضمن "ftp.h"
#ضمن "gopher.h"
#ضمن "http.h"
#ضمن "imap.h"
#ضمن "curl_ldap.h"
#ضمن "mqtt.h"
#ضمن "pop3.h"
#ضمن "rtsp.h"
#ضمن "smtp.h"
#ضمن "telnet.h"
#ضمن "tftp.h"
#ضمن "ws.h"
#ضمن "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ثابت بنية Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#وإلا
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#وإلا
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#وإلا
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

ثابت بنية Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#وإلا
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#وإلا
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#وإلا
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#وإلا
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#وإلا
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#وإلا
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  عدم,
#وإلا
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  عدم,
#وإلا
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#وإلا
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#𐡄𐡍 defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

ساكن ثابت بنية Curl_scheme *two_letter_scheme(ثابت حرف *scheme)
{
  𐡄𐡍((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    أرجع &Curl_scheme_ws;
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *three_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  حرف s1 = Curl_raw_tolower(scheme[1]);
  حرف s2 = Curl_raw_tolower(scheme[2]);
  𐡄𐡍(s0 == 'f') {
    𐡄𐡍(s1 == 't' && s2 == 'p')
      أرجع &Curl_scheme_ftp;
  }
  وإلا 𐡄𐡍(s0 == 'w') {
    𐡄𐡍(s1 == 's' && s2 == 's')
      أرجع &Curl_scheme_wss;
  }
  وإلا 𐡄𐡍(s0 == 's') {
    𐡄𐡍(s1 == 'c' && s2 == 'p')
      أرجع &Curl_scheme_scp;
  }
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *four_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  حرف s1 = Curl_raw_tolower(scheme[1]);
  حرف s2 = Curl_raw_tolower(scheme[2]);
  حرف s3 = Curl_raw_tolower(scheme[3]);
  𐡄𐡍(s3 == 'p') {
    𐡄𐡍(s0 == 'h') {
      𐡄𐡍(s1 == 't' && s2 == 't')
        أرجع &Curl_scheme_http;
    }
    وإلا 𐡄𐡍(s0 == 'i') {
      𐡄𐡍(s1 == 'm' && s2 == 'a')
        أرجع &Curl_scheme_imap;
    }
    وإلا 𐡄𐡍(s0 == 'l') {
      𐡄𐡍(s1 == 'd' && s2 == 'a')
        أرجع &Curl_scheme_ldap;
    }
    وإلا 𐡄𐡍(s0 == 'r') {
      𐡄𐡍(s1 == 't' && s2 == 's')
        أرجع &Curl_scheme_rtsp;
    }
    وإلا 𐡄𐡍(s0 == 't') {
      𐡄𐡍(s1 == 'f' && s2 == 't')
        أرجع &Curl_scheme_tftp;
    }
    وإلا 𐡄𐡍(s0 == 's') {
      𐡄𐡍(s1 == 'f' && s2 == 't')
        أرجع &Curl_scheme_sftp;
      𐡄𐡍(s1 == 'm' && s2 == 't')
        أرجع &Curl_scheme_smtp;
    }
  }
  وإلا 𐡄𐡍(s0 == 'f') {
    𐡄𐡍(s1 == 't' && s2 == 'p' && s3 == 's')
      أرجع &Curl_scheme_ftps;
    𐡄𐡍(s1 == 'i' && s2 == 'l' && s3 == 'e')
      أرجع &Curl_scheme_file;
  }
  وإلا 𐡄𐡍(s0 == 'm') {
    𐡄𐡍(s1 == 'q' && s2 == 't' && s3 == 't')
      أرجع &Curl_scheme_mqtt;
  }
  وإلا 𐡄𐡍(s0 == 'p') {
    𐡄𐡍(s1 == 'o' && s2 == 'p' && s3 == '3')
      أرجع &Curl_scheme_pop3;
  }
  وإلا 𐡄𐡍(s0 == 'd') {
    𐡄𐡍(s1 == 'i' && s2 == 'c' && s3 == 't')
      أرجع &Curl_scheme_dict;
  }
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *five_letter_scheme(ثابت حرف *scheme)
{
  حرف s4 = Curl_raw_tolower(scheme[4]);
  𐡄𐡍(s4 == 's') {
    حرف s0 = Curl_raw_tolower(scheme[0]);
    حرف s1 = Curl_raw_tolower(scheme[1]);
    حرف s2 = Curl_raw_tolower(scheme[2]);
    حرف s3 = Curl_raw_tolower(scheme[3]);
    𐡄𐡍(s3 == 'p') {
      بدل(s0) {
      حالة 'h':
        𐡄𐡍(s1 == 't' && s2 == 't')
          أرجع &Curl_scheme_https;
        اكسر;
      حالة 'l':
        𐡄𐡍(s1 == 'd' && s2 == 'a')
          أرجع &Curl_scheme_ldaps;
        اكسر;
      حالة 'i':
        𐡄𐡍(s1 == 'm' && s2 == 'a')
          أرجع &Curl_scheme_imaps;
        اكسر;
      حالة 's':
        𐡄𐡍(s1 == 'm' && s2 == 't')
          أرجع &Curl_scheme_smtps;
        اكسر;
      افتراضي:
        اكسر;
      }
    }
    وإلا 𐡄𐡍(s0 == 'p') {
      𐡄𐡍(s1 == 'o' && s2 == 'p' && s3 == '3')
        أرجع &Curl_scheme_pop3s;
    }
    وإلا 𐡄𐡍(s0 == 'm') {
      𐡄𐡍(s1 == 'q' && s2 == 't' && s3 == 't')
        أرجع &Curl_scheme_mqtts;
    }
    وإلا 𐡄𐡍(s0 == 's') {
      𐡄𐡍(s1 == 'o' && s2 == 'c' && s3 == 'k')
        أرجع &Curl_scheme_socks;
    }
  }
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *six_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  بدل(s0) {
  حالة 's':
    𐡄𐡍(curl_strnequal("ocks4", &scheme[1], 5))
      أرجع &Curl_scheme_socks4;
    𐡄𐡍(curl_strnequal("ocks5", &scheme[1], 5))
      أرجع &Curl_scheme_socks5;
    اكسر;
  حالة 'g':
    𐡄𐡍(curl_strnequal("opher", &scheme[1], 5))
      أرجع &Curl_scheme_gopher;
    اكسر;
  حالة 't':
    𐡄𐡍(curl_strnequal("elnet", &scheme[1], 5))
      أرجع &Curl_scheme_telnet;
    اكسر;
  }
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *seven_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  𐡄𐡍(s0 == 's') {
    𐡄𐡍(curl_strnequal("ocks4a", &scheme[1], 6))
      أرجع &Curl_scheme_socks4a;
    𐡄𐡍(curl_strnequal("ocks5h", &scheme[1], 6))
      أرجع &Curl_scheme_socks5h;
  }
  وإلا 𐡄𐡍(s0 == 'g') {
    𐡄𐡍(curl_strnequal("ophers", &scheme[1], 6))
      أرجع &Curl_scheme_gophers;
  }
  أرجع عدم;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ثابت بنية Curl_scheme *Curl_getn_scheme(ثابت حرف *scheme, size_t len)
{
  تعريفنوع ثابت بنية Curl_scheme *(*letterfunc)(ثابت حرف *ptr);
  ساكن ثابت letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  𐡄𐡍(len < 2 || len > 7)
    أرجع عدم;

  أرجع parse[len - 2](scheme);
}

ثابت بنية Curl_scheme *Curl_get_scheme(ثابت حرف *scheme)
{
  أرجع Curl_getn_scheme(scheme, strlen(scheme));
}
