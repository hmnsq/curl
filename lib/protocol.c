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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "protocol.h"
#অন্তর্ভাব "strcase.h"

#অন্তর্ভাব "dict.h"
#অন্তর্ভাব "file.h"
#অন্তর্ভাব "ftp.h"
#অন্তর্ভাব "gopher.h"
#অন্তর্ভাব "http.h"
#অন্তর্ভাব "imap.h"
#অন্তর্ভাব "curl_ldap.h"
#অন্তর্ভাব "mqtt.h"
#অন্তর্ভাব "pop3.h"
#অন্তর্ভাব "rtsp.h"
#অন্তর্ভাব "smtp.h"
#অন্তর্ভাব "telnet.h"
#অন্তর্ভাব "tftp.h"
#অন্তর্ভাব "ws.h"
#অন্তর্ভাব "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ধ্রুবক কাঠামো Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#অন্যথা
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

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#যদি defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#যদি defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#যদি defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#যদি defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#যদি defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#যদি defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#যদি defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  ফাঁকা,
#অন্যথা
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  ফাঁকা,
#অন্যথা
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#যদি defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#যদি defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

ধ্রুবক কাঠামো Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#যদি defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#অন্যথা
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

স্থির ধ্রুবক কাঠামো Curl_scheme *two_letter_scheme(ধ্রুবক অক্ষর *scheme)
{
  যদি((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    ফেরত &Curl_scheme_ws;
  ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো Curl_scheme *three_letter_scheme(ধ্রুবক অক্ষর *scheme)
{
  অক্ষর s0 = Curl_raw_tolower(scheme[0]);
  অক্ষর s1 = Curl_raw_tolower(scheme[1]);
  অক্ষর s2 = Curl_raw_tolower(scheme[2]);
  যদি(s0 == 'f') {
    যদি(s1 == 't' && s2 == 'p')
      ফেরত &Curl_scheme_ftp;
  }
  অন্যথা যদি(s0 == 'w') {
    যদি(s1 == 's' && s2 == 's')
      ফেরত &Curl_scheme_wss;
  }
  অন্যথা যদি(s0 == 's') {
    যদি(s1 == 'c' && s2 == 'p')
      ফেরত &Curl_scheme_scp;
  }
  ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো Curl_scheme *four_letter_scheme(ধ্রুবক অক্ষর *scheme)
{
  অক্ষর s0 = Curl_raw_tolower(scheme[0]);
  অক্ষর s1 = Curl_raw_tolower(scheme[1]);
  অক্ষর s2 = Curl_raw_tolower(scheme[2]);
  অক্ষর s3 = Curl_raw_tolower(scheme[3]);
  যদি(s3 == 'p') {
    যদি(s0 == 'h') {
      যদি(s1 == 't' && s2 == 't')
        ফেরত &Curl_scheme_http;
    }
    অন্যথা যদি(s0 == 'i') {
      যদি(s1 == 'm' && s2 == 'a')
        ফেরত &Curl_scheme_imap;
    }
    অন্যথা যদি(s0 == 'l') {
      যদি(s1 == 'd' && s2 == 'a')
        ফেরত &Curl_scheme_ldap;
    }
    অন্যথা যদি(s0 == 'r') {
      যদি(s1 == 't' && s2 == 's')
        ফেরত &Curl_scheme_rtsp;
    }
    অন্যথা যদি(s0 == 't') {
      যদি(s1 == 'f' && s2 == 't')
        ফেরত &Curl_scheme_tftp;
    }
    অন্যথা যদি(s0 == 's') {
      যদি(s1 == 'f' && s2 == 't')
        ফেরত &Curl_scheme_sftp;
      যদি(s1 == 'm' && s2 == 't')
        ফেরত &Curl_scheme_smtp;
    }
  }
  অন্যথা যদি(s0 == 'f') {
    যদি(s1 == 't' && s2 == 'p' && s3 == 's')
      ফেরত &Curl_scheme_ftps;
    যদি(s1 == 'i' && s2 == 'l' && s3 == 'e')
      ফেরত &Curl_scheme_file;
  }
  অন্যথা যদি(s0 == 'm') {
    যদি(s1 == 'q' && s2 == 't' && s3 == 't')
      ফেরত &Curl_scheme_mqtt;
  }
  অন্যথা যদি(s0 == 'p') {
    যদি(s1 == 'o' && s2 == 'p' && s3 == '3')
      ফেরত &Curl_scheme_pop3;
  }
  অন্যথা যদি(s0 == 'd') {
    যদি(s1 == 'i' && s2 == 'c' && s3 == 't')
      ফেরত &Curl_scheme_dict;
  }
  ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো Curl_scheme *five_letter_scheme(ধ্রুবক অক্ষর *scheme)
{
  অক্ষর s4 = Curl_raw_tolower(scheme[4]);
  যদি(s4 == 's') {
    অক্ষর s0 = Curl_raw_tolower(scheme[0]);
    অক্ষর s1 = Curl_raw_tolower(scheme[1]);
    অক্ষর s2 = Curl_raw_tolower(scheme[2]);
    অক্ষর s3 = Curl_raw_tolower(scheme[3]);
    যদি(s3 == 'p') {
      নির্বাচন(s0) {
      ক্ষেত্র 'h':
        যদি(s1 == 't' && s2 == 't')
          ফেরত &Curl_scheme_https;
        বিরতি;
      ক্ষেত্র 'l':
        যদি(s1 == 'd' && s2 == 'a')
          ফেরত &Curl_scheme_ldaps;
        বিরতি;
      ক্ষেত্র 'i':
        যদি(s1 == 'm' && s2 == 'a')
          ফেরত &Curl_scheme_imaps;
        বিরতি;
      ক্ষেত্র 's':
        যদি(s1 == 'm' && s2 == 't')
          ফেরত &Curl_scheme_smtps;
        বিরতি;
      পূর্বনির্ধারিত:
        বিরতি;
      }
    }
    অন্যথা যদি(s0 == 'p') {
      যদি(s1 == 'o' && s2 == 'p' && s3 == '3')
        ফেরত &Curl_scheme_pop3s;
    }
    অন্যথা যদি(s0 == 'm') {
      যদি(s1 == 'q' && s2 == 't' && s3 == 't')
        ফেরত &Curl_scheme_mqtts;
    }
    অন্যথা যদি(s0 == 's') {
      যদি(s1 == 'o' && s2 == 'c' && s3 == 'k')
        ফেরত &Curl_scheme_socks;
    }
  }
  ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো Curl_scheme *six_letter_scheme(ধ্রুবক অক্ষর *scheme)
{
  অক্ষর s0 = Curl_raw_tolower(scheme[0]);
  নির্বাচন(s0) {
  ক্ষেত্র 's':
    যদি(curl_strnequal("ocks4", &scheme[1], 5))
      ফেরত &Curl_scheme_socks4;
    যদি(curl_strnequal("ocks5", &scheme[1], 5))
      ফেরত &Curl_scheme_socks5;
    বিরতি;
  ক্ষেত্র 'g':
    যদি(curl_strnequal("opher", &scheme[1], 5))
      ফেরত &Curl_scheme_gopher;
    বিরতি;
  ক্ষেত্র 't':
    যদি(curl_strnequal("elnet", &scheme[1], 5))
      ফেরত &Curl_scheme_telnet;
    বিরতি;
  }
  ফেরত ফাঁকা;
}

স্থির ধ্রুবক কাঠামো Curl_scheme *seven_letter_scheme(ধ্রুবক অক্ষর *scheme)
{
  অক্ষর s0 = Curl_raw_tolower(scheme[0]);
  যদি(s0 == 's') {
    যদি(curl_strnequal("ocks4a", &scheme[1], 6))
      ফেরত &Curl_scheme_socks4a;
    যদি(curl_strnequal("ocks5h", &scheme[1], 6))
      ফেরত &Curl_scheme_socks5h;
  }
  অন্যথা যদি(s0 == 'g') {
    যদি(curl_strnequal("ophers", &scheme[1], 6))
      ফেরত &Curl_scheme_gophers;
  }
  ফেরত ফাঁকা;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ধ্রুবক কাঠামো Curl_scheme *Curl_getn_scheme(ধ্রুবক অক্ষর *scheme, size_t len)
{
  প্রকারলেখ ধ্রুবক কাঠামো Curl_scheme *(*letterfunc)(ধ্রুবক অক্ষর *ptr);
  স্থির ধ্রুবক letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  যদি(len < 2 || len > 7)
    ফেরত ফাঁকা;

  ফেরত parse[len - 2](scheme);
}

ধ্রুবক কাঠামো Curl_scheme *Curl_get_scheme(ধ্রুবক অক্ষর *scheme)
{
  ফেরত Curl_getn_scheme(scheme, strlen(scheme));
}
