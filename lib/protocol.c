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
#અન્તર્ભાવ "curl_setup.h"

#અન્તર્ભાવ "protocol.h"
#અન્તર્ભાવ "strcase.h"

#અન્તર્ભાવ "dict.h"
#અન્તર્ભાવ "file.h"
#અન્તર્ભાવ "ftp.h"
#અન્તર્ભાવ "gopher.h"
#અન્તર્ભાવ "http.h"
#અન્તર્ભાવ "imap.h"
#અન્તર્ભાવ "curl_ldap.h"
#અન્તર્ભાવ "mqtt.h"
#અન્તર્ભાવ "pop3.h"
#અન્તર્ભાવ "rtsp.h"
#અન્તર્ભાવ "smtp.h"
#અન્તર્ભાવ "telnet.h"
#અન્તર્ભાવ "tftp.h"
#અન્તર્ભાવ "ws.h"
#અન્તર્ભાવ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
અચલ સંરચના Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#ifdef CURL_DISABLE_DICT
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_dict,
#endif
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#ifdef CURL_DISABLE_FILE
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_file,
#endif
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#ifdef CURL_DISABLE_FTP
  ZERO_NULL,
#નહીંતર
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

અચલ સંરચના Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#જો defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_ftp,
#endif
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#ifdef CURL_DISABLE_GOPHER
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_gopher,
#endif
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#જો defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_gophers,
#endif
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#ifdef CURL_DISABLE_HTTP
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#જો defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_http,
#endif
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#ifdef CURL_DISABLE_IMAP
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#જો defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_imap,
#endif
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#ifdef CURL_DISABLE_LDAP
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#જો defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_ldap,
#endif
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#ifdef CURL_DISABLE_MQTT
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_mqtt,
#endif
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#જો defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_mqtts,
#endif
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#ifdef CURL_DISABLE_POP3
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#જો defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_pop3,
#endif
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#ifdef CURL_DISABLE_RTSP
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_rtsp,
#endif
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#ifndef USE_SSH
  ખાલી,
#નહીંતર
  &Curl_protocol_sftp,
#endif
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#ifndef USE_SSH
  ખાલી,
#નહીંતર
  &Curl_protocol_scp,
#endif
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#ifdef CURL_DISABLE_SMTP
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#જો defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_smtp,
#endif
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#ifdef CURL_DISABLE_TELNET
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_telnet,
#endif
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#ifdef CURL_DISABLE_TFTP
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_tftp,
#endif
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#જો defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_ws,
#endif
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

અચલ સંરચના Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#જો defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#નહીંતર
  &Curl_protocol_ws,
#endif
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

