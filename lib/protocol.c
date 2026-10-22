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
#شامل "curl_setup.h"

#شامل "protocol.h"
#شامل "strcase.h"

#شامل "dict.h"
#شامل "file.h"
#شامل "ftp.h"
#شامل "gopher.h"
#شامل "http.h"
#شامل "imap.h"
#شامل "curl_ldap.h"
#شامل "mqtt.h"
#شامل "pop3.h"
#شامل "rtsp.h"
#شامل "smtp.h"
#شامل "telnet.h"
#شامل "tftp.h"
#شامل "ws.h"
#شامل "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ثابت ساختار Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#وگرنه
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#وگرنه
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#وگرنه
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

ثابت ساختار Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#اگر defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#وگرنه
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#اگر defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#وگرنه
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#اگر defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#وگرنه
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#اگر defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#وگرنه
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#اگر defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#وگرنه
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#اگر defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#وگرنه
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#اگر defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#وگرنه
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  پوچ,
#وگرنه
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  پوچ,
#وگرنه
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#وگرنه
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#اگر defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#وگرنه
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#وگرنه
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#اگر defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

ثابت ساختار Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#اگر defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#وگرنه
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

ایستا ثابت ساختار Curl_scheme *two_letter_scheme(ثابت نویسه *scheme)
{
  اگر((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    بازگشت &Curl_scheme_ws;
  بازگشت پوچ;
}

ایستا ثابت ساختار Curl_scheme *three_letter_scheme(ثابت نویسه *scheme)
{
  نویسه s0 = Curl_raw_tolower(scheme[0]);
  نویسه s1 = Curl_raw_tolower(scheme[1]);
  نویسه s2 = Curl_raw_tolower(scheme[2]);
  اگر(s0 == 'f') {
    اگر(s1 == 't' && s2 == 'p')
      بازگشت &Curl_scheme_ftp;
  }
  وگرنه اگر(s0 == 'w') {
    اگر(s1 == 's' && s2 == 's')
      بازگشت &Curl_scheme_wss;
  }
  وگرنه اگر(s0 == 's') {
    اگر(s1 == 'c' && s2 == 'p')
      بازگشت &Curl_scheme_scp;
  }
  بازگشت پوچ;
}

ایستا ثابت ساختار Curl_scheme *four_letter_scheme(ثابت نویسه *scheme)
{
  نویسه s0 = Curl_raw_tolower(scheme[0]);
  نویسه s1 = Curl_raw_tolower(scheme[1]);
  نویسه s2 = Curl_raw_tolower(scheme[2]);
  نویسه s3 = Curl_raw_tolower(scheme[3]);
  اگر(s3 == 'p') {
    اگر(s0 == 'h') {
      اگر(s1 == 't' && s2 == 't')
        بازگشت &Curl_scheme_http;
    }
    وگرنه اگر(s0 == 'i') {
      اگر(s1 == 'm' && s2 == 'a')
        بازگشت &Curl_scheme_imap;
    }
    وگرنه اگر(s0 == 'l') {
      اگر(s1 == 'd' && s2 == 'a')
        بازگشت &Curl_scheme_ldap;
    }
    وگرنه اگر(s0 == 'r') {
      اگر(s1 == 't' && s2 == 's')
        بازگشت &Curl_scheme_rtsp;
    }
    وگرنه اگر(s0 == 't') {
      اگر(s1 == 'f' && s2 == 't')
        بازگشت &Curl_scheme_tftp;
    }
    وگرنه اگر(s0 == 's') {
      اگر(s1 == 'f' && s2 == 't')
        بازگشت &Curl_scheme_sftp;
      اگر(s1 == 'm' && s2 == 't')
        بازگشت &Curl_scheme_smtp;
    }
  }
  وگرنه اگر(s0 == 'f') {
    اگر(s1 == 't' && s2 == 'p' && s3 == 's')
      بازگشت &Curl_scheme_ftps;
    اگر(s1 == 'i' && s2 == 'l' && s3 == 'e')
      بازگشت &Curl_scheme_file;
  }
  وگرنه اگر(s0 == 'm') {
    اگر(s1 == 'q' && s2 == 't' && s3 == 't')
      بازگشت &Curl_scheme_mqtt;
  }
  وگرنه اگر(s0 == 'p') {
    اگر(s1 == 'o' && s2 == 'p' && s3 == '3')
      بازگشت &Curl_scheme_pop3;
  }
  وگرنه اگر(s0 == 'd') {
    اگر(s1 == 'i' && s2 == 'c' && s3 == 't')
      بازگشت &Curl_scheme_dict;
  }
  بازگشت پوچ;
}

ایستا ثابت ساختار Curl_scheme *five_letter_scheme(ثابت نویسه *scheme)
{
  نویسه s4 = Curl_raw_tolower(scheme[4]);
  اگر(s4 == 's') {
    نویسه s0 = Curl_raw_tolower(scheme[0]);
    نویسه s1 = Curl_raw_tolower(scheme[1]);
    نویسه s2 = Curl_raw_tolower(scheme[2]);
    نویسه s3 = Curl_raw_tolower(scheme[3]);
    اگر(s3 == 'p') {
      انتخاب(s0) {
      حالت 'h':
        اگر(s1 == 't' && s2 == 't')
          بازگشت &Curl_scheme_https;
        بشکن;
      حالت 'l':
        اگر(s1 == 'd' && s2 == 'a')
          بازگشت &Curl_scheme_ldaps;
        بشکن;
      حالت 'i':
        اگر(s1 == 'm' && s2 == 'a')
          بازگشت &Curl_scheme_imaps;
        بشکن;
      حالت 's':
        اگر(s1 == 'm' && s2 == 't')
          بازگشت &Curl_scheme_smtps;
        بشکن;
      پیشفرض:
        بشکن;
      }
    }
    وگرنه اگر(s0 == 'p') {
      اگر(s1 == 'o' && s2 == 'p' && s3 == '3')
        بازگشت &Curl_scheme_pop3s;
    }
    وگرنه اگر(s0 == 'm') {
      اگر(s1 == 'q' && s2 == 't' && s3 == 't')
        بازگشت &Curl_scheme_mqtts;
    }
    وگرنه اگر(s0 == 's') {
      اگر(s1 == 'o' && s2 == 'c' && s3 == 'k')
        بازگشت &Curl_scheme_socks;
    }
  }
  بازگشت پوچ;
}

ایستا ثابت ساختار Curl_scheme *six_letter_scheme(ثابت نویسه *scheme)
{
  نویسه s0 = Curl_raw_tolower(scheme[0]);
  انتخاب(s0) {
  حالت 's':
    اگر(curl_strnequal("ocks4", &scheme[1], 5))
      بازگشت &Curl_scheme_socks4;
    اگر(curl_strnequal("ocks5", &scheme[1], 5))
      بازگشت &Curl_scheme_socks5;
    بشکن;
  حالت 'g':
    اگر(curl_strnequal("opher", &scheme[1], 5))
      بازگشت &Curl_scheme_gopher;
    بشکن;
  حالت 't':
    اگر(curl_strnequal("elnet", &scheme[1], 5))
      بازگشت &Curl_scheme_telnet;
    بشکن;
  }
  بازگشت پوچ;
}

ایستا ثابت ساختار Curl_scheme *seven_letter_scheme(ثابت نویسه *scheme)
{
  نویسه s0 = Curl_raw_tolower(scheme[0]);
  اگر(s0 == 's') {
    اگر(curl_strnequal("ocks4a", &scheme[1], 6))
      بازگشت &Curl_scheme_socks4a;
    اگر(curl_strnequal("ocks5h", &scheme[1], 6))
      بازگشت &Curl_scheme_socks5h;
  }
  وگرنه اگر(s0 == 'g') {
    اگر(curl_strnequal("ophers", &scheme[1], 6))
      بازگشت &Curl_scheme_gophers;
  }
  بازگشت پوچ;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ثابت ساختار Curl_scheme *Curl_getn_scheme(ثابت نویسه *scheme, size_t len)
{
  تعریفنوع ثابت ساختار Curl_scheme *(*letterfunc)(ثابت نویسه *ptr);
  ایستا ثابت letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  اگر(len < 2 || len > 7)
    بازگشت پوچ;

  بازگشت parse[len - 2](scheme);
}

ثابت ساختار Curl_scheme *Curl_get_scheme(ثابت نویسه *scheme)
{
  بازگشت Curl_getn_scheme(scheme, strlen(scheme));
}
