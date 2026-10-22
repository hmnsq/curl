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
#ಅನ್ತರ್ಭಾವ "curl_setup.h"
#ಅನ್ತರ್ಭಾವ "urldata.h"
#ಅನ್ತರ್ಭಾವ "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#endif

#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "arpa_telnet.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  ಕುರು {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } ಯಾವತ್(0)
#define CURL_SB_ACCUM(x, c)                                         \
  ಕುರು {                                                              \
    ಯದಿ((x)->subpointer < ((x)->subbuffer + ಪರಿಮಾಣ((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } ಯಾವತ್(0)

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
ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ {
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

ಸಂರಚನಾ TELNET {
  ಪೂರ್ಣಾಙ್ಕ please_negotiate;
  ಪೂರ್ಣಾಙ್ಕ already_negotiated;
  ಪೂರ್ಣಾಙ್ಕ us[256];
  ಪೂರ್ಣಾಙ್ಕ usq[256];
  ಪೂರ್ಣಾಙ್ಕ us_preferred[256];
  ಪೂರ್ಣಾಙ್ಕ him[256];
  ಪೂರ್ಣಾಙ್ಕ himq[256];
  ಪೂರ್ಣಾಙ್ಕ him_preferred[256];
  ಪೂರ್ಣಾಙ್ಕ subnegotiation[256];
  ಧ್ರುವ ವರ್ಣ *subopt_ttype;          /* Set with suboption TTYPE */
  ಧ್ರುವ ವರ್ಣ *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  ಸಂರಚನಾ curl_slist *telnet_vars;    /* Environment variables */
  ಸಂರಚನಾ dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned ವರ್ಣ subbuffer[SUBBUFSIZE];
  unsigned ವರ್ಣ *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಶೂನ್ಯ printoption(ಸಂರಚನಾ Curl_easy *data,
                        ಧ್ರುವ ವರ್ಣ *direction, ಪೂರ್ಣಾಙ್ಕ cmd, ಪೂರ್ಣಾಙ್ಕ option)
{
  ಯದಿ(data->set.verbose) {
    ಯದಿ(cmd == CURL_IAC) {
      ಯದಿ(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      ಅನ್ಯಥಾ
        infof(data, "%s IAC %d", direction, option);
    }
    ಅನ್ಯಥಾ {
      ಧ್ರುವ ವರ್ಣ *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      ಯದಿ(fmt) {
        ಧ್ರುವ ವರ್ಣ *opt;
        ಯದಿ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        ಅನ್ಯಥಾ ಯದಿ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        ಅನ್ಯಥಾ
          opt = ರಿಕ್ತ;

        ಯದಿ(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        ಅನ್ಯಥಾ
          infof(data, "%s %s %d", direction, fmt, option);
      }
      ಅನ್ಯಥಾ
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

ಸ್ಥಿರ ಶೂನ್ಯ telnet_easy_dtor(ಧ್ರುವ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *entry)
{
  ಸಂರಚನಾ TELNET *tn = entry;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

ಸ್ಥಿರ CURLcode init_telnet(ಸಂರಚನಾ Curl_easy *data)
{
  ಸಂರಚನಾ TELNET *tn;

  tn = curlx_calloc(1, ಪರಿಮಾಣ(ಸಂರಚನಾ TELNET));
  ಯದಿ(!tn)
    ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_ದ್ವಿಮಾನ] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ದ್ವಿಮಾನ] = CURL_YES;

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

  ಪ್ರತ್ಯಾಗಮ Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ಸ್ಥಿರ ಶೂನ್ಯ send_negotiation(ಸಂರಚನಾ Curl_easy *data, ಪೂರ್ಣಾಙ್ಕ cmd, ಪೂರ್ಣಾಙ್ಕ option)
{
  unsigned ವರ್ಣ buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned ವರ್ಣ)cmd;
  buf[2] = (unsigned ವರ್ಣ)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  ಯದಿ(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

ಸ್ಥಿರ ಶೂನ್ಯ set_remote_option(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn,
                              ಪೂರ್ಣಾಙ್ಕ option, ಪೂರ್ಣಾಙ್ಕ newstate)
{
  ಯದಿ(newstate == CURL_YES) {
    ವಿಕಲ್ಪ(tn->him[option]) {
    ಪಕ್ಷ CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_YES:
      /* Already enabled */
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTNO:
      ವಿಕಲ್ಪ(tn->himq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTYES:
      ವಿಕಲ್ಪ(tn->himq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        /* Error: already negotiating for enable */
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    }
  }
  ಅನ್ಯಥಾ { /* NO */
    ವಿಕಲ್ಪ(tn->him[option]) {
    ಪಕ್ಷ CURL_NO:
      /* Already disabled */
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTNO:
      ವಿಕಲ್ಪ(tn->himq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        /* Already negotiating for NO */
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTYES:
      ವಿಕಲ್ಪ(tn->himq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ set_local_option(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn,
                             ಪೂರ್ಣಾಙ್ಕ option, ಪೂರ್ಣಾಙ್ಕ newstate)
{
  ಯದಿ(newstate == CURL_YES) {
    ವಿಕಲ್ಪ(tn->us[option]) {
    ಪಕ್ಷ CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_YES:
      /* Already enabled */
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTNO:
      ವಿಕಲ್ಪ(tn->usq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTYES:
      ವಿಕಲ್ಪ(tn->usq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        /* Error: already negotiating for enable */
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    }
  }
  ಅನ್ಯಥಾ { /* NO */
    ವಿಕಲ್ಪ(tn->us[option]) {
    ಪಕ್ಷ CURL_NO:
      /* Already disabled */
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTNO:
      ವಿಕಲ್ಪ(tn->usq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        /* Already negotiating for NO */
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_WANTYES:
      ವಿಕಲ್ಪ(tn->usq[option]) {
      ಪಕ್ಷ CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_OPPOSITE:
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ telnet_negotiate(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn)
{
  ಪೂರ್ಣಾಙ್ಕ i;

  ಪ್ರತಿ(i = 0; i < CURL_NTELOPTS; i++) {
    ಯದಿ(i == CURL_TELOPT_ECHO)
      ಅನುವರ್ತ;

    ಯದಿ(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    ಯದಿ(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_will(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn, ಪೂರ್ಣಾಙ್ಕ option)
{
  ವಿಕಲ್ಪ(tn->him[option]) {
  ಪಕ್ಷ CURL_NO:
    ಯದಿ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    ಅನ್ಯಥಾ
      send_negotiation(data, CURL_DONT, option);

    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_YES:
    /* Already enabled */
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTNO:
    ವಿಕಲ್ಪ(tn->himq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTYES:
    ವಿಕಲ್ಪ(tn->himq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      tn->him[option] = CURL_YES;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_wont(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn, ಪೂರ್ಣಾಙ್ಕ option)
{
  ವಿಕಲ್ಪ(tn->him[option]) {
  ಪಕ್ಷ CURL_NO:
    /* Already disabled */
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTNO:
    ವಿಕಲ್ಪ(tn->himq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTYES:
    ವಿಕಲ್ಪ(tn->himq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ printsub(ಸಂರಚನಾ Curl_easy *data,
                     ಪೂರ್ಣಾಙ್ಕ direction,                /* '<' or '>' */
                     ಧ್ರುವ unsigned ವರ್ಣ *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  ಯದಿ(data->set.verbose) {
    unsigned ಪೂರ್ಣಾಙ್ಕ i = 0;
    ಯದಿ(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      ಯದಿ(length >= 3) {
        ಪೂರ್ಣಾಙ್ಕ j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        ಯದಿ(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          ಯದಿ(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          ಅನ್ಯಥಾ ಯದಿ(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          ಅನ್ಯಥಾ
            infof(data, "%u ", i);
          ಯದಿ(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          ಅನ್ಯಥಾ ಯದಿ(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          ಅನ್ಯಥಾ
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      ಯದಿ(length >= 2)
        length -= 2;
      ಅನ್ಯಥಾ /* bad input */
        ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ(length <= 1) {
      infof(data, "(Empty suboption?)");
      ಪ್ರತ್ಯಾಗಮ;
    }

    ಯದಿ(CURL_TELOPT_OK(pointer[0])) {
      ವಿಕಲ್ಪ(pointer[0]) {
      ಪಕ್ಷ CURL_TELOPT_TTYPE:
      ಪಕ್ಷ CURL_TELOPT_XDISPLOC:
      ಪಕ್ಷ CURL_TELOPT_NEW_ENVIRON:
      ಪಕ್ಷ CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        ಭಙ್ಗ;
      }
    }
    ಅನ್ಯಥಾ
      infof(data, "%d (unknown)", pointer[0]);

    ವಿಕಲ್ಪ(pointer[0]) {
    ಪಕ್ಷ CURL_TELOPT_NAWS:
      ಯದಿ(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:
      ವಿಕಲ್ಪ(pointer[1]) {
      ಪಕ್ಷ CURL_TELQUAL_IS:
        infof(data, " IS");
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_TELQUAL_SEND:
        infof(data, " SEND");
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_TELQUAL_NAME:
        infof(data, " NAME");
        ಭಙ್ಗ;
      }

      ವಿಕಲ್ಪ(pointer[0]) {
      ಪಕ್ಷ CURL_TELOPT_TTYPE:
      ಪಕ್ಷ CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (ಪೂರ್ಣಾಙ್ಕ)((length > 2) ? (length - 2) : 0), &pointer[2]);
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_TELOPT_NEW_ENVIRON:
        ಯದಿ(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          ಪ್ರತಿ(i = 3; i < length; i++) {
            ವಿಕಲ್ಪ(pointer[i]) {
            ಪಕ್ಷ CURL_NEW_ENV_VAR:
              infof(data, ", ");
              ಭಙ್ಗ;
            ಪಕ್ಷ CURL_NEW_ENV_ಮೌಲ್ಯ:
              infof(data, " = ");
              ಭಙ್ಗ;
            ಸ್ವಭಾವ:
              infof(data, "%c", pointer[i]);
              ಭಙ್ಗ;
            }
          }
        }
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        ಪ್ರತಿ(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        ಭಙ್ಗ;
      }
    }
  }
}

/* Escape and send a telnet data block */
ಸ್ಥಿರ CURLcode send_telnet_data(ಸಂರಚನಾ Curl_easy *data,
                                 ಸಂರಚನಾ TELNET *tn,
                                 ಧ್ರುವ ವರ್ಣ *buffer, ssize_t nread)
{
  size_t i, outlen;
  ಧ್ರುವ unsigned ವರ್ಣ *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  ಸಂರಚನಾ connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  ಯದಿ(nread < 0)
    ಪ್ರತ್ಯಾಗಮ CURLE_TOO_LARGE;

  ಯದಿ(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    ಪ್ರತಿ(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      ಯದಿ(!result && ((unsigned ವರ್ಣ)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  ಅನ್ಯಥಾ {
    outlen = (size_t)nread;
    outbuf = (ಧ್ರುವ unsigned ವರ್ಣ *)buffer;
  }
  ಯಾವತ್(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    ಸಂರಚನಾ pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    ಯದಿ(timeout_ms < 0)
      ಪ್ರತ್ಯಾಗಮ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    ವಿಕಲ್ಪ(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    ಪಕ್ಷ -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      ಭಙ್ಗ;
    ಪಕ್ಷ 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      ಭಙ್ಗ;
    ಸ್ವಭಾವ:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      ಭಙ್ಗ;
    }
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ಸ್ಥಿರ ಶೂನ್ಯ sendsuboption(ಸಂರಚನಾ Curl_easy *data,
                          ಸಂರಚನಾ TELNET *tn, ಪೂರ್ಣಾಙ್ಕ option)
{
  size_t nwritten;
  unsigned short x, y;
  ಧ್ರುವ unsigned ವರ್ಣ *uc1, *uc2;
  CURLcode result;

  ವಿಕಲ್ಪ(option) {
  ಪಕ್ಷ CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ಧ್ರುವ unsigned ವರ್ಣ *)&x;
    uc2 = (ಧ್ರುವ unsigned ವರ್ಣ *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (ಧ್ರುವ unsigned ವರ್ಣ *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    ಯದಿ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (ಧ್ರುವ ವರ್ಣ *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    ಯದಿ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_do(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn, ಪೂರ್ಣಾಙ್ಕ option)
{
  ವಿಕಲ್ಪ(tn->us[option]) {
  ಪಕ್ಷ CURL_NO:
    ಯದಿ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      ಯದಿ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    ಅನ್ಯಥಾ ಯದಿ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    ಅನ್ಯಥಾ
      send_negotiation(data, CURL_WONT, option);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_YES:
    /* Already enabled */
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTNO:
    ವಿಕಲ್ಪ(tn->usq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTYES:
    ವಿಕಲ್ಪ(tn->usq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      tn->us[option] = CURL_YES;
      ಯದಿ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_dont(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn, ಪೂರ್ಣಾಙ್ಕ option)
{
  ವಿಕಲ್ಪ(tn->us[option]) {
  ಪಕ್ಷ CURL_NO:
    /* Already disabled */
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTNO:
    ವಿಕಲ್ಪ(tn->usq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;

  ಪಕ್ಷ CURL_WANTYES:
    ವಿಕಲ್ಪ(tn->usq[option]) {
    ಪಕ್ಷ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      ಭಙ್ಗ;
    ಪಕ್ಷ CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      ಭಙ್ಗ;
    }
    ಭಙ್ಗ;
  }
}

ಸ್ಥಿರ bool str_is_nonascii(ಧ್ರುವ ವರ್ಣ *str)
{
  ವರ್ಣ c;
  ಯಾವತ್((c = *str++) != 0)
    ಯದಿ(c & 0x80)
      ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ CURLcode check_telnet_options(ಸಂರಚನಾ Curl_easy *data,
                                     ಸಂರಚನಾ TELNET *tn)
{
  ಸಂರಚನಾ curl_slist *head;
  ಸಂರಚನಾ curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  ಯದಿ(data->state.creds) {
    ವರ್ಣ buffer[256];
    ಯದಿ(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, ಪರಿಮಾಣ(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    ಯದಿ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ರಿಕ್ತ;
      ಪ್ರತ್ಯಾಗಮ CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  ಪ್ರತಿ(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    ಧ್ರುವ ವರ್ಣ *option = head->data;
    ಧ್ರುವ ವರ್ಣ *arg;
    ಧ್ರುವ ವರ್ಣ *sep = strchr(option, '=');
    ಯದಿ(sep) {
      olen = sep - option;
      arg = ++sep;
      ಯದಿ(str_is_nonascii(arg))
        ಅನುವರ್ತ;
      ವಿಕಲ್ಪ(olen) {
      ಪಕ್ಷ 5:
        /* Terminal type */
        ಯದಿ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          ಭಙ್ಗ;
        }
        result = CURLE_UNKNOWN_OPTION;
        ಭಙ್ಗ;

      ಪಕ್ಷ 8:
        /* Display variable */
        ಯದಿ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          ಭಙ್ಗ;
        }
        result = CURLE_UNKNOWN_OPTION;
        ಭಙ್ಗ;

      ಪಕ್ಷ 7:
        /* Environment variable */
        ಯದಿ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          ಯದಿ(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            ಭಙ್ಗ;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        ಅನ್ಯಥಾ
          result = CURLE_UNKNOWN_OPTION;
        ಭಙ್ಗ;

      ಪಕ್ಷ 2:
        /* Window Size */
        ಯದಿ(curl_strnequal(option, "WS", 2)) {
          ಧ್ರುವ ವರ್ಣ *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_ಸಂಖ್ಯೆ(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          ಅನ್ಯಥಾ {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        ಅನ್ಯಥಾ
          result = CURLE_UNKNOWN_OPTION;
        ಭಙ್ಗ;

      ಪಕ್ಷ 6:
        /* To take care or not of the 8th bit in data exchange */
        ಯದಿ(curl_strnequal(option, "BINARY", 6)) {
          ಧ್ರುವ ವರ್ಣ *p = arg;
          curl_off_t ದ್ವಿಮಾನ_option;
          ಯದಿ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ದ್ವಿಮಾನ_option, 1) &&
             (ದ್ವಿಮಾನ_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ದ್ವಿಮಾನ] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ದ್ವಿಮಾನ] = CURL_NO;
          }
        }
        ಅನ್ಯಥಾ
          result = CURLE_UNKNOWN_OPTION;
        ಭಙ್ಗ;
      ಸ್ವಭಾವ:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        ಭಙ್ಗ;
      }
    }
    ಅನ್ಯಥಾ {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  ಯದಿ(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ರಿಕ್ತ;
  }

  ಪ್ರತ್ಯಾಗಮ result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ಸ್ಥಿರ bool bad_option(ಧ್ರುವ ವರ್ಣ *data)
{
  ಪ್ರತ್ಯಾಗಮ !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ಸ್ಥಿರ CURLcode suboption(ಸಂರಚನಾ Curl_easy *data, ಸಂರಚನಾ TELNET *tn)
{
  ಸಂರಚನಾ curl_slist *v;
  unsigned ವರ್ಣ temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  ಯದಿ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ಪ್ರತ್ಯಾಗಮ CURLE_OK;

  printsub(data, '<', (ಧ್ರುವ unsigned ವರ್ಣ *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  ವಿಕಲ್ಪ(CURL_SB_GET(tn)) {
  ಪಕ್ಷ CURL_TELOPT_TTYPE:
    ಯದಿ(bad_option(tn->subopt_ttype))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ವರ್ಣ *)temp, ಪರಿಮಾಣ(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ಯದಿ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ಪ್ರತ್ಯಾಗಮ result;
    }
    printsub(data, '>', &temp[2], len-2);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_TELOPT_XDISPLOC:
    ಯದಿ(bad_option(tn->subopt_xdisploc))
      ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
    ಯದಿ(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      ಪ್ರತ್ಯಾಗಮ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ವರ್ಣ *)temp, ಪರಿಮಾಣ(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ಯದಿ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ಪ್ರತ್ಯಾಗಮ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    ಭಙ್ಗ;
  ಪಕ್ಷ CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((ವರ್ಣ *)temp, ಪರಿಮಾಣ(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    ಪ್ರತಿ(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      ಯದಿ(bad_option(v->data))
        ಪ್ರತ್ಯಾಗಮ CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      ಯದಿ(len + tmplen < ಪರಿಮಾಣ(temp) - 6) {
        ಧ್ರುವ ವರ್ಣ *s = strchr(v->data, ',');
        ಯದಿ(!s)
          len += curl_msnprintf((ವರ್ಣ *)&temp[len], ಪರಿಮಾಣ(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        ಅನ್ಯಥಾ {
          size_t vlen = s - v->data;
          len += curl_msnprintf((ವರ್ಣ *)&temp[len], ಪರಿಮಾಣ(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (ಪೂರ್ಣಾಙ್ಕ)vlen, v->data, CURL_NEW_ENV_ಮೌಲ್ಯ, ++s);
        }
      }
    }
    curl_msnprintf((ವರ್ಣ *)&temp[len], ಪರಿಮಾಣ(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    ಯದಿ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ಪ್ರತ್ಯಾಗಮ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode telrcv(ಸಂರಚನಾ Curl_easy *data,
                       ಸಂರಚನಾ TELNET *tn,
                       ಧ್ರುವ unsigned ವರ್ಣ *inbuf, /* Data received from
                                                      socket */
                       ssize_t ಎಣಿಕೆ)              /* Number of bytes
                                                      received */
{
  unsigned ವರ್ಣ c;
  CURLcode result;
  ಪೂರ್ಣಾಙ್ಕ in = 0;
  ಪೂರ್ಣಾಙ್ಕ startwrite = -1;

#define startskipping()                                          \
  ಯದಿ(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (ಧ್ರುವ ವರ್ಣ *)&inbuf[startwrite], \
                               in-startwrite);                   \
    ಯದಿ(result)                                                   \
      ಪ್ರತ್ಯಾಗಮ result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  ಯದಿ(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  ಯಾವತ್(ಎಣಿಕೆ--) {
    c = inbuf[in];

    ವಿಕಲ್ಪ(tn->telrcv_state) {
    ಪಕ್ಷ CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      ಯದಿ(c == '\0') {
        startskipping();
        ಭಙ್ಗ;   /* Ignore \0 after CR */
      }
      writebyte();
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_DATA:
      ಯದಿ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        ಭಙ್ಗ;
      }
      ಅನ್ಯಥಾ ಯದಿ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      ವಿಕಲ್ಪ(c) {
      ಪಕ್ಷ CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        ಭಙ್ಗ;
      ಪಕ್ಷ CURL_DM:
      ಪಕ್ಷ CURL_NOP:
      ಪಕ್ಷ CURL_GA:
      ಸ್ವಭಾವ:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        ಭಙ್ಗ;
      }
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_SB:
      ಯದಿ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      ಅನ್ಯಥಾ
        CURL_SB_ACCUM(tn, c);
      ಭಙ್ಗ;

    ಪಕ್ಷ CURL_TS_SE:
      ಯದಿ(c != CURL_SE) {
        ಯದಿ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      ಅನ್ಯಥಾ {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        ಯದಿ(result)
          ಪ್ರತ್ಯಾಗಮ result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      ಭಙ್ಗ;
    }
    ++in;
  }
  bufferflush();
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode telnet_done(ಸಂರಚನಾ Curl_easy *data,
                            CURLcode status, bool premature)
{
  (ಶೂನ್ಯ)status;
  (ಶೂನ್ಯ)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  ಪ್ರತ್ಯಾಗಮ CURLE_OK;
}

ಸ್ಥಿರ CURLcode telnet_do(ಸಂರಚನಾ Curl_easy *data, bool *done)
{
  CURLcode result;
  ಸಂರಚನಾ connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_ಎಣಿಕೆ;
  DWORD wait_timeout;
  DWORD readfile_read;
#ಅನ್ಯಥಾ
  timediff_t interval_ms;
  ಸಂರಚನಾ pollfd pfd[2];
  ಪೂರ್ಣಾಙ್ಕ poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  ವರ್ಣ buffer[4 * 1024];
  ಸಂರಚನಾ TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  ಯದಿ(!tn)
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  ಯದಿ(result)
    ಪ್ರತ್ಯಾಗಮ result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  ಯದಿ(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    ಪ್ರತ್ಯಾಗಮ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  ಯದಿ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ಪ್ರತ್ಯಾಗಮ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_ಒಳಹರಿವು_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  ಯದಿ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_ಎಣಿಕೆ = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  ಅನ್ಯಥಾ {
    obj_ಎಣಿಕೆ = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  ಯಾವತ್(keepon) {
    ಧ್ರುವ DWORD buf_size = (DWORD)ಪರಿಮಾಣ(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_ಎಣಿಕೆ, objs,
                                           FALSE, wait_timeout);
    ವಿಕಲ್ಪ(waitret) {

    ಪಕ್ಷ WAIT_TIMEOUT: {
      ಪ್ರತಿ(;;) {
        ಯದಿ(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          ಯದಿ(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            ಭಙ್ಗ;
          }

          ಯದಿ(n == CURL_READFUNC_PAUSE)
            ಭಙ್ಗ;

          ಯದಿ(n == 0)                        /* no bytes */
            ಭಙ್ಗ;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        ಅನ್ಯಥಾ {
          /* read from stdin */
          ಯದಿ(!PeekNamedPipe(stdin_handle, ರಿಕ್ತ, 0, ರಿಕ್ತ,
                            &readfile_read, ರಿಕ್ತ)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            ಭಙ್ಗ;
          }

          ಯದಿ(!readfile_read)
            ಭಙ್ಗ;

          ಯದಿ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ರಿಕ್ತ)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            ಭಙ್ಗ;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        ಯದಿ(result) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }
      }
    }
    ಭಙ್ಗ;

    ಪಕ್ಷ WAIT_OBJECT_0 + 1: {
      ಯದಿ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ರಿಕ್ತ)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        ಭಙ್ಗ;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      ಯದಿ(result) {
        keepon = FALSE;
        ಭಙ್ಗ;
      }
    }
    ಭಙ್ಗ;

    ಪಕ್ಷ WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      ಯದಿ(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        ಪೂರ್ಣಾಙ್ಕ sockerr = SOCKERRNO;
        ಯದಿ(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        ಭಙ್ಗ;
      }
      ಯದಿ(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, ಪರಿಮಾಣ(buffer), &nread);
        /* read would have blocked. Loop again */
        ಯದಿ(result == CURLE_AGAIN)
          ಭಙ್ಗ;
        /* returned not-zero, this an error */
        ಅನ್ಯಥಾ ಯದಿ(result) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ಅನ್ಯಥಾ ಯದಿ(!nread) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }

        result = telrcv(data, tn, (unsigned ವರ್ಣ *)buffer, nread);
        ಯದಿ(result) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ಯದಿ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      ಯದಿ(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      ಭಙ್ಗ;
    }
    } /* switch */

    ಯದಿ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  ಯದಿ(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#ಅನ್ಯಥಾ
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  ಯದಿ(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  ಅನ್ಯಥಾ {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    ಯದಿ(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  ಯಾವತ್(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    ವಿಕಲ್ಪ(Curl_poll(pfd, (unsigned ಪೂರ್ಣಾಙ್ಕ)poll_cnt, interval_ms)) {
    ಪಕ್ಷ -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      ಅನುವರ್ತ;
    ಪಕ್ಷ 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    ಸ್ವಭಾವ:                    /* read! */
      ಯದಿ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, ಪರಿಮಾಣ(buffer), &nread);
        /* read would have blocked. Loop again */
        ಯದಿ(result == CURLE_AGAIN)
          ಭಙ್ಗ;
        /* returned not-zero, this an error */
        ಯದಿ(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          ಯದಿ(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          ಭಙ್ಗ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ಅನ್ಯಥಾ ಯದಿ(!nread) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned ವರ್ಣ *)buffer, nread);
        ಯದಿ(result) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ಯದಿ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      ಯದಿ(poll_cnt == 2) {
        ಯದಿ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = ಪಠ(pfd[1].fd, buffer, ಪರಿಮಾಣ(buffer));
        }
      }
      ಅನ್ಯಥಾ {
        /* read from user-supplied method */
        snread = (ಪೂರ್ಣಾಙ್ಕ)data->state.fread_func(buffer, 1, ಪರಿಮಾಣ(buffer),
                                             data->state.in);
        ಯದಿ(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          ಭಙ್ಗ;
        }
        ಯದಿ(snread == CURL_READFUNC_PAUSE)
          ಭಙ್ಗ;
      }

      ಯದಿ(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        ಯದಿ(result) {
          keepon = FALSE;
          ಭಙ್ಗ;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      ಅನ್ಯಥಾ ಯದಿ(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      ಭಙ್ಗ;
    } /* poll switch statement */

    ಯದಿ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    ಯದಿ(!result) {
      result = Curl_pgrsUpdate(data);
      ಯದಿ(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * TELNET protocol handler.
 */
ಧ್ರುವ ಸಂರಚನಾ Curl_protocol Curl_protocol_telnet = {
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
