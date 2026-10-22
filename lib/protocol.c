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
#समावेश "curl_setup.h"

#समावेश "protocol.h"
#समावेश "strcase.h"

#समावेश "dict.h"
#समावेश "file.h"
#समावेश "ftp.h"
#समावेश "gopher.h"
#समावेश "http.h"
#समावेश "imap.h"
#समावेश "curl_ldap.h"
#समावेश "mqtt.h"
#समावेश "pop3.h"
#समावेश "rtsp.h"
#समावेश "smtp.h"
#समावेश "telnet.h"
#समावेश "tftp.h"
#समावेश "ws.h"
#समावेश "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
स्थिर काष्ठा Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#अगर_घोषित CURL_DISABLE_DICT
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_dict,
#पूर्ण_अगर
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_DICT,                            /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#अगर_घोषित CURL_DISABLE_FILE
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_file,
#पूर्ण_अगर
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#अगर_घोषित CURL_DISABLE_FTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ftp,
#पूर्ण_अगर
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTP,                        /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#अगर defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ftp,
#पूर्ण_अगर
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  PORT_FTPS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#अगर_घोषित CURL_DISABLE_GOPHER
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_gopher,
#पूर्ण_अगर
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  PORT_GOPHER,                          /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#अगर defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_gophers,
#पूर्ण_अगर
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_GOPHER,                          /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#अगर_घोषित CURL_DISABLE_HTTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_http,
#पूर्ण_अगर
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  PORT_HTTP,                            /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#अगर defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_http,
#पूर्ण_अगर
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS,                           /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#अगर_घोषित CURL_DISABLE_IMAP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_imap,
#पूर्ण_अगर
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  PORT_IMAP,                        /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#अगर defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_imap,
#पूर्ण_अगर
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_IMAPS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#अगर_घोषित CURL_DISABLE_LDAP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ldap,
#पूर्ण_अगर
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  PORT_LDAP,                            /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#अगर defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ldap,
#पूर्ण_अगर
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  PORT_LDAPS,                           /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#अगर_घोषित CURL_DISABLE_MQTT
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_mqtt,
#पूर्ण_अगर
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  PORT_MQTT,                          /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#अगर defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_mqtts,
#पूर्ण_अगर
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  PORT_MQTTS,                         /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#अगर_घोषित CURL_DISABLE_POP3
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_pop3,
#पूर्ण_अगर
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_POP3,                        /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#अगर defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_pop3,
#पूर्ण_अगर
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_POP3S,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#अगर_घोषित CURL_DISABLE_RTSP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_rtsp,
#पूर्ण_अगर
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  PORT_RTSP,                            /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#अगर_अघोषित USE_SSH
  शून्य,
#अन्यथा
  &Curl_protocol_sftp,
#पूर्ण_अगर
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH                              /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#अगर_अघोषित USE_SSH
  शून्य,
#अन्यथा
  &Curl_protocol_scp,
#पूर्ण_अगर
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  PORT_SSH,                             /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#अगर_घोषित CURL_DISABLE_SMTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_smtp,
#पूर्ण_अगर
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  PORT_SMTP,                        /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#अगर defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_smtp,
#पूर्ण_अगर
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  PORT_SMTPS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  PORT_SOCKS,                       /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#अगर_घोषित CURL_DISABLE_TELNET
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_telnet,
#पूर्ण_अगर
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  PORT_TELNET,                          /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#अगर_घोषित CURL_DISABLE_TFTP
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_tftp,
#पूर्ण_अगर
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  PORT_TFTP,                            /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#अगर defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ws,
#पूर्ण_अगर
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTP                             /* defport */
};

स्थिर काष्ठा Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#अगर defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#अन्यथा
  &Curl_protocol_ws,
#पूर्ण_अगर
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  PORT_HTTPS                            /* defport */
};

