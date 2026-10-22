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
#که_تعریف_شوی CURL_DISABLE_DICT
  ZERO_NULL,
#وإلا
  &Curl_protocol_dict,
#د_که_پای
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  پورټ_DICT,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#که_تعریف_شوی CURL_DISABLE_FILE
  ZERO_NULL,
#وإلا
  &Curl_protocol_file,
#د_که_پای
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#که_تعریف_شوی CURL_DISABLE_FTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_ftp,
#د_که_پای
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  پورټ_FTP,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#إذا defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ftp,
#د_که_پای
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  پورټ_FTPS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#که_تعریف_شوی CURL_DISABLE_GOPHER
  ZERO_NULL,
#وإلا
  &Curl_protocol_gopher,
#د_که_پای
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  پورټ_GOPHER,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#إذا defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_gophers,
#د_که_پای
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  پورټ_GOPHER,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#که_تعریف_شوی CURL_DISABLE_HTTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_http,
#د_که_پای
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  پورټ_HTTP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#إذا defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_http,
#د_که_پای
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  پورټ_HTTPS,                           /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#که_تعریف_شوی CURL_DISABLE_IMAP
  ZERO_NULL,
#وإلا
  &Curl_protocol_imap,
#د_که_پای
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  پورټ_IMAP,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#إذا defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_imap,
#د_که_پای
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  پورټ_IMAPS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#که_تعریف_شوی CURL_DISABLE_LDAP
  ZERO_NULL,
#وإلا
  &Curl_protocol_ldap,
#د_که_پای
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  پورټ_LDAP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#إذا defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ldap,
#د_که_پای
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  پورټ_LDAPS,                           /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#که_تعریف_شوی CURL_DISABLE_MQTT
  ZERO_NULL,
#وإلا
  &Curl_protocol_mqtt,
#د_که_پای
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  پورټ_MQTT,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#إذا defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_mqtts,
#د_که_پای
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  پورټ_MQTTS,                         /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#که_تعریف_شوی CURL_DISABLE_POP3
  ZERO_NULL,
#وإلا
  &Curl_protocol_pop3,
#د_که_پای
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  پورټ_POP3,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#إذا defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_pop3,
#د_که_پای
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  پورټ_POP3_S,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#که_تعریف_شوی CURL_DISABLE_RTSP
  ZERO_NULL,
#وإلا
  &Curl_protocol_rtsp,
#د_که_پای
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  پورټ_RTSP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#که_نه_تعریف_شوی USE_SSH
  عدم,
#وإلا
  &Curl_protocol_sftp,
#د_که_پای
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  پورټ_SSH                              /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#که_نه_تعریف_شوی USE_SSH
  عدم,
#وإلا
  &Curl_protocol_scp,
#د_که_پای
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  پورټ_SSH,                             /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#که_تعریف_شوی CURL_DISABLE_SMTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_smtp,
#د_که_پای
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  پورټ_SMTP,                        /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#إذا defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_smtp,
#د_که_پای
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  پورټ_SMTPS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورټ_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورټ_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورټ_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورټ_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورټ_SOCKS,                       /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#که_تعریف_شوی CURL_DISABLE_TELNET
  ZERO_NULL,
#وإلا
  &Curl_protocol_telnet,
#د_که_پای
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  پورټ_TELNET,                          /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#که_تعریف_شوی CURL_DISABLE_TFTP
  ZERO_NULL,
#وإلا
  &Curl_protocol_tftp,
#د_که_پای
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  پورټ_TFTP,                            /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#إذا defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ws,
#د_که_پای
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  پورټ_HTTP                             /* defport */
};

ثابت بنية Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#إذا defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#وإلا
  &Curl_protocol_ws,
#د_که_پای
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  پورټ_HTTPS                            /* defport */
};

