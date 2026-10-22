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
ثابت ساخت Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_DICT
  ZERO_NULL,
#ورنہ
  &Curl_protocol_dict,
#اگر_ختم
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  پورٹ_DICT,                            /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_FILE
  ZERO_NULL,
#ورنہ
  &Curl_protocol_file,
#اگر_ختم
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_FTP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_ftp,
#اگر_ختم
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  پورٹ_FTP,                        /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#اگر defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_ftp,
#اگر_ختم
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  پورٹ_FTPS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_GOPHER
  ZERO_NULL,
#ورنہ
  &Curl_protocol_gopher,
#اگر_ختم
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  پورٹ_GOPHER,                          /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#اگر defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_gophers,
#اگر_ختم
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  پورٹ_GOPHER,                          /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_HTTP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_http,
#اگر_ختم
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  پورٹ_HTTP,                            /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#اگر defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_http,
#اگر_ختم
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  پورٹ_HTTPS,                           /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_IMAP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_imap,
#اگر_ختم
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  پورٹ_IMAP,                        /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#اگر defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_imap,
#اگر_ختم
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  پورٹ_IMAPS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_LDAP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_ldap,
#اگر_ختم
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  پورٹ_LDAP,                            /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#اگر defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_ldap,
#اگر_ختم
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  پورٹ_LDAPS,                           /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_MQTT
  ZERO_NULL,
#ورنہ
  &Curl_protocol_mqtt,
#اگر_ختم
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  پورٹ_MQTT,                          /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#اگر defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_mqtts,
#اگر_ختم
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  پورٹ_MQTTS,                         /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_POP3
  ZERO_NULL,
#ورنہ
  &Curl_protocol_pop3,
#اگر_ختم
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  پورٹ_POP3,                        /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#اگر defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_pop3,
#اگر_ختم
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  پورٹ_POP3_S,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_RTSP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_rtsp,
#اگر_ختم
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  پورٹ_RTSP,                            /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#اگر_غیر_تعریف_شدہ USE_SSH
  تہی,
#ورنہ
  &Curl_protocol_sftp,
#اگر_ختم
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  پورٹ_SSH                              /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#اگر_غیر_تعریف_شدہ USE_SSH
  تہی,
#ورنہ
  &Curl_protocol_scp,
#اگر_ختم
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  پورٹ_SSH,                             /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_SMTP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_smtp,
#اگر_ختم
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  پورٹ_SMTP,                        /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#اگر defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_smtp,
#اگر_ختم
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  پورٹ_SMTPS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورٹ_SOCKS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورٹ_SOCKS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورٹ_SOCKS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورٹ_SOCKS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  پورٹ_SOCKS,                       /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_TELNET
  ZERO_NULL,
#ورنہ
  &Curl_protocol_telnet,
#اگر_ختم
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  پورٹ_TELNET,                          /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#اگر_تعریف_شدہ CURL_DISABLE_TFTP
  ZERO_NULL,
#ورنہ
  &Curl_protocol_tftp,
#اگر_ختم
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  پورٹ_TFTP,                            /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#اگر defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_ws,
#اگر_ختم
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  پورٹ_HTTP                             /* defport */
};

ثابت ساخت Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#اگر defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#ورنہ
  &Curl_protocol_ws,
#اگر_ختم
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  پورٹ_HTTPS                            /* defport */
};

ساکن ثابت ساخت Curl_scheme *two_letter_scheme(ثابت حرف *scheme)
{
  اگر((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    واپس &Curl_scheme_ws;
  واپس تہی;
}

ساکن ثابت ساخت Curl_scheme *three_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  حرف s1 = Curl_raw_tolower(scheme[1]);
  حرف s2 = Curl_raw_tolower(scheme[2]);
  اگر(s0 == 'f') {
    اگر(s1 == 't' && s2 == 'p')
      واپس &Curl_scheme_ftp;
  }
  ورنہ اگر(s0 == 'w') {
    اگر(s1 == 's' && s2 == 's')
      واپس &Curl_scheme_wss;
  }
  ورنہ اگر(s0 == 's') {
    اگر(s1 == 'c' && s2 == 'p')
      واپس &Curl_scheme_scp;
  }
  واپس تہی;
}

ساکن ثابت ساخت Curl_scheme *four_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  حرف s1 = Curl_raw_tolower(scheme[1]);
  حرف s2 = Curl_raw_tolower(scheme[2]);
  حرف s3 = Curl_raw_tolower(scheme[3]);
  اگر(s3 == 'p') {
    اگر(s0 == 'h') {
      اگر(s1 == 't' && s2 == 't')
        واپس &Curl_scheme_http;
    }
    ورنہ اگر(s0 == 'i') {
      اگر(s1 == 'm' && s2 == 'a')
        واپس &Curl_scheme_imap;
    }
    ورنہ اگر(s0 == 'l') {
      اگر(s1 == 'd' && s2 == 'a')
        واپس &Curl_scheme_ldap;
    }
    ورنہ اگر(s0 == 'r') {
      اگر(s1 == 't' && s2 == 's')
        واپس &Curl_scheme_rtsp;
    }
    ورنہ اگر(s0 == 't') {
      اگر(s1 == 'f' && s2 == 't')
        واپس &Curl_scheme_tftp;
    }
    ورنہ اگر(s0 == 's') {
      اگر(s1 == 'f' && s2 == 't')
        واپس &Curl_scheme_sftp;
      اگر(s1 == 'm' && s2 == 't')
        واپس &Curl_scheme_smtp;
    }
  }
  ورنہ اگر(s0 == 'f') {
    اگر(s1 == 't' && s2 == 'p' && s3 == 's')
      واپس &Curl_scheme_ftps;
    اگر(s1 == 'i' && s2 == 'l' && s3 == 'e')
      واپس &Curl_scheme_file;
  }
  ورنہ اگر(s0 == 'm') {
    اگر(s1 == 'q' && s2 == 't' && s3 == 't')
      واپس &Curl_scheme_mqtt;
  }
  ورنہ اگر(s0 == 'p') {
    اگر(s1 == 'o' && s2 == 'p' && s3 == '3')
      واپس &Curl_scheme_pop3;
  }
  ورنہ اگر(s0 == 'd') {
    اگر(s1 == 'i' && s2 == 'c' && s3 == 't')
      واپس &Curl_scheme_dict;
  }
  واپس تہی;
}