अटल स्थिर काष्ठा Curl_scheme *two_letter_scheme(स्थिर अक्षर *scheme)
{
  अगर((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    वापस &Curl_scheme_ws;
  वापस शून्य;
}

अटल स्थिर काष्ठा Curl_scheme *three_letter_scheme(स्थिर अक्षर *scheme)
{
  अक्षर s0 = Curl_raw_tolower(scheme[0]);
  अक्षर s1 = Curl_raw_tolower(scheme[1]);
  अक्षर s2 = Curl_raw_tolower(scheme[2]);
  अगर(s0 == 'f') {
    अगर(s1 == 't' && s2 == 'p')
      वापस &Curl_scheme_ftp;
  }
  अन्यथा अगर(s0 == 'w') {
    अगर(s1 == 's' && s2 == 's')
      वापस &Curl_scheme_wss;
  }
  अन्यथा अगर(s0 == 's') {
    अगर(s1 == 'c' && s2 == 'p')
      वापस &Curl_scheme_scp;
  }
  वापस शून्य;
}

अटल स्थिर काष्ठा Curl_scheme *four_letter_scheme(स्थिर अक्षर *scheme)
{
  अक्षर s0 = Curl_raw_tolower(scheme[0]);
  अक्षर s1 = Curl_raw_tolower(scheme[1]);
  अक्षर s2 = Curl_raw_tolower(scheme[2]);
  अक्षर s3 = Curl_raw_tolower(scheme[3]);
  अगर(s3 == 'p') {
    अगर(s0 == 'h') {
      अगर(s1 == 't' && s2 == 't')
        वापस &Curl_scheme_http;
    }
    अन्यथा अगर(s0 == 'i') {
      अगर(s1 == 'm' && s2 == 'a')
        वापस &Curl_scheme_imap;
    }
    अन्यथा अगर(s0 == 'l') {
      अगर(s1 == 'd' && s2 == 'a')
        वापस &Curl_scheme_ldap;
    }
    अन्यथा अगर(s0 == 'r') {
      अगर(s1 == 't' && s2 == 's')
        वापस &Curl_scheme_rtsp;
    }
    अन्यथा अगर(s0 == 't') {
      अगर(s1 == 'f' && s2 == 't')
        वापस &Curl_scheme_tftp;
    }
    अन्यथा अगर(s0 == 's') {
      अगर(s1 == 'f' && s2 == 't')
        वापस &Curl_scheme_sftp;
      अगर(s1 == 'm' && s2 == 't')
        वापस &Curl_scheme_smtp;
    }
  }
  अन्यथा अगर(s0 == 'f') {
    अगर(s1 == 't' && s2 == 'p' && s3 == 's')
      वापस &Curl_scheme_ftps;
    अगर(s1 == 'i' && s2 == 'l' && s3 == 'e')
      वापस &Curl_scheme_file;
  }
  अन्यथा अगर(s0 == 'm') {
    अगर(s1 == 'q' && s2 == 't' && s3 == 't')
      वापस &Curl_scheme_mqtt;
  }
  अन्यथा अगर(s0 == 'p') {
    अगर(s1 == 'o' && s2 == 'p' && s3 == '3')
      वापस &Curl_scheme_pop3;
  }
  अन्यथा अगर(s0 == 'd') {
    अगर(s1 == 'i' && s2 == 'c' && s3 == 't')
      वापस &Curl_scheme_dict;
  }
  वापस शून्य;
}

अटल स्थिर काष्ठा Curl_scheme *five_letter_scheme(स्थिर अक्षर *scheme)
{
  अक्षर s4 = Curl_raw_tolower(scheme[4]);
  अगर(s4 == 's') {
    अक्षर s0 = Curl_raw_tolower(scheme[0]);
    अक्षर s1 = Curl_raw_tolower(scheme[1]);
    अक्षर s2 = Curl_raw_tolower(scheme[2]);
    अक्षर s3 = Curl_raw_tolower(scheme[3]);
    अगर(s3 == 'p') {
      चयन(s0) {
      हाल 'h':
        अगर(s1 == 't' && s2 == 't')
          वापस &Curl_scheme_https;
        अवरोध;
      हाल 'l':
        अगर(s1 == 'd' && s2 == 'a')
          वापस &Curl_scheme_ldaps;
        अवरोध;
      हाल 'i':
        अगर(s1 == 'm' && s2 == 'a')
          वापस &Curl_scheme_imaps;
        अवरोध;
      हाल 's':
        अगर(s1 == 'm' && s2 == 't')
          वापस &Curl_scheme_smtps;
        अवरोध;
      शेष:
        अवरोध;
      }
    }
    अन्यथा अगर(s0 == 'p') {
      अगर(s1 == 'o' && s2 == 'p' && s3 == '3')
        वापस &Curl_scheme_pop3s;
    }
    अन्यथा अगर(s0 == 'm') {
      अगर(s1 == 'q' && s2 == 't' && s3 == 't')
        वापस &Curl_scheme_mqtts;
    }
    अन्यथा अगर(s0 == 's') {
      अगर(s1 == 'o' && s2 == 'c' && s3 == 'k')
        वापस &Curl_scheme_socks;
    }
  }
  वापस शून्य;
}

अटल स्थिर काष्ठा Curl_scheme *six_letter_scheme(स्थिर अक्षर *scheme)
{
  अक्षर s0 = Curl_raw_tolower(scheme[0]);
  चयन(s0) {
  हाल 's':
    अगर(curl_strnequal("ocks4", &scheme[1], 5))
      वापस &Curl_scheme_socks4;
    अगर(curl_strnequal("ocks5", &scheme[1], 5))
      वापस &Curl_scheme_socks5;
    अवरोध;
  हाल 'g':
    अगर(curl_strnequal("opher", &scheme[1], 5))
      वापस &Curl_scheme_gopher;
    अवरोध;
  हाल 't':
    अगर(curl_strnequal("elnet", &scheme[1], 5))
      वापस &Curl_scheme_telnet;
    अवरोध;
  }
  वापस शून्य;
}

अटल स्थिर काष्ठा Curl_scheme *seven_letter_scheme(स्थिर अक्षर *scheme)
{
  अक्षर s0 = Curl_raw_tolower(scheme[0]);
  अगर(s0 == 's') {
    अगर(curl_strnequal("ocks4a", &scheme[1], 6))
      वापस &Curl_scheme_socks4a;
    अगर(curl_strnequal("ocks5h", &scheme[1], 6))
      वापस &Curl_scheme_socks5h;
  }
  अन्यथा अगर(s0 == 'g') {
    अगर(curl_strnequal("ophers", &scheme[1], 6))
      वापस &Curl_scheme_gophers;
  }
  वापस शून्य;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
स्थिर काष्ठा Curl_scheme *Curl_getn_scheme(स्थिर अक्षर *scheme, माप_प्रकार len)
{
  प्रकार स्थिर काष्ठा Curl_scheme *(*letterfunc)(स्थिर अक्षर *ptr);
  अटल स्थिर letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  अगर(len < 2 || len > 7)
    वापस शून्य;

  वापस parse[len - 2](scheme);
}

स्थिर काष्ठा Curl_scheme *Curl_get_scheme(स्थिर अक्षर *scheme)
{
  वापस Curl_getn_scheme(scheme, म_माप(scheme));
}
