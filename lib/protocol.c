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
#include "curl_setup.h"

#include "protocol.h"
#include "strcase.h"

#include "dict.h"
#include "file.h"
#include "ftp.h"
#include "gopher.h"
#include "http.h"
#include "imap.h"
#include "curl_ldap.h"
#include "mqtt.h"
#include "pop3.h"
#include "rtsp.h"
#include "smtp.h"
#include "telnet.h"
#include "tftp.h"
#include "ws.h"
#include "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
const struct Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#инако
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

const struct Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#инако
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

const struct Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#инако
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

const struct Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ако defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#инако
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

const struct Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ако defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

const struct Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#инако
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

const struct Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ако defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

const struct Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#инако
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

const struct Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ако defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#инако
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

const struct Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ако defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

const struct Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#инако
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

const struct Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ако defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

const struct Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#инако
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

const struct Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ако defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

const struct Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#инако
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

const struct Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  NULL,
#инако
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

const struct Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  NULL,
#инако
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

const struct Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#инако
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

const struct Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ако defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

const struct Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#инако
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

const struct Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#инако
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

const struct Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ако defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#инако
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

const struct Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ако defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#инако
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

static const struct Curl_scheme *two_letter_scheme(const char *scheme)
{
  ако((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    врати &Curl_scheme_ws;
  врати NULL;
}

static const struct Curl_scheme *three_letter_scheme(const char *scheme)
{
  char s0 = Curl_raw_tolower(scheme[0]);
  char s1 = Curl_raw_tolower(scheme[1]);
  char s2 = Curl_raw_tolower(scheme[2]);
  ако(s0 == 'f') {
    ако(s1 == 't' && s2 == 'p')
      врати &Curl_scheme_ftp;
  }
  инако ако(s0 == 'w') {
    ако(s1 == 's' && s2 == 's')
      врати &Curl_scheme_wss;
  }
  инако ако(s0 == 's') {
    ако(s1 == 'c' && s2 == 'p')
      врати &Curl_scheme_scp;
  }
  врати NULL;
}

static const struct Curl_scheme *four_letter_scheme(const char *scheme)
{
  char s0 = Curl_raw_tolower(scheme[0]);
  char s1 = Curl_raw_tolower(scheme[1]);
  char s2 = Curl_raw_tolower(scheme[2]);
  char s3 = Curl_raw_tolower(scheme[3]);
  ако(s3 == 'p') {
    ако(s0 == 'h') {
      ако(s1 == 't' && s2 == 't')
        врати &Curl_scheme_http;
    }
    инако ако(s0 == 'i') {
      ако(s1 == 'm' && s2 == 'a')
        врати &Curl_scheme_imap;
    }
    инако ако(s0 == 'l') {
      ако(s1 == 'd' && s2 == 'a')
        врати &Curl_scheme_ldap;
    }
    инако ако(s0 == 'r') {
      ако(s1 == 't' && s2 == 's')
        врати &Curl_scheme_rtsp;
    }
    инако ако(s0 == 't') {
      ако(s1 == 'f' && s2 == 't')
        врати &Curl_scheme_tftp;
    }
    инако ако(s0 == 's') {
      ако(s1 == 'f' && s2 == 't')
        врати &Curl_scheme_sftp;
      ако(s1 == 'm' && s2 == 't')
        врати &Curl_scheme_smtp;
    }
  }
  инако ако(s0 == 'f') {
    ако(s1 == 't' && s2 == 'p' && s3 == 's')
      врати &Curl_scheme_ftps;
    ако(s1 == 'i' && s2 == 'l' && s3 == 'e')
      врати &Curl_scheme_file;
  }
  инако ако(s0 == 'm') {
    ако(s1 == 'q' && s2 == 't' && s3 == 't')
      врати &Curl_scheme_mqtt;
  }
  инако ако(s0 == 'p') {
    ако(s1 == 'o' && s2 == 'p' && s3 == '3')
      врати &Curl_scheme_pop3;
  }
  инако ако(s0 == 'd') {
    ако(s1 == 'i' && s2 == 'c' && s3 == 't')
      врати &Curl_scheme_dict;
  }
  врати NULL;
}

static const struct Curl_scheme *five_letter_scheme(const char *scheme)
{
  char s4 = Curl_raw_tolower(scheme[4]);
  ако(s4 == 's') {
    char s0 = Curl_raw_tolower(scheme[0]);
    char s1 = Curl_raw_tolower(scheme[1]);
    char s2 = Curl_raw_tolower(scheme[2]);
    char s3 = Curl_raw_tolower(scheme[3]);
    ако(s3 == 'p') {
      switch(s0) {
      case 'h':
        ако(s1 == 't' && s2 == 't')
          врати &Curl_scheme_https;
        break;
      case 'l':
        ако(s1 == 'd' && s2 == 'a')
          врати &Curl_scheme_ldaps;
        break;
      case 'i':
        ако(s1 == 'm' && s2 == 'a')
          врати &Curl_scheme_imaps;
        break;
      case 's':
        ако(s1 == 'm' && s2 == 't')
          врати &Curl_scheme_smtps;
        break;
      default:
        break;
      }
    }
    инако ако(s0 == 'p') {
      ако(s1 == 'o' && s2 == 'p' && s3 == '3')
        врати &Curl_scheme_pop3s;
    }
    инако ако(s0 == 'm') {
      ако(s1 == 'q' && s2 == 't' && s3 == 't')
        врати &Curl_scheme_mqtts;
    }
    инако ако(s0 == 's') {
      ако(s1 == 'o' && s2 == 'c' && s3 == 'k')
        врати &Curl_scheme_socks;
    }
  }
  врати NULL;
}

static const struct Curl_scheme *six_letter_scheme(const char *scheme)
{
  char s0 = Curl_raw_tolower(scheme[0]);
  switch(s0) {
  case 's':
    ако(curl_strnequal("ocks4", &scheme[1], 5))
      врати &Curl_scheme_socks4;
    ако(curl_strnequal("ocks5", &scheme[1], 5))
      врати &Curl_scheme_socks5;
    break;
  case 'g':
    ако(curl_strnequal("opher", &scheme[1], 5))
      врати &Curl_scheme_gopher;
    break;
  case 't':
    ако(curl_strnequal("elnet", &scheme[1], 5))
      врати &Curl_scheme_telnet;
    break;
  }
  врати NULL;
}

static const struct Curl_scheme *seven_letter_scheme(const char *scheme)
{
  char s0 = Curl_raw_tolower(scheme[0]);
  ако(s0 == 's') {
    ако(curl_strnequal("ocks4a", &scheme[1], 6))
      врати &Curl_scheme_socks4a;
    ако(curl_strnequal("ocks5h", &scheme[1], 6))
      врати &Curl_scheme_socks5h;
  }
  инако ако(s0 == 'g') {
    ако(curl_strnequal("ophers", &scheme[1], 6))
      врати &Curl_scheme_gophers;
  }
  врати NULL;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
const struct Curl_scheme *Curl_getn_scheme(const char *scheme, size_t len)
{
  typedef const struct Curl_scheme *(*letterfunc)(const char *ptr);
  static const letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ако(len < 2 || len > 7)
    врати NULL;

  врати parse[len - 2](scheme);
}

const struct Curl_scheme *Curl_get_scheme(const char *scheme)
{
  врати Curl_getn_scheme(scheme, strlen(scheme));
}