ساکن ثابت ساخت Curl_scheme *five_letter_scheme(ثابت حرف *scheme)
{
  حرف s4 = Curl_raw_tolower(scheme[4]);
  اگر(s4 == 's') {
    حرف s0 = Curl_raw_tolower(scheme[0]);
    حرف s1 = Curl_raw_tolower(scheme[1]);
    حرف s2 = Curl_raw_tolower(scheme[2]);
    حرف s3 = Curl_raw_tolower(scheme[3]);
    اگر(s3 == 'p') {
      انتخاب(s0) {
      حالت 'h':
        اگر(s1 == 't' && s2 == 't')
          واپس &Curl_scheme_https;
        توڑو;
      حالت 'l':
        اگر(s1 == 'd' && s2 == 'a')
          واپس &Curl_scheme_ldaps;
        توڑو;
      حالت 'i':
        اگر(s1 == 'm' && s2 == 'a')
          واپس &Curl_scheme_imaps;
        توڑو;
      حالت 's':
        اگر(s1 == 'm' && s2 == 't')
          واپس &Curl_scheme_smtps;
        توڑو;
      طےشدہ:
        توڑو;
      }
    }
    ورنہ اگر(s0 == 'p') {
      اگر(s1 == 'o' && s2 == 'p' && s3 == '3')
        واپس &Curl_scheme_pop3s;
    }
    ورنہ اگر(s0 == 'm') {
      اگر(s1 == 'q' && s2 == 't' && s3 == 't')
        واپس &Curl_scheme_mqtts;
    }
    ورنہ اگر(s0 == 's') {
      اگر(s1 == 'o' && s2 == 'c' && s3 == 'k')
        واپس &Curl_scheme_socks;
    }
  }
  واپس تہی;
}

ساکن ثابت ساخت Curl_scheme *six_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  انتخاب(s0) {
  حالت 's':
    اگر(curl_strnequal("ocks4", &scheme[1], 5))
      واپس &Curl_scheme_socks4;
    اگر(curl_strnequal("ocks5", &scheme[1], 5))
      واپس &Curl_scheme_socks5;
    توڑو;
  حالت 'g':
    اگر(curl_strnequal("opher", &scheme[1], 5))
      واپس &Curl_scheme_gopher;
    توڑو;
  حالت 't':
    اگر(curl_strnequal("elnet", &scheme[1], 5))
      واپس &Curl_scheme_telnet;
    توڑو;
  }
  واپس تہی;
}

ساکن ثابت ساخت Curl_scheme *seven_letter_scheme(ثابت حرف *scheme)
{
  حرف s0 = Curl_raw_tolower(scheme[0]);
  اگر(s0 == 's') {
    اگر(curl_strnequal("ocks4a", &scheme[1], 6))
      واپس &Curl_scheme_socks4a;
    اگر(curl_strnequal("ocks5h", &scheme[1], 6))
      واپس &Curl_scheme_socks5h;
  }
  ورنہ اگر(s0 == 'g') {
    اگر(curl_strnequal("ophers", &scheme[1], 6))
      واپس &Curl_scheme_gophers;
  }
  واپس تہی;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ثابت ساخت Curl_scheme *Curl_getn_scheme(ثابت حرف *scheme, size_t len)
{
  قسمتعریف ثابت ساخت Curl_scheme *(*letterfunc)(ثابت حرف *ptr);
  ساکن ثابت letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  اگر(len < 2 || len > 7)
    واپس تہی;

  واپس parse[len - 2](scheme);
}

ثابت ساخت Curl_scheme *Curl_get_scheme(ثابت حرف *scheme)
{
  واپس Curl_getn_scheme(scheme, لڑی_لمبائی(scheme));
}
