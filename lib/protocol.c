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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "protocol.h"
#अन्तर्भाव "strcase.h"

#अन्तर्भाव "dict.h"
#अन्तर्भाव "file.h"
#अन्तर्भाव "ftp.h"
#अन्तर्भाव "gopher.h"
#अन्तर्भाव "http.h"
#अन्तर्भाव "imap.h"
#अन्तर्भाव "curl_ldap.h"
#अन्तर्भाव "mqtt.h"
#अन्तर्भाव "pop3.h"
#अन्तर्भाव "rtsp.h"
#अन्तर्भाव "smtp.h"
#अन्तर्भाव "telnet.h"
#अन्तर्भाव "tftp.h"
#अन्तर्भाव "ws.h"
#अन्तर्भाव "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
ध्रुव संरचना Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#अन्यथा
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

ध्रुव संरचना Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#यदि defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#यदि defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#यदि defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#यदि defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#यदि defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#यदि defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#यदि defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  रिक्त,
#अन्यथा
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  रिक्त,
#अन्यथा
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#यदि defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#यदि defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

ध्रुव संरचना Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#यदि defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

स्थिर ध्रुव संरचना Curl_scheme *two_letter_scheme(ध्रुव वर्ण *scheme)
{
  यदि((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    प्रत्यागम &Curl_scheme_ws;
  प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना Curl_scheme *three_letter_scheme(ध्रुव वर्ण *scheme)
{
  वर्ण s0 = Curl_raw_tolower(scheme[0]);
  वर्ण s1 = Curl_raw_tolower(scheme[1]);
  वर्ण s2 = Curl_raw_tolower(scheme[2]);
  यदि(s0 == 'f') {
    यदि(s1 == 't' && s2 == 'p')
      प्रत्यागम &Curl_scheme_ftp;
  }
  अन्यथा यदि(s0 == 'w') {
    यदि(s1 == 's' && s2 == 's')
      प्रत्यागम &Curl_scheme_wss;
  }
  अन्यथा यदि(s0 == 's') {
    यदि(s1 == 'c' && s2 == 'p')
      प्रत्यागम &Curl_scheme_scp;
  }
  प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना Curl_scheme *four_letter_scheme(ध्रुव वर्ण *scheme)
{
  वर्ण s0 = Curl_raw_tolower(scheme[0]);
  वर्ण s1 = Curl_raw_tolower(scheme[1]);
  वर्ण s2 = Curl_raw_tolower(scheme[2]);
  वर्ण s3 = Curl_raw_tolower(scheme[3]);
  यदि(s3 == 'p') {
    यदि(s0 == 'h') {
      यदि(s1 == 't' && s2 == 't')
        प्रत्यागम &Curl_scheme_http;
    }
    अन्यथा यदि(s0 == 'i') {
      यदि(s1 == 'm' && s2 == 'a')
        प्रत्यागम &Curl_scheme_imap;
    }
    अन्यथा यदि(s0 == 'l') {
      यदि(s1 == 'd' && s2 == 'a')
        प्रत्यागम &Curl_scheme_ldap;
    }
    अन्यथा यदि(s0 == 'r') {
      यदि(s1 == 't' && s2 == 's')
        प्रत्यागम &Curl_scheme_rtsp;
    }
    अन्यथा यदि(s0 == 't') {
      यदि(s1 == 'f' && s2 == 't')
        प्रत्यागम &Curl_scheme_tftp;
    }
    अन्यथा यदि(s0 == 's') {
      यदि(s1 == 'f' && s2 == 't')
        प्रत्यागम &Curl_scheme_sftp;
      यदि(s1 == 'm' && s2 == 't')
        प्रत्यागम &Curl_scheme_smtp;
    }
  }
  अन्यथा यदि(s0 == 'f') {
    यदि(s1 == 't' && s2 == 'p' && s3 == 's')
      प्रत्यागम &Curl_scheme_ftps;
    यदि(s1 == 'i' && s2 == 'l' && s3 == 'e')
      प्रत्यागम &Curl_scheme_file;
  }
  अन्यथा यदि(s0 == 'm') {
    यदि(s1 == 'q' && s2 == 't' && s3 == 't')
      प्रत्यागम &Curl_scheme_mqtt;
  }
  अन्यथा यदि(s0 == 'p') {
    यदि(s1 == 'o' && s2 == 'p' && s3 == '3')
      प्रत्यागम &Curl_scheme_pop3;
  }
  अन्यथा यदि(s0 == 'd') {
    यदि(s1 == 'i' && s2 == 'c' && s3 == 't')
      प्रत्यागम &Curl_scheme_dict;
  }
  प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना Curl_scheme *five_letter_scheme(ध्रुव वर्ण *scheme)
{
  वर्ण s4 = Curl_raw_tolower(scheme[4]);
  यदि(s4 == 's') {
    वर्ण s0 = Curl_raw_tolower(scheme[0]);
    वर्ण s1 = Curl_raw_tolower(scheme[1]);
    वर्ण s2 = Curl_raw_tolower(scheme[2]);
    वर्ण s3 = Curl_raw_tolower(scheme[3]);
    यदि(s3 == 'p') {
      विकल्प(s0) {
      पक्ष 'h':
        यदि(s1 == 't' && s2 == 't')
          प्रत्यागम &Curl_scheme_https;
        भङ्ग;
      पक्ष 'l':
        यदि(s1 == 'd' && s2 == 'a')
          प्रत्यागम &Curl_scheme_ldaps;
        भङ्ग;
      पक्ष 'i':
        यदि(s1 == 'm' && s2 == 'a')
          प्रत्यागम &Curl_scheme_imaps;
        भङ्ग;
      पक्ष 's':
        यदि(s1 == 'm' && s2 == 't')
          प्रत्यागम &Curl_scheme_smtps;
        भङ्ग;
      स्वभाव:
        भङ्ग;
      }
    }
    अन्यथा यदि(s0 == 'p') {
      यदि(s1 == 'o' && s2 == 'p' && s3 == '3')
        प्रत्यागम &Curl_scheme_pop3s;
    }
    अन्यथा यदि(s0 == 'm') {
      यदि(s1 == 'q' && s2 == 't' && s3 == 't')
        प्रत्यागम &Curl_scheme_mqtts;
    }
    अन्यथा यदि(s0 == 's') {
      यदि(s1 == 'o' && s2 == 'c' && s3 == 'k')
        प्रत्यागम &Curl_scheme_socks;
    }
  }
  प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना Curl_scheme *six_letter_scheme(ध्रुव वर्ण *scheme)
{
  वर्ण s0 = Curl_raw_tolower(scheme[0]);
  विकल्प(s0) {
  पक्ष 's':
    यदि(curl_strnequal("ocks4", &scheme[1], 5))
      प्रत्यागम &Curl_scheme_socks4;
    यदि(curl_strnequal("ocks5", &scheme[1], 5))
      प्रत्यागम &Curl_scheme_socks5;
    भङ्ग;
  पक्ष 'g':
    यदि(curl_strnequal("opher", &scheme[1], 5))
      प्रत्यागम &Curl_scheme_gopher;
    भङ्ग;
  पक्ष 't':
    यदि(curl_strnequal("elnet", &scheme[1], 5))
      प्रत्यागम &Curl_scheme_telnet;
    भङ्ग;
  }
  प्रत्यागम रिक्त;
}

स्थिर ध्रुव संरचना Curl_scheme *seven_letter_scheme(ध्रुव वर्ण *scheme)
{
  वर्ण s0 = Curl_raw_tolower(scheme[0]);
  यदि(s0 == 's') {
    यदि(curl_strnequal("ocks4a", &scheme[1], 6))
      प्रत्यागम &Curl_scheme_socks4a;
    यदि(curl_strnequal("ocks5h", &scheme[1], 6))
      प्रत्यागम &Curl_scheme_socks5h;
  }
  अन्यथा यदि(s0 == 'g') {
    यदि(curl_strnequal("ophers", &scheme[1], 6))
      प्रत्यागम &Curl_scheme_gophers;
  }
  प्रत्यागम रिक्त;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
ध्रुव संरचना Curl_scheme *Curl_getn_scheme(ध्रुव वर्ण *scheme, size_t len)
{
  प्रकारनिर्देश ध्रुव संरचना Curl_scheme *(*letterfunc)(ध्रुव वर्ण *ptr);
  स्थिर ध्रुव letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  यदि(len < 2 || len > 7)
    प्रत्यागम रिक्त;

  प्रत्यागम parse[len - 2](scheme);
}

ध्रुव संरचना Curl_scheme *Curl_get_scheme(ध्रुव वर्ण *scheme)
{
  प्रत्यागम Curl_getn_scheme(scheme, strlen(scheme));
}
