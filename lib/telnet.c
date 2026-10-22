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
#അന്തര്ഭാവ "curl_setup.h"
#അന്തര്ഭാവ "urldata.h"
#അന്തര്ഭാവ "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#അന്തര്ഭാവ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#അന്തര്ഭാവ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#അന്തര്ഭാവ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#അന്തര്ഭാവ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#അന്തര്ഭാവ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#അന്തര്ഭാവ <sys/param.h>
#endif

#അന്തര്ഭാവ "url.h"
#അന്തര്ഭാവ "transfer.h"
#അന്തര്ഭാവ "sendf.h"
#അന്തര്ഭാവ "curl_trc.h"
#അന്തര്ഭാവ "progress.h"
#അന്തര്ഭാവ "arpa_telnet.h"
#അന്തര്ഭാവ "connect.h"
#അന്തര്ഭാവ "select.h"
#അന്തര്ഭാവ "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  ചെയ്യുക {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } ആകുമ്പോൾ(0)
#define CURL_SB_ACCUM(x, c)                                         \
  ചെയ്യുക {                                                              \
    എങ്കിൽ((x)->subpointer < ((x)->subbuffer + വലുപ്പം((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } ആകുമ്പോൾ(0)

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
തരനിർവചനം എണ്ണൽ {
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

ഘടന TELNET {
  പൂർണ്ണസംഖ്യ please_negotiate;
  പൂർണ്ണസംഖ്യ already_negotiated;
  പൂർണ്ണസംഖ്യ us[256];
  പൂർണ്ണസംഖ്യ usq[256];
  പൂർണ്ണസംഖ്യ us_preferred[256];
  പൂർണ്ണസംഖ്യ him[256];
  പൂർണ്ണസംഖ്യ himq[256];
  പൂർണ്ണസംഖ്യ him_preferred[256];
  പൂർണ്ണസംഖ്യ subnegotiation[256];
  സ്ഥിരം അക്ഷരം *subopt_ttype;          /* Set with suboption TTYPE */
  സ്ഥിരം അക്ഷരം *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  ഘടന curl_slist *telnet_vars;    /* Environment variables */
  ഘടന dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned അക്ഷരം subbuffer[SUBBUFSIZE];
  unsigned അക്ഷരം *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#അല്ലെങ്കിൽ
സ്ഥിതം ശൂന്യം printoption(ഘടന Curl_easy *data,
                        സ്ഥിരം അക്ഷരം *direction, പൂർണ്ണസംഖ്യ cmd, പൂർണ്ണസംഖ്യ option)
{
  എങ്കിൽ(data->set.verbose) {
    എങ്കിൽ(cmd == CURL_IAC) {
      എങ്കിൽ(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      അല്ലെങ്കിൽ
        infof(data, "%s IAC %d", direction, option);
    }
    അല്ലെങ്കിൽ {
      സ്ഥിരം അക്ഷരം *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      എങ്കിൽ(fmt) {
        സ്ഥിരം അക്ഷരം *opt;
        എങ്കിൽ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        അല്ലെങ്കിൽ എങ്കിൽ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        അല്ലെങ്കിൽ
          opt = ശൂന്യ;

        എങ്കിൽ(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        അല്ലെങ്കിൽ
          infof(data, "%s %s %d", direction, fmt, option);
      }
      അല്ലെങ്കിൽ
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

സ്ഥിതം ശൂന്യം telnet_easy_dtor(സ്ഥിരം ശൂന്യം *താക്കോൽ, size_t klen, ശൂന്യം *entry)
{
  ഘടന TELNET *tn = entry;
  (ശൂന്യം)താക്കോൽ;
  (ശൂന്യം)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

സ്ഥിതം CURLcode init_telnet(ഘടന Curl_easy *data)
{
  ഘടന TELNET *tn;

  tn = curlx_calloc(1, വലുപ്പം(ഘടന TELNET));
  എങ്കിൽ(!tn)
    തിരികെ CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_ദ്വിമാന] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ദ്വിമാന] = CURL_YES;

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

  തിരികെ Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

സ്ഥിതം ശൂന്യം send_negotiation(ഘടന Curl_easy *data, പൂർണ്ണസംഖ്യ cmd, പൂർണ്ണസംഖ്യ option)
{
  unsigned അക്ഷരം buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned അക്ഷരം)cmd;
  buf[2] = (unsigned അക്ഷരം)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  എങ്കിൽ(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

സ്ഥിതം ശൂന്യം set_remote_option(ഘടന Curl_easy *data, ഘടന TELNET *tn,
                              പൂർണ്ണസംഖ്യ option, പൂർണ്ണസംഖ്യ newstate)
{
  എങ്കിൽ(newstate == CURL_YES) {
    തിരഞ്ഞെടുപ്പ്(tn->him[option]) {
    അവസ്ഥ CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      നിർത്തുക;

    അവസ്ഥ CURL_YES:
      /* Already enabled */
      നിർത്തുക;

    അവസ്ഥ CURL_WANTNO:
      തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
      അവസ്ഥ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        നിർത്തുക;
      }
      നിർത്തുക;

    അവസ്ഥ CURL_WANTYES:
      തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
      അവസ്ഥ CURL_EMPTY:
        /* Error: already negotiating for enable */
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        നിർത്തുക;
      }
      നിർത്തുക;
    }
  }
  അല്ലെങ്കിൽ { /* NO */
    തിരഞ്ഞെടുപ്പ്(tn->him[option]) {
    അവസ്ഥ CURL_NO:
      /* Already disabled */
      നിർത്തുക;

    അവസ്ഥ CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      നിർത്തുക;

    അവസ്ഥ CURL_WANTNO:
      തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
      അവസ്ഥ CURL_EMPTY:
        /* Already negotiating for NO */
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        നിർത്തുക;
      }
      നിർത്തുക;

    അവസ്ഥ CURL_WANTYES:
      തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
      അവസ്ഥ CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        നിർത്തുക;
      }
      നിർത്തുക;
    }
  }
}

സ്ഥിതം ശൂന്യം set_local_option(ഘടന Curl_easy *data, ഘടന TELNET *tn,
                             പൂർണ്ണസംഖ്യ option, പൂർണ്ണസംഖ്യ newstate)
{
  എങ്കിൽ(newstate == CURL_YES) {
    തിരഞ്ഞെടുപ്പ്(tn->us[option]) {
    അവസ്ഥ CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      നിർത്തുക;

    അവസ്ഥ CURL_YES:
      /* Already enabled */
      നിർത്തുക;

    അവസ്ഥ CURL_WANTNO:
      തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
      അവസ്ഥ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        നിർത്തുക;
      }
      നിർത്തുക;

    അവസ്ഥ CURL_WANTYES:
      തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
      അവസ്ഥ CURL_EMPTY:
        /* Error: already negotiating for enable */
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        നിർത്തുക;
      }
      നിർത്തുക;
    }
  }
  അല്ലെങ്കിൽ { /* NO */
    തിരഞ്ഞെടുപ്പ്(tn->us[option]) {
    അവസ്ഥ CURL_NO:
      /* Already disabled */
      നിർത്തുക;

    അവസ്ഥ CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      നിർത്തുക;

    അവസ്ഥ CURL_WANTNO:
      തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
      അവസ്ഥ CURL_EMPTY:
        /* Already negotiating for NO */
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        നിർത്തുക;
      }
      നിർത്തുക;

    അവസ്ഥ CURL_WANTYES:
      തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
      അവസ്ഥ CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        നിർത്തുക;
      അവസ്ഥ CURL_OPPOSITE:
        നിർത്തുക;
      }
      നിർത്തുക;
    }
  }
}

സ്ഥിതം ശൂന്യം telnet_negotiate(ഘടന Curl_easy *data, ഘടന TELNET *tn)
{
  പൂർണ്ണസംഖ്യ i;

  വേണ്ടി(i = 0; i < CURL_NTELOPTS; i++) {
    എങ്കിൽ(i == CURL_TELOPT_ECHO)
      തുടരുക;

    എങ്കിൽ(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    എങ്കിൽ(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

സ്ഥിതം ശൂന്യം rec_will(ഘടന Curl_easy *data, ഘടന TELNET *tn, പൂർണ്ണസംഖ്യ option)
{
  തിരഞ്ഞെടുപ്പ്(tn->him[option]) {
  അവസ്ഥ CURL_NO:
    എങ്കിൽ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    അല്ലെങ്കിൽ
      send_negotiation(data, CURL_DONT, option);

    നിർത്തുക;

  അവസ്ഥ CURL_YES:
    /* Already enabled */
    നിർത്തുക;

  അവസ്ഥ CURL_WANTNO:
    തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
    അവസ്ഥ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      നിർത്തുക;
    അവസ്ഥ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      നിർത്തുക;
    }
    നിർത്തുക;

  അവസ്ഥ CURL_WANTYES:
    തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
    അവസ്ഥ CURL_EMPTY:
      tn->him[option] = CURL_YES;
      നിർത്തുക;
    അവസ്ഥ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      നിർത്തുക;
    }
    നിർത്തുക;
  }
}

സ്ഥിതം ശൂന്യം rec_wont(ഘടന Curl_easy *data, ഘടന TELNET *tn, പൂർണ്ണസംഖ്യ option)
{
  തിരഞ്ഞെടുപ്പ്(tn->him[option]) {
  അവസ്ഥ CURL_NO:
    /* Already disabled */
    നിർത്തുക;

  അവസ്ഥ CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    നിർത്തുക;

  അവസ്ഥ CURL_WANTNO:
    തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
    അവസ്ഥ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      നിർത്തുക;

    അവസ്ഥ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      നിർത്തുക;
    }
    നിർത്തുക;

  അവസ്ഥ CURL_WANTYES:
    തിരഞ്ഞെടുപ്പ്(tn->himq[option]) {
    അവസ്ഥ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      നിർത്തുക;
    അവസ്ഥ CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      നിർത്തുക;
    }
    നിർത്തുക;
  }
}

സ്ഥിതം ശൂന്യം printsub(ഘടന Curl_easy *data,
                     പൂർണ്ണസംഖ്യ direction,                /* '<' or '>' */
                     സ്ഥിരം unsigned അക്ഷരം *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  എങ്കിൽ(data->set.verbose) {
    unsigned പൂർണ്ണസംഖ്യ i = 0;
    എങ്കിൽ(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      എങ്കിൽ(length >= 3) {
        പൂർണ്ണസംഖ്യ j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        എങ്കിൽ(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          എങ്കിൽ(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          അല്ലെങ്കിൽ എങ്കിൽ(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          അല്ലെങ്കിൽ
            infof(data, "%u ", i);
          എങ്കിൽ(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          അല്ലെങ്കിൽ എങ്കിൽ(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          അല്ലെങ്കിൽ
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      എങ്കിൽ(length >= 2)
        length -= 2;
      അല്ലെങ്കിൽ /* bad input */
        തിരികെ;
    }
    എങ്കിൽ(length <= 1) {
      infof(data, "(Empty suboption?)");
      തിരികെ;
    }

    എങ്കിൽ(CURL_TELOPT_OK(pointer[0])) {
      തിരഞ്ഞെടുപ്പ്(pointer[0]) {
      അവസ്ഥ CURL_TELOPT_TTYPE:
      അവസ്ഥ CURL_TELOPT_XDISPLOC:
      അവസ്ഥ CURL_TELOPT_NEW_ENVIRON:
      അവസ്ഥ CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        നിർത്തുക;
      }
    }
    അല്ലെങ്കിൽ
      infof(data, "%d (unknown)", pointer[0]);

    തിരഞ്ഞെടുപ്പ്(pointer[0]) {
    അവസ്ഥ CURL_TELOPT_NAWS:
      എങ്കിൽ(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:
      തിരഞ്ഞെടുപ്പ്(pointer[1]) {
      അവസ്ഥ CURL_TELQUAL_IS:
        infof(data, " IS");
        നിർത്തുക;
      അവസ്ഥ CURL_TELQUAL_SEND:
        infof(data, " SEND");
        നിർത്തുക;
      അവസ്ഥ CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        നിർത്തുക;
      അവസ്ഥ CURL_TELQUAL_NAME:
        infof(data, " NAME");
        നിർത്തുക;
      }

      തിരഞ്ഞെടുപ്പ്(pointer[0]) {
      അവസ്ഥ CURL_TELOPT_TTYPE:
      അവസ്ഥ CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (പൂർണ്ണസംഖ്യ)((length > 2) ? (length - 2) : 0), &pointer[2]);
        നിർത്തുക;
      അവസ്ഥ CURL_TELOPT_NEW_ENVIRON:
        എങ്കിൽ(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          വേണ്ടി(i = 3; i < length; i++) {
            തിരഞ്ഞെടുപ്പ്(pointer[i]) {
            അവസ്ഥ CURL_NEW_ENV_VAR:
              infof(data, ", ");
              നിർത്തുക;
            അവസ്ഥ CURL_NEW_ENV_മൂല്യം:
              infof(data, " = ");
              നിർത്തുക;
            സ്ഥിരസ്ഥിതി:
              infof(data, "%c", pointer[i]);
              നിർത്തുക;
            }
          }
        }
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        വേണ്ടി(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        നിർത്തുക;
      }
    }
  }
}

/* Escape and send a telnet data block */
സ്ഥിതം CURLcode send_telnet_data(ഘടന Curl_easy *data,
                                 ഘടന TELNET *tn,
                                 സ്ഥിരം അക്ഷരം *buffer, ssize_t nread)
{
  size_t i, outlen;
  സ്ഥിരം unsigned അക്ഷരം *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  ഘടന connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  എങ്കിൽ(nread < 0)
    തിരികെ CURLE_TOO_LARGE;

  എങ്കിൽ(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    വേണ്ടി(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      എങ്കിൽ(!result && ((unsigned അക്ഷരം)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  അല്ലെങ്കിൽ {
    outlen = (size_t)nread;
    outbuf = (സ്ഥിരം unsigned അക്ഷരം *)buffer;
  }
  ആകുമ്പോൾ(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    ഘടന pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    എങ്കിൽ(timeout_ms < 0)
      തിരികെ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    തിരഞ്ഞെടുപ്പ്(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    അവസ്ഥ -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      നിർത്തുക;
    അവസ്ഥ 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      നിർത്തുക;
    സ്ഥിരസ്ഥിതി:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      നിർത്തുക;
    }
  }

  തിരികെ result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
സ്ഥിതം ശൂന്യം sendsuboption(ഘടന Curl_easy *data,
                          ഘടന TELNET *tn, പൂർണ്ണസംഖ്യ option)
{
  size_t nwritten;
  unsigned short x, y;
  സ്ഥിരം unsigned അക്ഷരം *uc1, *uc2;
  CURLcode result;

  തിരഞ്ഞെടുപ്പ്(option) {
  അവസ്ഥ CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (സ്ഥിരം unsigned അക്ഷരം *)&x;
    uc2 = (സ്ഥിരം unsigned അക്ഷരം *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (സ്ഥിരം unsigned അക്ഷരം *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    എങ്കിൽ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (സ്ഥിരം അക്ഷരം *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    എങ്കിൽ(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    നിർത്തുക;
  }
}

സ്ഥിതം ശൂന്യം rec_do(ഘടന Curl_easy *data, ഘടന TELNET *tn, പൂർണ്ണസംഖ്യ option)
{
  തിരഞ്ഞെടുപ്പ്(tn->us[option]) {
  അവസ്ഥ CURL_NO:
    എങ്കിൽ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      എങ്കിൽ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    അല്ലെങ്കിൽ എങ്കിൽ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    അല്ലെങ്കിൽ
      send_negotiation(data, CURL_WONT, option);
    നിർത്തുക;

  അവസ്ഥ CURL_YES:
    /* Already enabled */
    നിർത്തുക;

  അവസ്ഥ CURL_WANTNO:
    തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
    അവസ്ഥ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      നിർത്തുക;
    അവസ്ഥ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      നിർത്തുക;
    }
    നിർത്തുക;

  അവസ്ഥ CURL_WANTYES:
    തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
    അവസ്ഥ CURL_EMPTY:
      tn->us[option] = CURL_YES;
      എങ്കിൽ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      നിർത്തുക;
    അവസ്ഥ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      നിർത്തുക;
    }
    നിർത്തുക;
  }
}

സ്ഥിതം ശൂന്യം rec_dont(ഘടന Curl_easy *data, ഘടന TELNET *tn, പൂർണ്ണസംഖ്യ option)
{
  തിരഞ്ഞെടുപ്പ്(tn->us[option]) {
  അവസ്ഥ CURL_NO:
    /* Already disabled */
    നിർത്തുക;

  അവസ്ഥ CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    നിർത്തുക;

  അവസ്ഥ CURL_WANTNO:
    തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
    അവസ്ഥ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      നിർത്തുക;

    അവസ്ഥ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      നിർത്തുക;
    }
    നിർത്തുക;

  അവസ്ഥ CURL_WANTYES:
    തിരഞ്ഞെടുപ്പ്(tn->usq[option]) {
    അവസ്ഥ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      നിർത്തുക;
    അവസ്ഥ CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      നിർത്തുക;
    }
    നിർത്തുക;
  }
}

സ്ഥിതം bool str_is_nonascii(സ്ഥിരം അക്ഷരം *str)
{
  അക്ഷരം c;
  ആകുമ്പോൾ((c = *str++) != 0)
    എങ്കിൽ(c & 0x80)
      തിരികെ TRUE;

  തിരികെ FALSE;
}

സ്ഥിതം CURLcode check_telnet_options(ഘടന Curl_easy *data,
                                     ഘടന TELNET *tn)
{
  ഘടന curl_slist *head;
  ഘടന curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  എങ്കിൽ(data->state.creds) {
    അക്ഷരം buffer[256];
    എങ്കിൽ(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, വലുപ്പം(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    എങ്കിൽ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ശൂന്യ;
      തിരികെ CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  വേണ്ടി(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    സ്ഥിരം അക്ഷരം *option = head->data;
    സ്ഥിരം അക്ഷരം *arg;
    സ്ഥിരം അക്ഷരം *sep = strchr(option, '=');
    എങ്കിൽ(sep) {
      olen = sep - option;
      arg = ++sep;
      എങ്കിൽ(str_is_nonascii(arg))
        തുടരുക;
      തിരഞ്ഞെടുപ്പ്(olen) {
      അവസ്ഥ 5:
        /* Terminal type */
        എങ്കിൽ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          നിർത്തുക;
        }
        result = CURLE_UNKNOWN_OPTION;
        നിർത്തുക;

      അവസ്ഥ 8:
        /* Display variable */
        എങ്കിൽ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          നിർത്തുക;
        }
        result = CURLE_UNKNOWN_OPTION;
        നിർത്തുക;

      അവസ്ഥ 7:
        /* Environment variable */
        എങ്കിൽ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          എങ്കിൽ(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            നിർത്തുക;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        അല്ലെങ്കിൽ
          result = CURLE_UNKNOWN_OPTION;
        നിർത്തുക;

      അവസ്ഥ 2:
        /* Window Size */
        എങ്കിൽ(curl_strnequal(option, "WS", 2)) {
          സ്ഥിരം അക്ഷരം *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          എങ്കിൽ(curlx_str_സംഖ്യ(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_സംഖ്യ(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          അല്ലെങ്കിൽ {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        അല്ലെങ്കിൽ
          result = CURLE_UNKNOWN_OPTION;
        നിർത്തുക;

      അവസ്ഥ 6:
        /* To take care or not of the 8th bit in data exchange */
        എങ്കിൽ(curl_strnequal(option, "BINARY", 6)) {
          സ്ഥിരം അക്ഷരം *p = arg;
          curl_off_t ദ്വിമാന_option;
          എങ്കിൽ(!curlx_str_സംഖ്യ(&p, &ദ്വിമാന_option, 1) &&
             (ദ്വിമാന_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ദ്വിമാന] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ദ്വിമാന] = CURL_NO;
          }
        }
        അല്ലെങ്കിൽ
          result = CURLE_UNKNOWN_OPTION;
        നിർത്തുക;
      സ്ഥിരസ്ഥിതി:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        നിർത്തുക;
      }
    }
    അല്ലെങ്കിൽ {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  എങ്കിൽ(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ശൂന്യ;
  }

  തിരികെ result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
സ്ഥിതം bool bad_option(സ്ഥിരം അക്ഷരം *data)
{
  തിരികെ !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
സ്ഥിതം CURLcode suboption(ഘടന Curl_easy *data, ഘടന TELNET *tn)
{
  ഘടന curl_slist *v;
  unsigned അക്ഷരം temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  എങ്കിൽ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    തിരികെ CURLE_OK;

  printsub(data, '<', (സ്ഥിരം unsigned അക്ഷരം *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  തിരഞ്ഞെടുപ്പ്(CURL_SB_GET(tn)) {
  അവസ്ഥ CURL_TELOPT_TTYPE:
    എങ്കിൽ(bad_option(tn->subopt_ttype))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      തിരികെ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((അക്ഷരം *)temp, വലുപ്പം(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    എങ്കിൽ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      തിരികെ result;
    }
    printsub(data, '>', &temp[2], len-2);
    നിർത്തുക;
  അവസ്ഥ CURL_TELOPT_XDISPLOC:
    എങ്കിൽ(bad_option(tn->subopt_xdisploc))
      തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
    എങ്കിൽ(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      തിരികെ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((അക്ഷരം *)temp, വലുപ്പം(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    എങ്കിൽ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      തിരികെ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    നിർത്തുക;
  അവസ്ഥ CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((അക്ഷരം *)temp, വലുപ്പം(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    വേണ്ടി(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      എങ്കിൽ(bad_option(v->data))
        തിരികെ CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      എങ്കിൽ(len + tmplen < വലുപ്പം(temp) - 6) {
        സ്ഥിരം അക്ഷരം *s = strchr(v->data, ',');
        എങ്കിൽ(!s)
          len += curl_msnprintf((അക്ഷരം *)&temp[len], വലുപ്പം(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        അല്ലെങ്കിൽ {
          size_t vlen = s - v->data;
          len += curl_msnprintf((അക്ഷരം *)&temp[len], വലുപ്പം(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (പൂർണ്ണസംഖ്യ)vlen, v->data, CURL_NEW_ENV_മൂല്യം, ++s);
        }
      }
    }
    curl_msnprintf((അക്ഷരം *)&temp[len], വലുപ്പം(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    എങ്കിൽ(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      തിരികെ result;
    }
    printsub(data, '>', &temp[2], len - 2);
    നിർത്തുക;
  }
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode telrcv(ഘടന Curl_easy *data,
                       ഘടന TELNET *tn,
                       സ്ഥിരം unsigned അക്ഷരം *inbuf, /* Data received from
                                                      socket */
                       ssize_t എണ്ണം)              /* Number of bytes
                                                      received */
{
  unsigned അക്ഷരം c;
  CURLcode result;
  പൂർണ്ണസംഖ്യ in = 0;
  പൂർണ്ണസംഖ്യ startwrite = -1;

#define startskipping()                                          \
  എങ്കിൽ(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (സ്ഥിരം അക്ഷരം *)&inbuf[startwrite], \
                               in-startwrite);                   \
    എങ്കിൽ(result)                                                   \
      തിരികെ result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  എങ്കിൽ(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  ആകുമ്പോൾ(എണ്ണം--) {
    c = inbuf[in];

    തിരഞ്ഞെടുപ്പ്(tn->telrcv_state) {
    അവസ്ഥ CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      എങ്കിൽ(c == '\0') {
        startskipping();
        നിർത്തുക;   /* Ignore \0 after CR */
      }
      writebyte();
      നിർത്തുക;

    അവസ്ഥ CURL_TS_DATA:
      എങ്കിൽ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        നിർത്തുക;
      }
      അല്ലെങ്കിൽ എങ്കിൽ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      നിർത്തുക;

    അവസ്ഥ CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      തിരഞ്ഞെടുപ്പ്(c) {
      അവസ്ഥ CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        നിർത്തുക;
      അവസ്ഥ CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        നിർത്തുക;
      അവസ്ഥ CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        നിർത്തുക;
      അവസ്ഥ CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        നിർത്തുക;
      അവസ്ഥ CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        നിർത്തുക;
      അവസ്ഥ CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        നിർത്തുക;
      അവസ്ഥ CURL_DM:
      അവസ്ഥ CURL_NOP:
      അവസ്ഥ CURL_GA:
      സ്ഥിരസ്ഥിതി:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        നിർത്തുക;
      }
      നിർത്തുക;

    അവസ്ഥ CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      നിർത്തുക;

    അവസ്ഥ CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      നിർത്തുക;

    അവസ്ഥ CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      നിർത്തുക;

    അവസ്ഥ CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      നിർത്തുക;

    അവസ്ഥ CURL_TS_SB:
      എങ്കിൽ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      അല്ലെങ്കിൽ
        CURL_SB_ACCUM(tn, c);
      നിർത്തുക;

    അവസ്ഥ CURL_TS_SE:
      എങ്കിൽ(c != CURL_SE) {
        എങ്കിൽ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          തിരികെ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      അല്ലെങ്കിൽ {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        എങ്കിൽ(result)
          തിരികെ result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      നിർത്തുക;
    }
    ++in;
  }
  bufferflush();
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode telnet_done(ഘടന Curl_easy *data,
                            CURLcode status, bool premature)
{
  (ശൂന്യം)status;
  (ശൂന്യം)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  തിരികെ CURLE_OK;
}

സ്ഥിതം CURLcode telnet_do(ഘടന Curl_easy *data, bool *done)
{
  CURLcode result;
  ഘടന connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_എണ്ണം;
  DWORD wait_timeout;
  DWORD readfile_read;
#അല്ലെങ്കിൽ
  timediff_t interval_ms;
  ഘടന pollfd pfd[2];
  പൂർണ്ണസംഖ്യ poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  അക്ഷരം buffer[4 * 1024];
  ഘടന TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  എങ്കിൽ(result)
    തിരികെ result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  എങ്കിൽ(!tn)
    തിരികെ CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  എങ്കിൽ(result)
    തിരികെ result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  എങ്കിൽ(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    തിരികെ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  എങ്കിൽ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    തിരികെ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_നിവേശം_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  എങ്കിൽ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_എണ്ണം = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  അല്ലെങ്കിൽ {
    obj_എണ്ണം = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  ആകുമ്പോൾ(keepon) {
    സ്ഥിരം DWORD buf_size = (DWORD)വലുപ്പം(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_എണ്ണം, objs,
                                           FALSE, wait_timeout);
    തിരഞ്ഞെടുപ്പ്(waitret) {

    അവസ്ഥ WAIT_TIMEOUT: {
      വേണ്ടി(;;) {
        എങ്കിൽ(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          എങ്കിൽ(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            നിർത്തുക;
          }

          എങ്കിൽ(n == CURL_READFUNC_PAUSE)
            നിർത്തുക;

          എങ്കിൽ(n == 0)                        /* no bytes */
            നിർത്തുക;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        അല്ലെങ്കിൽ {
          /* read from stdin */
          എങ്കിൽ(!PeekNamedPipe(stdin_handle, ശൂന്യ, 0, ശൂന്യ,
                            &readfile_read, ശൂന്യ)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            നിർത്തുക;
          }

          എങ്കിൽ(!readfile_read)
            നിർത്തുക;

          എങ്കിൽ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ശൂന്യ)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            നിർത്തുക;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        എങ്കിൽ(result) {
          keepon = FALSE;
          നിർത്തുക;
        }
      }
    }
    നിർത്തുക;

    അവസ്ഥ WAIT_OBJECT_0 + 1: {
      എങ്കിൽ(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, ശൂന്യ)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        നിർത്തുക;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      എങ്കിൽ(result) {
        keepon = FALSE;
        നിർത്തുക;
      }
    }
    നിർത്തുക;

    അവസ്ഥ WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      എങ്കിൽ(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        പൂർണ്ണസംഖ്യ sockerr = SOCKERRNO;
        എങ്കിൽ(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        നിർത്തുക;
      }
      എങ്കിൽ(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, വലുപ്പം(buffer), &nread);
        /* read would have blocked. Loop again */
        എങ്കിൽ(result == CURLE_AGAIN)
          നിർത്തുക;
        /* returned not-zero, this an error */
        അല്ലെങ്കിൽ എങ്കിൽ(result) {
          keepon = FALSE;
          നിർത്തുക;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        അല്ലെങ്കിൽ എങ്കിൽ(!nread) {
          keepon = FALSE;
          നിർത്തുക;
        }

        result = telrcv(data, tn, (unsigned അക്ഷരം *)buffer, nread);
        എങ്കിൽ(result) {
          keepon = FALSE;
          നിർത്തുക;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        എങ്കിൽ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      എങ്കിൽ(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      നിർത്തുക;
    }
    } /* switch */

    എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  എങ്കിൽ(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#അല്ലെങ്കിൽ
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  എങ്കിൽ(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  അല്ലെങ്കിൽ {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    എങ്കിൽ(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  ആകുമ്പോൾ(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    തിരഞ്ഞെടുപ്പ്(Curl_poll(pfd, (unsigned പൂർണ്ണസംഖ്യ)poll_cnt, interval_ms)) {
    അവസ്ഥ -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      തുടരുക;
    അവസ്ഥ 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    സ്ഥിരസ്ഥിതി:                    /* read! */
      എങ്കിൽ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, വലുപ്പം(buffer), &nread);
        /* read would have blocked. Loop again */
        എങ്കിൽ(result == CURLE_AGAIN)
          നിർത്തുക;
        /* returned not-zero, this an error */
        എങ്കിൽ(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          എങ്കിൽ(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          നിർത്തുക;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        അല്ലെങ്കിൽ എങ്കിൽ(!nread) {
          keepon = FALSE;
          നിർത്തുക;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned അക്ഷരം *)buffer, nread);
        എങ്കിൽ(result) {
          keepon = FALSE;
          നിർത്തുക;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        എങ്കിൽ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      എങ്കിൽ(poll_cnt == 2) {
        എങ്കിൽ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = പഠ(pfd[1].fd, buffer, വലുപ്പം(buffer));
        }
      }
      അല്ലെങ്കിൽ {
        /* read from user-supplied method */
        snread = (പൂർണ്ണസംഖ്യ)data->state.fread_func(buffer, 1, വലുപ്പം(buffer),
                                             data->state.in);
        എങ്കിൽ(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          നിർത്തുക;
        }
        എങ്കിൽ(snread == CURL_READFUNC_PAUSE)
          നിർത്തുക;
      }

      എങ്കിൽ(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        എങ്കിൽ(result) {
          keepon = FALSE;
          നിർത്തുക;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      അല്ലെങ്കിൽ എങ്കിൽ(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      നിർത്തുക;
    } /* poll switch statement */

    എങ്കിൽ(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    എങ്കിൽ(!result) {
      result = Curl_pgrsUpdate(data);
      എങ്കിൽ(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  തിരികെ result;
}

/*
 * TELNET protocol handler.
 */
സ്ഥിരം ഘടന Curl_protocol Curl_protocol_telnet = {
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
