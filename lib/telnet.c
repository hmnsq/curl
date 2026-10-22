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
#అన్తర్భావ "urldata.h"
#అన్తర్భావ "telnet.h"

#నిర్వచితం_కాకపోతే CURL_DISABLE_TELNET

#నిర్వచితమైతే HAVE_NETINET_లోపల_H
#అన్తర్భావ <netinet/in.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NETDB_H
#అన్తర్భావ <netdb.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_ARPA_INET_H
#అన్తర్భావ <arpa/inet.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NET_IF_H
#అన్తర్భావ <net/if.h>
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_SYS_IOCTL_H
#అన్తర్భావ <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PARAM_H
#అన్తర్భావ <sys/param.h>
#అయితే_ముగింపు

#అన్తర్భావ "url.h"
#అన్తర్భావ "transfer.h"
#అన్తర్భావ "sendf.h"
#అన్తర్భావ "curl_trc.h"
#అన్తర్భావ "progress.h"
#అన్తర్భావ "arpa_telnet.h"
#అన్తర్భావ "connect.h"
#అన్తర్భావ "select.h"
#అన్తర్భావ "curlx/strparse.h"

#నిర్వచించు SUBBUFSIZE 512

#నిర్వచించు CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#నిర్వచించు CURL_SB_పదం(x)            \
  చేయి {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } అయితే(0)
