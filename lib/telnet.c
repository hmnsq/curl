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
#अन्तर्भाव "urldata.h"
#अन्तर्भाव "telnet.h"

#यदि_अपरिभाषितम् CURL_DISABLE_TELNET

#यदि_परिभाषितम् HAVE_NETINET_अन्तर्_H
#अन्तर्भाव <netinet/in.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NETDB_H
#अन्तर्भाव <netdb.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_ARPA_INET_H
#अन्तर्भाव <arpa/inet.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_NET_IF_H
#अन्तर्भाव <net/if.h>
#यदि_अन्तः
#यदि_परिभाषितम् HAVE_SYS_IOCTL_H
#अन्तर्भाव <sys/ioctl.h>
#यदि_अन्तः

#यदि_परिभाषितम् HAVE_SYS_PARAM_H
#अन्तर्भाव <sys/param.h>
#यदि_अन्तः

#अन्तर्भाव "url.h"
#अन्तर्भाव "transfer.h"
#अन्तर्भाव "sendf.h"
#अन्तर्भाव "curl_trc.h"
#अन्तर्भाव "progress.h"
#अन्तर्भाव "arpa_telnet.h"
#अन्तर्भाव "connect.h"
#अन्तर्भाव "select.h"
#अन्तर्भाव "curlx/strparse.h"

#परिभाषय SUBBUFSIZE 512

#परिभाषय CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#परिभाषय CURL_SB_पदांशः(x)            \
  कुरु {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } यावत्(0)