સ્થિર અચલ સંરચના Curl_scheme *two_letter_scheme(અચલ અક્ષર *scheme)
{
  જો((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    પાછા &Curl_scheme_ws;
  પાછા ખાલી;
}

સ્થિર અચલ સંરચના Curl_scheme *three_letter_scheme(અચલ અક્ષર *scheme)
{
  અક્ષર s0 = Curl_raw_tolower(scheme[0]);
  અક્ષર s1 = Curl_raw_tolower(scheme[1]);
  અક્ષર s2 = Curl_raw_tolower(scheme[2]);
  જો(s0 == 'f') {
    જો(s1 == 't' && s2 == 'p')
      પાછા &Curl_scheme_ftp;
  }
  નહીંતર જો(s0 == 'w') {
    જો(s1 == 's' && s2 == 's')
      પાછા &Curl_scheme_wss;
  }
  નહીંતર જો(s0 == 's') {
    જો(s1 == 'c' && s2 == 'p')
      પાછા &Curl_scheme_scp;
  }
  પાછા ખાલી;
}

સ્થિર અચલ સંરચના Curl_scheme *four_letter_scheme(અચલ અક્ષર *scheme)
{
  અક્ષર s0 = Curl_raw_tolower(scheme[0]);
  અક્ષર s1 = Curl_raw_tolower(scheme[1]);
  અક્ષર s2 = Curl_raw_tolower(scheme[2]);
  અક્ષર s3 = Curl_raw_tolower(scheme[3]);
  જો(s3 == 'p') {
    જો(s0 == 'h') {
      જો(s1 == 't' && s2 == 't')
        પાછા &Curl_scheme_http;
    }
    નહીંતર જો(s0 == 'i') {
      જો(s1 == 'm' && s2 == 'a')
        પાછા &Curl_scheme_imap;
    }
    નહીંતર જો(s0 == 'l') {
      જો(s1 == 'd' && s2 == 'a')
        પાછા &Curl_scheme_ldap;
    }
    નહીંતર જો(s0 == 'r') {
      જો(s1 == 't' && s2 == 's')
        પાછા &Curl_scheme_rtsp;
    }
    નહીંતર જો(s0 == 't') {
      જો(s1 == 'f' && s2 == 't')
        પાછા &Curl_scheme_tftp;
    }
    નહીંતર જો(s0 == 's') {
      જો(s1 == 'f' && s2 == 't')
        પાછા &Curl_scheme_sftp;
      જો(s1 == 'm' && s2 == 't')
        પાછા &Curl_scheme_smtp;
    }
  }
  નહીંતર જો(s0 == 'f') {
    જો(s1 == 't' && s2 == 'p' && s3 == 's')
      પાછા &Curl_scheme_ftps;
    જો(s1 == 'i' && s2 == 'l' && s3 == 'e')
      પાછા &Curl_scheme_file;
  }
  નહીંતર જો(s0 == 'm') {
    જો(s1 == 'q' && s2 == 't' && s3 == 't')
      પાછા &Curl_scheme_mqtt;
  }
  નહીંતર જો(s0 == 'p') {
    જો(s1 == 'o' && s2 == 'p' && s3 == '3')
      પાછા &Curl_scheme_pop3;
  }
  નહીંતર જો(s0 == 'd') {
    જો(s1 == 'i' && s2 == 'c' && s3 == 't')
      પાછા &Curl_scheme_dict;
  }
  પાછા ખાલી;
}

સ્થિર અચલ સંરચના Curl_scheme *five_letter_scheme(અચલ અક્ષર *scheme)
{
  અક્ષર s4 = Curl_raw_tolower(scheme[4]);
  જો(s4 == 's') {
    અક્ષર s0 = Curl_raw_tolower(scheme[0]);
    અક્ષર s1 = Curl_raw_tolower(scheme[1]);
    અક્ષર s2 = Curl_raw_tolower(scheme[2]);
    અક્ષર s3 = Curl_raw_tolower(scheme[3]);
    જો(s3 == 'p') {
      પસંદગી(s0) {
      સ્થિતિ 'h':
        જો(s1 == 't' && s2 == 't')
          પાછા &Curl_scheme_https;
        તોડો;
      સ્થિતિ 'l':
        જો(s1 == 'd' && s2 == 'a')
          પાછા &Curl_scheme_ldaps;
        તોડો;
      સ્થિતિ 'i':
        જો(s1 == 'm' && s2 == 'a')
          પાછા &Curl_scheme_imaps;
        તોડો;
      સ્થિતિ 's':
        જો(s1 == 'm' && s2 == 't')
          પાછા &Curl_scheme_smtps;
        તોડો;
      મૂળભૂત:
        તોડો;
      }
    }
    નહીંતર જો(s0 == 'p') {
      જો(s1 == 'o' && s2 == 'p' && s3 == '3')
        પાછા &Curl_scheme_pop3s;
    }
    નહીંતર જો(s0 == 'm') {
      જો(s1 == 'q' && s2 == 't' && s3 == 't')
        પાછા &Curl_scheme_mqtts;
    }
    નહીંતર જો(s0 == 's') {
      જો(s1 == 'o' && s2 == 'c' && s3 == 'k')
        પાછા &Curl_scheme_socks;
    }
  }
  પાછા ખાલી;
}

સ્થિર અચલ સંરચના Curl_scheme *six_letter_scheme(અચલ અક્ષર *scheme)
{
  અક્ષર s0 = Curl_raw_tolower(scheme[0]);
  પસંદગી(s0) {
  સ્થિતિ 's':
    જો(curl_strnequal("ocks4", &scheme[1], 5))
      પાછા &Curl_scheme_socks4;
    જો(curl_strnequal("ocks5", &scheme[1], 5))
      પાછા &Curl_scheme_socks5;
    તોડો;
  સ્થિતિ 'g':
    જો(curl_strnequal("opher", &scheme[1], 5))
      પાછા &Curl_scheme_gopher;
    તોડો;
  સ્થિતિ 't':
    જો(curl_strnequal("elnet", &scheme[1], 5))
      પાછા &Curl_scheme_telnet;
    તોડો;
  }
  પાછા ખાલી;
}

સ્થિર અચલ સંરચના Curl_scheme *seven_letter_scheme(અચલ અક્ષર *scheme)
{
  અક્ષર s0 = Curl_raw_tolower(scheme[0]);
  જો(s0 == 's') {
    જો(curl_strnequal("ocks4a", &scheme[1], 6))
      પાછા &Curl_scheme_socks4a;
    જો(curl_strnequal("ocks5h", &scheme[1], 6))
      પાછા &Curl_scheme_socks5h;
  }
  નહીંતર જો(s0 == 'g') {
    જો(curl_strnequal("ophers", &scheme[1], 6))
      પાછા &Curl_scheme_gophers;
  }
  પાછા ખાલી;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
અચલ સંરચના Curl_scheme *Curl_getn_scheme(અચલ અક્ષર *scheme, size_t len)
{
  પ્રકારવ્યાખ્યા અચલ સંરચના Curl_scheme *(*letterfunc)(અચલ અક્ષર *ptr);
  સ્થિર અચલ letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  જો(len < 2 || len > 7)
    પાછા ખાલી;

  પાછા parse[len - 2](scheme);
}

અચલ સંરચના Curl_scheme *Curl_get_scheme(અચલ અક્ષર *scheme)
{
  પાછા Curl_getn_scheme(scheme, strlen(scheme));
}
