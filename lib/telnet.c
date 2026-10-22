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
#অন্তর্ভাৱ "curl_setup.h"
#অন্তর্ভাৱ "urldata.h"
#অন্তর্ভাৱ "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাৱ <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাৱ <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাৱ <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাৱ <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাৱ <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাৱ <sys/param.h>
#endif

#অন্তর্ভাৱ "url.h"
#অন্তর্ভাৱ "transfer.h"
#অন্তর্ভাৱ "sendf.h"
#অন্তর্ভাৱ "curl_trc.h"
#অন্তর্ভাৱ "progress.h"
#অন্তর্ভাৱ "arpa_telnet.h"
#অন্তর্ভাৱ "connect.h"
#অন্তর্ভাৱ "select.h"
#অন্তর্ভাৱ "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  কৰা {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } যেতিয়ালৈ(0)
#define CURL_SB_ACCUM(x, c)                                         \
  কৰা {                                                              \
    যদি((x)->subpointer < ((x)->subbuffer + পরিমাণ((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } যেতিয়ালৈ(0)

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
প্রকারনির্দেশ গণনা {
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

সংরচনা TELNET {
  পূর্ণাঙ্ক please_negotiate;
  পূর্ণাঙ্ক already_negotiated;
  পূর্ণাঙ্ক us[256];
  পূর্ণাঙ্ক usq[256];
  পূর্ণাঙ্ক us_preferred[256];
  পূর্ণাঙ্ক him[256];
  পূর্ণাঙ্ক himq[256];
  পূর্ণাঙ্ক him_preferred[256];
  পূর্ণাঙ্ক subnegotiation[256];
  ধ্রুৱ ৱর্ণ *subopt_ttype;          /* Set with suboption TTYPE */
  ধ্রুৱ ৱর্ণ *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  সংরচনা curl_slist *telnet_vars;    /* Environment variables */
  সংরচনা dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned ৱর্ণ subbuffer[SUBBUFSIZE];
  unsigned ৱর্ণ *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#অন্যথা
স্থির শূন্য printoption(সংরচনা Curl_easy *data,
                        ধ্রুৱ ৱর্ণ *direction, পূর্ণাঙ্ক cmd, পূর্ণাঙ্ক option)
{
  যদি(data->set.verbose) {
    যদি(cmd == CURL_IAC) {
      যদি(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      অন্যথা
        infof(data, "%s IAC %d", direction, option);
    }
    অন্যথা {
      ধ্রুৱ ৱর্ণ *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      যদি(fmt) {
        ধ্রুৱ ৱর্ণ *opt;
        যদি(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        অন্যথা যদি(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        অন্যথা
          opt = রিক্ত;

        যদি(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        অন্যথা
          infof(data, "%s %s %d", direction, fmt, option);
      }
      অন্যথা
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

স্থির শূন্য telnet_easy_dtor(ধ্রুৱ শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  সংরচনা TELNET *tn = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

স্থির CURLcode init_telnet(সংরচনা Curl_easy *data)
{
  সংরচনা TELNET *tn;

  tn = curlx_calloc(1, পরিমাণ(সংরচনা TELNET));
  যদি(!tn)
    ঘূৰাই CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_দ্বিমিক] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_দ্বিমিক] = CURL_YES;

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

  ঘূৰাই Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

স্থির শূন্য send_negotiation(সংরচনা Curl_easy *data, পূর্ণাঙ্ক cmd, পূর্ণাঙ্ক option)
{
  unsigned ৱর্ণ buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned ৱর্ণ)cmd;
  buf[2] = (unsigned ৱর্ণ)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  যদি(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

স্থির শূন্য set_remote_option(সংরচনা Curl_easy *data, সংরচনা TELNET *tn,
                              পূর্ণাঙ্ক option, পূর্ণাঙ্ক newstate)
{
  যদি(newstate == CURL_YES) {
    ৱিকল্প(tn->him[option]) {
    পক্ষ CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      ভঙ্গ;

    পক্ষ CURL_YES:
      /* Already enabled */
      ভঙ্গ;

    পক্ষ CURL_WANTNO:
      ৱিকল্প(tn->himq[option]) {
      পক্ষ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ভঙ্গ;
      }
      ভঙ্গ;

    পক্ষ CURL_WANTYES:
      ৱিকল্প(tn->himq[option]) {
      পক্ষ CURL_EMPTY:
        /* Error: already negotiating for enable */
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        ভঙ্গ;
      }
      ভঙ্গ;
    }
  }
  অন্যথা { /* NO */
    ৱিকল্প(tn->him[option]) {
    পক্ষ CURL_NO:
      /* Already disabled */
      ভঙ্গ;

    পক্ষ CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      ভঙ্গ;

    পক্ষ CURL_WANTNO:
      ৱিকল্প(tn->himq[option]) {
      পক্ষ CURL_EMPTY:
        /* Already negotiating for NO */
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        ভঙ্গ;
      }
      ভঙ্গ;

    পক্ষ CURL_WANTYES:
      ৱিকল্প(tn->himq[option]) {
      পক্ষ CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        ভঙ্গ;
      }
      ভঙ্গ;
    }
  }
}

স্থির শূন্য set_local_option(সংরচনা Curl_easy *data, সংরচনা TELNET *tn,
                             পূর্ণাঙ্ক option, পূর্ণাঙ্ক newstate)
{
  যদি(newstate == CURL_YES) {
    ৱিকল্প(tn->us[option]) {
    পক্ষ CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      ভঙ্গ;

    পক্ষ CURL_YES:
      /* Already enabled */
      ভঙ্গ;

    পক্ষ CURL_WANTNO:
      ৱিকল্প(tn->usq[option]) {
      পক্ষ CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ভঙ্গ;
      }
      ভঙ্গ;

    পক্ষ CURL_WANTYES:
      ৱিকল্প(tn->usq[option]) {
      পক্ষ CURL_EMPTY:
        /* Error: already negotiating for enable */
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        ভঙ্গ;
      }
      ভঙ্গ;
    }
  }
  অন্যথা { /* NO */
    ৱিকল্প(tn->us[option]) {
    পক্ষ CURL_NO:
      /* Already disabled */
      ভঙ্গ;

    পক্ষ CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      ভঙ্গ;

    পক্ষ CURL_WANTNO:
      ৱিকল্প(tn->usq[option]) {
      পক্ষ CURL_EMPTY:
        /* Already negotiating for NO */
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        ভঙ্গ;
      }
      ভঙ্গ;

    পক্ষ CURL_WANTYES:
      ৱিকল্প(tn->usq[option]) {
      পক্ষ CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        ভঙ্গ;
      পক্ষ CURL_OPPOSITE:
        ভঙ্গ;
      }
      ভঙ্গ;
    }
  }
}

স্থির শূন্য telnet_negotiate(সংরচনা Curl_easy *data, সংরচনা TELNET *tn)
{
  পূর্ণাঙ্ক i;

  বাবে(i = 0; i < CURL_NTELOPTS; i++) {
    যদি(i == CURL_TELOPT_ECHO)
      অনুৱর্ত;

    যদি(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    যদি(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

স্থির শূন্য rec_will(সংরচনা Curl_easy *data, সংরচনা TELNET *tn, পূর্ণাঙ্ক option)
{
  ৱিকল্প(tn->him[option]) {
  পক্ষ CURL_NO:
    যদি(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    অন্যথা
      send_negotiation(data, CURL_DONT, option);

    ভঙ্গ;

  পক্ষ CURL_YES:
    /* Already enabled */
    ভঙ্গ;

  পক্ষ CURL_WANTNO:
    ৱিকল্প(tn->himq[option]) {
    পক্ষ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      ভঙ্গ;
    পক্ষ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      ভঙ্গ;
    }
    ভঙ্গ;

  পক্ষ CURL_WANTYES:
    ৱিকল্প(tn->himq[option]) {
    পক্ষ CURL_EMPTY:
      tn->him[option] = CURL_YES;
      ভঙ্গ;
    পক্ষ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      ভঙ্গ;
    }
    ভঙ্গ;
  }
}

স্থির শূন্য rec_wont(সংরচনা Curl_easy *data, সংরচনা TELNET *tn, পূর্ণাঙ্ক option)
{
  ৱিকল্প(tn->him[option]) {
  পক্ষ CURL_NO:
    /* Already disabled */
    ভঙ্গ;

  পক্ষ CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    ভঙ্গ;

  পক্ষ CURL_WANTNO:
    ৱিকল্প(tn->himq[option]) {
    পক্ষ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      ভঙ্গ;

    পক্ষ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      ভঙ্গ;
    }
    ভঙ্গ;

  পক্ষ CURL_WANTYES:
    ৱিকল্প(tn->himq[option]) {
    পক্ষ CURL_EMPTY:
      tn->him[option] = CURL_NO;
      ভঙ্গ;
    পক্ষ CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      ভঙ্গ;
    }
    ভঙ্গ;
  }
}

স্থির শূন্য printsub(সংরচনা Curl_easy *data,
                     পূর্ণাঙ্ক direction,                /* '<' or '>' */
                     ধ্রুৱ unsigned ৱর্ণ *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  যদি(data->set.verbose) {
    unsigned পূর্ণাঙ্ক i = 0;
    যদি(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      যদি(length >= 3) {
        পূর্ণাঙ্ক j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        যদি(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          যদি(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          অন্যথা যদি(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          অন্যথা
            infof(data, "%u ", i);
          যদি(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          অন্যথা যদি(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          অন্যথা
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      যদি(length >= 2)
        length -= 2;
      অন্যথা /* bad input */
        ঘূৰাই;
    }
    যদি(length <= 1) {
      infof(data, "(Empty suboption?)");
      ঘূৰাই;
    }

    যদি(CURL_TELOPT_OK(pointer[0])) {
      ৱিকল্প(pointer[0]) {
      পক্ষ CURL_TELOPT_TTYPE:
      পক্ষ CURL_TELOPT_XDISPLOC:
      পক্ষ CURL_TELOPT_NEW_ENVIRON:
      পক্ষ CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        ভঙ্গ;
      স্ৱভাৱ:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        ভঙ্গ;
      }
    }
    অন্যথা
      infof(data, "%d (unknown)", pointer[0]);

    ৱিকল্প(pointer[0]) {
    পক্ষ CURL_TELOPT_NAWS:
      যদি(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      ভঙ্গ;
    স্ৱভাৱ:
      ৱিকল্প(pointer[1]) {
      পক্ষ CURL_TELQUAL_IS:
        infof(data, " IS");
        ভঙ্গ;
      পক্ষ CURL_TELQUAL_SEND:
        infof(data, " SEND");
        ভঙ্গ;
      পক্ষ CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        ভঙ্গ;
      পক্ষ CURL_TELQUAL_NAME:
        infof(data, " NAME");
        ভঙ্গ;
      }

      ৱিকল্প(pointer[0]) {
      পক্ষ CURL_TELOPT_TTYPE:
      পক্ষ CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (পূর্ণাঙ্ক)((length > 2) ? (length - 2) : 0), &pointer[2]);
        ভঙ্গ;
      পক্ষ CURL_TELOPT_NEW_ENVIRON:
        যদি(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          বাবে(i = 3; i < length; i++) {
            ৱিকল্প(pointer[i]) {
            পক্ষ CURL_NEW_ENV_VAR:
              infof(data, ", ");
              ভঙ্গ;
            পক্ষ CURL_NEW_ENV_মান:
              infof(data, " = ");
              ভঙ্গ;
            স্ৱভাৱ:
              infof(data, "%c", pointer[i]);
              ভঙ্গ;
            }
          }
        }
        ভঙ্গ;
      স্ৱভাৱ:
        বাবে(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        ভঙ্গ;
      }
    }
  }
}

/* Escape and send a telnet data block */
স্থির CURLcode send_telnet_data(সংরচনা Curl_easy *data,
                                 সংরচনা TELNET *tn,
                                 ধ্রুৱ ৱর্ণ *buffer, ssize_t nread)
{
  size_t i, outlen;
  ধ্রুৱ unsigned ৱর্ণ *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  সংরচনা connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  যদি(nread < 0)
    ঘূৰাই CURLE_TOO_LARGE;

  যদি(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    বাবে(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      যদি(!result && ((unsigned ৱর্ণ)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  অন্যথা {
    outlen = (size_t)nread;
    outbuf = (ধ্রুৱ unsigned ৱর্ণ *)buffer;
  }
  যেতিয়ালৈ(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    সংরচনা pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    যদি(timeout_ms < 0)
      ঘূৰাই CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    ৱিকল্প(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    পক্ষ -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      ভঙ্গ;
    পক্ষ 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      ভঙ্গ;
    স্ৱভাৱ:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      ভঙ্গ;
    }
  }

  ঘূৰাই result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
স্থির শূন্য sendsuboption(সংরচনা Curl_easy *data,
                          সংরচনা TELNET *tn, পূর্ণাঙ্ক option)
{
  size_t nwritten;
  unsigned short x, y;
  ধ্রুৱ unsigned ৱর্ণ *uc1, *uc2;
  CURLcode result;

  ৱিকল্প(option) {
  পক্ষ CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ধ্রুৱ unsigned ৱর্ণ *)&x;
    uc2 = (ধ্রুৱ unsigned ৱর্ণ *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (ধ্রুৱ unsigned ৱর্ণ *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    যদি(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (ধ্রুৱ ৱর্ণ *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    যদি(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    ভঙ্গ;
  }
}

স্থির শূন্য rec_do(সংরচনা Curl_easy *data, সংরচনা TELNET *tn, পূর্ণাঙ্ক option)
{
  ৱিকল্প(tn->us[option]) {
  পক্ষ CURL_NO:
    যদি(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      যদি(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    অন্যথা যদি(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    অন্যথা
      send_negotiation(data, CURL_WONT, option);
    ভঙ্গ;

  পক্ষ CURL_YES:
    /* Already enabled */
    ভঙ্গ;

  পক্ষ CURL_WANTNO:
    ৱিকল্প(tn->usq[option]) {
    পক্ষ CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      ভঙ্গ;
    পক্ষ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      ভঙ্গ;
    }
    ভঙ্গ;

  পক্ষ CURL_WANTYES:
    ৱিকল্প(tn->usq[option]) {
    পক্ষ CURL_EMPTY:
      tn->us[option] = CURL_YES;
      যদি(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      ভঙ্গ;
    পক্ষ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      ভঙ্গ;
    }
    ভঙ্গ;
  }
}

স্থির শূন্য rec_dont(সংরচনা Curl_easy *data, সংরচনা TELNET *tn, পূর্ণাঙ্ক option)
{
  ৱিকল্প(tn->us[option]) {
  পক্ষ CURL_NO:
    /* Already disabled */
    ভঙ্গ;

  পক্ষ CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    ভঙ্গ;

  পক্ষ CURL_WANTNO:
    ৱিকল্প(tn->usq[option]) {
    পক্ষ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      ভঙ্গ;

    পক্ষ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      ভঙ্গ;
    }
    ভঙ্গ;

  পক্ষ CURL_WANTYES:
    ৱিকল্প(tn->usq[option]) {
    পক্ষ CURL_EMPTY:
      tn->us[option] = CURL_NO;
      ভঙ্গ;
    পক্ষ CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      ভঙ্গ;
    }
    ভঙ্গ;
  }
}

স্থির bool str_is_nonascii(ধ্রুৱ ৱর্ণ *str)
{
  ৱর্ণ c;
  যেতিয়ালৈ((c = *str++) != 0)
    যদি(c & 0x80)
      ঘূৰাই TRUE;

  ঘূৰাই FALSE;
}

স্থির CURLcode check_telnet_options(সংরচনা Curl_easy *data,
                                     সংরচনা TELNET *tn)
{
  সংরচনা curl_slist *head;
  সংরচনা curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  যদি(data->state.creds) {
    ৱর্ণ buffer[256];
    যদি(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, পরিমাণ(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    যদি(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = রিক্ত;
      ঘূৰাই CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  বাবে(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    ধ্রুৱ ৱর্ণ *option = head->data;
    ধ্রুৱ ৱর্ণ *arg;
    ধ্রুৱ ৱর্ণ *sep = strchr(option, '=');
    যদি(sep) {
      olen = sep - option;
      arg = ++sep;
      যদি(str_is_nonascii(arg))
        অনুৱর্ত;
      ৱিকল্প(olen) {
      পক্ষ 5:
        /* Terminal type */
        যদি(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          ভঙ্গ;
        }
        result = CURLE_UNKNOWN_OPTION;
        ভঙ্গ;

      পক্ষ 8:
        /* Display variable */
        যদি(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          ভঙ্গ;
        }
        result = CURLE_UNKNOWN_OPTION;
        ভঙ্গ;

      পক্ষ 7:
        /* Environment variable */
        যদি(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          যদি(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            ভঙ্গ;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        অন্যথা
          result = CURLE_UNKNOWN_OPTION;
        ভঙ্গ;

      পক্ষ 2:
        /* Window Size */
        যদি(curl_strnequal(option, "WS", 2)) {
          ধ্রুৱ ৱর্ণ *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          যদি(curlx_str_সংখ্যা(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_সংখ্যা(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          অন্যথা {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        অন্যথা
          result = CURLE_UNKNOWN_OPTION;
        ভঙ্গ;

      পক্ষ 6:
        /* To take care or not of the 8th bit in data exchange */
        যদি(curl_strnequal(option, "BINARY", 6)) {
          ধ্রুৱ ৱর্ণ *p = arg;
          curl_off_t দ্বিমিক_option;
          যদি(!curlx_str_সংখ্যা(&p, &দ্বিমিক_option, 1) &&
             (দ্বিমিক_option != 1)) {
            tn->us_preferred[CURL_TELOPT_দ্বিমিক] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_দ্বিমিক] = CURL_NO;
          }
        }
        অন্যথা
          result = CURLE_UNKNOWN_OPTION;
        ভঙ্গ;
      স্ৱভাৱ:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        ভঙ্গ;
      }
    }
    অন্যথা {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  যদি(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = রিক্ত;
  }

  ঘূৰাই result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
স্থির bool bad_option(ধ্রুৱ ৱর্ণ *data)
{
  ঘূৰাই !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
স্থির CURLcode suboption(সংরচনা Curl_easy *data, সংরচনা TELNET *tn)
{
  সংরচনা curl_slist *v;
  unsigned ৱর্ণ temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  যদি(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ঘূৰাই CURLE_OK;

  printsub(data, '<', (ধ্রুৱ unsigned ৱর্ণ *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  ৱিকল্প(CURL_SB_GET(tn)) {
  পক্ষ CURL_TELOPT_TTYPE:
    যদি(bad_option(tn->subopt_ttype))
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      ঘূৰাই CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ৱর্ণ *)temp, পরিমাণ(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    যদি(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ঘূৰাই result;
    }
    printsub(data, '>', &temp[2], len-2);
    ভঙ্গ;
  পক্ষ CURL_TELOPT_XDISPLOC:
    যদি(bad_option(tn->subopt_xdisploc))
      ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      ঘূৰাই CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ৱর্ণ *)temp, পরিমাণ(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    যদি(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ঘূৰাই result;
    }
    printsub(data, '>', &temp[2], len - 2);
    ভঙ্গ;
  পক্ষ CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((ৱর্ণ *)temp, পরিমাণ(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    বাবে(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      যদি(bad_option(v->data))
        ঘূৰাই CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      যদি(len + tmplen < পরিমাণ(temp) - 6) {
        ধ্রুৱ ৱর্ণ *s = strchr(v->data, ',');
        যদি(!s)
          len += curl_msnprintf((ৱর্ণ *)&temp[len], পরিমাণ(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        অন্যথা {
          size_t vlen = s - v->data;
          len += curl_msnprintf((ৱর্ণ *)&temp[len], পরিমাণ(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (পূর্ণাঙ্ক)vlen, v->data, CURL_NEW_ENV_মান, ++s);
        }
      }
    }
    curl_msnprintf((ৱর্ণ *)&temp[len], পরিমাণ(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    যদি(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      ঘূৰাই result;
    }
    printsub(data, '>', &temp[2], len - 2);
    ভঙ্গ;
  }
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode telrcv(সংরচনা Curl_easy *data,
                       সংরচনা TELNET *tn,
                       ধ্রুৱ unsigned ৱর্ণ *inbuf, /* Data received from
                                                      socket */
                       ssize_t গুনতি)              /* Number of bytes
                                                      received */
{
  unsigned ৱর্ণ c;
  CURLcode result;
  পূর্ণাঙ্ক in = 0;
  পূর্ণাঙ্ক startwrite = -1;

#define startskipping()                                          \
  যদি(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (ধ্রুৱ ৱর্ণ *)&inbuf[startwrite], \
                               in-startwrite);                   \
    যদি(result)                                                   \
      ঘূৰাই result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  যদি(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  যেতিয়ালৈ(গুনতি--) {
    c = inbuf[in];

    ৱিকল্প(tn->telrcv_state) {
    পক্ষ CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      যদি(c == '\0') {
        startskipping();
        ভঙ্গ;   /* Ignore \0 after CR */
      }
      writebyte();
      ভঙ্গ;

    পক্ষ CURL_TS_DATA:
      যদি(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        ভঙ্গ;
      }
      অন্যথা যদি(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      ভঙ্গ;

    পক্ষ CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      ৱিকল্প(c) {
      পক্ষ CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        ভঙ্গ;
      পক্ষ CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        ভঙ্গ;
      পক্ষ CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        ভঙ্গ;
      পক্ষ CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        ভঙ্গ;
      পক্ষ CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        ভঙ্গ;
      পক্ষ CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        ভঙ্গ;
      পক্ষ CURL_DM:
      পক্ষ CURL_NOP:
      পক্ষ CURL_GA:
      স্ৱভাৱ:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        ভঙ্গ;
      }
      ভঙ্গ;

    পক্ষ CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ভঙ্গ;

    পক্ষ CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ভঙ্গ;

    পক্ষ CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ভঙ্গ;

    পক্ষ CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      ভঙ্গ;

    পক্ষ CURL_TS_SB:
      যদি(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      অন্যথা
        CURL_SB_ACCUM(tn, c);
      ভঙ্গ;

    পক্ষ CURL_TS_SE:
      যদি(c != CURL_SE) {
        যদি(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          ঘূৰাই CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      অন্যথা {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        যদি(result)
          ঘূৰাই result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      ভঙ্গ;
    }
    ++in;
  }
  bufferflush();
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode telnet_done(সংরচনা Curl_easy *data,
                            CURLcode status, bool premature)
{
  (শূন্য)status;
  (শূন্য)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  ঘূৰাই CURLE_OK;
}

স্থির CURLcode telnet_do(সংরচনা Curl_easy *data, bool *done)
{
  CURLcode result;
  সংরচনা connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_গুনতি;
  DWORD wait_timeout;
  DWORD readfile_read;
#অন্যথা
  timediff_t interval_ms;
  সংরচনা pollfd pfd[2];
  পূর্ণাঙ্ক poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  ৱর্ণ buffer[4 * 1024];
  সংরচনা TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  যদি(result)
    ঘূৰাই result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  যদি(!tn)
    ঘূৰাই CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  যদি(result)
    ঘূৰাই result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  যদি(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    ঘূৰাই CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  যদি(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ঘূৰাই CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_প্ৰবেশ_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  যদি(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_গুনতি = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  অন্যথা {
    obj_গুনতি = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  যেতিয়ালৈ(keepon) {
    ধ্রুৱ DWORD buf_size = (DWORD)পরিমাণ(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_গুনতি, objs,
                                           FALSE, wait_timeout);
    ৱিকল্প(waitret) {

    পক্ষ WAIT_TIMEOUT: {
      বাবে(;;) {
        যদি(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          যদি(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            ভঙ্গ;
          }

          যদি(n == CURL_READFUNC_PAUSE)
            ভঙ্গ;

          যদি(n == 0)                        /* no bytes */
            ভঙ্গ;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        অন্যথা {
          /* read from stdin */
          যদি(!PeekNamedPipe(stdin_handle, রিক্ত, 0, রিক্ত,
                            &readfile_read, রিক্ত)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            ভঙ্গ;
          }

          যদি(!readfile_read)
            ভঙ্গ;

          যদি(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, রিক্ত)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            ভঙ্গ;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        যদি(result) {
          keepon = FALSE;
          ভঙ্গ;
        }
      }
    }
    ভঙ্গ;

    পক্ষ WAIT_OBJECT_0 + 1: {
      যদি(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, রিক্ত)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        ভঙ্গ;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      যদি(result) {
        keepon = FALSE;
        ভঙ্গ;
      }
    }
    ভঙ্গ;

    পক্ষ WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      যদি(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        পূর্ণাঙ্ক sockerr = SOCKERRNO;
        যদি(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        ভঙ্গ;
      }
      যদি(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, পরিমাণ(buffer), &nread);
        /* read would have blocked. Loop again */
        যদি(result == CURLE_AGAIN)
          ভঙ্গ;
        /* returned not-zero, this an error */
        অন্যথা যদি(result) {
          keepon = FALSE;
          ভঙ্গ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        অন্যথা যদি(!nread) {
          keepon = FALSE;
          ভঙ্গ;
        }

        result = telrcv(data, tn, (unsigned ৱর্ণ *)buffer, nread);
        যদি(result) {
          keepon = FALSE;
          ভঙ্গ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        যদি(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      যদি(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      ভঙ্গ;
    }
    } /* switch */

    যদি(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  যদি(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#অন্যথা
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  যদি(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  অন্যথা {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    যদি(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  যেতিয়ালৈ(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    ৱিকল্প(Curl_poll(pfd, (unsigned পূর্ণাঙ্ক)poll_cnt, interval_ms)) {
    পক্ষ -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      অনুৱর্ত;
    পক্ষ 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    স্ৱভাৱ:                    /* read! */
      যদি(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, পরিমাণ(buffer), &nread);
        /* read would have blocked. Loop again */
        যদি(result == CURLE_AGAIN)
          ভঙ্গ;
        /* returned not-zero, this an error */
        যদি(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          যদি(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          ভঙ্গ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        অন্যথা যদি(!nread) {
          keepon = FALSE;
          ভঙ্গ;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned ৱর্ণ *)buffer, nread);
        যদি(result) {
          keepon = FALSE;
          ভঙ্গ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        যদি(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      যদি(poll_cnt == 2) {
        যদি(pfd[1].revents & POLLIN) { /* read from in file */
          snread = পঠ(pfd[1].fd, buffer, পরিমাণ(buffer));
        }
      }
      অন্যথা {
        /* read from user-supplied method */
        snread = (পূর্ণাঙ্ক)data->state.fread_func(buffer, 1, পরিমাণ(buffer),
                                             data->state.in);
        যদি(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          ভঙ্গ;
        }
        যদি(snread == CURL_READFUNC_PAUSE)
          ভঙ্গ;
      }

      যদি(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        যদি(result) {
          keepon = FALSE;
          ভঙ্গ;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      অন্যথা যদি(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      ভঙ্গ;
    } /* poll switch statement */

    যদি(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    যদি(!result) {
      result = Curl_pgrsUpdate(data);
      যদি(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  ঘূৰাই result;
}

/*
 * TELNET protocol handler.
 */
ধ্রুৱ সংরচনা Curl_protocol Curl_protocol_telnet = {
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
