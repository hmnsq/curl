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
#অন্তর্ভাব "curl_setup.h"
#অন্তর্ভাব "urldata.h"
#অন্তর্ভাব "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#অন্তর্ভাব <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#অন্তর্ভাব <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#অন্তর্ভাব <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#অন্তর্ভাব <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#অন্তর্ভাব <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#অন্তর্ভাব <sys/param.h>
#endif

#অন্তর্ভাব "url.h"
#অন্তর্ভাব "transfer.h"
#অন্তর্ভাব "sendf.h"
#অন্তর্ভাব "curl_trc.h"
#অন্তর্ভাব "progress.h"
#অন্তর্ভাব "arpa_telnet.h"
#অন্তর্ভাব "connect.h"
#অন্তর্ভাব "select.h"
#অন্তর্ভাব "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  করো {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } যখন(0)
#define CURL_SB_ACCUM(x, c)                                         \
  করো {                                                              \
    যদি((x)->subpointer < ((x)->subbuffer + আকার((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } যখন(0)

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
প্রকারলেখ গণনা {
  CURL_TS_তথ্য = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

কাঠামো TELNET {
  পূর্ণসংখ্যা please_negotiate;
  পূর্ণসংখ্যা already_negotiated;
  পূর্ণসংখ্যা us[256];
  পূর্ণসংখ্যা usq[256];
  পূর্ণসংখ্যা us_preferred[256];
  পূর্ণসংখ্যা him[256];
  পূর্ণসংখ্যা himq[256];
  পূর্ণসংখ্যা him_preferred[256];
  পূর্ণসংখ্যা subnegotiation[256];
  ধ্রুবক অক্ষর *subopt_ttype;          /* Set with suboption TTYPE */
  ধ্রুবক অক্ষর *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  কাঠামো curl_slist *telnet_vars;    /* Environment variables */
  কাঠামো dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned অক্ষর subbuffer[SUBBUFSIZE];
  unsigned অক্ষর *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#অন্যথা
স্থির শূন্য printoption(কাঠামো Curl_easy *তথ্য,
                        ধ্রুবক অক্ষর *direction, পূর্ণসংখ্যা cmd, পূর্ণসংখ্যা option)
{
  যদি(তথ্য->set.verbose) {
    যদি(cmd == CURL_IAC) {
      যদি(CURL_TELCMD_OK(option))
        infof(তথ্য, "%s IAC %s", direction, CURL_TELCMD(option));
      অন্যথা
        infof(তথ্য, "%s IAC %d", direction, option);
    }
    অন্যথা {
      ধ্রুবক অক্ষর *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      যদি(fmt) {
        ধ্রুবক অক্ষর *opt;
        যদি(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        অন্যথা যদি(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        অন্যথা
          opt = ফাঁকা;

        যদি(opt)
          infof(তথ্য, "%s %s %s", direction, fmt, opt);
        অন্যথা
          infof(তথ্য, "%s %s %d", direction, fmt, option);
      }
      অন্যথা
        infof(তথ্য, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

স্থির শূন্য telnet_easy_dtor(ধ্রুবক শূন্য *চাবি, size_t klen, শূন্য *entry)
{
  কাঠামো TELNET *tn = entry;
  (শূন্য)চাবি;
  (শূন্য)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

স্থির CURLcode init_telnet(কাঠামো Curl_easy *তথ্য)
{
  কাঠামো TELNET *tn;

  tn = curlx_calloc(1, আকার(কাঠামো TELNET));
  যদি(!tn)
    ফেরত CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&tn->out, 0xffff);

  tn->telrcv_state = CURL_TS_তথ্য;

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

  ফেরত Curl_meta_set(তথ্য, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

স্থির শূন্য send_negotiation(কাঠামো Curl_easy *তথ্য, পূর্ণসংখ্যা cmd, পূর্ণসংখ্যা option)
{
  unsigned অক্ষর buf[3];
  size_t nwritten;
  CURLcode ফলাফল;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned অক্ষর)cmd;
  buf[2] = (unsigned অক্ষর)option;

  ফলাফল = Curl_xfer_send(তথ্য, buf, 3, FALSE, &nwritten);
  যদি(ফলাফল)
    failf(তথ্য, "Sending data failed: %s", curl_easy_strerror(ফলাফল));

  printoption(তথ্য, "SENT", cmd, option);
}

স্থির শূন্য set_remote_option(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn,
                              পূর্ণসংখ্যা option, পূর্ণসংখ্যা newstate)
{
  যদি(newstate == CURL_YES) {
    নির্বাচন(tn->him[option]) {
    ক্ষেত্র CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(তথ্য, CURL_DO, option);
      বিরতি;

    ক্ষেত্র CURL_YES:
      /* Already enabled */
      বিরতি;

    ক্ষেত্র CURL_WANTNO:
      নির্বাচন(tn->himq[option]) {
      ক্ষেত্র CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        /* Error: already queued an enable request */
        বিরতি;
      }
      বিরতি;

    ক্ষেত্র CURL_WANTYES:
      নির্বাচন(tn->himq[option]) {
      ক্ষেত্র CURL_EMPTY:
        /* Error: already negotiating for enable */
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        বিরতি;
      }
      বিরতি;
    }
  }
  অন্যথা { /* NO */
    নির্বাচন(tn->him[option]) {
    ক্ষেত্র CURL_NO:
      /* Already disabled */
      বিরতি;

    ক্ষেত্র CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(তথ্য, CURL_DONT, option);
      বিরতি;

    ক্ষেত্র CURL_WANTNO:
      নির্বাচন(tn->himq[option]) {
      ক্ষেত্র CURL_EMPTY:
        /* Already negotiating for NO */
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        বিরতি;
      }
      বিরতি;

    ক্ষেত্র CURL_WANTYES:
      নির্বাচন(tn->himq[option]) {
      ক্ষেত্র CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        বিরতি;
      }
      বিরতি;
    }
  }
}

স্থির শূন্য set_local_option(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn,
                             পূর্ণসংখ্যা option, পূর্ণসংখ্যা newstate)
{
  যদি(newstate == CURL_YES) {
    নির্বাচন(tn->us[option]) {
    ক্ষেত্র CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(তথ্য, CURL_WILL, option);
      বিরতি;

    ক্ষেত্র CURL_YES:
      /* Already enabled */
      বিরতি;

    ক্ষেত্র CURL_WANTNO:
      নির্বাচন(tn->usq[option]) {
      ক্ষেত্র CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        /* Error: already queued an enable request */
        বিরতি;
      }
      বিরতি;

    ক্ষেত্র CURL_WANTYES:
      নির্বাচন(tn->usq[option]) {
      ক্ষেত্র CURL_EMPTY:
        /* Error: already negotiating for enable */
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        বিরতি;
      }
      বিরতি;
    }
  }
  অন্যথা { /* NO */
    নির্বাচন(tn->us[option]) {
    ক্ষেত্র CURL_NO:
      /* Already disabled */
      বিরতি;

    ক্ষেত্র CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(তথ্য, CURL_WONT, option);
      বিরতি;

    ক্ষেত্র CURL_WANTNO:
      নির্বাচন(tn->usq[option]) {
      ক্ষেত্র CURL_EMPTY:
        /* Already negotiating for NO */
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        বিরতি;
      }
      বিরতি;

    ক্ষেত্র CURL_WANTYES:
      নির্বাচন(tn->usq[option]) {
      ক্ষেত্র CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        বিরতি;
      ক্ষেত্র CURL_OPPOSITE:
        বিরতি;
      }
      বিরতি;
    }
  }
}

স্থির শূন্য telnet_negotiate(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn)
{
  পূর্ণসংখ্যা i;

  জন্য(i = 0; i < CURL_NTELOPTS; i++) {
    যদি(i == CURL_TELOPT_ECHO)
      চালু;

    যদি(tn->us_preferred[i] == CURL_YES)
      set_local_option(তথ্য, tn, i, CURL_YES);

    যদি(tn->him_preferred[i] == CURL_YES)
      set_remote_option(তথ্য, tn, i, CURL_YES);
  }
}

স্থির শূন্য rec_will(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn, পূর্ণসংখ্যা option)
{
  নির্বাচন(tn->him[option]) {
  ক্ষেত্র CURL_NO:
    যদি(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(তথ্য, CURL_DO, option);
    }
    অন্যথা
      send_negotiation(তথ্য, CURL_DONT, option);

    বিরতি;

  ক্ষেত্র CURL_YES:
    /* Already enabled */
    বিরতি;

  ক্ষেত্র CURL_WANTNO:
    নির্বাচন(tn->himq[option]) {
    ক্ষেত্র CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      বিরতি;
    ক্ষেত্র CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      বিরতি;
    }
    বিরতি;

  ক্ষেত্র CURL_WANTYES:
    নির্বাচন(tn->himq[option]) {
    ক্ষেত্র CURL_EMPTY:
      tn->him[option] = CURL_YES;
      বিরতি;
    ক্ষেত্র CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(তথ্য, CURL_DONT, option);
      বিরতি;
    }
    বিরতি;
  }
}

স্থির শূন্য rec_wont(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn, পূর্ণসংখ্যা option)
{
  নির্বাচন(tn->him[option]) {
  ক্ষেত্র CURL_NO:
    /* Already disabled */
    বিরতি;

  ক্ষেত্র CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(তথ্য, CURL_DONT, option);
    বিরতি;

  ক্ষেত্র CURL_WANTNO:
    নির্বাচন(tn->himq[option]) {
    ক্ষেত্র CURL_EMPTY:
      tn->him[option] = CURL_NO;
      বিরতি;

    ক্ষেত্র CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(তথ্য, CURL_DO, option);
      বিরতি;
    }
    বিরতি;

  ক্ষেত্র CURL_WANTYES:
    নির্বাচন(tn->himq[option]) {
    ক্ষেত্র CURL_EMPTY:
      tn->him[option] = CURL_NO;
      বিরতি;
    ক্ষেত্র CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      বিরতি;
    }
    বিরতি;
  }
}

স্থির শূন্য printsub(কাঠামো Curl_easy *তথ্য,
                     পূর্ণসংখ্যা direction,                /* '<' or '>' */
                     ধ্রুবক unsigned অক্ষর *pointer, /* ptr to suboption data */
                     size_t দৈর্ঘ্য)                /* suboption data length */
{
  যদি(তথ্য->set.verbose) {
    unsigned পূর্ণসংখ্যা i = 0;
    যদি(direction) {
      infof(তথ্য, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      যদি(দৈর্ঘ্য >= 3) {
        পূর্ণসংখ্যা j;

        i = pointer[দৈর্ঘ্য - 2];
        j = pointer[দৈর্ঘ্য - 1];

        যদি(i != CURL_IAC || j != CURL_SE) {
          infof(তথ্য, "(terminated by ");
          যদি(CURL_TELOPT_OK(i))
            infof(তথ্য, "%s ", CURL_TELOPT(i));
          অন্যথা যদি(CURL_TELCMD_OK(i))
            infof(তথ্য, "%s ", CURL_TELCMD(i));
          অন্যথা
            infof(তথ্য, "%u ", i);
          যদি(CURL_TELOPT_OK(j))
            infof(তথ্য, "%s", CURL_TELOPT(j));
          অন্যথা যদি(CURL_TELCMD_OK(j))
            infof(তথ্য, "%s", CURL_TELCMD(j));
          অন্যথা
            infof(তথ্য, "%d", j);
          infof(তথ্য, ", not IAC SE) ");
        }
      }
      যদি(দৈর্ঘ্য >= 2)
        দৈর্ঘ্য -= 2;
      অন্যথা /* bad input */
        ফেরত;
    }
    যদি(দৈর্ঘ্য <= 1) {
      infof(তথ্য, "(Empty suboption?)");
      ফেরত;
    }

    যদি(CURL_TELOPT_OK(pointer[0])) {
      নির্বাচন(pointer[0]) {
      ক্ষেত্র CURL_TELOPT_TTYPE:
      ক্ষেত্র CURL_TELOPT_XDISPLOC:
      ক্ষেত্র CURL_TELOPT_NEW_ENVIRON:
      ক্ষেত্র CURL_TELOPT_NAWS:
        infof(তথ্য, "%s", CURL_TELOPT(pointer[0]));
        বিরতি;
      পূর্বনির্ধারিত:
        infof(তথ্য, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        বিরতি;
      }
    }
    অন্যথা
      infof(তথ্য, "%d (unknown)", pointer[0]);

    নির্বাচন(pointer[0]) {
    ক্ষেত্র CURL_TELOPT_NAWS:
      যদি(দৈর্ঘ্য > 4)
        infof(তথ্য, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      বিরতি;
    পূর্বনির্ধারিত:
      নির্বাচন(pointer[1]) {
      ক্ষেত্র CURL_TELQUAL_IS:
        infof(তথ্য, " IS");
        বিরতি;
      ক্ষেত্র CURL_TELQUAL_SEND:
        infof(তথ্য, " SEND");
        বিরতি;
      ক্ষেত্র CURL_TELQUAL_INFO:
        infof(তথ্য, " INFO/REPLY");
        বিরতি;
      ক্ষেত্র CURL_TELQUAL_NAME:
        infof(তথ্য, " NAME");
        বিরতি;
      }

      নির্বাচন(pointer[0]) {
      ক্ষেত্র CURL_TELOPT_TTYPE:
      ক্ষেত্র CURL_TELOPT_XDISPLOC:
        infof(তথ্য, " \"%.*s\"",
              (পূর্ণসংখ্যা)((দৈর্ঘ্য > 2) ? (দৈর্ঘ্য - 2) : 0), &pointer[2]);
        বিরতি;
      ক্ষেত্র CURL_TELOPT_NEW_ENVIRON:
        যদি(pointer[1] == CURL_TELQUAL_IS) {
          infof(তথ্য, " ");
          জন্য(i = 3; i < দৈর্ঘ্য; i++) {
            নির্বাচন(pointer[i]) {
            ক্ষেত্র CURL_NEW_ENV_VAR:
              infof(তথ্য, ", ");
              বিরতি;
            ক্ষেত্র CURL_NEW_ENV_মান:
              infof(তথ্য, " = ");
              বিরতি;
            পূর্বনির্ধারিত:
              infof(তথ্য, "%c", pointer[i]);
              বিরতি;
            }
          }
        }
        বিরতি;
      পূর্বনির্ধারিত:
        জন্য(i = 2; i < দৈর্ঘ্য; i++)
          infof(তথ্য, " %.2x", pointer[i]);
        বিরতি;
      }
    }
  }
}

/* Escape and send a telnet data block */
স্থির CURLcode send_telnet_তথ্য(কাঠামো Curl_easy *তথ্য,
                                 কাঠামো TELNET *tn,
                                 ধ্রুবক অক্ষর *বাফার, ssize_t nread)
{
  size_t i, outlen;
  ধ্রুবক unsigned অক্ষর *outbuf;
  CURLcode ফলাফল = CURLE_OK;
  size_t bytes_written;
  size_t যোগ_written = 0;
  কাঠামো connectdata *conn = তথ্য->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  যদি(nread < 0)
    ফেরত CURLE_TOO_LARGE;

  যদি(memchr(বাফার, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    জন্য(i = 0; i < (size_t)nread && !ফলাফল; i++) {
      ফলাফল = curlx_dyn_addn(&tn->out, &বাফার[i], 1);
      যদি(!ফলাফল && ((unsigned অক্ষর)বাফার[i] == CURL_IAC))
        /* IAC is FF in hex */
        ফলাফল = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  অন্যথা {
    outlen = (size_t)nread;
    outbuf = (ধ্রুবক unsigned অক্ষর *)বাফার;
  }
  যখন(!ফলাফল && যোগ_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    কাঠামো pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(তথ্য);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    যদি(timeout_ms < 0)
      ফেরত CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    নির্বাচন(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    ক্ষেত্র -1:                    /* error, abort writing */
      ফলাফল = CURLE_SEND_ERROR;
      বিরতি;
    ক্ষেত্র 0:                     /* timeout */
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      বিরতি;
    পূর্বনির্ধারিত:                    /* write! */
      bytes_written = 0;
      ফলাফল = Curl_xfer_send(তথ্য, outbuf + যোগ_written,
                              outlen - যোগ_written, FALSE, &bytes_written);
      যোগ_written += bytes_written;
      বিরতি;
    }
  }

  ফেরত ফলাফল;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
স্থির শূন্য sendsuboption(কাঠামো Curl_easy *তথ্য,
                          কাঠামো TELNET *tn, পূর্ণসংখ্যা option)
{
  size_t nwritten;
  unsigned short x, y;
  ধ্রুবক unsigned অক্ষর *uc1, *uc2;
  CURLcode ফলাফল;

  নির্বাচন(option) {
  ক্ষেত্র CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ধ্রুবক unsigned অক্ষর *)&x;
    uc2 = (ধ্রুবক unsigned অক্ষর *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(তথ্য, '>', (ধ্রুবক unsigned অক্ষর *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    ফলাফল = Curl_xfer_send(তথ্য, tn->subbuffer, 3, FALSE, &nwritten);
    যদি(ফলাফল)
      failf(তথ্য, "Sending data failed: %s", curl_easy_strerror(ফলাফল));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_তথ্য(তথ্য, tn, (ধ্রুবক অক্ষর *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    ফলাফল = Curl_xfer_send(তথ্য, tn->subbuffer + 7, 2, FALSE, &nwritten);
    যদি(ফলাফল)
      failf(তথ্য, "Sending data failed: %s", curl_easy_strerror(ফলাফল));
    বিরতি;
  }
}

স্থির শূন্য rec_do(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn, পূর্ণসংখ্যা option)
{
  নির্বাচন(tn->us[option]) {
  ক্ষেত্র CURL_NO:
    যদি(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(তথ্য, CURL_WILL, option);
      যদি(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(তথ্য, tn, option);
    }
    অন্যথা যদি(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(তথ্য, CURL_WILL, option);
      sendsuboption(তথ্য, tn, option);
    }
    অন্যথা
      send_negotiation(তথ্য, CURL_WONT, option);
    বিরতি;

  ক্ষেত্র CURL_YES:
    /* Already enabled */
    বিরতি;

  ক্ষেত্র CURL_WANTNO:
    নির্বাচন(tn->usq[option]) {
    ক্ষেত্র CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      বিরতি;
    ক্ষেত্র CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      বিরতি;
    }
    বিরতি;

  ক্ষেত্র CURL_WANTYES:
    নির্বাচন(tn->usq[option]) {
    ক্ষেত্র CURL_EMPTY:
      tn->us[option] = CURL_YES;
      যদি(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(তথ্য, tn, option);
      }
      বিরতি;
    ক্ষেত্র CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(তথ্য, CURL_WONT, option);
      বিরতি;
    }
    বিরতি;
  }
}

স্থির শূন্য rec_dont(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn, পূর্ণসংখ্যা option)
{
  নির্বাচন(tn->us[option]) {
  ক্ষেত্র CURL_NO:
    /* Already disabled */
    বিরতি;

  ক্ষেত্র CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(তথ্য, CURL_WONT, option);
    বিরতি;

  ক্ষেত্র CURL_WANTNO:
    নির্বাচন(tn->usq[option]) {
    ক্ষেত্র CURL_EMPTY:
      tn->us[option] = CURL_NO;
      বিরতি;

    ক্ষেত্র CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(তথ্য, CURL_WILL, option);
      বিরতি;
    }
    বিরতি;

  ক্ষেত্র CURL_WANTYES:
    নির্বাচন(tn->usq[option]) {
    ক্ষেত্র CURL_EMPTY:
      tn->us[option] = CURL_NO;
      বিরতি;
    ক্ষেত্র CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      বিরতি;
    }
    বিরতি;
  }
}

স্থির bool str_is_nonascii(ধ্রুবক অক্ষর *str)
{
  অক্ষর c;
  যখন((c = *str++) != 0)
    যদি(c & 0x80)
      ফেরত TRUE;

  ফেরত FALSE;
}

স্থির CURLcode check_telnet_options(কাঠামো Curl_easy *তথ্য,
                                     কাঠামো TELNET *tn)
{
  কাঠামো curl_slist *শীর্ষ;
  কাঠামো curl_slist *beg;
  CURLcode ফলাফল = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  যদি(তথ্য->state.creds) {
    অক্ষর বাফার[256];
    যদি(str_is_nonascii(Curl_creds_user(তথ্য->conn->creds))) {
      DEBUGF(infof(তথ্য, "set a non ASCII username in telnet"));
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(বাফার, আকার(বাফার), "USER,%s",
                   Curl_creds_user(তথ্য->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, বাফার);
    যদি(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ফাঁকা;
      ফেরত CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  জন্য(শীর্ষ = তথ্য->set.telnet_options; শীর্ষ && !ফলাফল; শীর্ষ = শীর্ষ->পরবর্তী) {
    size_t olen;
    ধ্রুবক অক্ষর *option = শীর্ষ->তথ্য;
    ধ্রুবক অক্ষর *arg;
    ধ্রুবক অক্ষর *sep = strchr(option, '=');
    যদি(sep) {
      olen = sep - option;
      arg = ++sep;
      যদি(str_is_nonascii(arg))
        চালু;
      নির্বাচন(olen) {
      ক্ষেত্র 5:
        /* Terminal type */
        যদি(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          বিরতি;
        }
        ফলাফল = CURLE_UNKNOWN_OPTION;
        বিরতি;

      ক্ষেত্র 8:
        /* Display variable */
        যদি(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          বিরতি;
        }
        ফলাফল = CURLE_UNKNOWN_OPTION;
        বিরতি;

      ক্ষেত্র 7:
        /* Environment variable */
        যদি(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          যদি(!beg) {
            ফলাফল = CURLE_OUT_OF_MEMORY;
            বিরতি;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        অন্যথা
          ফলাফল = CURLE_UNKNOWN_OPTION;
        বিরতি;

      ক্ষেত্র 2:
        /* Window Size */
        যদি(curl_strnequal(option, "WS", 2)) {
          ধ্রুবক অক্ষর *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          যদি(curlx_str_সংখ্যা(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_সংখ্যা(&p, &y, 0xffff)) {
            failf(তথ্য, "Syntax error in telnet option: %s", শীর্ষ->তথ্য);
            ফলাফল = CURLE_SETOPT_OPTION_SYNTAX;
          }
          অন্যথা {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        অন্যথা
          ফলাফল = CURLE_UNKNOWN_OPTION;
        বিরতি;

      ক্ষেত্র 6:
        /* To take care or not of the 8th bit in data exchange */
        যদি(curl_strnequal(option, "BINARY", 6)) {
          ধ্রুবক অক্ষর *p = arg;
          curl_off_t দ্বিমিক_option;
          যদি(!curlx_str_সংখ্যা(&p, &দ্বিমিক_option, 1) &&
             (দ্বিমিক_option != 1)) {
            tn->us_preferred[CURL_TELOPT_দ্বিমিক] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_দ্বিমিক] = CURL_NO;
          }
        }
        অন্যথা
          ফলাফল = CURLE_UNKNOWN_OPTION;
        বিরতি;
      পূর্বনির্ধারিত:
        failf(তথ্য, "Unknown telnet option %s", শীর্ষ->তথ্য);
        ফলাফল = CURLE_UNKNOWN_OPTION;
        বিরতি;
      }
    }
    অন্যথা {
      failf(তথ্য, "Syntax error in telnet option: %s", শীর্ষ->তথ্য);
      ফলাফল = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  যদি(ফলাফল) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ফাঁকা;
  }

  ফেরত ফলাফল;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
স্থির bool bad_option(ধ্রুবক অক্ষর *তথ্য)
{
  ফেরত !তথ্য || !!strchr(তথ্য, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
স্থির CURLcode suboption(কাঠামো Curl_easy *তথ্য, কাঠামো TELNET *tn)
{
  কাঠামো curl_slist *v;
  unsigned অক্ষর অস্থায়ী[2048];
  size_t nwritten;
  size_t len;
  CURLcode ফলাফল = CURLE_OK;

  যদি(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ফেরত CURLE_OK;

  printsub(তথ্য, '<', (ধ্রুবক unsigned অক্ষর *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  নির্বাচন(CURL_SB_GET(tn)) {
  ক্ষেত্র CURL_TELOPT_TTYPE:
    যদি(bad_option(tn->subopt_ttype))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(strlen(tn->subopt_ttype) > 1000) {
      failf(তথ্য, "Too long telnet TTYPE");
      ফেরত CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((অক্ষর *)অস্থায়ী, আকার(অস্থায়ী), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    ফলাফল = Curl_xfer_send(তথ্য, অস্থায়ী, len, FALSE, &nwritten);
    যদি(ফলাফল) {
      failf(তথ্য, "Sending data failed: %s", curl_easy_strerror(ফলাফল));
      ফেরত ফলাফল;
    }
    printsub(তথ্য, '>', &অস্থায়ী[2], len-2);
    বিরতি;
  ক্ষেত্র CURL_TELOPT_XDISPLOC:
    যদি(bad_option(tn->subopt_xdisploc))
      ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
    যদি(strlen(tn->subopt_xdisploc) > 1000) {
      failf(তথ্য, "Too long telnet XDISPLOC");
      ফেরত CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((অক্ষর *)অস্থায়ী, আকার(অস্থায়ী), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    ফলাফল = Curl_xfer_send(তথ্য, অস্থায়ী, len, FALSE, &nwritten);
    যদি(ফলাফল) {
      failf(তথ্য, "Sending data failed: %s", curl_easy_strerror(ফলাফল));
      ফেরত ফলাফল;
    }
    printsub(তথ্য, '>', &অস্থায়ী[2], len - 2);
    বিরতি;
  ক্ষেত্র CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((অক্ষর *)অস্থায়ী, আকার(অস্থায়ী), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    জন্য(v = tn->telnet_vars; v; v = v->পরবর্তী) {
      size_t tmplen = (strlen(v->তথ্য) + 1);
      যদি(bad_option(v->তথ্য))
        ফেরত CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      যদি(len + tmplen < আকার(অস্থায়ী) - 6) {
        ধ্রুবক অক্ষর *s = strchr(v->তথ্য, ',');
        যদি(!s)
          len += curl_msnprintf((অক্ষর *)&অস্থায়ী[len], আকার(অস্থায়ী) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->তথ্য);
        অন্যথা {
          size_t vlen = s - v->তথ্য;
          len += curl_msnprintf((অক্ষর *)&অস্থায়ী[len], আকার(অস্থায়ী) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (পূর্ণসংখ্যা)vlen, v->তথ্য, CURL_NEW_ENV_মান, ++s);
        }
      }
    }
    curl_msnprintf((অক্ষর *)&অস্থায়ী[len], আকার(অস্থায়ী) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    ফলাফল = Curl_xfer_send(তথ্য, অস্থায়ী, len, FALSE, &nwritten);
    যদি(ফলাফল) {
      failf(তথ্য, "Sending data failed: %s", curl_easy_strerror(ফলাফল));
      ফেরত ফলাফল;
    }
    printsub(তথ্য, '>', &অস্থায়ী[2], len - 2);
    বিরতি;
  }
  ফেরত CURLE_OK;
}

স্থির CURLcode telrcv(কাঠামো Curl_easy *তথ্য,
                       কাঠামো TELNET *tn,
                       ধ্রুবক unsigned অক্ষর *inbuf, /* Data received from
                                                      socket */
                       ssize_t গুনতি)              /* Number of bytes
                                                      received */
{
  unsigned অক্ষর c;
  CURLcode ফলাফল;
  পূর্ণসংখ্যা in = 0;
  পূর্ণসংখ্যা startwrite = -1;

#define startskipping()                                          \
  যদি(startwrite >= 0) {                                          \
    ফলাফল = Curl_client_write(তথ্য,                             \
                               CLIENTWRITE_BODY,                 \
                               (ধ্রুবক অক্ষর *)&inbuf[startwrite], \
                               in-startwrite);                   \
    যদি(ফলাফল)                                                   \
      ফেরত ফলাফল;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  যদি(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  যখন(গুনতি--) {
    c = inbuf[in];

    নির্বাচন(tn->telrcv_state) {
    ক্ষেত্র CURL_TS_CR:
      tn->telrcv_state = CURL_TS_তথ্য;
      যদি(c == '\0') {
        startskipping();
        বিরতি;   /* Ignore \0 after CR */
      }
      writebyte();
      বিরতি;

    ক্ষেত্র CURL_TS_তথ্য:
      যদি(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        বিরতি;
      }
      অন্যথা যদি(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      বিরতি;

    ক্ষেত্র CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      নির্বাচন(c) {
      ক্ষেত্র CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        বিরতি;
      ক্ষেত্র CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        বিরতি;
      ক্ষেত্র CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        বিরতি;
      ক্ষেত্র CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        বিরতি;
      ক্ষেত্র CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        বিরতি;
      ক্ষেত্র CURL_IAC:
        tn->telrcv_state = CURL_TS_তথ্য;
        writebyte();
        বিরতি;
      ক্ষেত্র CURL_DM:
      ক্ষেত্র CURL_NOP:
      ক্ষেত্র CURL_GA:
      পূর্বনির্ধারিত:
        tn->telrcv_state = CURL_TS_তথ্য;
        printoption(তথ্য, "RCVD", CURL_IAC, c);
        বিরতি;
      }
      বিরতি;

    ক্ষেত্র CURL_TS_WILL:
      printoption(তথ্য, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(তথ্য, tn, c);
      tn->telrcv_state = CURL_TS_তথ্য;
      বিরতি;

    ক্ষেত্র CURL_TS_WONT:
      printoption(তথ্য, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(তথ্য, tn, c);
      tn->telrcv_state = CURL_TS_তথ্য;
      বিরতি;

    ক্ষেত্র CURL_TS_DO:
      printoption(তথ্য, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(তথ্য, tn, c);
      tn->telrcv_state = CURL_TS_তথ্য;
      বিরতি;

    ক্ষেত্র CURL_TS_DONT:
      printoption(তথ্য, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(তথ্য, tn, c);
      tn->telrcv_state = CURL_TS_তথ্য;
      বিরতি;

    ক্ষেত্র CURL_TS_SB:
      যদি(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      অন্যথা
        CURL_SB_ACCUM(tn, c);
      বিরতি;

    ক্ষেত্র CURL_TS_SE:
      যদি(c != CURL_SE) {
        যদি(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(তথ্য, "telnet: suboption error");
          ফেরত CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      অন্যথা {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        ফলাফল = suboption(তথ্য, tn);   /* handle sub-option */
        যদি(ফলাফল)
          ফেরত ফলাফল;
        tn->telrcv_state = CURL_TS_তথ্য;
      }
      বিরতি;
    }
    ++in;
  }
  bufferflush();
  ফেরত CURLE_OK;
}

স্থির CURLcode telnet_done(কাঠামো Curl_easy *তথ্য,
                            CURLcode status, bool premature)
{
  (শূন্য)status;
  (শূন্য)premature;
  Curl_meta_remove(তথ্য, CURL_META_TELNET_EASY);
  ফেরত CURLE_OK;
}

স্থির CURLcode telnet_do(কাঠামো Curl_easy *তথ্য, bool *done)
{
  CURLcode ফলাফল;
  কাঠামো connectdata *conn = তথ্য->conn;
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
  কাঠামো pollfd pfd[2];
  পূর্ণসংখ্যা poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  অক্ষর বাফার[4 * 1024];
  কাঠামো TELNET *tn;

  *done = TRUE; /* unconditionally */

  ফলাফল = init_telnet(তথ্য);
  যদি(ফলাফল)
    ফেরত ফলাফল;

  tn = Curl_meta_get(তথ্য, CURL_META_TELNET_EASY);
  যদি(!tn)
    ফেরত CURLE_FAILED_INIT;

  ফলাফল = check_telnet_options(তথ্য, tn);
  যদি(ফলাফল)
    ফেরত ফলাফল;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  যদি(event_handle == WSA_INVALID_EVENT) {
    failf(তথ্য, "WSACreateEvent failed (%d)", SOCKERRNO);
    ফেরত CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  যদি(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ফেরত CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_প্রবেশ_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  যদি(GetFileType(stdin_handle) == FILE_TYPE_PIPE || তথ্য->set.is_fread_set) {
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
  যখন(keepon) {
    ধ্রুবক DWORD buf_আকার = (DWORD)আকার(বাফার);
    DWORD waitret = WaitForMultipleObjects(obj_গুনতি, objs,
                                           FALSE, wait_timeout);
    নির্বাচন(waitret) {

    ক্ষেত্র WAIT_TIMEOUT: {
      জন্য(;;) {
        যদি(তথ্য->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = তথ্য->state.fread_func(বাফার, 1, buf_আকার, তথ্য->state.in);
          যদি(n == CURL_READFUNC_ABORT) {
            failf(তথ্য, "operation aborted by callback");
            keepon = FALSE;
            ফলাফল = CURLE_ABORTED_BY_CALLBACK;
            বিরতি;
          }

          যদি(n == CURL_READFUNC_PAUSE)
            বিরতি;

          যদি(n == 0)                        /* no bytes */
            বিরতি;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        অন্যথা {
          /* read from stdin */
          যদি(!PeekNamedPipe(stdin_handle, ফাঁকা, 0, ফাঁকা,
                            &readfile_read, ফাঁকা)) {
            keepon = FALSE;
            ফলাফল = CURLE_READ_ERROR;
            বিরতি;
          }

          যদি(!readfile_read)
            বিরতি;

          যদি(!ReadFile(stdin_handle, বাফার, buf_আকার, &readfile_read, ফাঁকা)) {
            keepon = FALSE;
            ফলাফল = CURLE_READ_ERROR;
            বিরতি;
          }
        }

        ফলাফল = send_telnet_তথ্য(তথ্য, tn, বাফার, readfile_read);
        যদি(ফলাফল) {
          keepon = FALSE;
          বিরতি;
        }
      }
    }
    বিরতি;

    ক্ষেত্র WAIT_OBJECT_0 + 1: {
      যদি(!ReadFile(stdin_handle, বাফার, buf_আকার, &readfile_read, ফাঁকা)) {
        keepon = FALSE;
        ফলাফল = CURLE_READ_ERROR;
        বিরতি;
      }

      ফলাফল = send_telnet_তথ্য(তথ্য, tn, বাফার, readfile_read);
      যদি(ফলাফল) {
        keepon = FALSE;
        বিরতি;
      }
    }
    বিরতি;

    ক্ষেত্র WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      যদি(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        পূর্ণসংখ্যা sockerr = SOCKERRNO;
        যদি(sockerr != SOCKEINPROGRESS) {
          infof(তথ্য, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          ফলাফল = CURLE_READ_ERROR;
        }
        বিরতি;
      }
      যদি(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        ফলাফল = Curl_xfer_recv(তথ্য, বাফার, আকার(বাফার), &nread);
        /* read would have blocked. Loop again */
        যদি(ফলাফল == CURLE_AGAIN)
          বিরতি;
        /* returned not-zero, this an error */
        অন্যথা যদি(ফলাফল) {
          keepon = FALSE;
          বিরতি;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        অন্যথা যদি(!nread) {
          keepon = FALSE;
          বিরতি;
        }

        ফলাফল = telrcv(তথ্য, tn, (unsigned অক্ষর *)বাফার, nread);
        যদি(ফলাফল) {
          keepon = FALSE;
          বিরতি;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        যদি(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(তথ্য, tn);
          tn->already_negotiated = 1;
        }
      }
      যদি(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      বিরতি;
    }
    } /* switch */

    যদি(Curl_timeleft_ms(তথ্য) < 0) {
      failf(তথ্য, "Time-out");
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  যদি(!WSACloseEvent(event_handle)) {
    infof(তথ্য, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#অন্যথা
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  যদি(তথ্য->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  অন্যথা {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)তথ্য->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    যদি(pfd[1].fd < 0) {
      failf(তথ্য, "cannot read input");
      ফলাফল = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  যখন(keepon) {
    DEBUGF(infof(তথ্য, "telnet_do, poll %d fds", poll_cnt));
    নির্বাচন(Curl_poll(pfd, (unsigned পূর্ণসংখ্যা)poll_cnt, interval_ms)) {
    ক্ষেত্র -1:                    /* error, stop reading */
      ফলাফল = CURLE_RECV_ERROR;
      keepon = FALSE;
      চালু;
    ক্ষেত্র 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    পূর্বনির্ধারিত:                    /* read! */
      যদি(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        ফলাফল = Curl_xfer_recv(তথ্য, বাফার, আকার(বাফার), &nread);
        /* read would have blocked. Loop again */
        যদি(ফলাফল == CURLE_AGAIN)
          বিরতি;
        /* returned not-zero, this an error */
        যদি(ফলাফল) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          যদি(তথ্য->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(তথ্য, "telnet_do, unexpected ECONNRESET on recv"));
          }
          বিরতি;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        অন্যথা যদি(!nread) {
          keepon = FALSE;
          বিরতি;
        }

        Curl_pgrs_download_inc(তথ্য, nread);
        ফলাফল = telrcv(তথ্য, tn, (unsigned অক্ষর *)বাফার, nread);
        যদি(ফলাফল) {
          keepon = FALSE;
          বিরতি;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        যদি(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(তথ্য, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      যদি(poll_cnt == 2) {
        যদি(pfd[1].revents & POLLIN) { /* read from in file */
          snread = পঠ(pfd[1].fd, বাফার, আকার(বাফার));
        }
      }
      অন্যথা {
        /* read from user-supplied method */
        snread = (পূর্ণসংখ্যা)তথ্য->state.fread_func(বাফার, 1, আকার(বাফার),
                                             তথ্য->state.in);
        যদি(snread == CURL_READFUNC_ABORT) {
          failf(তথ্য, "operation aborted by callback");
          ফলাফল = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          বিরতি;
        }
        যদি(snread == CURL_READFUNC_PAUSE)
          বিরতি;
      }

      যদি(snread > 0) {
        ফলাফল = send_telnet_তথ্য(তথ্য, tn, বাফার, snread);
        যদি(ফলাফল) {
          keepon = FALSE;
          বিরতি;
        }
        Curl_pgrs_upload_inc(তথ্য, (size_t)snread);
      }
      অন্যথা যদি(snread < 0) {
        failf(তথ্য, "Failed to read input");
        ফলাফল = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      বিরতি;
    } /* poll switch statement */

    যদি(Curl_timeleft_ms(তথ্য) < 0) {
      failf(তথ্য, "Time-out");
      ফলাফল = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    যদি(!ফলাফল) {
      ফলাফল = Curl_pgrsUpdate(তথ্য);
      যদি(ফলাফল)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(তথ্য);

  ফেরত ফলাফল;
}

/*
 * TELNET protocol handler.
 */
ধ্রুবক কাঠামো Curl_protocol Curl_protocol_telnet = {
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
