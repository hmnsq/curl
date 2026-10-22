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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "protocol.h"
#అన్తర్భావ "strcase.h"

#అన్తర్భావ "dict.h"
#అన్తర్భావ "file.h"
#అన్తర్భావ "ftp.h"
#అన్తర్భావ "gopher.h"
#అన్తర్భావ "http.h"
#అన్తర్భావ "imap.h"
#అన్తర్భావ "curl_ldap.h"
#అన్తర్భావ "mqtt.h"
#అన్తర్భావ "pop3.h"
#అన్తర్భావ "rtsp.h"
#అన్తర్భావ "smtp.h"
#అన్తర్భావ "telnet.h"
#అన్తర్భావ "tftp.h"
#అన్తర్భావ "ws.h"
#అన్తర్భావ "vssh/ssh.h"


/* All URI schemes known to libcurl, but not necessarily implemented
 * by protocol handlers. */
స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_dict = {
  "dict",                               /* scheme */
#నిర్వచితమైతే CURL_DISABLE_DICT
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_dict,
#అయితే_ముగింపు
  CURLPROTO_DICT,                       /* protocol */
  CURLPROTO_DICT,                       /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  పోర్టు_DICT,                            /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_file = {
  "file",                               /* scheme */
#నిర్వచితమైతే CURL_DISABLE_FILE
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_file,
#అయితే_ముగింపు
  CURLPROTO_FILE,                       /* protocol */
  CURLPROTO_FILE,                       /* family */
  PROTOPT_NONETWORK | PROTOPT_NOURLQUERY, /* flags */
  0                                     /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ftp = {
  "ftp",                           /* scheme */
#నిర్వచితమైతే CURL_DISABLE_FTP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_ftp,
#అయితే_ముగింపు
  CURLPROTO_FTP,                   /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_DUAL | PROTOPT_CLOSEACTION | PROTOPT_NEEDSPWD |
  PROTOPT_NOURLQUERY | PROTOPT_PROXY_AS_HTTP |
  PROTOPT_WILDCARD | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE, /* flags */
  పోర్టు_FTP,                        /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ftps = {
  "ftps",                          /* scheme */
#ఒకవేళ defined(CURL_DISABLE_FTP) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_ftp,
#అయితే_ముగింపు
  CURLPROTO_FTPS,                  /* protocol */
  CURLPROTO_FTP,                   /* family */
  PROTOPT_SSL | PROTOPT_DUAL | PROTOPT_CLOSEACTION |
  PROTOPT_NEEDSPWD | PROTOPT_NOURLQUERY | PROTOPT_WILDCARD |
  PROTOPT_CONN_REUSE, /* flags */
  పోర్టు_FTPS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_gopher = {
  "gopher",                             /* scheme */
#నిర్వచితమైతే CURL_DISABLE_GOPHER
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_gopher,
#అయితే_ముగింపు
  CURLPROTO_GOPHER,                     /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_NONE,                         /* flags */
  పోర్టు_GOPHER,                          /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_gophers = {
  "gophers",                            /* scheme */
#ఒకవేళ defined(CURL_DISABLE_GOPHER) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_gophers,
#అయితే_ముగింపు
  CURLPROTO_GOPHERS,                    /* protocol */
  CURLPROTO_GOPHER,                     /* family */
  PROTOPT_SSL,                          /* flags */
  పోర్టు_GOPHER,                          /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_http = {
  "http",                               /* scheme */
#నిర్వచితమైతే CURL_DISABLE_HTTP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_http,
#అయితే_ముగింపు
  CURLPROTO_HTTP,                       /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE,
  పోర్టు_HTTP,                            /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_https = {
  "https",                              /* scheme */
#ఒకవేళ defined(CURL_DISABLE_HTTP) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_http,
#అయితే_ముగింపు
  CURLPROTO_HTTPS,                      /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | PROTOPT_ALPN | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_CONN_REUSE |
  PROTOPT_HTTP_PROXY_TUNNEL,
  పోర్టు_HTTPS,                           /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_imap = {
  "imap",                           /* scheme */
#నిర్వచితమైతే CURL_DISABLE_IMAP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_imap,
#అయితే_ముగింపు
  CURLPROTO_IMAP,                   /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION |             /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE |
  PROTOPT_CONN_REUSE,
  పోర్టు_IMAP,                        /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_imaps = {
  "imaps",                          /* scheme */
#ఒకవేళ defined(CURL_DISABLE_IMAP) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_imap,
#అయితే_ముగింపు
  CURLPROTO_IMAPS,                  /* protocol */
  CURLPROTO_IMAP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  పోర్టు_IMAPS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ldap = {
  "ldap",                               /* scheme */
#నిర్వచితమైతే CURL_DISABLE_LDAP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_ldap,
#అయితే_ముగింపు
  CURLPROTO_LDAP,                       /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL_REUSE,                    /* flags */
  పోర్టు_LDAP,                            /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ldaps = {
  "ldaps",                              /* scheme */
#ఒకవేళ defined(CURL_DISABLE_LDAP) || !defined(HAVE_LDAP_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_ldap,
#అయితే_ముగింపు
  CURLPROTO_LDAPS,                      /* protocol */
  CURLPROTO_LDAP,                       /* family */
  PROTOPT_SSL,                          /* flags */
  పోర్టు_LDAPS,                           /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_mqtt = {
  "mqtt",                             /* scheme */
#నిర్వచితమైతే CURL_DISABLE_MQTT
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_mqtt,
#అయితే_ముగింపు
  CURLPROTO_MQTT,                     /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_NONE,                       /* flags */
  పోర్టు_MQTT,                          /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_mqtts = {
  "mqtts",                            /* scheme */
#ఒకవేళ defined(CURL_DISABLE_MQTT) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_mqtts,
#అయితే_ముగింపు
  CURLPROTO_MQTTS,                    /* protocol */
  CURLPROTO_MQTT,                     /* family */
  PROTOPT_SSL,                        /* flags */
  పోర్టు_MQTTS,                         /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_pop3 = {
  "pop3",                           /* scheme */
#నిర్వచితమైతే CURL_DISABLE_POP3
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_pop3,
#అయితే_ముగింపు
  CURLPROTO_POP3,                   /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  పోర్టు_POP3,                        /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_pop3s = {
  "pop3s",                          /* scheme */
#ఒకవేళ defined(CURL_DISABLE_POP3) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_pop3,
#అయితే_ముగింపు
  CURLPROTO_POP3S,                  /* protocol */
  CURLPROTO_POP3,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  పోర్టు_POP3_S,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_rtsp = {
  "rtsp",                               /* scheme */
#నిర్వచితమైతే CURL_DISABLE_RTSP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_rtsp,
#అయితే_ముగింపు
  CURLPROTO_RTSP,                       /* protocol */
  CURLPROTO_RTSP,                       /* family */
  PROTOPT_CONN_REUSE,                   /* flags */
  పోర్టు_RTSP,                            /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_sftp = {
  "sftp",                               /* scheme */
#నిర్వచితం_కాకపోతే USE_SSH
  ఖాళీ,
#లేకపోతే
  &Curl_protocol_sftp,
#అయితే_ముగింపు
  CURLPROTO_SFTP,                       /* protocol */
  CURLPROTO_SFTP,                       /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  పోర్టు_SSH                              /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_scp = {
  "scp",                                /* scheme */
#నిర్వచితం_కాకపోతే USE_SSH
  ఖాళీ,
#లేకపోతే
  &Curl_protocol_scp,
#అయితే_ముగింపు
  CURLPROTO_SCP,                        /* protocol */
  CURLPROTO_SCP,                        /* family */
  PROTOPT_DIRLOCK | PROTOPT_CLOSEACTION | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_CONN_REUSE,
  పోర్టు_SSH,                             /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_smtp = {
  "smtp",                           /* scheme */
#నిర్వచితమైతే CURL_DISABLE_SMTP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_smtp,
#అయితే_ముగింపు
  CURLPROTO_SMTP,                   /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_NOURLQUERY | /* flags */
  PROTOPT_URLOPTIONS | PROTOPT_SSL_REUSE | PROTOPT_CONN_REUSE,
  పోర్టు_SMTP,                        /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_smtps = {
  "smtps",                          /* scheme */
#ఒకవేళ defined(CURL_DISABLE_SMTP) || !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_smtp,
#అయితే_ముగింపు
  CURLPROTO_SMTPS,                  /* protocol */
  CURLPROTO_SMTP,                   /* family */
  PROTOPT_CLOSEACTION | PROTOPT_SSL | /* flags */
  PROTOPT_NOURLQUERY | PROTOPT_URLOPTIONS | PROTOPT_CONN_REUSE,
  పోర్టు_SMTPS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks = {
  "socks",                          /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  పోర్టు_SOCKS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks4 = {
  "socks4",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  పోర్టు_SOCKS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks4a = {
  "socks4a",                        /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  పోర్టు_SOCKS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks5 = {
  "socks5",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  పోర్టు_SOCKS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_socks5h = {
  "socks5h",                         /* scheme */
  ZERO_NULL,
  CURLPROTO_SOCKS,                  /* protocol */
  CURLPROTO_SOCKS,                  /* family */
  PROTOPT_NO_TRANSFER,              /* flags */
  పోర్టు_SOCKS,                       /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_telnet = {
  "telnet",                             /* scheme */
#నిర్వచితమైతే CURL_DISABLE_TELNET
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_telnet,
#అయితే_ముగింపు
  CURLPROTO_TELNET,                     /* protocol */
  CURLPROTO_TELNET,                     /* family */
  PROTOPT_NONE | PROTOPT_NOURLQUERY,    /* flags */
  పోర్టు_TELNET,                          /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_tftp = {
  "tftp",                               /* scheme */
#నిర్వచితమైతే CURL_DISABLE_TFTP
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_tftp,
#అయితే_ముగింపు
  CURLPROTO_TFTP,                       /* protocol */
  CURLPROTO_TFTP,                       /* family */
  PROTOPT_NOTCPPROXY | PROTOPT_NOURLQUERY, /* flags */
  పోర్టు_TFTP,                            /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_ws = {
  "ws",                                 /* scheme */
#ఒకవేళ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_ws,
#అయితే_ముగింపు
  CURLPROTO_WS,                         /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_CREDSPERREQUEST |             /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  పోర్టు_HTTP                             /* defport */
};

స్థిరరాశి నిర్మాణం Curl_scheme Curl_scheme_wss = {
  "wss",                                /* scheme */
#ఒకవేళ defined(CURL_DISABLE_WEBSOCKETS) || defined(CURL_DISABLE_HTTP) || \
  !defined(USE_SSL)
  ZERO_NULL,
#లేకపోతే
  &Curl_protocol_ws,
#అయితే_ముగింపు
  CURLPROTO_WSS,                        /* protocol */
  CURLPROTO_HTTP,                       /* family */
  PROTOPT_SSL | PROTOPT_CREDSPERREQUEST | /* flags */
  PROTOPT_USERPWDCTRL | PROTOPT_HTTP_PROXY_TUNNEL,
  పోర్టు_HTTPS                            /* defport */
};

స్థిర స్థిరరాశి నిర్మాణం Curl_scheme *two_letter_scheme(స్థిరరాశి అక్షరం *scheme)
{
  ఒకవేళ((Curl_raw_tolower(scheme[0]) == 'w') &&
     (Curl_raw_tolower(scheme[1]) == 's'))
    తిరిగి &Curl_scheme_ws;
  తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_scheme *three_letter_scheme(స్థిరరాశి అక్షరం *scheme)
{
  అక్షరం s0 = Curl_raw_tolower(scheme[0]);
  అక్షరం s1 = Curl_raw_tolower(scheme[1]);
  అక్షరం s2 = Curl_raw_tolower(scheme[2]);
  ఒకవేళ(s0 == 'f') {
    ఒకవేళ(s1 == 't' && s2 == 'p')
      తిరిగి &Curl_scheme_ftp;
  }
  లేకపోతే ఒకవేళ(s0 == 'w') {
    ఒకవేళ(s1 == 's' && s2 == 's')
      తిరిగి &Curl_scheme_wss;
  }
  లేకపోతే ఒకవేళ(s0 == 's') {
    ఒకవేళ(s1 == 'c' && s2 == 'p')
      తిరిగి &Curl_scheme_scp;
  }
  తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_scheme *four_letter_scheme(స్థిరరాశి అక్షరం *scheme)
{
  అక్షరం s0 = Curl_raw_tolower(scheme[0]);
  అక్షరం s1 = Curl_raw_tolower(scheme[1]);
  అక్షరం s2 = Curl_raw_tolower(scheme[2]);
  అక్షరం s3 = Curl_raw_tolower(scheme[3]);
  ఒకవేళ(s3 == 'p') {
    ఒకవేళ(s0 == 'h') {
      ఒకవేళ(s1 == 't' && s2 == 't')
        తిరిగి &Curl_scheme_http;
    }
    లేకపోతే ఒకవేళ(s0 == 'i') {
      ఒకవేళ(s1 == 'm' && s2 == 'a')
        తిరిగి &Curl_scheme_imap;
    }
    లేకపోతే ఒకవేళ(s0 == 'l') {
      ఒకవేళ(s1 == 'd' && s2 == 'a')
        తిరిగి &Curl_scheme_ldap;
    }
    లేకపోతే ఒకవేళ(s0 == 'r') {
      ఒకవేళ(s1 == 't' && s2 == 's')
        తిరిగి &Curl_scheme_rtsp;
    }
    లేకపోతే ఒకవేళ(s0 == 't') {
      ఒకవేళ(s1 == 'f' && s2 == 't')
        తిరిగి &Curl_scheme_tftp;
    }
    లేకపోతే ఒకవేళ(s0 == 's') {
      ఒకవేళ(s1 == 'f' && s2 == 't')
        తిరిగి &Curl_scheme_sftp;
      ఒకవేళ(s1 == 'm' && s2 == 't')
        తిరిగి &Curl_scheme_smtp;
    }
  }
  లేకపోతే ఒకవేళ(s0 == 'f') {
    ఒకవేళ(s1 == 't' && s2 == 'p' && s3 == 's')
      తిరిగి &Curl_scheme_ftps;
    ఒకవేళ(s1 == 'i' && s2 == 'l' && s3 == 'e')
      తిరిగి &Curl_scheme_file;
  }
  లేకపోతే ఒకవేళ(s0 == 'm') {
    ఒకవేళ(s1 == 'q' && s2 == 't' && s3 == 't')
      తిరిగి &Curl_scheme_mqtt;
  }
  లేకపోతే ఒకవేళ(s0 == 'p') {
    ఒకవేళ(s1 == 'o' && s2 == 'p' && s3 == '3')
      తిరిగి &Curl_scheme_pop3;
  }
  లేకపోతే ఒకవేళ(s0 == 'd') {
    ఒకవేళ(s1 == 'i' && s2 == 'c' && s3 == 't')
      తిరిగి &Curl_scheme_dict;
  }
  తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_scheme *five_letter_scheme(స్థిరరాశి అక్షరం *scheme)
{
  అక్షరం s4 = Curl_raw_tolower(scheme[4]);
  ఒకవేళ(s4 == 's') {
    అక్షరం s0 = Curl_raw_tolower(scheme[0]);
    అక్షరం s1 = Curl_raw_tolower(scheme[1]);
    అక్షరం s2 = Curl_raw_tolower(scheme[2]);
    అక్షరం s3 = Curl_raw_tolower(scheme[3]);
    ఒకవేళ(s3 == 'p') {
      ఎంపిక(s0) {
      స్థితి 'h':
        ఒకవేళ(s1 == 't' && s2 == 't')
          తిరిగి &Curl_scheme_https;
        ఆపు;
      స్థితి 'l':
        ఒకవేళ(s1 == 'd' && s2 == 'a')
          తిరిగి &Curl_scheme_ldaps;
        ఆపు;
      స్థితి 'i':
        ఒకవేళ(s1 == 'm' && s2 == 'a')
          తిరిగి &Curl_scheme_imaps;
        ఆపు;
      స్థితి 's':
        ఒకవేళ(s1 == 'm' && s2 == 't')
          తిరిగి &Curl_scheme_smtps;
        ఆపు;
      అప్రమేయ:
        ఆపు;
      }
    }
    లేకపోతే ఒకవేళ(s0 == 'p') {
      ఒకవేళ(s1 == 'o' && s2 == 'p' && s3 == '3')
        తిరిగి &Curl_scheme_pop3s;
    }
    లేకపోతే ఒకవేళ(s0 == 'm') {
      ఒకవేళ(s1 == 'q' && s2 == 't' && s3 == 't')
        తిరిగి &Curl_scheme_mqtts;
    }
    లేకపోతే ఒకవేళ(s0 == 's') {
      ఒకవేళ(s1 == 'o' && s2 == 'c' && s3 == 'k')
        తిరిగి &Curl_scheme_socks;
    }
  }
  తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_scheme *six_letter_scheme(స్థిరరాశి అక్షరం *scheme)
{
  అక్షరం s0 = Curl_raw_tolower(scheme[0]);
  ఎంపిక(s0) {
  స్థితి 's':
    ఒకవేళ(curl_strnequal("ocks4", &scheme[1], 5))
      తిరిగి &Curl_scheme_socks4;
    ఒకవేళ(curl_strnequal("ocks5", &scheme[1], 5))
      తిరిగి &Curl_scheme_socks5;
    ఆపు;
  స్థితి 'g':
    ఒకవేళ(curl_strnequal("opher", &scheme[1], 5))
      తిరిగి &Curl_scheme_gopher;
    ఆపు;
  స్థితి 't':
    ఒకవేళ(curl_strnequal("elnet", &scheme[1], 5))
      తిరిగి &Curl_scheme_telnet;
    ఆపు;
  }
  తిరిగి ఖాళీ;
}

స్థిర స్థిరరాశి నిర్మాణం Curl_scheme *seven_letter_scheme(స్థిరరాశి అక్షరం *scheme)
{
  అక్షరం s0 = Curl_raw_tolower(scheme[0]);
  ఒకవేళ(s0 == 's') {
    ఒకవేళ(curl_strnequal("ocks4a", &scheme[1], 6))
      తిరిగి &Curl_scheme_socks4a;
    ఒకవేళ(curl_strnequal("ocks5h", &scheme[1], 6))
      తిరిగి &Curl_scheme_socks5h;
  }
  లేకపోతే ఒకవేళ(s0 == 'g') {
    ఒకవేళ(curl_strnequal("ophers", &scheme[1], 6))
      తిరిగి &Curl_scheme_gophers;
  }
  తిరిగి ఖాళీ;
}

/* Returns a struct scheme pointer if the name is a known scheme. Check the
   ->run struct field for non-NULL to figure out if an implementation is
   present. */
స్థిరరాశి నిర్మాణం Curl_scheme *Curl_getn_scheme(స్థిరరాశి అక్షరం *scheme, size_t len)
{
  రకనిర్వచనం స్థిరరాశి నిర్మాణం Curl_scheme *(*letterfunc)(స్థిరరాశి అక్షరం *ptr);
  స్థిర స్థిరరాశి letterfunc parse[] = {
    two_letter_scheme,
    three_letter_scheme,
    four_letter_scheme,
    five_letter_scheme,
    six_letter_scheme,
    seven_letter_scheme
  };

  ఒకవేళ(len < 2 || len > 7)
    తిరిగి ఖాళీ;

  తిరిగి parse[len - 2](scheme);
}

స్థిరరాశి నిర్మాణం Curl_scheme *Curl_get_scheme(స్థిరరాశి అక్షరం *scheme)
{
  తిరిగి Curl_getn_scheme(scheme, అక్షరశ్రేణి_పొడవు(scheme));
}