#परिभाषय CURL_SB_ACCUM(x, c)                                         \
  कुरु {                                                              \
    यदि((x)->subpointer < ((x)->subbuffer + परिमाण((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } यावत्(0)

#परिभाषय CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#परिभाषय CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#परिभाषय CURL_NO          0
#परिभाषय CURL_YES         1
#परिभाषय CURL_WANTYES     2
#परिभाषय CURL_WANTNO      3

#परिभाषय CURL_रिक्तानि       0
#परिभाषय CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#परिभाषय CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
प्रकारनिर्देश गणना {
  CURL_TS_दत्त = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

संरचना TELNET {
  पूर्णाङ्क please_negotiate;
  पूर्णाङ्क already_negotiated;
  पूर्णाङ्क us[256];
  पूर्णाङ्क usq[256];
  पूर्णाङ्क us_preferred[256];
  पूर्णाङ्क him[256];
  पूर्णाङ्क himq[256];
  पूर्णाङ्क him_preferred[256];
  पूर्णाङ्क subnegotiation[256];
  ध्रुव वर्ण *subopt_ttype;          /* Set with suboption TTYPE */
  ध्रुव वर्ण *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  अचिह्नित ह्रस्व subopt_wsx;         /* Set with suboption NAWS */
  अचिह्नित ह्रस्व subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  संरचना curl_slist *telnet_vars;    /* Environment variables */
  संरचना dynbuf बहिर्गमनम्;                 /* output buffer */

  /* suboptions */
  अचिह्नित वर्ण subbuffer[SUBBUFSIZE];
  अचिह्नित वर्ण *subpointer, *subend;      /* buffer for sub-options */
};

#यदि_अपरिभाषितम् CURLVERBOSE
#परिभाषय printoption(a, b, c, d) Curl_nop_stmt
#अन्यथा
स्थिर शून्य printoption(संरचना Curl_easy *दत्त,
                        ध्रुव वर्ण *direction, पूर्णाङ्क cmd, पूर्णाङ्क option)
{
  यदि(दत्त->समुच्चयः.verbose) {
    यदि(cmd == CURL_IAC) {
      यदि(CURL_TELCMD_OK(option))
        infof(दत्त, "%s IAC %s", direction, CURL_TELCMD(option));
      अन्यथा
        infof(दत्त, "%s IAC %d", direction, option);
    }
    अन्यथा {
      ध्रुव वर्ण *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      यदि(fmt) {
        ध्रुव वर्ण *opt;
        यदि(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        अन्यथा यदि(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        अन्यथा
          opt = रिक्त;

        यदि(opt)
          infof(दत्त, "%s %s %s", direction, fmt, opt);
        अन्यथा
          infof(दत्त, "%s %s %d", direction, fmt, option);
      }
      अन्यथा
        infof(दत्त, "%s %d %d", direction, cmd, option);
    }
  }
}
#यदि_अन्तः /* !CURLVERBOSE */

स्थिर शून्य telnet_easy_dtor(ध्रुव शून्य *कुञ्चिका, size_t klen, शून्य *प्रविष्टिः)
{
  संरचना TELNET *tn = प्रविष्टिः;
  (शून्य)कुञ्चिका;
  (शून्य)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->बहिर्गमनम्);
  curlx_free(tn);
}

स्थिर CURLcode init_telnet(संरचना Curl_easy *दत्त)
{
  संरचना TELNET *tn;

  tn = curlx_calloc(1, परिमाण(संरचना TELNET));
  यदि(!tn)
    प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;

  curlx_dyn_init(&tn->बहिर्गमनम्, 0xffff);

  tn->telrcv_state = CURL_TS_दत्त;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_द्विमान] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_द्विमान] = CURL_YES;

  /* We must allow the server to echo what we sent but it is not necessary
     to request the server to do so (it might force the server to close
     the connection). Hence, we ignore ECHO in the negotiate function */
  tn->him_preferred[CURL_TELOPT_ECHO] = CURL_YES;

  /* Set the subnegotiation fields to send information after negotiation
     passed (do/will)

     Default values are (0,0) initialized by calloc.
     According to the RFC1013 it is valid:
     A value equal to zero is acceptable for the width (or height), and means
     that no character width (or height) is being sent. In this case, the width
     (or height) that will be assumed by the Telnet server is operating system
     specific (it will probably be based upon the terminal type information
     that may have been sent using the TERMINAL TYPE Telnet option). */
  tn->subnegotiation[CURL_TELOPT_NAWS] = CURL_YES;

  प्रत्यागम Curl_meta_समुच्चयः(दत्त, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

स्थिर शून्य send_negotiation(संरचना Curl_easy *दत्त, पूर्णाङ्क cmd, पूर्णाङ्क option)
{
  अचिह्नित वर्ण buf[3];
  size_t nwritten;
  CURLcode परिणाम;

  buf[0] = CURL_IAC;
  buf[1] = (अचिह्नित वर्ण)cmd;
  buf[2] = (अचिह्नित वर्ण)option;

  परिणाम = Curl_xfer_send(दत्त, buf, 3, FALSE, &nwritten);
  यदि(परिणाम)
    failf(दत्त, "Sending data failed: %s", curl_easy_strerror(परिणाम));

  printoption(दत्त, "SENT", cmd, option);
}

स्थिर शून्य समुच्चयः_remote_option(संरचना Curl_easy *दत्त, संरचना TELNET *tn,
                              पूर्णाङ्क option, पूर्णाङ्क newstate)
{
  यदि(newstate == CURL_YES) {
    विकल्प(tn->him[option]) {
    पक्ष CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(दत्त, CURL_DO, option);
      भङ्ग;

    पक्ष CURL_YES:
      /* Already enabled */
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_रिक्तानि:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        /* Error: already queued an enable request */
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_रिक्तानि:
        /* Error: already negotiating for enable */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->himq[option] = CURL_रिक्तानि;
        भङ्ग;
      }
      भङ्ग;
    }
  }
  अन्यथा { /* NO */
    विकल्प(tn->him[option]) {
    पक्ष CURL_NO:
      /* Already disabled */
      भङ्ग;

    पक्ष CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(दत्त, CURL_DONT, option);
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_रिक्तानि:
        /* Already negotiating for NO */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->himq[option] = CURL_रिक्तानि;
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->himq[option]) {
      पक्ष CURL_रिक्तानि:
        tn->himq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        भङ्ग;
      }
      भङ्ग;
    }
  }
}

स्थिर शून्य समुच्चयः_local_option(संरचना Curl_easy *दत्त, संरचना TELNET *tn,
                             पूर्णाङ्क option, पूर्णाङ्क newstate)
{
  यदि(newstate == CURL_YES) {
    विकल्प(tn->us[option]) {
    पक्ष CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(दत्त, CURL_WILL, option);
      भङ्ग;

    पक्ष CURL_YES:
      /* Already enabled */
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_रिक्तानि:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        /* Error: already queued an enable request */
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_रिक्तानि:
        /* Error: already negotiating for enable */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->usq[option] = CURL_रिक्तानि;
        भङ्ग;
      }
      भङ्ग;
    }
  }
  अन्यथा { /* NO */
    विकल्प(tn->us[option]) {
    पक्ष CURL_NO:
      /* Already disabled */
      भङ्ग;

    पक्ष CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(दत्त, CURL_WONT, option);
      भङ्ग;

    पक्ष CURL_WANTNO:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_रिक्तानि:
        /* Already negotiating for NO */
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        tn->usq[option] = CURL_रिक्तानि;
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_WANTYES:
      विकल्प(tn->usq[option]) {
      पक्ष CURL_रिक्तानि:
        tn->usq[option] = CURL_OPPOSITE;
        भङ्ग;
      पक्ष CURL_OPPOSITE:
        भङ्ग;
      }
      भङ्ग;
    }
  }
}

स्थिर शून्य telnet_negotiate(संरचना Curl_easy *दत्त, संरचना TELNET *tn)
{
  पूर्णाङ्क i;

  प्रति(i = 0; i < CURL_NTELOPTS; i++) {
    यदि(i == CURL_TELOPT_ECHO)
      अनुवर्त;

    यदि(tn->us_preferred[i] == CURL_YES)
      समुच्चयः_local_option(दत्त, tn, i, CURL_YES);

    यदि(tn->him_preferred[i] == CURL_YES)
      समुच्चयः_remote_option(दत्त, tn, i, CURL_YES);
  }
}

स्थिर शून्य rec_will(संरचना Curl_easy *दत्त, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->him[option]) {
  पक्ष CURL_NO:
    यदि(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(दत्त, CURL_DO, option);
    }
    अन्यथा
      send_negotiation(दत्त, CURL_DONT, option);

    भङ्ग;

  पक्ष CURL_YES:
    /* Already enabled */
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_रिक्तानि:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_रिक्तानि;
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_रिक्तानि:
      tn->him[option] = CURL_YES;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_रिक्तानि;
      send_negotiation(दत्त, CURL_DONT, option);
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर शून्य rec_wont(संरचना Curl_easy *दत्त, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->him[option]) {
  पक्ष CURL_NO:
    /* Already disabled */
    भङ्ग;

  पक्ष CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(दत्त, CURL_DONT, option);
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_रिक्तानि:
      tn->him[option] = CURL_NO;
      भङ्ग;

    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_रिक्तानि;
      send_negotiation(दत्त, CURL_DO, option);
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->himq[option]) {
    पक्ष CURL_रिक्तानि:
      tn->him[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_रिक्तानि;
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर शून्य printsub(संरचना Curl_easy *दत्त,
                     पूर्णाङ्क direction,                /* '<' or '>' */
                     ध्रुव अचिह्नित वर्ण *pointer, /* ptr to suboption data */
                     size_t दीर्घता)                /* suboption data length */
{
  यदि(दत्त->समुच्चयः.verbose) {
    अचिह्नित पूर्णाङ्क i = 0;
    यदि(direction) {
      infof(दत्त, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      यदि(दीर्घता >= 3) {
        पूर्णाङ्क j;

        i = pointer[दीर्घता - 2];
        j = pointer[दीर्घता - 1];

        यदि(i != CURL_IAC || j != CURL_SE) {
          infof(दत्त, "(terminated by ");
          यदि(CURL_TELOPT_OK(i))
            infof(दत्त, "%s ", CURL_TELOPT(i));
          अन्यथा यदि(CURL_TELCMD_OK(i))
            infof(दत्त, "%s ", CURL_TELCMD(i));
          अन्यथा
            infof(दत्त, "%u ", i);
          यदि(CURL_TELOPT_OK(j))
            infof(दत्त, "%s", CURL_TELOPT(j));
          अन्यथा यदि(CURL_TELCMD_OK(j))
            infof(दत्त, "%s", CURL_TELCMD(j));
          अन्यथा
            infof(दत्त, "%d", j);
          infof(दत्त, ", not IAC SE) ");
        }
      }
      यदि(दीर्घता >= 2)
        दीर्घता -= 2;
      अन्यथा /* bad input */
        प्रत्यागम;
    }
    यदि(दीर्घता <= 1) {
      infof(दत्त, "(Empty suboption?)");
      प्रत्यागम;
    }

    यदि(CURL_TELOPT_OK(pointer[0])) {
      विकल्प(pointer[0]) {
      पक्ष CURL_TELOPT_TTYPE:
      पक्ष CURL_TELOPT_XDISPLOC:
      पक्ष CURL_TELOPT_NEW_ENVIRON:
      पक्ष CURL_TELOPT_NAWS:
        infof(दत्त, "%s", CURL_TELOPT(pointer[0]));
        भङ्ग;
      स्वभाव:
        infof(दत्त, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        भङ्ग;
      }
    }
    अन्यथा
      infof(दत्त, "%d (unknown)", pointer[0]);

    विकल्प(pointer[0]) {
    पक्ष CURL_TELOPT_NAWS:
      यदि(दीर्घता > 4)
        infof(दत्त, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      भङ्ग;
    स्वभाव:
      विकल्प(pointer[1]) {
      पक्ष CURL_TELQUAL_IS:
        infof(दत्त, " IS");
        भङ्ग;
      पक्ष CURL_TELQUAL_SEND:
        infof(दत्त, " SEND");
        भङ्ग;
      पक्ष CURL_TELQUAL_INFO:
        infof(दत्त, " INFO/REPLY");
        भङ्ग;
      पक्ष CURL_TELQUAL_NAME:
        infof(दत्त, " NAME");
        भङ्ग;
      }

      विकल्प(pointer[0]) {
      पक्ष CURL_TELOPT_TTYPE:
      पक्ष CURL_TELOPT_XDISPLOC:
        infof(दत्त, " \"%.*s\"",
              (पूर्णाङ्क)((दीर्घता > 2) ? (दीर्घता - 2) : 0), &pointer[2]);
        भङ्ग;
      पक्ष CURL_TELOPT_NEW_ENVIRON:
        यदि(pointer[1] == CURL_TELQUAL_IS) {
          infof(दत्त, " ");
          प्रति(i = 3; i < दीर्घता; i++) {
            विकल्प(pointer[i]) {
            पक्ष CURL_NEW_ENV_VAR:
              infof(दत्त, ", ");
              भङ्ग;
            पक्ष CURL_NEW_ENV_मानम्:
              infof(दत्त, " = ");
              भङ्ग;
            स्वभाव:
              infof(दत्त, "%c", pointer[i]);
              भङ्ग;
            }
          }
        }
        भङ्ग;
      स्वभाव:
        प्रति(i = 2; i < दीर्घता; i++)
          infof(दत्त, " %.2x", pointer[i]);
        भङ्ग;
      }
    }
  }
}

/* Escape and send a telnet data block */
स्थिर CURLcode send_telnet_दत्त(संरचना Curl_easy *दत्त,
                                 संरचना TELNET *tn,
                                 ध्रुव वर्ण *बफर, ssize_t nread)
{
  size_t i, outlen;
  ध्रुव अचिह्नित वर्ण *outbuf;
  CURLcode परिणाम = CURLE_OK;
  size_t अष्टकानि_written;
  size_t योग_written = 0;
  संरचना connectdata *conn = दत्त->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  यदि(nread < 0)
    प्रत्यागम CURLE_TOO_LARGE;

  यदि(memchr(बफर, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->बहिर्गमनम्);

    प्रति(i = 0; i < (size_t)nread && !परिणाम; i++) {
      परिणाम = curlx_dyn_addn(&tn->बहिर्गमनम्, &बफर[i], 1);
      यदि(!परिणाम && ((अचिह्नित वर्ण)बफर[i] == CURL_IAC))
        /* IAC is FF in hex */
        परिणाम = curlx_dyn_addn(&tn->बहिर्गमनम्, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->बहिर्गमनम्);
    outbuf = curlx_dyn_uptr(&tn->बहिर्गमनम्);
  }
  अन्यथा {
    outlen = (size_t)nread;
    outbuf = (ध्रुव अचिह्नित वर्ण *)बफर;
  }
  यावत्(!परिणाम && योग_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    संरचना pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(दत्त);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    यदि(timeout_ms < 0)
      प्रत्यागम CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    विकल्प(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    पक्ष -1:                    /* error, abort writing */
      परिणाम = CURLE_SEND_ERROR;
      भङ्ग;
    पक्ष 0:                     /* timeout */
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      भङ्ग;
    स्वभाव:                    /* write! */
      अष्टकानि_written = 0;
      परिणाम = Curl_xfer_send(दत्त, outbuf + योग_written,
                              outlen - योग_written, FALSE, &अष्टकानि_written);
      योग_written += अष्टकानि_written;
      भङ्ग;
    }
  }

  प्रत्यागम परिणाम;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
स्थिर शून्य sendsuboption(संरचना Curl_easy *दत्त,
                          संरचना TELNET *tn, पूर्णाङ्क option)
{
  size_t nwritten;
  अचिह्नित ह्रस्व x, y;
  ध्रुव अचिह्नित वर्ण *uc1, *uc2;
  CURLcode परिणाम;

  विकल्प(option) {
  पक्ष CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ध्रुव अचिह्नित वर्ण *)&x;
    uc2 = (ध्रुव अचिह्नित वर्ण *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_पदांशः(tn);
    /* data suboption is now ready */

    printsub(दत्त, '>', (ध्रुव अचिह्नित वर्ण *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    परिणाम = Curl_xfer_send(दत्त, tn->subbuffer, 3, FALSE, &nwritten);
    यदि(परिणाम)
      failf(दत्त, "Sending data failed: %s", curl_easy_strerror(परिणाम));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_दत्त(दत्त, tn, (ध्रुव वर्ण *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    परिणाम = Curl_xfer_send(दत्त, tn->subbuffer + 7, 2, FALSE, &nwritten);
    यदि(परिणाम)
      failf(दत्त, "Sending data failed: %s", curl_easy_strerror(परिणाम));
    भङ्ग;
  }
}

स्थिर शून्य rec_do(संरचना Curl_easy *दत्त, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->us[option]) {
  पक्ष CURL_NO:
    यदि(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(दत्त, CURL_WILL, option);
      यदि(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(दत्त, tn, option);
    }
    अन्यथा यदि(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(दत्त, CURL_WILL, option);
      sendsuboption(दत्त, tn, option);
    }
    अन्यथा
      send_negotiation(दत्त, CURL_WONT, option);
    भङ्ग;

  पक्ष CURL_YES:
    /* Already enabled */
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_रिक्तानि:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_रिक्तानि;
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_रिक्तानि:
      tn->us[option] = CURL_YES;
      यदि(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(दत्त, tn, option);
      }
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_रिक्तानि;
      send_negotiation(दत्त, CURL_WONT, option);
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर शून्य rec_dont(संरचना Curl_easy *दत्त, संरचना TELNET *tn, पूर्णाङ्क option)
{
  विकल्प(tn->us[option]) {
  पक्ष CURL_NO:
    /* Already disabled */
    भङ्ग;

  पक्ष CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(दत्त, CURL_WONT, option);
    भङ्ग;

  पक्ष CURL_WANTNO:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_रिक्तानि:
      tn->us[option] = CURL_NO;
      भङ्ग;

    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_रिक्तानि;
      send_negotiation(दत्त, CURL_WILL, option);
      भङ्ग;
    }
    भङ्ग;

  पक्ष CURL_WANTYES:
    विकल्प(tn->usq[option]) {
    पक्ष CURL_रिक्तानि:
      tn->us[option] = CURL_NO;
      भङ्ग;
    पक्ष CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_रिक्तानि;
      भङ्ग;
    }
    भङ्ग;
  }
}

स्थिर bool str_is_nonascii(ध्रुव वर्ण *str)
{
  वर्ण c;
  यावत्((c = *str++) != 0)
    यदि(c & 0x80)
      प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

स्थिर CURLcode check_telnet_options(संरचना Curl_easy *दत्त,
                                     संरचना TELNET *tn)
{
  संरचना curl_slist *शीर्ष;
  संरचना curl_slist *beg;
  CURLcode परिणाम = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  यदि(दत्त->state.creds) {
    वर्ण बफर[256];
    यदि(str_is_nonascii(Curl_creds_user(दत्त->conn->creds))) {
      DEBUGF(infof(दत्त, "set a non ASCII username in telnet"));
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    }
    curl_msnprintf(बफर, परिमाण(बफर), "USER,%s",
                   Curl_creds_user(दत्त->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, बफर);
    यदि(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = रिक्त;
      प्रत्यागम CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  प्रति(शीर्ष = दत्त->समुच्चयः.telnet_options; शीर्ष && !परिणाम; शीर्ष = शीर्ष->अग्र) {
    size_t olen;
    ध्रुव वर्ण *option = शीर्ष->दत्त;
    ध्रुव वर्ण *arg;
    ध्रुव वर्ण *sep = strchr(option, '=');
    यदि(sep) {
      olen = sep - option;
      arg = ++sep;
      यदि(str_is_nonascii(arg))
        अनुवर्त;
      विकल्प(olen) {
      पक्ष 5:
        /* Terminal type */
        यदि(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          भङ्ग;
        }
        परिणाम = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 8:
        /* Display variable */
        यदि(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          भङ्ग;
        }
        परिणाम = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 7:
        /* Environment variable */
        यदि(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          यदि(!beg) {
            परिणाम = CURLE_बहिर्गमनम्_सम्बन्धि_स्मृतिः;
            भङ्ग;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        अन्यथा
          परिणाम = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 2:
        /* Window Size */
        यदि(curl_strnequal(option, "WS", 2)) {
          ध्रुव वर्ण *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          यदि(curlx_str_सङ्ख्या(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_सङ्ख्या(&p, &y, 0xffff)) {
            failf(दत्त, "Syntax error in telnet option: %s", शीर्ष->दत्त);
            परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
          }
          अन्यथा {
            tn->subopt_wsx = (अचिह्नित ह्रस्व)x;
            tn->subopt_wsy = (अचिह्नित ह्रस्व)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        अन्यथा
          परिणाम = CURLE_UNKNOWN_OPTION;
        भङ्ग;

      पक्ष 6:
        /* To take care or not of the 8th bit in data exchange */
        यदि(curl_strnequal(option, "BINARY", 6)) {
          ध्रुव वर्ण *p = arg;
          curl_off_t द्विमान_option;
          यदि(!curlx_str_सङ्ख्या(&p, &द्विमान_option, 1) &&
             (द्विमान_option != 1)) {
            tn->us_preferred[CURL_TELOPT_द्विमान] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_द्विमान] = CURL_NO;
          }
        }
        अन्यथा
          परिणाम = CURLE_UNKNOWN_OPTION;
        भङ्ग;
      स्वभाव:
        failf(दत्त, "Unknown telnet option %s", शीर्ष->दत्त);
        परिणाम = CURLE_UNKNOWN_OPTION;
        भङ्ग;
      }
    }
    अन्यथा {
      failf(दत्त, "Syntax error in telnet option: %s", शीर्ष->दत्त);
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  यदि(परिणाम) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = रिक्त;
  }

  प्रत्यागम परिणाम;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
स्थिर bool bad_option(ध्रुव वर्ण *दत्त)
{
  प्रत्यागम !दत्त || !!strchr(दत्त, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
स्थिर CURLcode suboption(संरचना Curl_easy *दत्त, संरचना TELNET *tn)
{
  संरचना curl_slist *v;
  अचिह्नित वर्ण अस्थायी[2048];
  size_t nwritten;
  size_t len;
  CURLcode परिणाम = CURLE_OK;

  यदि(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    प्रत्यागम CURLE_OK;

  printsub(दत्त, '<', (ध्रुव अचिह्नित वर्ण *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  विकल्प(CURL_SB_GET(tn)) {
  पक्ष CURL_TELOPT_TTYPE:
    यदि(bad_option(tn->subopt_ttype))
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    यदि(वाक्यदैर्घ्यम्(tn->subopt_ttype) > 1000) {
      failf(दत्त, "Too long telnet TTYPE");
      प्रत्यागम CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((वर्ण *)अस्थायी, परिमाण(अस्थायी), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    परिणाम = Curl_xfer_send(दत्त, अस्थायी, len, FALSE, &nwritten);
    यदि(परिणाम) {
      failf(दत्त, "Sending data failed: %s", curl_easy_strerror(परिणाम));
      प्रत्यागम परिणाम;
    }
    printsub(दत्त, '>', &अस्थायी[2], len-2);
    भङ्ग;
  पक्ष CURL_TELOPT_XDISPLOC:
    यदि(bad_option(tn->subopt_xdisploc))
      प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
    यदि(वाक्यदैर्घ्यम्(tn->subopt_xdisploc) > 1000) {
      failf(दत्त, "Too long telnet XDISPLOC");
      प्रत्यागम CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((वर्ण *)अस्थायी, परिमाण(अस्थायी), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    परिणाम = Curl_xfer_send(दत्त, अस्थायी, len, FALSE, &nwritten);
    यदि(परिणाम) {
      failf(दत्त, "Sending data failed: %s", curl_easy_strerror(परिणाम));
      प्रत्यागम परिणाम;
    }
    printsub(दत्त, '>', &अस्थायी[2], len - 2);
    भङ्ग;
  पक्ष CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((वर्ण *)अस्थायी, परिमाण(अस्थायी), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    प्रति(v = tn->telnet_vars; v; v = v->अग्र) {
      size_t tmplen = (वाक्यदैर्घ्यम्(v->दत्त) + 1);
      यदि(bad_option(v->दत्त))
        प्रत्यागम CURLE_BAD_FUNCTION_तर्कः;
      /* Add the variable if it fits */
      यदि(len + tmplen < परिमाण(अस्थायी) - 6) {
        ध्रुव वर्ण *s = strchr(v->दत्त, ',');
        यदि(!s)
          len += curl_msnprintf((वर्ण *)&अस्थायी[len], परिमाण(अस्थायी) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->दत्त);
        अन्यथा {
          size_t vlen = s - v->दत्त;
          len += curl_msnprintf((वर्ण *)&अस्थायी[len], परिमाण(अस्थायी) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (पूर्णाङ्क)vlen, v->दत्त, CURL_NEW_ENV_मानम्, ++s);
        }
      }
    }
    curl_msnprintf((वर्ण *)&अस्थायी[len], परिमाण(अस्थायी) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    परिणाम = Curl_xfer_send(दत्त, अस्थायी, len, FALSE, &nwritten);
    यदि(परिणाम) {
      failf(दत्त, "Sending data failed: %s", curl_easy_strerror(परिणाम));
      प्रत्यागम परिणाम;
    }
    printsub(दत्त, '>', &अस्थायी[2], len - 2);
    भङ्ग;
  }
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode telrcv(संरचना Curl_easy *दत्त,
                       संरचना TELNET *tn,
                       ध्रुव अचिह्नित वर्ण *inbuf, /* Data received from
                                                      socket */
                       ssize_t गणनम्)              /* Number of bytes
                                                      received */
{
  अचिह्नित वर्ण c;
  CURLcode परिणाम;
  पूर्णाङ्क अन्तर् = 0;
  पूर्णाङ्क startwrite = -1;

#परिभाषय startskipping()                                          \
  यदि(startwrite >= 0) {                                          \
    परिणाम = Curl_client_write(दत्त,                             \
                               CLIENTWRITE_BODY,                 \
                               (ध्रुव वर्ण *)&inbuf[startwrite], \
                               अन्तर्-startwrite);                   \
    यदि(परिणाम)                                                   \
      प्रत्यागम परिणाम;                                             \
  }                                                              \
  startwrite = -1

#परिभाषय writebyte()  \
  यदि(startwrite < 0) \
    startwrite = अन्तर्

#परिभाषय bufferflush() startskipping()

  यावत्(गणनम्--) {
    c = inbuf[अन्तर्];

    विकल्प(tn->telrcv_state) {
    पक्ष CURL_TS_CR:
      tn->telrcv_state = CURL_TS_दत्त;
      यदि(c == '\0') {
        startskipping();
        भङ्ग;   /* Ignore \0 after CR */
      }
      writebyte();
      भङ्ग;

    पक्ष CURL_TS_दत्त:
      यदि(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        भङ्ग;
      }
      अन्यथा यदि(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      भङ्ग;

    पक्ष CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      विकल्प(c) {
      पक्ष CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        भङ्ग;
      पक्ष CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        भङ्ग;
      पक्ष CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        भङ्ग;
      पक्ष CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        भङ्ग;
      पक्ष CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        भङ्ग;
      पक्ष CURL_IAC:
        tn->telrcv_state = CURL_TS_दत्त;
        writebyte();
        भङ्ग;
      पक्ष CURL_DM:
      पक्ष CURL_NOP:
      पक्ष CURL_GA:
      स्वभाव:
        tn->telrcv_state = CURL_TS_दत्त;
        printoption(दत्त, "RCVD", CURL_IAC, c);
        भङ्ग;
      }
      भङ्ग;

    पक्ष CURL_TS_WILL:
      printoption(दत्त, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(दत्त, tn, c);
      tn->telrcv_state = CURL_TS_दत्त;
      भङ्ग;

    पक्ष CURL_TS_WONT:
      printoption(दत्त, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(दत्त, tn, c);
      tn->telrcv_state = CURL_TS_दत्त;
      भङ्ग;

    पक्ष CURL_TS_DO:
      printoption(दत्त, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(दत्त, tn, c);
      tn->telrcv_state = CURL_TS_दत्त;
      भङ्ग;

    पक्ष CURL_TS_DONT:
      printoption(दत्त, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(दत्त, tn, c);
      tn->telrcv_state = CURL_TS_दत्त;
      भङ्ग;

    पक्ष CURL_TS_SB:
      यदि(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      अन्यथा
        CURL_SB_ACCUM(tn, c);
      भङ्ग;

    पक्ष CURL_TS_SE:
      यदि(c != CURL_SE) {
        यदि(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(दत्त, "telnet: suboption error");
          प्रत्यागम CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      अन्यथा {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_पदांशः(tn);
        परिणाम = suboption(दत्त, tn);   /* handle sub-option */
        यदि(परिणाम)
          प्रत्यागम परिणाम;
        tn->telrcv_state = CURL_TS_दत्त;
      }
      भङ्ग;
    }
    ++अन्तर्;
  }
  bufferflush();
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode telnet_समाप्तम्(संरचना Curl_easy *दत्त,
                            CURLcode स्थितिः, bool premature)
{
  (शून्य)स्थितिः;
  (शून्य)premature;
  Curl_meta_remove(दत्त, CURL_META_TELNET_EASY);
  प्रत्यागम CURLE_OK;
}

स्थिर CURLcode telnet_do(संरचना Curl_easy *दत्त, bool *समाप्तम्)
{
  CURLcode परिणाम;
  संरचना connectdata *conn = दत्त->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#यदि_परिभाषितम् USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_गणनम्;
  DWORD wait_timeout;
  DWORD readfile_read;
#अन्यथा
  timediff_t interval_ms;
  संरचना pollfd pfd[2];
  पूर्णाङ्क poll_cnt;
  ssize_t snread;
#यदि_अन्तः
  bool keepon = TRUE;
  वर्ण बफर[4 * 1024];
  संरचना TELNET *tn;

  *समाप्तम् = TRUE; /* unconditionally */

  परिणाम = init_telnet(दत्त);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

  tn = Curl_meta_get(दत्त, CURL_META_TELNET_EASY);
  यदि(!tn)
    प्रत्यागम CURLE_FAILED_INIT;

  परिणाम = check_telnet_options(दत्त, tn);
  यदि(परिणाम)
    प्रत्यागम परिणाम;

#यदि_परिभाषितम् USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  यदि(event_handle == WSA_INVALID_EVENT) {
    failf(दत्त, "WSACreateEvent failed (%d)", SOCKERRNO);
    प्रत्यागम CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  यदि(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    प्रत्यागम CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_निवेशः_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  यदि(GetFileType(stdin_handle) == FILE_TYPE_PIPE || दत्त->समुच्चयः.is_fread_समुच्चयः) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_गणनम् = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  अन्यथा {
    obj_गणनम् = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  यावत्(keepon) {
    ध्रुव DWORD buf_आकार = (DWORD)परिमाण(बफर);
    DWORD waitret = Wait_For_गुणजः_Objects(obj_गणनम्, objs,
                                           FALSE, wait_timeout);
    विकल्प(waitret) {

    पक्ष WAIT_TIMEOUT: {
      प्रति(;;) {
        यदि(दत्त->समुच्चयः.is_fread_समुच्चयः) {
          size_t n;
          /* read from user-supplied method */
          n = दत्त->state.fread_func(बफर, 1, buf_आकार, दत्त->state.अन्तर्);
          यदि(n == CURL_READFUNC_ABORT) {
            failf(दत्त, "operation aborted by callback");
            keepon = FALSE;
            परिणाम = CURLE_ABORTED_BY_CALLBACK;
            भङ्ग;
          }

          यदि(n == CURL_READFUNC_PAUSE)
            भङ्ग;

          यदि(n == 0)                        /* no bytes */
            भङ्ग;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        अन्यथा {
          /* read from stdin */
          यदि(!PeekNamedPipe(stdin_handle, रिक्त, 0, रिक्त,
                            &readfile_read, रिक्त)) {
            keepon = FALSE;
            परिणाम = CURLE_READ_ERROR;
            भङ्ग;
          }

          यदि(!readfile_read)
            भङ्ग;

          यदि(!ReadFile(stdin_handle, बफर, buf_आकार, &readfile_read, रिक्त)) {
            keepon = FALSE;
            परिणाम = CURLE_READ_ERROR;
            भङ्ग;
          }
        }

        परिणाम = send_telnet_दत्त(दत्त, tn, बफर, readfile_read);
        यदि(परिणाम) {
          keepon = FALSE;
          भङ्ग;
        }
      }
    }
    भङ्ग;

    पक्ष WAIT_OBJECT_0 + 1: {
      यदि(!ReadFile(stdin_handle, बफर, buf_आकार, &readfile_read, रिक्त)) {
        keepon = FALSE;
        परिणाम = CURLE_READ_ERROR;
        भङ्ग;
      }

      परिणाम = send_telnet_दत्त(दत्त, tn, बफर, readfile_read);
      यदि(परिणाम) {
        keepon = FALSE;
        भङ्ग;
      }
    }
    भङ्ग;

    पक्ष WAIT_OBJECT_0: {
      events.l_जालम्_Events = 0;
      यदि(WSAEnum_जालम्_Events(sockfd, event_handle, &events) != 0) {
        पूर्णाङ्क sockerr = SOCKERRNO;
        यदि(sockerr != SOCKEINPROGRESS) {
          infof(दत्त, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          परिणाम = CURLE_READ_ERROR;
        }
        भङ्ग;
      }
      यदि(events.l_जालम्_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        परिणाम = Curl_xfer_recv(दत्त, बफर, परिमाण(बफर), &nread);
        /* read would have blocked. Loop again */
        यदि(परिणाम == CURLE_AGAIN)
          भङ्ग;
        /* returned not-zero, this an error */
        अन्यथा यदि(परिणाम) {
          keepon = FALSE;
          भङ्ग;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा यदि(!nread) {
          keepon = FALSE;
          भङ्ग;
        }

        परिणाम = telrcv(दत्त, tn, (अचिह्नित वर्ण *)बफर, nread);
        यदि(परिणाम) {
          keepon = FALSE;
          भङ्ग;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        यदि(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(दत्त, tn);
          tn->already_negotiated = 1;
        }
      }
      यदि(events.l_जालम्_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      भङ्ग;
    }
    } /* switch */

    यदि(Curl_timeleft_ms(दत्त) < 0) {
      failf(दत्त, "Time-out");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  यदि(!WSACloseEvent(event_handle)) {
    infof(दत्त, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#अन्यथा
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  यदि(दत्त->समुच्चयः.is_fread_समुच्चयः) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  अन्यथा {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)दत्त->state.अन्तर्);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    यदि(pfd[1].fd < 0) {
      failf(दत्त, "cannot read input");
      परिणाम = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  यावत्(keepon) {
    DEBUGF(infof(दत्त, "telnet_do, poll %d fds", poll_cnt));
    विकल्प(Curl_poll(pfd, (अचिह्नित पूर्णाङ्क)poll_cnt, interval_ms)) {
    पक्ष -1:                    /* error, stop reading */
      परिणाम = CURLE_RECV_ERROR;
      keepon = FALSE;
      अनुवर्त;
    पक्ष 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    स्वभाव:                    /* read! */
      यदि(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        परिणाम = Curl_xfer_recv(दत्त, बफर, परिमाण(बफर), &nread);
        /* read would have blocked. Loop again */
        यदि(परिणाम == CURLE_AGAIN)
          भङ्ग;
        /* returned not-zero, this an error */
        यदि(परिणाम) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          यदि(दत्त->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(दत्त, "telnet_do, unexpected ECONNRESET on recv"));
          }
          भङ्ग;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा यदि(!nread) {
          keepon = FALSE;
          भङ्ग;
        }

        Curl_pgrs_download_inc(दत्त, nread);
        परिणाम = telrcv(दत्त, tn, (अचिह्नित वर्ण *)बफर, nread);
        यदि(परिणाम) {
          keepon = FALSE;
          भङ्ग;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        यदि(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(दत्त, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      यदि(poll_cnt == 2) {
        यदि(pfd[1].revents & POLLIN) { /* read from in file */
          snread = पठ(pfd[1].fd, बफर, परिमाण(बफर));
        }
      }
      अन्यथा {
        /* read from user-supplied method */
        snread = (पूर्णाङ्क)दत्त->state.fread_func(बफर, 1, परिमाण(बफर),
                                             दत्त->state.अन्तर्);
        यदि(snread == CURL_READFUNC_ABORT) {
          failf(दत्त, "operation aborted by callback");
          परिणाम = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          भङ्ग;
        }
        यदि(snread == CURL_READFUNC_PAUSE)
          भङ्ग;
      }

      यदि(snread > 0) {
        परिणाम = send_telnet_दत्त(दत्त, tn, बफर, snread);
        यदि(परिणाम) {
          keepon = FALSE;
          भङ्ग;
        }
        Curl_pgrs_upload_inc(दत्त, (size_t)snread);
      }
      अन्यथा यदि(snread < 0) {
        failf(दत्त, "Failed to read input");
        परिणाम = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      भङ्ग;
    } /* poll switch statement */

    यदि(Curl_timeleft_ms(दत्त) < 0) {
      failf(दत्त, "Time-out");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    यदि(!परिणाम) {
      परिणाम = Curl_pgrsUpdate(दत्त);
      यदि(परिणाम)
        keepon = FALSE;
    }
  }
#यदि_अन्तः
  /* mark this as "no further transfer wanted" */
  Curl_xfer_सज्जता_nop(दत्त);

  प्रत्यागम परिणाम;
}

/*
 * TELNET protocol handler.
 */
ध्रुव संरचना Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_समाप्तम्,                          /* done */
  ZERO_NULL,                            /* do_more */
  ZERO_NULL,                            /* connect_it */
  ZERO_NULL,                            /* connecting */
  ZERO_NULL,                            /* doing */
  ZERO_NULL,                            /* proto_pollset */
  ZERO_NULL,                            /* doing_pollset */
  ZERO_NULL,                            /* domore_pollset */
  ZERO_NULL,                            /* perform_pollset */
  ZERO_NULL,                            /* disconnect */
  ZERO_NULL,                            /* write_resp */
  ZERO_NULL,                            /* write_resp_hd */
  ZERO_NULL,                            /* connection_is_dead */
  ZERO_NULL,                            /* attach connection */
  ZERO_NULL,                            /* follow */
};

#यदि_अन्तः /* !CURL_DISABLE_TELNET */