ساكن ثابت بنية Curl_scheme *two_letter_scheme(ثابت حرف *scheme)
{
  إذا((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    أرجع &Curl_scheme_ws;
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *three_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  حرف s1 = Curl_raw_tolower(scheme[1]);
  حرف s2 = Curl_raw_tolower(scheme[2]);
  إذا(s0 == 'f') {
    إذا(s1 == 't' && s2 == 'p')
      أرجع &Curl_scheme_ftp;
  }
  وإلا إذا(s0 == 'w') {
    إذا(s1 == 's' && s2 == 's')
      أرجع &Curl_scheme_wss;
  }
  وإلا إذا(s0 == 's') {
    إذا(s1 == 'c' && s2 == 'p')
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
  إذا(s3 == 'p') {
    إذا(s0 == 'h') {
      إذا(s1 == 't' && s2 == 't')
        أرجع &Curl_scheme_http;
    }
    وإلا إذا(s0 == 'i') {
      إذا(s1 == 'm' && s2 == 'a')
        أرجع &Curl_scheme_imap;
    }
    وإلا إذا(s0 == 'l') {
      إذا(s1 == 'd' && s2 == 'a')
        أرجع &Curl_scheme_ldap;
    }
    وإلا إذا(s0 == 'r') {
      إذا(s1 == 't' && s2 == 's')
        أرجع &Curl_scheme_rtsp;
    }
    وإلا إذا(s0 == 't') {
      إذا(s1 == 'f' && s2 == 't')
        أرجع &Curl_scheme_tftp;
    }
    وإلا إذا(s0 == 's') {
      إذا(s1 == 'f' && s2 == 't')
        أرجع &Curl_scheme_sftp;
      إذا(s1 == 'm' && s2 == 't')
        أرجع &Curl_scheme_smtp;
    }
  }
  وإلا إذا(s0 == 'f') {
    إذا(s1 == 't' && s2 == 'p' && s3 == 's')
      أرجع &Curl_scheme_ftps;
    إذا(s1 == 'i' && s2 == 'l' && s3 == 'e')
      أرجع &Curl_scheme_file;
  }
  وإلا إذا(s0 == 'm') {
    إذا(s1 == 'q' && s2 == 't' && s3 == 't')
      أرجع &Curl_scheme_mqtt;
  }
  وإلا إذا(s0 == 'p') {
    إذا(s1 == 'o' && s2 == 'p' && s3 == '3')
      أرجع &Curl_scheme_pop3;
  }
  وإلا إذا(s0 == 'd') {
    إذا(s1 == 'i' && s2 == 'c' && s3 == 't')
      أرجع &Curl_scheme_dict;
  }
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *five_letter_scheme(ثابت حرف *scheme)
{
  حرف s4 = Curl_raw_tolower(scheme[4]);
  إذا(s4 == 's') {
    حرف s0 = Curl_raw_tolower(scheme[0]);
    حرف s1 = Curl_raw_tolower(scheme[1]);
    حرف s2 = Curl_raw_tolower(scheme[2]);
    حرف s3 = Curl_raw_tolower(scheme[3]);
    إذا(s3 == 'p') {
      بدل(s0) {
      حالة 'h':
        إذا(s1 == 't' && s2 == 't')
          أرجع &Curl_scheme_https;
        اكسر;
      حالة 'l':
        إذا(s1 == 'd' && s2 == 'a')
          أرجع &Curl_scheme_ldaps;
        اكسر;
      حالة 'i':
        إذا(s1 == 'm' && s2 == 'a')
          أرجع &Curl_scheme_imaps;
        اكسر;
      حالة 's':
        إذا(s1 == 'm' && s2 == 't')
          أرجع &Curl_scheme_smtps;
        اكسر;
      افتراضي:
        اكسر;
      }
    }
    وإلا إذا(s0 == 'p') {
      إذا(s1 == 'o' && s2 == 'p' && s3 == '3')
        أرجع &Curl_scheme_pop3s;
    }
    وإلا إذا(s0 == 'm') {
      إذا(s1 == 'q' && s2 == 't' && s3 == 't')
        أرجع &Curl_scheme_mqtts;
    }
    وإلا إذا(s0 == 's') {
      إذا(s1 == 'o' && s2 == 'c' && s3 == 'k')
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
    إذا(curl_strnequal("ocks4", &scheme[1], 5))
      أرجع &Curl_scheme_socks4;
    إذا(curl_strnequal("ocks5", &scheme[1], 5))
      أرجع &Curl_scheme_socks5;
    اكسر;
  حالة 'g':
    إذا(curl_strnequal("opher", &scheme[1], 5))
      أرجع &Curl_scheme_gopher;
    اكسر;
  حالة 't':
    إذا(curl_strnequal("elnet", &scheme[1], 5))
      أرجع &Curl_scheme_telnet;
    اكسر;
  }
  أرجع عدم;
}

ساكن ثابت بنية Curl_scheme *seven_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  إذا(s0 == 's') {
    إذا(curl_strnequal("ocks4a", &scheme[1], 6))
      أرجع &Curl_scheme_socks4a;
    إذا(curl_strnequal("ocks5h", &scheme[1], 6))
      أرجع &Curl_scheme_socks5h;
  }
  وإلا إذا(s0 == 'g') {
    إذا(curl_strnequal("ophers", &scheme[1], 6))
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

  إذا(len < 2 || len > 7)
    أرجع عدم;

  أرجع parse[len - 2](scheme);
}

ثابت بنية Curl_scheme *Curl_get_scheme(ثابت حرف *scheme)
{
  أرجع Curl_getn_scheme(scheme, مزي_اوږدوالی(scheme));
}
