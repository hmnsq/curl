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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"
#ଅନ୍ତର୍ଭାଵ "urldata.h"
#ଅନ୍ତର୍ଭାଵ "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#ଅନ୍ତର୍ଭାଵ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ଅନ୍ତର୍ଭାଵ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ଅନ୍ତର୍ଭାଵ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ଅନ୍ତର୍ଭାଵ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ଅନ୍ତର୍ଭାଵ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ଅନ୍ତର୍ଭାଵ <sys/param.h>
#endif

#ଅନ୍ତର୍ଭାଵ "url.h"
#ଅନ୍ତର୍ଭାଵ "transfer.h"
#ଅନ୍ତର୍ଭାଵ "sendf.h"
#ଅନ୍ତର୍ଭାଵ "curl_trc.h"
#ଅନ୍ତର୍ଭାଵ "progress.h"
#ଅନ୍ତର୍ଭାଵ "arpa_telnet.h"
#ଅନ୍ତର୍ଭାଵ "connect.h"
#ଅନ୍ତର୍ଭାଵ "select.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  କର {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } ଯେତେବେଳେ(0)
#define CURL_SB_ACCUM(x, c)                                         \
  କର {                                                              \
    ଯଦି((x)->subpointer < ((x)->subbuffer + ଆକାର((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } ଯେତେବେଳେ(0)

#define CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#define CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#define CURL_NO          0
#define CURL_YES         1
#define CURL_WANTYES     2
#define CURL_WANTNO      3

#define CURL_EMPTY       0
#define CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#define CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା {
  CURL_TS_DATA = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

ସଂରଚନା TELNET {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା please_negotiate;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା already_negotiated;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା us[256];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା usq[256];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା us_preferred[256];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା him[256];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା himq[256];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା him_preferred[256];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା subnegotiation[256];
  ଅଚଳ ଅକ୍ଷର *subopt_ttype;          /* Set with suboption TTYPE */
  ଅଚଳ ଅକ୍ଷର *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  ସଂରଚନା curl_slist *telnet_vars;    /* Environment variables */
  ସଂରଚନା dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned ଅକ୍ଷର subbuffer[SUBBUFSIZE];
  unsigned ଅକ୍ଷର *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#ଅନ୍ୟଥା
ସ୍ଥିର ଶୂନ୍ୟ printoption(ସଂରଚନା Curl_easy *data,
                        ଅଚଳ ଅକ୍ଷର *direction, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  ଯଦି(data->set.verbose) {
    ଯଦି(cmd == CURL_IAC) {
      ଯଦି(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      ଅନ୍ୟଥା
        infof(data, "%s IAC %d", direction, option);
    }
    ଅନ୍ୟଥା {
      ଅଚଳ ଅକ୍ଷର *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      ଯଦି(fmt) {
        ଅଚଳ ଅକ୍ଷର *opt;
        ଯଦି(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        ଅନ୍ୟଥା ଯଦି(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        ଅନ୍ୟଥା
          opt = ଖାଲି;

        ଯଦି(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        ଅନ୍ୟଥା
          infof(data, "%s %s %d", direction, fmt, option);
      }
      ଅନ୍ୟଥା
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

ସ୍ଥିର ଶୂନ୍ୟ telnet_easy_dtor(ଅଚଳ ଶୂନ୍ୟ *ଚାବି, size_t klen, ଶୂନ୍ୟ *entry)
{
  ସଂରଚନା TELNET *tn = entry;
  (ଶୂନ୍ୟ)ଚାବି;
  (ଶୂନ୍ୟ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

ସ୍ଥିର CURLcode init_telnet(ସଂରଚନା Curl_easy *data)
{
  ସଂରଚନା TELNET *tn;

  tn = curlx_calloc(1, ଆକାର(ସଂରଚନା TELNET));
  ଯଦି(!tn)
    ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&tn->out, 0xffff);

  tn->telrcv_state = CURL_TS_DATA;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_ଦ୍ୱିମିକ] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ଦ୍ୱିମିକ] = CURL_YES;

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

  ଫେରନ୍ତୁ Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ସ୍ଥିର ଶୂନ୍ୟ send_negotiation(ସଂରଚନା Curl_easy *data, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cmd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  unsigned ଅକ୍ଷର buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned ଅକ୍ଷର)cmd;
  buf[2] = (unsigned ଅକ୍ଷର)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  ଯଦି(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

ସ୍ଥିର ଶୂନ୍ୟ set_remote_option(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn,
                              ପୂର୍ଣ୍ଣସଂଖ୍ୟା option, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newstate)
{
  ଯଦି(newstate == CURL_YES) {
    ବାଛ(tn->him[option]) {
    ସ୍ଥିତି CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_YES:
      /* Already enabled */
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTNO:
      ବାଛ(tn->himq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTYES:
      ବାଛ(tn->himq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        /* Error: already negotiating for enable */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    }
  }
  ଅନ୍ୟଥା { /* NO */
    ବାଛ(tn->him[option]) {
    ସ୍ଥିତି CURL_NO:
      /* Already disabled */
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTNO:
      ବାଛ(tn->himq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        /* Already negotiating for NO */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTYES:
      ବାଛ(tn->himq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    }
  }
}

ସ୍ଥିର ଶୂନ୍ୟ set_local_option(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn,
                             ପୂର୍ଣ୍ଣସଂଖ୍ୟା option, ପୂର୍ଣ୍ଣସଂଖ୍ୟା newstate)
{
  ଯଦି(newstate == CURL_YES) {
    ବାଛ(tn->us[option]) {
    ସ୍ଥିତି CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_YES:
      /* Already enabled */
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTNO:
      ବାଛ(tn->usq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTYES:
      ବାଛ(tn->usq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        /* Error: already negotiating for enable */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    }
  }
  ଅନ୍ୟଥା { /* NO */
    ବାଛ(tn->us[option]) {
    ସ୍ଥିତି CURL_NO:
      /* Already disabled */
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTNO:
      ବାଛ(tn->usq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        /* Already negotiating for NO */
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_WANTYES:
      ବାଛ(tn->usq[option]) {
      ସ୍ଥିତି CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_OPPOSITE:
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;
    }
  }
}

ସ୍ଥିର ଶୂନ୍ୟ telnet_negotiate(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

  ପାଇଁ(i = 0; i < CURL_NTELOPTS; i++) {
    ଯଦି(i == CURL_TELOPT_ECHO)
      ଜାରି;

    ଯଦି(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    ଯଦି(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

ସ୍ଥିର ଶୂନ୍ୟ rec_will(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  ବାଛ(tn->him[option]) {
  ସ୍ଥିତି CURL_NO:
    ଯଦି(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    ଅନ୍ୟଥା
      send_negotiation(data, CURL_DONT, option);

    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_YES:
    /* Already enabled */
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTNO:
    ବାଛ(tn->himq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTYES:
    ବାଛ(tn->himq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      tn->him[option] = CURL_YES;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ rec_wont(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  ବାଛ(tn->him[option]) {
  ସ୍ଥିତି CURL_NO:
    /* Already disabled */
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTNO:
    ବାଛ(tn->himq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      tn->him[option] = CURL_NO;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTYES:
    ବାଛ(tn->himq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      tn->him[option] = CURL_NO;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ printsub(ସଂରଚନା Curl_easy *data,
                     ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction,                /* '<' or '>' */
                     ଅଚଳ unsigned ଅକ୍ଷର *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  ଯଦି(data->set.verbose) {
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0;
    ଯଦି(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      ଯଦି(length >= 3) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        ଯଦି(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          ଯଦି(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          ଅନ୍ୟଥା ଯଦି(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          ଅନ୍ୟଥା
            infof(data, "%u ", i);
          ଯଦି(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          ଅନ୍ୟଥା ଯଦି(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          ଅନ୍ୟଥା
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      ଯଦି(length >= 2)
        length -= 2;
      ଅନ୍ୟଥା /* bad input */
        ଫେରନ୍ତୁ;
    }
    ଯଦି(length <= 1) {
      infof(data, "(Empty suboption?)");
      ଫେରନ୍ତୁ;
    }

    ଯଦି(CURL_TELOPT_OK(pointer[0])) {
      ବାଛ(pointer[0]) {
      ସ୍ଥିତି CURL_TELOPT_TTYPE:
      ସ୍ଥିତି CURL_TELOPT_XDISPLOC:
      ସ୍ଥିତି CURL_TELOPT_NEW_ENVIRON:
      ସ୍ଥିତି CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        ଭାଙ୍ଗ;
      }
    }
    ଅନ୍ୟଥା
      infof(data, "%d (unknown)", pointer[0]);

    ବାଛ(pointer[0]) {
    ସ୍ଥିତି CURL_TELOPT_NAWS:
      ଯଦି(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
      ବାଛ(pointer[1]) {
      ସ୍ଥିତି CURL_TELQUAL_IS:
        infof(data, " IS");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_TELQUAL_SEND:
        infof(data, " SEND");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_TELQUAL_NAME:
        infof(data, " NAME");
        ଭାଙ୍ଗ;
      }

      ବାଛ(pointer[0]) {
      ସ୍ଥିତି CURL_TELOPT_TTYPE:
      ସ୍ଥିତି CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)((length > 2) ? (length - 2) : 0), &pointer[2]);
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_TELOPT_NEW_ENVIRON:
        ଯଦି(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          ପାଇଁ(i = 3; i < length; i++) {
            ବାଛ(pointer[i]) {
            ସ୍ଥିତି CURL_NEW_ENV_VAR:
              infof(data, ", ");
              ଭାଙ୍ଗ;
            ସ୍ଥିତି CURL_NEW_ENV_ମୂଲ୍ୟ:
              infof(data, " = ");
              ଭାଙ୍ଗ;
            ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
              infof(data, "%c", pointer[i]);
              ଭାଙ୍ଗ;
            }
          }
        }
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        ପାଇଁ(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        ଭାଙ୍ଗ;
      }
    }
  }
}

/* Escape and send a telnet data block */
ସ୍ଥିର CURLcode send_telnet_data(ସଂରଚନା Curl_easy *data,
                                 ସଂରଚନା TELNET *tn,
                                 ଅଚଳ ଅକ୍ଷର *buffer, ssize_t nread)
{
  size_t i, outlen;
  ଅଚଳ unsigned ଅକ୍ଷର *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  ସଂରଚନା connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  ଯଦି(nread < 0)
    ଫେରନ୍ତୁ CURLE_TOO_LARGE;

  ଯଦି(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    ପାଇଁ(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      ଯଦି(!result && ((unsigned ଅକ୍ଷର)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  ଅନ୍ୟଥା {
    outlen = (size_t)nread;
    outbuf = (ଅଚଳ unsigned ଅକ୍ଷର *)buffer;
  }
  ଯେତେବେଳେ(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    ସଂରଚନା pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    ଯଦି(timeout_ms < 0)
      ଫେରନ୍ତୁ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    ବାଛ(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    ସ୍ଥିତି -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      ଭାଙ୍ଗ;
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      ଭାଙ୍ଗ;
    }
  }

  ଫେରନ୍ତୁ result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ସ୍ଥିର ଶୂନ୍ୟ sendsuboption(ସଂରଚନା Curl_easy *data,
                          ସଂରଚନା TELNET *tn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  size_t nwritten;
  unsigned short x, y;
  ଅଚଳ unsigned ଅକ୍ଷର *uc1, *uc2;
  CURLcode result;

  ବାଛ(option) {
  ସ୍ଥିତି CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ଅଚଳ unsigned ଅକ୍ଷର *)&x;
    uc2 = (ଅଚଳ unsigned ଅକ୍ଷର *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (ଅଚଳ unsigned ଅକ୍ଷର *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    ଯଦି(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (ଅଚଳ ଅକ୍ଷର *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    ଯଦି(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ rec_do(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  ବାଛ(tn->us[option]) {
  ସ୍ଥିତି CURL_NO:
    ଯଦି(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      ଯଦି(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    ଅନ୍ୟଥା ଯଦି(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    ଅନ୍ୟଥା
      send_negotiation(data, CURL_WONT, option);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_YES:
    /* Already enabled */
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTNO:
    ବାଛ(tn->usq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTYES:
    ବାଛ(tn->usq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      tn->us[option] = CURL_YES;
      ଯଦି(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର ଶୂନ୍ୟ rec_dont(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn, ପୂର୍ଣ୍ଣସଂଖ୍ୟା option)
{
  ବାଛ(tn->us[option]) {
  ସ୍ଥିତି CURL_NO:
    /* Already disabled */
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTNO:
    ବାଛ(tn->usq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      tn->us[option] = CURL_NO;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;

  ସ୍ଥିତି CURL_WANTYES:
    ବାଛ(tn->usq[option]) {
    ସ୍ଥିତି CURL_EMPTY:
      tn->us[option] = CURL_NO;
      ଭାଙ୍ଗ;
    ସ୍ଥିତି CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      ଭାଙ୍ଗ;
    }
    ଭାଙ୍ଗ;
  }
}

ସ୍ଥିର bool str_is_nonascii(ଅଚଳ ଅକ୍ଷର *str)
{
  ଅକ୍ଷର c;
  ଯେତେବେଳେ((c = *str++) != 0)
    ଯଦି(c & 0x80)
      ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର CURLcode check_telnet_options(ସଂରଚନା Curl_easy *data,
                                     ସଂରଚନା TELNET *tn)
{
  ସଂରଚନା curl_slist *head;
  ସଂରଚନା curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  ଯଦି(data->state.creds) {
    ଅକ୍ଷର buffer[256];
    ଯଦି(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, ଆକାର(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    ଯଦି(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ଖାଲି;
      ଫେରନ୍ତୁ CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  ପାଇଁ(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    ଅଚଳ ଅକ୍ଷର *option = head->data;
    ଅଚଳ ଅକ୍ଷର *arg;
    ଅଚଳ ଅକ୍ଷର *sep = strchr(option, '=');
    ଯଦି(sep) {
      olen = sep - option;
      arg = ++sep;
      ଯଦି(str_is_nonascii(arg))
        ଜାରି;
      ବାଛ(olen) {
      ସ୍ଥିତି 5:
        /* Terminal type */
        ଯଦି(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          ଭାଙ୍ଗ;
        }
        result = CURLE_UNKNOWN_OPTION;
        ଭାଙ୍ଗ;

      ସ୍ଥିତି 8:
        /* Display variable */
        ଯଦି(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          ଭାଙ୍ଗ;
        }
        result = CURLE_UNKNOWN_OPTION;
        ଭାଙ୍ଗ;

      ସ୍ଥିତି 7:
        /* Environment variable */
        ଯଦି(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          ଯଦି(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            ଭାଙ୍ଗ;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        ଅନ୍ୟଥା
          result = CURLE_UNKNOWN_OPTION;
        ଭାଙ୍ଗ;

      ସ୍ଥିତି 2:
        /* Window Size */
        ଯଦି(curl_strnequal(option, "WS", 2)) {
          ଅଚଳ ଅକ୍ଷର *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_ସଂଖ୍ୟା(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          ଅନ୍ୟଥା {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        ଅନ୍ୟଥା
          result = CURLE_UNKNOWN_OPTION;
        ଭାଙ୍ଗ;

      ସ୍ଥିତି 6:
        /* To take care or not of the 8th bit in data exchange */
        ଯଦି(curl_strnequal(option, "BINARY", 6)) {
          ଅଚଳ ଅକ୍ଷର *p = arg;
          curl_off_t ଦ୍ୱିମିକ_option;
          ଯଦି(!curlx_str_ସଂଖ୍ୟା(&p, &ଦ୍ୱିମିକ_option, 1) &&
             (ଦ୍ୱିମିକ_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ଦ୍ୱିମିକ] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ଦ୍ୱିମିକ] = CURL_NO;
          }
        }
        ଅନ୍ୟଥା
          result = CURLE_UNKNOWN_OPTION;
        ଭାଙ୍ଗ;
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        ଭାଙ୍ଗ;
      }
    }
    ଅନ୍ୟଥା {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  ଯଦି(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ଖାଲି;
  }

  ଫେରନ୍ତୁ result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ସ୍ଥିର bool bad_option(ଅଚଳ ଅକ୍ଷର *data)
{
  ଫେରନ୍ତୁ !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ସ୍ଥିର CURLcode suboption(ସଂରଚନା Curl_easy *data, ସଂରଚନା TELNET *tn)
{
  ସଂରଚନା curl_slist *v;
  unsigned ଅକ୍ଷର temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  ଯଦି(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ଫେରନ୍ତୁ CURLE_OK;

  printsub(data, '<', (ଅଚଳ unsigned ଅକ୍ଷର *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  ବାଛ(CURL_SB_GET(tn)) {
  ସ୍ଥିତି CURL_TELOPT_TTYPE:
    ଯଦି(bad_option(tn->subopt_ttype))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ଅକ୍ଷର *)temp, ଆକାର(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ଯଦି(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ଫେରନ୍ତୁ result;
    }
    printsub(data, '>', &temp[2], len-2);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_TELOPT_XDISPLOC:
    ଯଦି(bad_option(tn->subopt_xdisploc))
      ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
    ଯଦି(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      ଫେରନ୍ତୁ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ଅକ୍ଷର *)temp, ଆକାର(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ଯଦି(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ଫେରନ୍ତୁ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    ଭାଙ୍ଗ;
  ସ୍ଥିତି CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((ଅକ୍ଷର *)temp, ଆକାର(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    ପାଇଁ(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      ଯଦି(bad_option(v->data))
        ଫେରନ୍ତୁ CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      ଯଦି(len + tmplen < ଆକାର(temp) - 6) {
        ଅଚଳ ଅକ୍ଷର *s = strchr(v->data, ',');
        ଯଦି(!s)
          len += curl_msnprintf((ଅକ୍ଷର *)&temp[len], ଆକାର(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        ଅନ୍ୟଥା {
          size_t vlen = s - v->data;
          len += curl_msnprintf((ଅକ୍ଷର *)&temp[len], ଆକାର(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)vlen, v->data, CURL_NEW_ENV_ମୂଲ୍ୟ, ++s);
        }
      }
    }
    curl_msnprintf((ଅକ୍ଷର *)&temp[len], ଆକାର(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ଯଦି(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ଫେରନ୍ତୁ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode telrcv(ସଂରଚନା Curl_easy *data,
                       ସଂରଚନା TELNET *tn,
                       ଅଚଳ unsigned ଅକ୍ଷର *inbuf, /* Data received from
                                                      socket */
                       ssize_t ଗଣନା_2)              /* Number of bytes
                                                      received */
{
  unsigned ଅକ୍ଷର c;
  CURLcode result;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା in = 0;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା startwrite = -1;

#define startskipping()                                          \
  ଯଦି(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (ଅଚଳ ଅକ୍ଷର *)&inbuf[startwrite], \
                               in-startwrite);                   \
    ଯଦି(result)                                                   \
      ଫେରନ୍ତୁ result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  ଯଦି(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  ଯେତେବେଳେ(ଗଣନା_2--) {
    c = inbuf[in];

    ବାଛ(tn->telrcv_state) {
    ସ୍ଥିତି CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      ଯଦି(c == '\0') {
        startskipping();
        ଭାଙ୍ଗ;   /* Ignore \0 after CR */
      }
      writebyte();
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_DATA:
      ଯଦି(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        ଭାଙ୍ଗ;
      }
      ଅନ୍ୟଥା ଯଦି(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      ବାଛ(c) {
      ସ୍ଥିତି CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        ଭାଙ୍ଗ;
      ସ୍ଥିତି CURL_DM:
      ସ୍ଥିତି CURL_NOP:
      ସ୍ଥିତି CURL_GA:
      ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        ଭାଙ୍ଗ;
      }
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_SB:
      ଯଦି(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      ଅନ୍ୟଥା
        CURL_SB_ACCUM(tn, c);
      ଭାଙ୍ଗ;

    ସ୍ଥିତି CURL_TS_SE:
      ଯଦି(c != CURL_SE) {
        ଯଦି(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          ଫେରନ୍ତୁ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      ଅନ୍ୟଥା {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        ଯଦି(result)
          ଫେରନ୍ତୁ result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      ଭାଙ୍ଗ;
    }
    ++in;
  }
  bufferflush();
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode telnet_done(ସଂରଚନା Curl_easy *data,
                            CURLcode status, bool premature)
{
  (ଶୂନ୍ୟ)status;
  (ଶୂନ୍ୟ)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  ଫେରନ୍ତୁ CURLE_OK;
}

ସ୍ଥିର CURLcode telnet_do(ସଂରଚନା Curl_easy *data, bool *done)
{
  CURLcode result;
  ସଂରଚନା connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_ଗଣନା;
  DWORD wait_timeout;
  DWORD readfile_read;
#ଅନ୍ୟଥା
  timediff_t interval_ms;
  ସଂରଚନା pollfd pfd[2];
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  ଅକ୍ଷର buffer[4 * 1024];
  ସଂରଚନା TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  ଯଦି(!tn)
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  ଯଦି(result)
    ଫେରନ୍ତୁ result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  ଯଦି(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    ଫେରନ୍ତୁ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  ଯଦି(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ଫେରନ୍ତୁ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_ନିବେଶ_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  ଯଦି(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_ଗଣନା = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  ଅନ୍ୟଥା {
    obj_ଗଣନା = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  ଯେତେବେଳେ(keepon) {
    ଅଚଳ DWORD buf_size = (DWORD)ଆକାର(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_ଗଣନା, objs,
                                           FALSE, wait_timeout);
    ବାଛ(waitret) {

    ସ୍ଥିତି WAIT_TIMEOUT: {
      ପାଇଁ(;;) {
        ଯଦି(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          ଯଦି(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            ଭାଙ୍ଗ;
          }

          ଯଦି(n == CURL_READFUNC_PAUSE)
            ଭାଙ୍ଗ;

          ଯଦି(n == 0)                        /* no bytes */
            ଭାଙ୍ଗ;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        ଅନ୍ୟଥା {
          /* read from stdin */
          ଯଦି(!PeekNamedPipe(stdin_handle, ଖାଲି, 0, ଖାଲି,
                            &readfile_read, ଖାଲି)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            ଭାଙ୍ଗ;
          }

          ଯଦି(!readfile_read)
            ଭାଙ୍ଗ;

          ଯଦି(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ଖାଲି)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            ଭାଙ୍ଗ;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        ଯଦି(result) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }
      }
    }
    ଭାଙ୍ଗ;

    ସ୍ଥିତି WAIT_OBJECT_0 + 1: {
      ଯଦି(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ଖାଲି)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        ଭାଙ୍ଗ;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      ଯଦି(result) {
        keepon = FALSE;
        ଭାଙ୍ଗ;
      }
    }
    ଭାଙ୍ଗ;

    ସ୍ଥିତି WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      ଯଦି(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        ପୂର୍ଣ୍ଣସଂଖ୍ୟା sockerr = SOCKERRNO;
        ଯଦି(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        ଭାଙ୍ଗ;
      }
      ଯଦି(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, ଆକାର(buffer), &nread);
        /* read would have blocked. Loop again */
        ଯଦି(result == CURLE_AGAIN)
          ଭାଙ୍ଗ;
        /* returned not-zero, this an error */
        ଅନ୍ୟଥା ଯଦି(result) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ଅନ୍ୟଥା ଯଦି(!nread) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }

        result = telrcv(data, tn, (unsigned ଅକ୍ଷର *)buffer, nread);
        ଯଦି(result) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ଯଦି(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      ଯଦି(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      ଭାଙ୍ଗ;
    }
    } /* switch */

    ଯଦି(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  ଯଦି(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#ଅନ୍ୟଥା
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  ଯଦି(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  ଅନ୍ୟଥା {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    ଯଦି(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  ଯେତେବେଳେ(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    ବାଛ(Curl_poll(pfd, (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)poll_cnt, interval_ms)) {
    ସ୍ଥିତି -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      ଜାରି;
    ସ୍ଥିତି 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:                    /* read! */
      ଯଦି(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, ଆକାର(buffer), &nread);
        /* read would have blocked. Loop again */
        ଯଦି(result == CURLE_AGAIN)
          ଭାଙ୍ଗ;
        /* returned not-zero, this an error */
        ଯଦି(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          ଯଦି(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          ଭାଙ୍ଗ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ଅନ୍ୟଥା ଯଦି(!nread) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned ଅକ୍ଷର *)buffer, nread);
        ଯଦି(result) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ଯଦି(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      ଯଦି(poll_cnt == 2) {
        ଯଦି(pfd[1].revents & POLLIN) { /* read from in file */
          snread = ପଠ(pfd[1].fd, buffer, ଆକାର(buffer));
        }
      }
      ଅନ୍ୟଥା {
        /* read from user-supplied method */
        snread = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)data->state.fread_func(buffer, 1, ଆକାର(buffer),
                                             data->state.in);
        ଯଦି(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }
        ଯଦି(snread == CURL_READFUNC_PAUSE)
          ଭାଙ୍ଗ;
      }

      ଯଦି(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        ଯଦି(result) {
          keepon = FALSE;
          ଭାଙ୍ଗ;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      ଅନ୍ୟଥା ଯଦି(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      ଭାଙ୍ଗ;
    } /* poll switch statement */

    ଯଦି(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    ଯଦି(!result) {
      result = Curl_pgrsUpdate(data);
      ଯଦି(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  ଫେରନ୍ତୁ result;
}

/*
 * TELNET protocol handler.
 */
ଅଚଳ ସଂରଚନା Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_done,                          /* done */
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

#endif /* !CURL_DISABLE_TELNET */
