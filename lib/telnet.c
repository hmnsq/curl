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
#ضمن "curl_setup.h"
#ضمن "urldata.h"
#ضمن "telnet.h"

#ifndef CURL_DISABLE_TELNET

#ifdef HAVE_NETINET_IN_H
#ضمن <netinet/in.h>
#endif
#ifdef HAVE_NETDB_H
#ضمن <netdb.h>
#endif
#ifdef HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#endif
#ifdef HAVE_NET_IF_H
#ضمن <net/if.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#endif

#ifdef HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#endif

#ضمن "url.h"
#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "arpa_telnet.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"

#define SUBBUFSIZE 512

#define CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#define CURL_SB_TERM(x)            \
  𐡏𐡁𐡃 {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } طالما(0)
#define CURL_SB_ACCUM(x, c)                                         \
  𐡏𐡁𐡃 {                                                              \
    𐡄𐡍((x)->subpointer < ((x)->subbuffer + حجم((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } طالما(0)

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
تعريفنوع تعداد {
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

بنية TELNET {
  صحيح please_negotiate;
  صحيح already_negotiated;
  صحيح us[256];
  صحيح usq[256];
  صحيح us_preferred[256];
  صحيح him[256];
  صحيح himq[256];
  صحيح him_preferred[256];
  صحيح subnegotiation[256];
  ثابت حرف *subopt_ttype;          /* Set with suboption TTYPE */
  ثابت حرف *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  unsigned short subopt_wsx;         /* Set with suboption NAWS */
  unsigned short subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  بنية curl_slist *telnet_vars;    /* Environment variables */
  بنية dynbuf out;                 /* output buffer */

  /* suboptions */
  unsigned حرف subbuffer[SUBBUFSIZE];
  unsigned حرف *subpointer, *subend;      /* buffer for sub-options */
};

#ifndef CURLVERBOSE
#define printoption(a, b, c, d) Curl_nop_stmt
#وإلا
ساكن فراغ printoption(بنية Curl_easy *data,
                        ثابت حرف *direction, صحيح cmd, صحيح option)
{
  𐡄𐡍(data->set.verbose) {
    𐡄𐡍(cmd == CURL_IAC) {
      𐡄𐡍(CURL_TELCMD_OK(option))
        infof(data, "%s IAC %s", direction, CURL_TELCMD(option));
      وإلا
        infof(data, "%s IAC %d", direction, option);
    }
    وإلا {
      ثابت حرف *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      𐡄𐡍(fmt) {
        ثابت حرف *opt;
        𐡄𐡍(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        وإلا 𐡄𐡍(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        وإلا
          opt = عدم;

        𐡄𐡍(opt)
          infof(data, "%s %s %s", direction, fmt, opt);
        وإلا
          infof(data, "%s %s %d", direction, fmt, option);
      }
      وإلا
        infof(data, "%s %d %d", direction, cmd, option);
    }
  }
}
#endif /* !CURLVERBOSE */

ساكن فراغ telnet_easy_dtor(ثابت فراغ *key, size_t klen, فراغ *entry)
{
  بنية TELNET *tn = entry;
  (فراغ)key;
  (فراغ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

ساكن CURLcode init_telnet(بنية Curl_easy *data)
{
  بنية TELNET *tn;

  tn = curlx_calloc(1, حجم(بنية TELNET));
  𐡄𐡍(!tn)
    أرجع CURLE_OUT_OF_MEMORY;

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
  tn->us_preferred[CURL_TELOPT_BINARY] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_BINARY] = CURL_YES;

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

  أرجع Curl_meta_set(data, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ساكن فراغ send_negotiation(بنية Curl_easy *data, صحيح cmd, صحيح option)
{
  unsigned حرف buf[3];
  size_t nwritten;
  CURLcode result;

  buf[0] = CURL_IAC;
  buf[1] = (unsigned حرف)cmd;
  buf[2] = (unsigned حرف)option;

  result = Curl_xfer_send(data, buf, 3, FALSE, &nwritten);
  𐡄𐡍(result)
    failf(data, "Sending data failed: %s", curl_easy_strerror(result));

  printoption(data, "SENT", cmd, option);
}

ساكن فراغ set_remote_option(بنية Curl_easy *data, بنية TELNET *tn,
                              صحيح option, صحيح newstate)
{
  𐡄𐡍(newstate == CURL_YES) {
    بدل(tn->him[option]) {
    حالة CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(data, CURL_DO, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        /* Error: already queued an enable request */
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->himq[option]) {
      حالة CURL_EMPTY:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        اكسر;
      }
      اكسر;
    }
  }
  وإلا { /* NO */
    بدل(tn->him[option]) {
    حالة CURL_NO:
      /* Already disabled */
      اكسر;

    حالة CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(data, CURL_DONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_EMPTY:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->himq[option]) {
      حالة CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ set_local_option(بنية Curl_easy *data, بنية TELNET *tn,
                             صحيح option, صحيح newstate)
{
  𐡄𐡍(newstate == CURL_YES) {
    بدل(tn->us[option]) {
    حالة CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(data, CURL_WILL, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        /* Error: already queued an enable request */
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->usq[option]) {
      حالة CURL_EMPTY:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        اكسر;
      }
      اكسر;
    }
  }
  وإلا { /* NO */
    بدل(tn->us[option]) {
    حالة CURL_NO:
      /* Already disabled */
      اكسر;

    حالة CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(data, CURL_WONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_EMPTY:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->usq[option]) {
      حالة CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ telnet_negotiate(بنية Curl_easy *data, بنية TELNET *tn)
{
  صحيح i;

  لكل(i = 0; i < CURL_NTELOPTS; i++) {
    𐡄𐡍(i == CURL_TELOPT_ECHO)
      استمر;

    𐡄𐡍(tn->us_preferred[i] == CURL_YES)
      set_local_option(data, tn, i, CURL_YES);

    𐡄𐡍(tn->him_preferred[i] == CURL_YES)
      set_remote_option(data, tn, i, CURL_YES);
  }
}

ساكن فراغ rec_will(بنية Curl_easy *data, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    𐡄𐡍(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(data, CURL_DO, option);
    }
    وإلا
      send_negotiation(data, CURL_DONT, option);

    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_EMPTY:
      tn->him[option] = CURL_YES;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_wont(بنية Curl_easy *data, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(data, CURL_DONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_EMPTY:
      tn->him[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_DO, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_EMPTY:
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ printsub(بنية Curl_easy *data,
                     صحيح direction,                /* '<' or '>' */
                     ثابت unsigned حرف *pointer, /* ptr to suboption data */
                     size_t length)                /* suboption data length */
{
  𐡄𐡍(data->set.verbose) {
    unsigned صحيح i = 0;
    𐡄𐡍(direction) {
      infof(data, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      𐡄𐡍(length >= 3) {
        صحيح j;

        i = pointer[length - 2];
        j = pointer[length - 1];

        𐡄𐡍(i != CURL_IAC || j != CURL_SE) {
          infof(data, "(terminated by ");
          𐡄𐡍(CURL_TELOPT_OK(i))
            infof(data, "%s ", CURL_TELOPT(i));
          وإلا 𐡄𐡍(CURL_TELCMD_OK(i))
            infof(data, "%s ", CURL_TELCMD(i));
          وإلا
            infof(data, "%u ", i);
          𐡄𐡍(CURL_TELOPT_OK(j))
            infof(data, "%s", CURL_TELOPT(j));
          وإلا 𐡄𐡍(CURL_TELCMD_OK(j))
            infof(data, "%s", CURL_TELCMD(j));
          وإلا
            infof(data, "%d", j);
          infof(data, ", not IAC SE) ");
        }
      }
      𐡄𐡍(length >= 2)
        length -= 2;
      وإلا /* bad input */
        أرجع;
    }
    𐡄𐡍(length <= 1) {
      infof(data, "(Empty suboption?)");
      أرجع;
    }

    𐡄𐡍(CURL_TELOPT_OK(pointer[0])) {
      بدل(pointer[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
      حالة CURL_TELOPT_NEW_ENVIRON:
      حالة CURL_TELOPT_NAWS:
        infof(data, "%s", CURL_TELOPT(pointer[0]));
        اكسر;
      افتراضي:
        infof(data, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        اكسر;
      }
    }
    وإلا
      infof(data, "%d (unknown)", pointer[0]);

    بدل(pointer[0]) {
    حالة CURL_TELOPT_NAWS:
      𐡄𐡍(length > 4)
        infof(data, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      اكسر;
    افتراضي:
      بدل(pointer[1]) {
      حالة CURL_TELQUAL_IS:
        infof(data, " IS");
        اكسر;
      حالة CURL_TELQUAL_SEND:
        infof(data, " SEND");
        اكسر;
      حالة CURL_TELQUAL_INFO:
        infof(data, " INFO/REPLY");
        اكسر;
      حالة CURL_TELQUAL_NAME:
        infof(data, " NAME");
        اكسر;
      }

      بدل(pointer[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
        infof(data, " \"%.*s\"",
              (صحيح)((length > 2) ? (length - 2) : 0), &pointer[2]);
        اكسر;
      حالة CURL_TELOPT_NEW_ENVIRON:
        𐡄𐡍(pointer[1] == CURL_TELQUAL_IS) {
          infof(data, " ");
          لكل(i = 3; i < length; i++) {
            بدل(pointer[i]) {
            حالة CURL_NEW_ENV_VAR:
              infof(data, ", ");
              اكسر;
            حالة CURL_NEW_ENV_VALUE:
              infof(data, " = ");
              اكسر;
            افتراضي:
              infof(data, "%c", pointer[i]);
              اكسر;
            }
          }
        }
        اكسر;
      افتراضي:
        لكل(i = 2; i < length; i++)
          infof(data, " %.2x", pointer[i]);
        اكسر;
      }
    }
  }
}

/* Escape and send a telnet data block */
ساكن CURLcode send_telnet_data(بنية Curl_easy *data,
                                 بنية TELNET *tn,
                                 ثابت حرف *buffer, ssize_t nread)
{
  size_t i, outlen;
  ثابت unsigned حرف *outbuf;
  CURLcode result = CURLE_OK;
  size_t bytes_written;
  size_t total_written = 0;
  بنية connectdata *conn = data->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  𐡄𐡍(nread < 0)
    أرجع CURLE_TOO_LARGE;

  𐡄𐡍(memchr(buffer, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    لكل(i = 0; i < (size_t)nread && !result; i++) {
      result = curlx_dyn_addn(&tn->out, &buffer[i], 1);
      𐡄𐡍(!result && ((unsigned حرف)buffer[i] == CURL_IAC))
        /* IAC is FF in hex */
        result = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  وإلا {
    outlen = (size_t)nread;
    outbuf = (ثابت unsigned حرف *)buffer;
  }
  طالما(!result && total_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    بنية pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(data);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    𐡄𐡍(timeout_ms < 0)
      أرجع CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    بدل(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    حالة -1:                    /* error, abort writing */
      result = CURLE_SEND_ERROR;
      اكسر;
    حالة 0:                     /* timeout */
      result = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    افتراضي:                    /* write! */
      bytes_written = 0;
      result = Curl_xfer_send(data, outbuf + total_written,
                              outlen - total_written, FALSE, &bytes_written);
      total_written += bytes_written;
      اكسر;
    }
  }

  أرجع result;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ساكن فراغ sendsuboption(بنية Curl_easy *data,
                          بنية TELNET *tn, صحيح option)
{
  size_t nwritten;
  unsigned short x, y;
  ثابت unsigned حرف *uc1, *uc2;
  CURLcode result;

  بدل(option) {
  حالة CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ثابت unsigned حرف *)&x;
    uc2 = (ثابت unsigned حرف *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(data, '>', (ثابت unsigned حرف *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    result = Curl_xfer_send(data, tn->subbuffer, 3, FALSE, &nwritten);
    𐡄𐡍(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_data(data, tn, (ثابت حرف *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    result = Curl_xfer_send(data, tn->subbuffer + 7, 2, FALSE, &nwritten);
    𐡄𐡍(result)
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
    اكسر;
  }
}

ساكن فراغ rec_do(بنية Curl_easy *data, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    𐡄𐡍(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      𐡄𐡍(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(data, tn, option);
    }
    وإلا 𐡄𐡍(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(data, CURL_WILL, option);
      sendsuboption(data, tn, option);
    }
    وإلا
      send_negotiation(data, CURL_WONT, option);
    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_EMPTY:
      tn->us[option] = CURL_YES;
      𐡄𐡍(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(data, tn, option);
      }
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_dont(بنية Curl_easy *data, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(data, CURL_WONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_EMPTY:
      tn->us[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(data, CURL_WILL, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_EMPTY:
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      اكسر;
    }
    اكسر;
  }
}

ساكن bool str_is_nonascii(ثابت حرف *str)
{
  حرف c;
  طالما((c = *str++) != 0)
    𐡄𐡍(c & 0x80)
      أرجع TRUE;

  أرجع FALSE;
}

ساكن CURLcode check_telnet_options(بنية Curl_easy *data,
                                     بنية TELNET *tn)
{
  بنية curl_slist *head;
  بنية curl_slist *beg;
  CURLcode result = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  𐡄𐡍(data->state.creds) {
    حرف buffer[256];
    𐡄𐡍(str_is_nonascii(Curl_creds_user(data->conn->creds))) {
      DEBUGF(infof(data, "set a non ASCII username in telnet"));
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(buffer, حجم(buffer), "USER,%s",
                   Curl_creds_user(data->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, buffer);
    𐡄𐡍(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = عدم;
      أرجع CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  لكل(head = data->set.telnet_options; head && !result; head = head->next) {
    size_t olen;
    ثابت حرف *option = head->data;
    ثابت حرف *arg;
    ثابت حرف *sep = strchr(option, '=');
    𐡄𐡍(sep) {
      olen = sep - option;
      arg = ++sep;
      𐡄𐡍(str_is_nonascii(arg))
        استمر;
      بدل(olen) {
      حالة 5:
        /* Terminal type */
        𐡄𐡍(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          اكسر;
        }
        result = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 8:
        /* Display variable */
        𐡄𐡍(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          اكسر;
        }
        result = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 7:
        /* Environment variable */
        𐡄𐡍(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          𐡄𐡍(!beg) {
            result = CURLE_OUT_OF_MEMORY;
            اكسر;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        وإلا
          result = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 2:
        /* Window Size */
        𐡄𐡍(curl_strnequal(option, "WS", 2)) {
          ثابت حرف *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          𐡄𐡍(curlx_str_number(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_number(&p, &y, 0xffff)) {
            failf(data, "Syntax error in telnet option: %s", head->data);
            result = CURLE_SETOPT_OPTION_SYNTAX;
          }
          وإلا {
            tn->subopt_wsx = (unsigned short)x;
            tn->subopt_wsy = (unsigned short)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        وإلا
          result = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 6:
        /* To take care or not of the 8th bit in data exchange */
        𐡄𐡍(curl_strnequal(option, "BINARY", 6)) {
          ثابت حرف *p = arg;
          curl_off_t binary_option;
          𐡄𐡍(!curlx_str_number(&p, &binary_option, 1) &&
             (binary_option != 1)) {
            tn->us_preferred[CURL_TELOPT_BINARY] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_BINARY] = CURL_NO;
          }
        }
        وإلا
          result = CURLE_UNKNOWN_OPTION;
        اكسر;
      افتراضي:
        failf(data, "Unknown telnet option %s", head->data);
        result = CURLE_UNKNOWN_OPTION;
        اكسر;
      }
    }
    وإلا {
      failf(data, "Syntax error in telnet option: %s", head->data);
      result = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  𐡄𐡍(result) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = عدم;
  }

  أرجع result;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ساكن bool bad_option(ثابت حرف *data)
{
  أرجع !data || !!strchr(data, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ساكن CURLcode suboption(بنية Curl_easy *data, بنية TELNET *tn)
{
  بنية curl_slist *v;
  unsigned حرف temp[2048];
  size_t nwritten;
  size_t len;
  CURLcode result = CURLE_OK;

  𐡄𐡍(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    أرجع CURLE_OK;

  printsub(data, '<', (ثابت unsigned حرف *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  بدل(CURL_SB_GET(tn)) {
  حالة CURL_TELOPT_TTYPE:
    𐡄𐡍(bad_option(tn->subopt_ttype))
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    𐡄𐡍(strlen(tn->subopt_ttype) > 1000) {
      failf(data, "Too long telnet TTYPE");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)temp, حجم(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    𐡄𐡍(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      أرجع result;
    }
    printsub(data, '>', &temp[2], len-2);
    اكسر;
  حالة CURL_TELOPT_XDISPLOC:
    𐡄𐡍(bad_option(tn->subopt_xdisploc))
      أرجع CURLE_BAD_FUNCTION_ARGUMENT;
    𐡄𐡍(strlen(tn->subopt_xdisploc) > 1000) {
      failf(data, "Too long telnet XDISPLOC");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)temp, حجم(temp), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    𐡄𐡍(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      أرجع result;
    }
    printsub(data, '>', &temp[2], len - 2);
    اكسر;
  حالة CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((حرف *)temp, حجم(temp), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    لكل(v = tn->telnet_vars; v; v = v->next) {
      size_t tmplen = (strlen(v->data) + 1);
      𐡄𐡍(bad_option(v->data))
        أرجع CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      𐡄𐡍(len + tmplen < حجم(temp) - 6) {
        ثابت حرف *s = strchr(v->data, ',');
        𐡄𐡍(!s)
          len += curl_msnprintf((حرف *)&temp[len], حجم(temp) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->data);
        وإلا {
          size_t vlen = s - v->data;
          len += curl_msnprintf((حرف *)&temp[len], حجم(temp) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (صحيح)vlen, v->data, CURL_NEW_ENV_VALUE, ++s);
        }
      }
    }
    curl_msnprintf((حرف *)&temp[len], حجم(temp) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    result = Curl_xfer_send(data, temp, len, FALSE, &nwritten);
    𐡄𐡍(result) {
      failf(data, "Sending data failed: %s", curl_easy_strerror(result));
      أرجع result;
    }
    printsub(data, '>', &temp[2], len - 2);
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode telrcv(بنية Curl_easy *data,
                       بنية TELNET *tn,
                       ثابت unsigned حرف *inbuf, /* Data received from
                                                      socket */
                       ssize_t count)              /* Number of bytes
                                                      received */
{
  unsigned حرف c;
  CURLcode result;
  صحيح in = 0;
  صحيح startwrite = -1;

#define startskipping()                                          \
  𐡄𐡍(startwrite >= 0) {                                          \
    result = Curl_client_write(data,                             \
                               CLIENTWRITE_BODY,                 \
                               (ثابت حرف *)&inbuf[startwrite], \
                               in-startwrite);                   \
    𐡄𐡍(result)                                                   \
      أرجع result;                                             \
  }                                                              \
  startwrite = -1

#define writebyte()  \
  𐡄𐡍(startwrite < 0) \
    startwrite = in

#define bufferflush() startskipping()

  طالما(count--) {
    c = inbuf[in];

    بدل(tn->telrcv_state) {
    حالة CURL_TS_CR:
      tn->telrcv_state = CURL_TS_DATA;
      𐡄𐡍(c == '\0') {
        startskipping();
        اكسر;   /* Ignore \0 after CR */
      }
      writebyte();
      اكسر;

    حالة CURL_TS_DATA:
      𐡄𐡍(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        اكسر;
      }
      وإلا 𐡄𐡍(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      اكسر;

    حالة CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      بدل(c) {
      حالة CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        اكسر;
      حالة CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        اكسر;
      حالة CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        اكسر;
      حالة CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        اكسر;
      حالة CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        اكسر;
      حالة CURL_IAC:
        tn->telrcv_state = CURL_TS_DATA;
        writebyte();
        اكسر;
      حالة CURL_DM:
      حالة CURL_NOP:
      حالة CURL_GA:
      افتراضي:
        tn->telrcv_state = CURL_TS_DATA;
        printoption(data, "RCVD", CURL_IAC, c);
        اكسر;
      }
      اكسر;

    حالة CURL_TS_WILL:
      printoption(data, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      اكسر;

    حالة CURL_TS_WONT:
      printoption(data, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      اكسر;

    حالة CURL_TS_DO:
      printoption(data, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      اكسر;

    حالة CURL_TS_DONT:
      printoption(data, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(data, tn, c);
      tn->telrcv_state = CURL_TS_DATA;
      اكسر;

    حالة CURL_TS_SB:
      𐡄𐡍(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      وإلا
        CURL_SB_ACCUM(tn, c);
      اكسر;

    حالة CURL_TS_SE:
      𐡄𐡍(c != CURL_SE) {
        𐡄𐡍(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(data, "telnet: suboption error");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      وإلا {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        result = suboption(data, tn);   /* handle sub-option */
        𐡄𐡍(result)
          أرجع result;
        tn->telrcv_state = CURL_TS_DATA;
      }
      اكسر;
    }
    ++in;
  }
  bufferflush();
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_done(بنية Curl_easy *data,
                            CURLcode status, bool premature)
{
  (فراغ)status;
  (فراغ)premature;
  Curl_meta_remove(data, CURL_META_TELNET_EASY);
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_do(بنية Curl_easy *data, bool *done)
{
  CURLcode result;
  بنية connectdata *conn = data->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ifdef USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_count;
  DWORD wait_timeout;
  DWORD readfile_read;
#وإلا
  timediff_t interval_ms;
  بنية pollfd pfd[2];
  صحيح poll_cnt;
  ssize_t snread;
#endif
  bool keepon = TRUE;
  حرف buffer[4 * 1024];
  بنية TELNET *tn;

  *done = TRUE; /* unconditionally */

  result = init_telnet(data);
  𐡄𐡍(result)
    أرجع result;

  tn = Curl_meta_get(data, CURL_META_TELNET_EASY);
  𐡄𐡍(!tn)
    أرجع CURLE_FAILED_INIT;

  result = check_telnet_options(data, tn);
  𐡄𐡍(result)
    أرجع result;

#ifdef USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  𐡄𐡍(event_handle == WSA_INVALID_EVENT) {
    failf(data, "WSACreateEvent failed (%d)", SOCKERRNO);
    أرجع CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  𐡄𐡍(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    أرجع CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_INPUT_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  𐡄𐡍(GetFileType(stdin_handle) == FILE_TYPE_PIPE || data->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_count = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  وإلا {
    obj_count = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  طالما(keepon) {
    ثابت DWORD buf_size = (DWORD)حجم(buffer);
    DWORD waitret = WaitForMultipleObjects(obj_count, objs,
                                           FALSE, wait_timeout);
    بدل(waitret) {

    حالة WAIT_TIMEOUT: {
      لكل(;;) {
        𐡄𐡍(data->set.is_fread_set) {
          size_t n;
          /* read from user-supplied method */
          n = data->state.fread_func(buffer, 1, buf_size, data->state.in);
          𐡄𐡍(n == CURL_READFUNC_ABORT) {
            failf(data, "operation aborted by callback");
            keepon = FALSE;
            result = CURLE_ABORTED_BY_CALLBACK;
            اكسر;
          }

          𐡄𐡍(n == CURL_READFUNC_PAUSE)
            اكسر;

          𐡄𐡍(n == 0)                        /* no bytes */
            اكسر;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        وإلا {
          /* read from stdin */
          𐡄𐡍(!PeekNamedPipe(stdin_handle, عدم, 0, عدم,
                            &readfile_read, عدم)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            اكسر;
          }

          𐡄𐡍(!readfile_read)
            اكسر;

          𐡄𐡍(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, عدم)) {
            keepon = FALSE;
            result = CURLE_READ_ERROR;
            اكسر;
          }
        }

        result = send_telnet_data(data, tn, buffer, readfile_read);
        𐡄𐡍(result) {
          keepon = FALSE;
          اكسر;
        }
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0 + 1: {
      𐡄𐡍(!ReadFile(stdin_handle, buffer, buf_size, &readfile_read, عدم)) {
        keepon = FALSE;
        result = CURLE_READ_ERROR;
        اكسر;
      }

      result = send_telnet_data(data, tn, buffer, readfile_read);
      𐡄𐡍(result) {
        keepon = FALSE;
        اكسر;
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      𐡄𐡍(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        صحيح sockerr = SOCKERRNO;
        𐡄𐡍(sockerr != SOCKEINPROGRESS) {
          infof(data, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          result = CURLE_READ_ERROR;
        }
        اكسر;
      }
      𐡄𐡍(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, حجم(buffer), &nread);
        /* read would have blocked. Loop again */
        𐡄𐡍(result == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        وإلا 𐡄𐡍(result) {
          keepon = FALSE;
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا 𐡄𐡍(!nread) {
          keepon = FALSE;
          اكسر;
        }

        result = telrcv(data, tn, (unsigned حرف *)buffer, nread);
        𐡄𐡍(result) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        𐡄𐡍(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }
      𐡄𐡍(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      اكسر;
    }
    } /* switch */

    𐡄𐡍(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  𐡄𐡍(!WSACloseEvent(event_handle)) {
    infof(data, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#وإلا
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  𐡄𐡍(data->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  وإلا {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)data->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    𐡄𐡍(pfd[1].fd < 0) {
      failf(data, "cannot read input");
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  طالما(keepon) {
    DEBUGF(infof(data, "telnet_do, poll %d fds", poll_cnt));
    بدل(Curl_poll(pfd, (unsigned صحيح)poll_cnt, interval_ms)) {
    حالة -1:                    /* error, stop reading */
      result = CURLE_RECV_ERROR;
      keepon = FALSE;
      استمر;
    حالة 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    افتراضي:                    /* read! */
      𐡄𐡍(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        result = Curl_xfer_recv(data, buffer, حجم(buffer), &nread);
        /* read would have blocked. Loop again */
        𐡄𐡍(result == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        𐡄𐡍(result) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          𐡄𐡍(data->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(data, "telnet_do, unexpected ECONNRESET on recv"));
          }
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا 𐡄𐡍(!nread) {
          keepon = FALSE;
          اكسر;
        }

        Curl_pgrs_download_inc(data, nread);
        result = telrcv(data, tn, (unsigned حرف *)buffer, nread);
        𐡄𐡍(result) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        𐡄𐡍(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(data, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      𐡄𐡍(poll_cnt == 2) {
        𐡄𐡍(pfd[1].revents & POLLIN) { /* read from in file */
          snread = اقرأ(pfd[1].fd, buffer, حجم(buffer));
        }
      }
      وإلا {
        /* read from user-supplied method */
        snread = (صحيح)data->state.fread_func(buffer, 1, حجم(buffer),
                                             data->state.in);
        𐡄𐡍(snread == CURL_READFUNC_ABORT) {
          failf(data, "operation aborted by callback");
          result = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          اكسر;
        }
        𐡄𐡍(snread == CURL_READFUNC_PAUSE)
          اكسر;
      }

      𐡄𐡍(snread > 0) {
        result = send_telnet_data(data, tn, buffer, snread);
        𐡄𐡍(result) {
          keepon = FALSE;
          اكسر;
        }
        Curl_pgrs_upload_inc(data, (size_t)snread);
      }
      وإلا 𐡄𐡍(snread < 0) {
        failf(data, "Failed to read input");
        result = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      اكسر;
    } /* poll switch statement */

    𐡄𐡍(Curl_timeleft_ms(data) < 0) {
      failf(data, "Time-out");
      result = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    𐡄𐡍(!result) {
      result = Curl_pgrsUpdate(data);
      𐡄𐡍(result)
        keepon = FALSE;
    }
  }
#endif
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(data);

  أرجع result;
}

/*
 * TELNET protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_telnet = {
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