#నిర్వచించు CURL_SB_ACCUM(x, c)                                         \
  చేయి {                                                              \
    ఒకవేళ((x)->subpointer < ((x)->subbuffer + పరిమాణం((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } అయితే(0)

#నిర్వచించు CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#నిర్వచించు CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#నిర్వచించు CURL_NO          0
#నిర్వచించు CURL_YES         1
#నిర్వచించు CURL_WANTYES     2
#నిర్వచించు CURL_WANTNO      3

#నిర్వచించు CURL_ఖాళీలు       0
#నిర్వచించు CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#నిర్వచించు CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
రకనిర్వచనం గణన {
  CURL_TS_దత్తాంశం = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

నిర్మాణం TELNET {
  పూర్ణాంకం please_negotiate;
  పూర్ణాంకం already_negotiated;
  పూర్ణాంకం us[256];
  పూర్ణాంకం usq[256];
  పూర్ణాంకం us_preferred[256];
  పూర్ణాంకం him[256];
  పూర్ణాంకం himq[256];
  పూర్ణాంకం him_preferred[256];
  పూర్ణాంకం subnegotiation[256];
  స్థిరరాశి అక్షరం *subopt_ttype;          /* Set with suboption TTYPE */
  స్థిరరాశి అక్షరం *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  గుర్తులేని పొట్టి subopt_wsx;         /* Set with suboption NAWS */
  గుర్తులేని పొట్టి subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  నిర్మాణం curl_slist *telnet_vars;    /* Environment variables */
  నిర్మాణం dynbuf బహిర్గతం;                 /* output buffer */

  /* suboptions */
  గుర్తులేని అక్షరం subbuffer[SUBBUFSIZE];
  గుర్తులేని అక్షరం *subpointer, *subend;      /* buffer for sub-options */
};

#నిర్వచితం_కాకపోతే CURLVERBOSE
#నిర్వచించు printoption(a, b, c, d) Curl_nop_stmt
#లేకపోతే
స్థిర శూన్య printoption(నిర్మాణం Curl_easy *దత్తాంశం,
                        స్థిరరాశి అక్షరం *direction, పూర్ణాంకం cmd, పూర్ణాంకం option)
{
  ఒకవేళ(దత్తాంశం->సమితి.verbose) {
    ఒకవేళ(cmd == CURL_IAC) {
      ఒకవేళ(CURL_TELCMD_OK(option))
        infof(దత్తాంశం, "%s IAC %s", direction, CURL_TELCMD(option));
      లేకపోతే
        infof(దత్తాంశం, "%s IAC %d", direction, option);
    }
    లేకపోతే {
      స్థిరరాశి అక్షరం *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      ఒకవేళ(fmt) {
        స్థిరరాశి అక్షరం *opt;
        ఒకవేళ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        లేకపోతే ఒకవేళ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        లేకపోతే
          opt = ఖాళీ;

        ఒకవేళ(opt)
          infof(దత్తాంశం, "%s %s %s", direction, fmt, opt);
        లేకపోతే
          infof(దత్తాంశం, "%s %s %d", direction, fmt, option);
      }
      లేకపోతే
        infof(దత్తాంశం, "%s %d %d", direction, cmd, option);
    }
  }
}
#అయితే_ముగింపు /* !CURLVERBOSE */

స్థిర శూన్య telnet_easy_dtor(స్థిరరాశి శూన్య *కీలకం, size_t klen, శూన్య *నమోదు_2)
{
  నిర్మాణం TELNET *tn = నమోదు_2;
  (శూన్య)కీలకం;
  (శూన్య)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->బహిర్గతం);
  curlx_free(tn);
}

స్థిర CURLcode init_telnet(నిర్మాణం Curl_easy *దత్తాంశం)
{
  నిర్మాణం TELNET *tn;

  tn = curlx_calloc(1, పరిమాణం(నిర్మాణం TELNET));
  ఒకవేళ(!tn)
    తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;

  curlx_dyn_init(&tn->బహిర్గతం, 0xffff);

  tn->telrcv_state = CURL_TS_దత్తాంశం;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_ద్విమాన] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ద్విమాన] = CURL_YES;

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

  తిరిగి Curl_meta_సమితి(దత్తాంశం, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

స్థిర శూన్య send_negotiation(నిర్మాణం Curl_easy *దత్తాంశం, పూర్ణాంకం cmd, పూర్ణాంకం option)
{
  గుర్తులేని అక్షరం buf[3];
  size_t nwritten;
  CURLcode ఫలితం;

  buf[0] = CURL_IAC;
  buf[1] = (గుర్తులేని అక్షరం)cmd;
  buf[2] = (గుర్తులేని అక్షరం)option;

  ఫలితం = Curl_xfer_send(దత్తాంశం, buf, 3, FALSE, &nwritten);
  ఒకవేళ(ఫలితం)
    failf(దత్తాంశం, "Sending data failed: %s", curl_easy_strerror(ఫలితం));

  printoption(దత్తాంశం, "SENT", cmd, option);
}

స్థిర శూన్య సమితి_remote_option(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn,
                              పూర్ణాంకం option, పూర్ణాంకం newstate)
{
  ఒకవేళ(newstate == CURL_YES) {
    ఎంపిక(tn->him[option]) {
    స్థితి CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(దత్తాంశం, CURL_DO, option);
      ఆపు;

    స్థితి CURL_YES:
      /* Already enabled */
      ఆపు;

    స్థితి CURL_WANTNO:
      ఎంపిక(tn->himq[option]) {
      స్థితి CURL_ఖాళీలు:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        ఆపు;
      స్థితి CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ఆపు;
      }
      ఆపు;

    స్థితి CURL_WANTYES:
      ఎంపిక(tn->himq[option]) {
      స్థితి CURL_ఖాళీలు:
        /* Error: already negotiating for enable */
        ఆపు;
      స్థితి CURL_OPPOSITE:
        tn->himq[option] = CURL_ఖాళీలు;
        ఆపు;
      }
      ఆపు;
    }
  }
  లేకపోతే { /* NO */
    ఎంపిక(tn->him[option]) {
    స్థితి CURL_NO:
      /* Already disabled */
      ఆపు;

    స్థితి CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(దత్తాంశం, CURL_DONT, option);
      ఆపు;

    స్థితి CURL_WANTNO:
      ఎంపిక(tn->himq[option]) {
      స్థితి CURL_ఖాళీలు:
        /* Already negotiating for NO */
        ఆపు;
      స్థితి CURL_OPPOSITE:
        tn->himq[option] = CURL_ఖాళీలు;
        ఆపు;
      }
      ఆపు;

    స్థితి CURL_WANTYES:
      ఎంపిక(tn->himq[option]) {
      స్థితి CURL_ఖాళీలు:
        tn->himq[option] = CURL_OPPOSITE;
        ఆపు;
      స్థితి CURL_OPPOSITE:
        ఆపు;
      }
      ఆపు;
    }
  }
}

స్థిర శూన్య సమితి_local_option(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn,
                             పూర్ణాంకం option, పూర్ణాంకం newstate)
{
  ఒకవేళ(newstate == CURL_YES) {
    ఎంపిక(tn->us[option]) {
    స్థితి CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(దత్తాంశం, CURL_WILL, option);
      ఆపు;

    స్థితి CURL_YES:
      /* Already enabled */
      ఆపు;

    స్థితి CURL_WANTNO:
      ఎంపిక(tn->usq[option]) {
      స్థితి CURL_ఖాళీలు:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        ఆపు;
      స్థితి CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ఆపు;
      }
      ఆపు;

    స్థితి CURL_WANTYES:
      ఎంపిక(tn->usq[option]) {
      స్థితి CURL_ఖాళీలు:
        /* Error: already negotiating for enable */
        ఆపు;
      స్థితి CURL_OPPOSITE:
        tn->usq[option] = CURL_ఖాళీలు;
        ఆపు;
      }
      ఆపు;
    }
  }
  లేకపోతే { /* NO */
    ఎంపిక(tn->us[option]) {
    స్థితి CURL_NO:
      /* Already disabled */
      ఆపు;

    స్థితి CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(దత్తాంశం, CURL_WONT, option);
      ఆపు;

    స్థితి CURL_WANTNO:
      ఎంపిక(tn->usq[option]) {
      స్థితి CURL_ఖాళీలు:
        /* Already negotiating for NO */
        ఆపు;
      స్థితి CURL_OPPOSITE:
        tn->usq[option] = CURL_ఖాళీలు;
        ఆపు;
      }
      ఆపు;

    స్థితి CURL_WANTYES:
      ఎంపిక(tn->usq[option]) {
      స్థితి CURL_ఖాళీలు:
        tn->usq[option] = CURL_OPPOSITE;
        ఆపు;
      స్థితి CURL_OPPOSITE:
        ఆపు;
      }
      ఆపు;
    }
  }
}

స్థిర శూన్య telnet_negotiate(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn)
{
  పూర్ణాంకం i;

  కొరకు(i = 0; i < CURL_NTELOPTS; i++) {
    ఒకవేళ(i == CURL_TELOPT_ECHO)
      కొనసాగు;

    ఒకవేళ(tn->us_preferred[i] == CURL_YES)
      సమితి_local_option(దత్తాంశం, tn, i, CURL_YES);

    ఒకవేళ(tn->him_preferred[i] == CURL_YES)
      సమితి_remote_option(దత్తాంశం, tn, i, CURL_YES);
  }
}

స్థిర శూన్య rec_will(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn, పూర్ణాంకం option)
{
  ఎంపిక(tn->him[option]) {
  స్థితి CURL_NO:
    ఒకవేళ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(దత్తాంశం, CURL_DO, option);
    }
    లేకపోతే
      send_negotiation(దత్తాంశం, CURL_DONT, option);

    ఆపు;

  స్థితి CURL_YES:
    /* Already enabled */
    ఆపు;

  స్థితి CURL_WANTNO:
    ఎంపిక(tn->himq[option]) {
    స్థితి CURL_ఖాళీలు:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      ఆపు;
    స్థితి CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_ఖాళీలు;
      ఆపు;
    }
    ఆపు;

  స్థితి CURL_WANTYES:
    ఎంపిక(tn->himq[option]) {
    స్థితి CURL_ఖాళీలు:
      tn->him[option] = CURL_YES;
      ఆపు;
    స్థితి CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_ఖాళీలు;
      send_negotiation(దత్తాంశం, CURL_DONT, option);
      ఆపు;
    }
    ఆపు;
  }
}

స్థిర శూన్య rec_wont(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn, పూర్ణాంకం option)
{
  ఎంపిక(tn->him[option]) {
  స్థితి CURL_NO:
    /* Already disabled */
    ఆపు;

  స్థితి CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(దత్తాంశం, CURL_DONT, option);
    ఆపు;

  స్థితి CURL_WANTNO:
    ఎంపిక(tn->himq[option]) {
    స్థితి CURL_ఖాళీలు:
      tn->him[option] = CURL_NO;
      ఆపు;

    స్థితి CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_ఖాళీలు;
      send_negotiation(దత్తాంశం, CURL_DO, option);
      ఆపు;
    }
    ఆపు;

  స్థితి CURL_WANTYES:
    ఎంపిక(tn->himq[option]) {
    స్థితి CURL_ఖాళీలు:
      tn->him[option] = CURL_NO;
      ఆపు;
    స్థితి CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_ఖాళీలు;
      ఆపు;
    }
    ఆపు;
  }
}

స్థిర శూన్య printsub(నిర్మాణం Curl_easy *దత్తాంశం,
                     పూర్ణాంకం direction,                /* '<' or '>' */
                     స్థిరరాశి గుర్తులేని అక్షరం *సూచిక, /* ptr to suboption data */
                     size_t నిడివి)                /* suboption data length */
{
  ఒకవేళ(దత్తాంశం->సమితి.verbose) {
    గుర్తులేని పూర్ణాంకం i = 0;
    ఒకవేళ(direction) {
      infof(దత్తాంశం, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      ఒకవేళ(నిడివి >= 3) {
        పూర్ణాంకం j;

        i = సూచిక[నిడివి - 2];
        j = సూచిక[నిడివి - 1];

        ఒకవేళ(i != CURL_IAC || j != CURL_SE) {
          infof(దత్తాంశం, "(terminated by ");
          ఒకవేళ(CURL_TELOPT_OK(i))
            infof(దత్తాంశం, "%s ", CURL_TELOPT(i));
          లేకపోతే ఒకవేళ(CURL_TELCMD_OK(i))
            infof(దత్తాంశం, "%s ", CURL_TELCMD(i));
          లేకపోతే
            infof(దత్తాంశం, "%u ", i);
          ఒకవేళ(CURL_TELOPT_OK(j))
            infof(దత్తాంశం, "%s", CURL_TELOPT(j));
          లేకపోతే ఒకవేళ(CURL_TELCMD_OK(j))
            infof(దత్తాంశం, "%s", CURL_TELCMD(j));
          లేకపోతే
            infof(దత్తాంశం, "%d", j);
          infof(దత్తాంశం, ", not IAC SE) ");
        }
      }
      ఒకవేళ(నిడివి >= 2)
        నిడివి -= 2;
      లేకపోతే /* bad input */
        తిరిగి;
    }
    ఒకవేళ(నిడివి <= 1) {
      infof(దత్తాంశం, "(Empty suboption?)");
      తిరిగి;
    }

    ఒకవేళ(CURL_TELOPT_OK(సూచిక[0])) {
      ఎంపిక(సూచిక[0]) {
      స్థితి CURL_TELOPT_TTYPE:
      స్థితి CURL_TELOPT_XDISPLOC:
      స్థితి CURL_TELOPT_NEW_ENVIRON:
      స్థితి CURL_TELOPT_NAWS:
        infof(దత్తాంశం, "%s", CURL_TELOPT(సూచిక[0]));
        ఆపు;
      అప్రమేయ:
        infof(దత్తాంశం, "%s (unsupported)", CURL_TELOPT(సూచిక[0]));
        ఆపు;
      }
    }
    లేకపోతే
      infof(దత్తాంశం, "%d (unknown)", సూచిక[0]);

    ఎంపిక(సూచిక[0]) {
    స్థితి CURL_TELOPT_NAWS:
      ఒకవేళ(నిడివి > 4)
        infof(దత్తాంశం, "Width: %d ; Height: %d", (సూచిక[1] << 8) | సూచిక[2],
              (సూచిక[3] << 8) | సూచిక[4]);
      ఆపు;
    అప్రమేయ:
      ఎంపిక(సూచిక[1]) {
      స్థితి CURL_TELQUAL_IS:
        infof(దత్తాంశం, " IS");
        ఆపు;
      స్థితి CURL_TELQUAL_SEND:
        infof(దత్తాంశం, " SEND");
        ఆపు;
      స్థితి CURL_TELQUAL_INFO:
        infof(దత్తాంశం, " INFO/REPLY");
        ఆపు;
      స్థితి CURL_TELQUAL_NAME:
        infof(దత్తాంశం, " NAME");
        ఆపు;
      }

      ఎంపిక(సూచిక[0]) {
      స్థితి CURL_TELOPT_TTYPE:
      స్థితి CURL_TELOPT_XDISPLOC:
        infof(దత్తాంశం, " \"%.*s\"",
              (పూర్ణాంకం)((నిడివి > 2) ? (నిడివి - 2) : 0), &సూచిక[2]);
        ఆపు;
      స్థితి CURL_TELOPT_NEW_ENVIRON:
        ఒకవేళ(సూచిక[1] == CURL_TELQUAL_IS) {
          infof(దత్తాంశం, " ");
          కొరకు(i = 3; i < నిడివి; i++) {
            ఎంపిక(సూచిక[i]) {
            స్థితి CURL_NEW_ENV_VAR:
              infof(దత్తాంశం, ", ");
              ఆపు;
            స్థితి CURL_NEW_ENV_విలువ:
              infof(దత్తాంశం, " = ");
              ఆపు;
            అప్రమేయ:
              infof(దత్తాంశం, "%c", సూచిక[i]);
              ఆపు;
            }
          }
        }
        ఆపు;
      అప్రమేయ:
        కొరకు(i = 2; i < నిడివి; i++)
          infof(దత్తాంశం, " %.2x", సూచిక[i]);
        ఆపు;
      }
    }
  }
}

/* Escape and send a telnet data block */
స్థిర CURLcode send_telnet_దత్తాంశం(నిర్మాణం Curl_easy *దత్తాంశం,
                                 నిర్మాణం TELNET *tn,
                                 స్థిరరాశి అక్షరం *బఫరు, ssize_t nread)
{
  size_t i, outlen;
  స్థిరరాశి గుర్తులేని అక్షరం *outbuf;
  CURLcode ఫలితం = CURLE_OK;
  size_t బైట్లు_written;
  size_t మొత్తం_written = 0;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  ఒకవేళ(nread < 0)
    తిరిగి CURLE_TOO_LARGE;

  ఒకవేళ(memchr(బఫరు, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->బహిర్గతం);

    కొరకు(i = 0; i < (size_t)nread && !ఫలితం; i++) {
      ఫలితం = curlx_dyn_addn(&tn->బహిర్గతం, &బఫరు[i], 1);
      ఒకవేళ(!ఫలితం && ((గుర్తులేని అక్షరం)బఫరు[i] == CURL_IAC))
        /* IAC is FF in hex */
        ఫలితం = curlx_dyn_addn(&tn->బహిర్గతం, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->బహిర్గతం);
    outbuf = curlx_dyn_uptr(&tn->బహిర్గతం);
  }
  లేకపోతే {
    outlen = (size_t)nread;
    outbuf = (స్థిరరాశి గుర్తులేని అక్షరం *)బఫరు;
  }
  అయితే(!ఫలితం && మొత్తం_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    నిర్మాణం pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(దత్తాంశం);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    ఒకవేళ(timeout_ms < 0)
      తిరిగి CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    ఎంపిక(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    స్థితి -1:                    /* error, abort writing */
      ఫలితం = CURLE_SEND_ERROR;
      ఆపు;
    స్థితి 0:                     /* timeout */
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      ఆపు;
    అప్రమేయ:                    /* write! */
      బైట్లు_written = 0;
      ఫలితం = Curl_xfer_send(దత్తాంశం, outbuf + మొత్తం_written,
                              outlen - మొత్తం_written, FALSE, &బైట్లు_written);
      మొత్తం_written += బైట్లు_written;
      ఆపు;
    }
  }

  తిరిగి ఫలితం;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
స్థిర శూన్య sendsuboption(నిర్మాణం Curl_easy *దత్తాంశం,
                          నిర్మాణం TELNET *tn, పూర్ణాంకం option)
{
  size_t nwritten;
  గుర్తులేని పొట్టి x, y;
  స్థిరరాశి గుర్తులేని అక్షరం *uc1, *uc2;
  CURLcode ఫలితం;

  ఎంపిక(option) {
  స్థితి CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (స్థిరరాశి గుర్తులేని అక్షరం *)&x;
    uc2 = (స్థిరరాశి గుర్తులేని అక్షరం *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_పదం(tn);
    /* data suboption is now ready */

    printsub(దత్తాంశం, '>', (స్థిరరాశి గుర్తులేని అక్షరం *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    ఫలితం = Curl_xfer_send(దత్తాంశం, tn->subbuffer, 3, FALSE, &nwritten);
    ఒకవేళ(ఫలితం)
      failf(దత్తాంశం, "Sending data failed: %s", curl_easy_strerror(ఫలితం));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_దత్తాంశం(దత్తాంశం, tn, (స్థిరరాశి అక్షరం *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    ఫలితం = Curl_xfer_send(దత్తాంశం, tn->subbuffer + 7, 2, FALSE, &nwritten);
    ఒకవేళ(ఫలితం)
      failf(దత్తాంశం, "Sending data failed: %s", curl_easy_strerror(ఫలితం));
    ఆపు;
  }
}

స్థిర శూన్య rec_do(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn, పూర్ణాంకం option)
{
  ఎంపిక(tn->us[option]) {
  స్థితి CURL_NO:
    ఒకవేళ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(దత్తాంశం, CURL_WILL, option);
      ఒకవేళ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(దత్తాంశం, tn, option);
    }
    లేకపోతే ఒకవేళ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(దత్తాంశం, CURL_WILL, option);
      sendsuboption(దత్తాంశం, tn, option);
    }
    లేకపోతే
      send_negotiation(దత్తాంశం, CURL_WONT, option);
    ఆపు;

  స్థితి CURL_YES:
    /* Already enabled */
    ఆపు;

  స్థితి CURL_WANTNO:
    ఎంపిక(tn->usq[option]) {
    స్థితి CURL_ఖాళీలు:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      ఆపు;
    స్థితి CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_ఖాళీలు;
      ఆపు;
    }
    ఆపు;

  స్థితి CURL_WANTYES:
    ఎంపిక(tn->usq[option]) {
    స్థితి CURL_ఖాళీలు:
      tn->us[option] = CURL_YES;
      ఒకవేళ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(దత్తాంశం, tn, option);
      }
      ఆపు;
    స్థితి CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_ఖాళీలు;
      send_negotiation(దత్తాంశం, CURL_WONT, option);
      ఆపు;
    }
    ఆపు;
  }
}

స్థిర శూన్య rec_dont(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn, పూర్ణాంకం option)
{
  ఎంపిక(tn->us[option]) {
  స్థితి CURL_NO:
    /* Already disabled */
    ఆపు;

  స్థితి CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(దత్తాంశం, CURL_WONT, option);
    ఆపు;

  స్థితి CURL_WANTNO:
    ఎంపిక(tn->usq[option]) {
    స్థితి CURL_ఖాళీలు:
      tn->us[option] = CURL_NO;
      ఆపు;

    స్థితి CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_ఖాళీలు;
      send_negotiation(దత్తాంశం, CURL_WILL, option);
      ఆపు;
    }
    ఆపు;

  స్థితి CURL_WANTYES:
    ఎంపిక(tn->usq[option]) {
    స్థితి CURL_ఖాళీలు:
      tn->us[option] = CURL_NO;
      ఆపు;
    స్థితి CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_ఖాళీలు;
      ఆపు;
    }
    ఆపు;
  }
}

స్థిర bool str_is_nonascii(స్థిరరాశి అక్షరం *str)
{
  అక్షరం c;
  అయితే((c = *str++) != 0)
    ఒకవేళ(c & 0x80)
      తిరిగి TRUE;

  తిరిగి FALSE;
}

స్థిర CURLcode check_telnet_options(నిర్మాణం Curl_easy *దత్తాంశం,
                                     నిర్మాణం TELNET *tn)
{
  నిర్మాణం curl_slist *తల;
  నిర్మాణం curl_slist *beg;
  CURLcode ఫలితం = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  ఒకవేళ(దత్తాంశం->state.creds) {
    అక్షరం బఫరు[256];
    ఒకవేళ(str_is_nonascii(Curl_creds_user(దత్తాంశం->conn->creds))) {
      DEBUGF(infof(దత్తాంశం, "set a non ASCII username in telnet"));
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    }
    curl_msnprintf(బఫరు, పరిమాణం(బఫరు), "USER,%s",
                   Curl_creds_user(దత్తాంశం->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, బఫరు);
    ఒకవేళ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ఖాళీ;
      తిరిగి CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  కొరకు(తల = దత్తాంశం->సమితి.telnet_options; తల && !ఫలితం; తల = తల->తదుపరి) {
    size_t olen;
    స్థిరరాశి అక్షరం *option = తల->దత్తాంశం;
    స్థిరరాశి అక్షరం *arg;
    స్థిరరాశి అక్షరం *sep = strchr(option, '=');
    ఒకవేళ(sep) {
      olen = sep - option;
      arg = ++sep;
      ఒకవేళ(str_is_nonascii(arg))
        కొనసాగు;
      ఎంపిక(olen) {
      స్థితి 5:
        /* Terminal type */
        ఒకవేళ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          ఆపు;
        }
        ఫలితం = CURLE_UNKNOWN_OPTION;
        ఆపు;

      స్థితి 8:
        /* Display variable */
        ఒకవేళ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          ఆపు;
        }
        ఫలితం = CURLE_UNKNOWN_OPTION;
        ఆపు;

      స్థితి 7:
        /* Environment variable */
        ఒకవేళ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          ఒకవేళ(!beg) {
            ఫలితం = CURLE_బహిర్గతం_యొక్క_జ్ఞాపకం;
            ఆపు;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        లేకపోతే
          ఫలితం = CURLE_UNKNOWN_OPTION;
        ఆపు;

      స్థితి 2:
        /* Window Size */
        ఒకవేళ(curl_strnequal(option, "WS", 2)) {
          స్థిరరాశి అక్షరం *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          ఒకవేళ(curlx_str_సంఖ్య(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_సంఖ్య(&p, &y, 0xffff)) {
            failf(దత్తాంశం, "Syntax error in telnet option: %s", తల->దత్తాంశం);
            ఫలితం = CURLE_SETOPT_OPTION_SYNTAX;
          }
          లేకపోతే {
            tn->subopt_wsx = (గుర్తులేని పొట్టి)x;
            tn->subopt_wsy = (గుర్తులేని పొట్టి)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        లేకపోతే
          ఫలితం = CURLE_UNKNOWN_OPTION;
        ఆపు;

      స్థితి 6:
        /* To take care or not of the 8th bit in data exchange */
        ఒకవేళ(curl_strnequal(option, "BINARY", 6)) {
          స్థిరరాశి అక్షరం *p = arg;
          curl_off_t ద్విమాన_option;
          ఒకవేళ(!curlx_str_సంఖ్య(&p, &ద్విమాన_option, 1) &&
             (ద్విమాన_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ద్విమాన] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ద్విమాన] = CURL_NO;
          }
        }
        లేకపోతే
          ఫలితం = CURLE_UNKNOWN_OPTION;
        ఆపు;
      అప్రమేయ:
        failf(దత్తాంశం, "Unknown telnet option %s", తల->దత్తాంశం);
        ఫలితం = CURLE_UNKNOWN_OPTION;
        ఆపు;
      }
    }
    లేకపోతే {
      failf(దత్తాంశం, "Syntax error in telnet option: %s", తల->దత్తాంశం);
      ఫలితం = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  ఒకవేళ(ఫలితం) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ఖాళీ;
  }

  తిరిగి ఫలితం;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
స్థిర bool bad_option(స్థిరరాశి అక్షరం *దత్తాంశం)
{
  తిరిగి !దత్తాంశం || !!strchr(దత్తాంశం, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
స్థిర CURLcode suboption(నిర్మాణం Curl_easy *దత్తాంశం, నిర్మాణం TELNET *tn)
{
  నిర్మాణం curl_slist *v;
  గుర్తులేని అక్షరం తాత్కాలిక[2048];
  size_t nwritten;
  size_t len;
  CURLcode ఫలితం = CURLE_OK;

  ఒకవేళ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    తిరిగి CURLE_OK;

  printsub(దత్తాంశం, '<', (స్థిరరాశి గుర్తులేని అక్షరం *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  ఎంపిక(CURL_SB_GET(tn)) {
  స్థితి CURL_TELOPT_TTYPE:
    ఒకవేళ(bad_option(tn->subopt_ttype))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(అక్షరశ్రేణి_పొడవు(tn->subopt_ttype) > 1000) {
      failf(దత్తాంశం, "Too long telnet TTYPE");
      తిరిగి CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((అక్షరం *)తాత్కాలిక, పరిమాణం(తాత్కాలిక), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    ఫలితం = Curl_xfer_send(దత్తాంశం, తాత్కాలిక, len, FALSE, &nwritten);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Sending data failed: %s", curl_easy_strerror(ఫలితం));
      తిరిగి ఫలితం;
    }
    printsub(దత్తాంశం, '>', &తాత్కాలిక[2], len-2);
    ఆపు;
  స్థితి CURL_TELOPT_XDISPLOC:
    ఒకవేళ(bad_option(tn->subopt_xdisploc))
      తిరిగి CURLE_BAD_FUNCTION_వాదం;
    ఒకవేళ(అక్షరశ్రేణి_పొడవు(tn->subopt_xdisploc) > 1000) {
      failf(దత్తాంశం, "Too long telnet XDISPLOC");
      తిరిగి CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((అక్షరం *)తాత్కాలిక, పరిమాణం(తాత్కాలిక), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    ఫలితం = Curl_xfer_send(దత్తాంశం, తాత్కాలిక, len, FALSE, &nwritten);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Sending data failed: %s", curl_easy_strerror(ఫలితం));
      తిరిగి ఫలితం;
    }
    printsub(దత్తాంశం, '>', &తాత్కాలిక[2], len - 2);
    ఆపు;
  స్థితి CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((అక్షరం *)తాత్కాలిక, పరిమాణం(తాత్కాలిక), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    కొరకు(v = tn->telnet_vars; v; v = v->తదుపరి) {
      size_t tmplen = (అక్షరశ్రేణి_పొడవు(v->దత్తాంశం) + 1);
      ఒకవేళ(bad_option(v->దత్తాంశం))
        తిరిగి CURLE_BAD_FUNCTION_వాదం;
      /* Add the variable if it fits */
      ఒకవేళ(len + tmplen < పరిమాణం(తాత్కాలిక) - 6) {
        స్థిరరాశి అక్షరం *s = strchr(v->దత్తాంశం, ',');
        ఒకవేళ(!s)
          len += curl_msnprintf((అక్షరం *)&తాత్కాలిక[len], పరిమాణం(తాత్కాలిక) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->దత్తాంశం);
        లేకపోతే {
          size_t vlen = s - v->దత్తాంశం;
          len += curl_msnprintf((అక్షరం *)&తాత్కాలిక[len], పరిమాణం(తాత్కాలిక) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (పూర్ణాంకం)vlen, v->దత్తాంశం, CURL_NEW_ENV_విలువ, ++s);
        }
      }
    }
    curl_msnprintf((అక్షరం *)&తాత్కాలిక[len], పరిమాణం(తాత్కాలిక) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    ఫలితం = Curl_xfer_send(దత్తాంశం, తాత్కాలిక, len, FALSE, &nwritten);
    ఒకవేళ(ఫలితం) {
      failf(దత్తాంశం, "Sending data failed: %s", curl_easy_strerror(ఫలితం));
      తిరిగి ఫలితం;
    }
    printsub(దత్తాంశం, '>', &తాత్కాలిక[2], len - 2);
    ఆపు;
  }
  తిరిగి CURLE_OK;
}

స్థిర CURLcode telrcv(నిర్మాణం Curl_easy *దత్తాంశం,
                       నిర్మాణం TELNET *tn,
                       స్థిరరాశి గుర్తులేని అక్షరం *inbuf, /* Data received from
                                                      socket */
                       ssize_t లెక్క)              /* Number of bytes
                                                      received */
{
  గుర్తులేని అక్షరం c;
  CURLcode ఫలితం;
  పూర్ణాంకం లోపల = 0;
  పూర్ణాంకం startwrite = -1;

#నిర్వచించు startskipping()                                          \
  ఒకవేళ(startwrite >= 0) {                                          \
    ఫలితం = Curl_client_write(దత్తాంశం,                             \
                               CLIENTWRITE_BODY,                 \
                               (స్థిరరాశి అక్షరం *)&inbuf[startwrite], \
                               లోపల-startwrite);                   \
    ఒకవేళ(ఫలితం)                                                   \
      తిరిగి ఫలితం;                                             \
  }                                                              \
  startwrite = -1

#నిర్వచించు writebyte()  \
  ఒకవేళ(startwrite < 0) \
    startwrite = లోపల

#నిర్వచించు bufferflush() startskipping()

  అయితే(లెక్క--) {
    c = inbuf[లోపల];

    ఎంపిక(tn->telrcv_state) {
    స్థితి CURL_TS_CR:
      tn->telrcv_state = CURL_TS_దత్తాంశం;
      ఒకవేళ(c == '\0') {
        startskipping();
        ఆపు;   /* Ignore \0 after CR */
      }
      writebyte();
      ఆపు;

    స్థితి CURL_TS_దత్తాంశం:
      ఒకవేళ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        ఆపు;
      }
      లేకపోతే ఒకవేళ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      ఆపు;

    స్థితి CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      ఎంపిక(c) {
      స్థితి CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        ఆపు;
      స్థితి CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        ఆపు;
      స్థితి CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        ఆపు;
      స్థితి CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        ఆపు;
      స్థితి CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        ఆపు;
      స్థితి CURL_IAC:
        tn->telrcv_state = CURL_TS_దత్తాంశం;
        writebyte();
        ఆపు;
      స్థితి CURL_DM:
      స్థితి CURL_NOP:
      స్థితి CURL_GA:
      అప్రమేయ:
        tn->telrcv_state = CURL_TS_దత్తాంశం;
        printoption(దత్తాంశం, "RCVD", CURL_IAC, c);
        ఆపు;
      }
      ఆపు;

    స్థితి CURL_TS_WILL:
      printoption(దత్తాంశం, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(దత్తాంశం, tn, c);
      tn->telrcv_state = CURL_TS_దత్తాంశం;
      ఆపు;

    స్థితి CURL_TS_WONT:
      printoption(దత్తాంశం, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(దత్తాంశం, tn, c);
      tn->telrcv_state = CURL_TS_దత్తాంశం;
      ఆపు;

    స్థితి CURL_TS_DO:
      printoption(దత్తాంశం, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(దత్తాంశం, tn, c);
      tn->telrcv_state = CURL_TS_దత్తాంశం;
      ఆపు;

    స్థితి CURL_TS_DONT:
      printoption(దత్తాంశం, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(దత్తాంశం, tn, c);
      tn->telrcv_state = CURL_TS_దత్తాంశం;
      ఆపు;

    స్థితి CURL_TS_SB:
      ఒకవేళ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      లేకపోతే
        CURL_SB_ACCUM(tn, c);
      ఆపు;

    స్థితి CURL_TS_SE:
      ఒకవేళ(c != CURL_SE) {
        ఒకవేళ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(దత్తాంశం, "telnet: suboption error");
          తిరిగి CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      లేకపోతే {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_పదం(tn);
        ఫలితం = suboption(దత్తాంశం, tn);   /* handle sub-option */
        ఒకవేళ(ఫలితం)
          తిరిగి ఫలితం;
        tn->telrcv_state = CURL_TS_దత్తాంశం;
      }
      ఆపు;
    }
    ++లోపల;
  }
  bufferflush();
  తిరిగి CURLE_OK;
}

స్థిర CURLcode telnet_పూర్తి(నిర్మాణం Curl_easy *దత్తాంశం,
                            CURLcode స్థితి_2, bool premature)
{
  (శూన్య)స్థితి_2;
  (శూన్య)premature;
  Curl_meta_remove(దత్తాంశం, CURL_META_TELNET_EASY);
  తిరిగి CURLE_OK;
}

స్థిర CURLcode telnet_do(నిర్మాణం Curl_easy *దత్తాంశం, bool *పూర్తి)
{
  CURLcode ఫలితం;
  నిర్మాణం connectdata *conn = దత్తాంశం->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#నిర్వచితమైతే USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_లెక్క;
  DWORD wait_timeout;
  DWORD readfile_read;
#లేకపోతే
  timediff_t interval_ms;
  నిర్మాణం pollfd pfd[2];
  పూర్ణాంకం poll_cnt;
  ssize_t snread;
#అయితే_ముగింపు
  bool keepon = TRUE;
  అక్షరం బఫరు[4 * 1024];
  నిర్మాణం TELNET *tn;

  *పూర్తి = TRUE; /* unconditionally */

  ఫలితం = init_telnet(దత్తాంశం);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

  tn = Curl_meta_get(దత్తాంశం, CURL_META_TELNET_EASY);
  ఒకవేళ(!tn)
    తిరిగి CURLE_FAILED_INIT;

  ఫలితం = check_telnet_options(దత్తాంశం, tn);
  ఒకవేళ(ఫలితం)
    తిరిగి ఫలితం;

#నిర్వచితమైతే USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  ఒకవేళ(event_handle == WSA_INVALID_EVENT) {
    failf(దత్తాంశం, "WSACreateEvent failed (%d)", SOCKERRNO);
    తిరిగి CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  ఒకవేళ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    తిరిగి CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_ఆదానం_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  ఒకవేళ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || దత్తాంశం->సమితి.is_fread_సమితి) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_లెక్క = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  లేకపోతే {
    obj_లెక్క = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  అయితే(keepon) {
    స్థిరరాశి DWORD buf_కొలత = (DWORD)పరిమాణం(బఫరు);
    DWORD waitret = Wait_For_గుణిజం_Objects(obj_లెక్క, objs,
                                           FALSE, wait_timeout);
    ఎంపిక(waitret) {

    స్థితి WAIT_TIMEOUT: {
      కొరకు(;;) {
        ఒకవేళ(దత్తాంశం->సమితి.is_fread_సమితి) {
          size_t n;
          /* read from user-supplied method */
          n = దత్తాంశం->state.fread_func(బఫరు, 1, buf_కొలత, దత్తాంశం->state.లోపల);
          ఒకవేళ(n == CURL_READFUNC_ABORT) {
            failf(దత్తాంశం, "operation aborted by callback");
            keepon = FALSE;
            ఫలితం = CURLE_ABORTED_BY_CALLBACK;
            ఆపు;
          }

          ఒకవేళ(n == CURL_READFUNC_PAUSE)
            ఆపు;

          ఒకవేళ(n == 0)                        /* no bytes */
            ఆపు;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        లేకపోతే {
          /* read from stdin */
          ఒకవేళ(!PeekNamedPipe(stdin_handle, ఖాళీ, 0, ఖాళీ,
                            &readfile_read, ఖాళీ)) {
            keepon = FALSE;
            ఫలితం = CURLE_READ_ERROR;
            ఆపు;
          }

          ఒకవేళ(!readfile_read)
            ఆపు;

          ఒకవేళ(!ReadFile(stdin_handle, బఫరు, buf_కొలత, &readfile_read, ఖాళీ)) {
            keepon = FALSE;
            ఫలితం = CURLE_READ_ERROR;
            ఆపు;
          }
        }

        ఫలితం = send_telnet_దత్తాంశం(దత్తాంశం, tn, బఫరు, readfile_read);
        ఒకవేళ(ఫలితం) {
          keepon = FALSE;
          ఆపు;
        }
      }
    }
    ఆపు;

    స్థితి WAIT_OBJECT_0 + 1: {
      ఒకవేళ(!ReadFile(stdin_handle, బఫరు, buf_కొలత, &readfile_read, ఖాళీ)) {
        keepon = FALSE;
        ఫలితం = CURLE_READ_ERROR;
        ఆపు;
      }

      ఫలితం = send_telnet_దత్తాంశం(దత్తాంశం, tn, బఫరు, readfile_read);
      ఒకవేళ(ఫలితం) {
        keepon = FALSE;
        ఆపు;
      }
    }
    ఆపు;

    స్థితి WAIT_OBJECT_0: {
      events.l_జాలం_Events = 0;
      ఒకవేళ(WSAEnum_జాలం_Events(sockfd, event_handle, &events) != 0) {
        పూర్ణాంకం sockerr = SOCKERRNO;
        ఒకవేళ(sockerr != SOCKEINPROGRESS) {
          infof(దత్తాంశం, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          ఫలితం = CURLE_READ_ERROR;
        }
        ఆపు;
      }
      ఒకవేళ(events.l_జాలం_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        ఫలితం = Curl_xfer_recv(దత్తాంశం, బఫరు, పరిమాణం(బఫరు), &nread);
        /* read would have blocked. Loop again */
        ఒకవేళ(ఫలితం == CURLE_AGAIN)
          ఆపు;
        /* returned not-zero, this an error */
        లేకపోతే ఒకవేళ(ఫలితం) {
          keepon = FALSE;
          ఆపు;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        లేకపోతే ఒకవేళ(!nread) {
          keepon = FALSE;
          ఆపు;
        }

        ఫలితం = telrcv(దత్తాంశం, tn, (గుర్తులేని అక్షరం *)బఫరు, nread);
        ఒకవేళ(ఫలితం) {
          keepon = FALSE;
          ఆపు;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ఒకవేళ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(దత్తాంశం, tn);
          tn->already_negotiated = 1;
        }
      }
      ఒకవేళ(events.l_జాలం_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      ఆపు;
    }
    } /* switch */

    ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
      failf(దత్తాంశం, "Time-out");
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  ఒకవేళ(!WSACloseEvent(event_handle)) {
    infof(దత్తాంశం, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#లేకపోతే
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  ఒకవేళ(దత్తాంశం->సమితి.is_fread_సమితి) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  లేకపోతే {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)దత్తాంశం->state.లోపల);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    ఒకవేళ(pfd[1].fd < 0) {
      failf(దత్తాంశం, "cannot read input");
      ఫలితం = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  అయితే(keepon) {
    DEBUGF(infof(దత్తాంశం, "telnet_do, poll %d fds", poll_cnt));
    ఎంపిక(Curl_poll(pfd, (గుర్తులేని పూర్ణాంకం)poll_cnt, interval_ms)) {
    స్థితి -1:                    /* error, stop reading */
      ఫలితం = CURLE_RECV_ERROR;
      keepon = FALSE;
      కొనసాగు;
    స్థితి 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    అప్రమేయ:                    /* read! */
      ఒకవేళ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        ఫలితం = Curl_xfer_recv(దత్తాంశం, బఫరు, పరిమాణం(బఫరు), &nread);
        /* read would have blocked. Loop again */
        ఒకవేళ(ఫలితం == CURLE_AGAIN)
          ఆపు;
        /* returned not-zero, this an error */
        ఒకవేళ(ఫలితం) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          ఒకవేళ(దత్తాంశం->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(దత్తాంశం, "telnet_do, unexpected ECONNRESET on recv"));
          }
          ఆపు;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        లేకపోతే ఒకవేళ(!nread) {
          keepon = FALSE;
          ఆపు;
        }

        Curl_pgrs_download_inc(దత్తాంశం, nread);
        ఫలితం = telrcv(దత్తాంశం, tn, (గుర్తులేని అక్షరం *)బఫరు, nread);
        ఒకవేళ(ఫలితం) {
          keepon = FALSE;
          ఆపు;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ఒకవేళ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(దత్తాంశం, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      ఒకవేళ(poll_cnt == 2) {
        ఒకవేళ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = పఠ(pfd[1].fd, బఫరు, పరిమాణం(బఫరు));
        }
      }
      లేకపోతే {
        /* read from user-supplied method */
        snread = (పూర్ణాంకం)దత్తాంశం->state.fread_func(బఫరు, 1, పరిమాణం(బఫరు),
                                             దత్తాంశం->state.లోపల);
        ఒకవేళ(snread == CURL_READFUNC_ABORT) {
          failf(దత్తాంశం, "operation aborted by callback");
          ఫలితం = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          ఆపు;
        }
        ఒకవేళ(snread == CURL_READFUNC_PAUSE)
          ఆపు;
      }

      ఒకవేళ(snread > 0) {
        ఫలితం = send_telnet_దత్తాంశం(దత్తాంశం, tn, బఫరు, snread);
        ఒకవేళ(ఫలితం) {
          keepon = FALSE;
          ఆపు;
        }
        Curl_pgrs_upload_inc(దత్తాంశం, (size_t)snread);
      }
      లేకపోతే ఒకవేళ(snread < 0) {
        failf(దత్తాంశం, "Failed to read input");
        ఫలితం = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      ఆపు;
    } /* poll switch statement */

    ఒకవేళ(Curl_timeleft_ms(దత్తాంశం) < 0) {
      failf(దత్తాంశం, "Time-out");
      ఫలితం = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    ఒకవేళ(!ఫలితం) {
      ఫలితం = Curl_pgrsUpdate(దత్తాంశం);
      ఒకవేళ(ఫలితం)
        keepon = FALSE;
    }
  }
#అయితే_ముగింపు
  /* mark this as "no further transfer wanted" */
  Curl_xfer_అమరిక_nop(దత్తాంశం);

  తిరిగి ఫలితం;
}

/*
 * TELNET protocol handler.
 */
స్థిరరాశి నిర్మాణం Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_పూర్తి,                          /* done */
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

#అయితే_ముగింపు /* !CURL_DISABLE_TELNET */
