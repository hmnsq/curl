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

#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TELNET

#جيڪڏهن_وصف_ٿيل HAVE_NETINET_اندريون_H
#ضمن <netinet/in.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NETDB_H
#ضمن <netdb.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NET_IF_H
#ضمن <net/if.h>
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جيڪڏهن_پورو

#ضمن "url.h"
#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "arpa_telnet.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"

#وصف_ڪريو SUBBUFSIZE 512

#وصف_ڪريو CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#وصف_ڪريو CURL_SB_رڪن(x)            \
  افعل {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } طالما(0)
#وصف_ڪريو CURL_SB_ACCUM(x, c)                                         \
  افعل {                                                              \
    إذا((x)->subpointer < ((x)->subbuffer + حجم((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } طالما(0)

#وصف_ڪريو CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#وصف_ڪريو CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#وصف_ڪريو CURL_NO          0
#وصف_ڪريو CURL_YES         1
#وصف_ڪريو CURL_WANTYES     2
#وصف_ڪريو CURL_WANTNO      3

#وصف_ڪريو CURL_خالي_جايون       0
#وصف_ڪريو CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#وصف_ڪريو CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
تعريفنوع تعداد {
  CURL_TS_معلومات = 0,
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
  بنانشان ننڍو subopt_wsx;         /* Set with suboption NAWS */
  بنانشان ننڍو subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  بنية curl_slist *telnet_vars;    /* Environment variables */
  بنية dynbuf ٻاهريون;                 /* output buffer */

  /* suboptions */
  بنانشان حرف subbuffer[SUBBUFSIZE];
  بنانشان حرف *subpointer, *subend;      /* buffer for sub-options */
};

#جيڪڏهن_وصف_نه_ٿيل CURLVERBOSE
#وصف_ڪريو printoption(a, b, c, d) Curl_nop_stmt
#وإلا
ساكن فراغ printoption(بنية Curl_easy *معلومات,
                        ثابت حرف *direction, صحيح cmd, صحيح option)
{
  إذا(معلومات->سيٽ.verbose) {
    إذا(cmd == CURL_IAC) {
      إذا(CURL_TELCMD_OK(option))
        infof(معلومات, "%s IAC %s", direction, CURL_TELCMD(option));
      وإلا
        infof(معلومات, "%s IAC %d", direction, option);
    }
    وإلا {
      ثابت حرف *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      إذا(fmt) {
        ثابت حرف *opt;
        إذا(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        وإلا إذا(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        وإلا
          opt = عدم;

        إذا(opt)
          infof(معلومات, "%s %s %s", direction, fmt, opt);
        وإلا
          infof(معلومات, "%s %s %d", direction, fmt, option);
      }
      وإلا
        infof(معلومات, "%s %d %d", direction, cmd, option);
    }
  }
}
#جيڪڏهن_پورو /* !CURLVERBOSE */

ساكن فراغ telnet_easy_dtor(ثابت فراغ *ڪنجي, size_t klen, فراغ *داخلا_2)
{
  بنية TELNET *tn = داخلا_2;
  (فراغ)ڪنجي;
  (فراغ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->ٻاهريون);
  curlx_free(tn);
}

ساكن CURLcode init_telnet(بنية Curl_easy *معلومات)
{
  بنية TELNET *tn;

  tn = curlx_calloc(1, حجم(بنية TELNET));
  إذا(!tn)
    أرجع CURLE_ٻاهريون_جو_يادگيري;

  curlx_dyn_init(&tn->ٻاهريون, 0xffff);

  tn->telrcv_state = CURL_TS_معلومات;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_ٻٽو] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ٻٽو] = CURL_YES;

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

  أرجع Curl_meta_سيٽ(معلومات, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ساكن فراغ send_negotiation(بنية Curl_easy *معلومات, صحيح cmd, صحيح option)
{
  بنانشان حرف buf[3];
  size_t nwritten;
  CURLcode نتيجو;

  buf[0] = CURL_IAC;
  buf[1] = (بنانشان حرف)cmd;
  buf[2] = (بنانشان حرف)option;

  نتيجو = Curl_xfer_send(معلومات, buf, 3, FALSE, &nwritten);
  إذا(نتيجو)
    failf(معلومات, "Sending data failed: %s", curl_easy_strerror(نتيجو));

  printoption(معلومات, "SENT", cmd, option);
}

ساكن فراغ سيٽ_remote_option(بنية Curl_easy *معلومات, بنية TELNET *tn,
                              صحيح option, صحيح newstate)
{
  إذا(newstate == CURL_YES) {
    بدل(tn->him[option]) {
    حالة CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(معلومات, CURL_DO, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_خالي_جايون:
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
      حالة CURL_خالي_جايون:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_خالي_جايون;
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
      send_negotiation(معلومات, CURL_DONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_خالي_جايون:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_خالي_جايون;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->himq[option]) {
      حالة CURL_خالي_جايون:
        tn->himq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ سيٽ_local_option(بنية Curl_easy *معلومات, بنية TELNET *tn,
                             صحيح option, صحيح newstate)
{
  إذا(newstate == CURL_YES) {
    بدل(tn->us[option]) {
    حالة CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(معلومات, CURL_WILL, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_خالي_جايون:
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
      حالة CURL_خالي_جايون:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_خالي_جايون;
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
      send_negotiation(معلومات, CURL_WONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_خالي_جايون:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_خالي_جايون;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->usq[option]) {
      حالة CURL_خالي_جايون:
        tn->usq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ telnet_negotiate(بنية Curl_easy *معلومات, بنية TELNET *tn)
{
  صحيح i;

  لكل(i = 0; i < CURL_NTELOPTS; i++) {
    إذا(i == CURL_TELOPT_ECHO)
      استمر;

    إذا(tn->us_preferred[i] == CURL_YES)
      سيٽ_local_option(معلومات, tn, i, CURL_YES);

    إذا(tn->him_preferred[i] == CURL_YES)
      سيٽ_remote_option(معلومات, tn, i, CURL_YES);
  }
}

ساكن فراغ rec_will(بنية Curl_easy *معلومات, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    إذا(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(معلومات, CURL_DO, option);
    }
    وإلا
      send_negotiation(معلومات, CURL_DONT, option);

    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_خالي_جايون:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_خالي_جايون;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_خالي_جايون:
      tn->him[option] = CURL_YES;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_خالي_جايون;
      send_negotiation(معلومات, CURL_DONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_wont(بنية Curl_easy *معلومات, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(معلومات, CURL_DONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_خالي_جايون:
      tn->him[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_خالي_جايون;
      send_negotiation(معلومات, CURL_DO, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_خالي_جايون:
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_خالي_جايون;
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ printsub(بنية Curl_easy *معلومات,
                     صحيح direction,                /* '<' or '>' */
                     ثابت بنانشان حرف *اشارو, /* ptr to suboption data */
                     size_t ڊيگهائي)                /* suboption data length */
{
  إذا(معلومات->سيٽ.verbose) {
    بنانشان صحيح i = 0;
    إذا(direction) {
      infof(معلومات, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      إذا(ڊيگهائي >= 3) {
        صحيح j;

        i = اشارو[ڊيگهائي - 2];
        j = اشارو[ڊيگهائي - 1];

        إذا(i != CURL_IAC || j != CURL_SE) {
          infof(معلومات, "(terminated by ");
          إذا(CURL_TELOPT_OK(i))
            infof(معلومات, "%s ", CURL_TELOPT(i));
          وإلا إذا(CURL_TELCMD_OK(i))
            infof(معلومات, "%s ", CURL_TELCMD(i));
          وإلا
            infof(معلومات, "%u ", i);
          إذا(CURL_TELOPT_OK(j))
            infof(معلومات, "%s", CURL_TELOPT(j));
          وإلا إذا(CURL_TELCMD_OK(j))
            infof(معلومات, "%s", CURL_TELCMD(j));
          وإلا
            infof(معلومات, "%d", j);
          infof(معلومات, ", not IAC SE) ");
        }
      }
      إذا(ڊيگهائي >= 2)
        ڊيگهائي -= 2;
      وإلا /* bad input */
        أرجع;
    }
    إذا(ڊيگهائي <= 1) {
      infof(معلومات, "(Empty suboption?)");
      أرجع;
    }

    إذا(CURL_TELOPT_OK(اشارو[0])) {
      بدل(اشارو[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
      حالة CURL_TELOPT_NEW_ENVIRON:
      حالة CURL_TELOPT_NAWS:
        infof(معلومات, "%s", CURL_TELOPT(اشارو[0]));
        اكسر;
      افتراضي:
        infof(معلومات, "%s (unsupported)", CURL_TELOPT(اشارو[0]));
        اكسر;
      }
    }
    وإلا
      infof(معلومات, "%d (unknown)", اشارو[0]);

    بدل(اشارو[0]) {
    حالة CURL_TELOPT_NAWS:
      إذا(ڊيگهائي > 4)
        infof(معلومات, "Width: %d ; Height: %d", (اشارو[1] << 8) | اشارو[2],
              (اشارو[3] << 8) | اشارو[4]);
      اكسر;
    افتراضي:
      بدل(اشارو[1]) {
      حالة CURL_TELQUAL_IS:
        infof(معلومات, " IS");
        اكسر;
      حالة CURL_TELQUAL_SEND:
        infof(معلومات, " SEND");
        اكسر;
      حالة CURL_TELQUAL_INFO:
        infof(معلومات, " INFO/REPLY");
        اكسر;
      حالة CURL_TELQUAL_NAME:
        infof(معلومات, " NAME");
        اكسر;
      }

      بدل(اشارو[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
        infof(معلومات, " \"%.*s\"",
              (صحيح)((ڊيگهائي > 2) ? (ڊيگهائي - 2) : 0), &اشارو[2]);
        اكسر;
      حالة CURL_TELOPT_NEW_ENVIRON:
        إذا(اشارو[1] == CURL_TELQUAL_IS) {
          infof(معلومات, " ");
          لكل(i = 3; i < ڊيگهائي; i++) {
            بدل(اشارو[i]) {
            حالة CURL_NEW_ENV_VAR:
              infof(معلومات, ", ");
              اكسر;
            حالة CURL_NEW_ENV_قدر:
              infof(معلومات, " = ");
              اكسر;
            افتراضي:
              infof(معلومات, "%c", اشارو[i]);
              اكسر;
            }
          }
        }
        اكسر;
      افتراضي:
        لكل(i = 2; i < ڊيگهائي; i++)
          infof(معلومات, " %.2x", اشارو[i]);
        اكسر;
      }
    }
  }
}

/* Escape and send a telnet data block */
ساكن CURLcode send_telnet_معلومات(بنية Curl_easy *معلومات,
                                 بنية TELNET *tn,
                                 ثابت حرف *بفر, ssize_t nread)
{
  size_t i, outlen;
  ثابت بنانشان حرف *outbuf;
  CURLcode نتيجو = CURLE_OK;
  size_t بائيٽون_written;
  size_t ڪل_written = 0;
  بنية connectdata *conn = معلومات->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  إذا(nread < 0)
    أرجع CURLE_TOO_LARGE;

  إذا(memchr(بفر, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->ٻاهريون);

    لكل(i = 0; i < (size_t)nread && !نتيجو; i++) {
      نتيجو = curlx_dyn_addn(&tn->ٻاهريون, &بفر[i], 1);
      إذا(!نتيجو && ((بنانشان حرف)بفر[i] == CURL_IAC))
        /* IAC is FF in hex */
        نتيجو = curlx_dyn_addn(&tn->ٻاهريون, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->ٻاهريون);
    outbuf = curlx_dyn_uptr(&tn->ٻاهريون);
  }
  وإلا {
    outlen = (size_t)nread;
    outbuf = (ثابت بنانشان حرف *)بفر;
  }
  طالما(!نتيجو && ڪل_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    بنية pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(معلومات);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    إذا(timeout_ms < 0)
      أرجع CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    بدل(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    حالة -1:                    /* error, abort writing */
      نتيجو = CURLE_SEND_ERROR;
      اكسر;
    حالة 0:                     /* timeout */
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    افتراضي:                    /* write! */
      بائيٽون_written = 0;
      نتيجو = Curl_xfer_send(معلومات, outbuf + ڪل_written,
                              outlen - ڪل_written, FALSE, &بائيٽون_written);
      ڪل_written += بائيٽون_written;
      اكسر;
    }
  }

  أرجع نتيجو;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ساكن فراغ sendsuboption(بنية Curl_easy *معلومات,
                          بنية TELNET *tn, صحيح option)
{
  size_t nwritten;
  بنانشان ننڍو x, y;
  ثابت بنانشان حرف *uc1, *uc2;
  CURLcode نتيجو;

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
    uc1 = (ثابت بنانشان حرف *)&x;
    uc2 = (ثابت بنانشان حرف *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_رڪن(tn);
    /* data suboption is now ready */

    printsub(معلومات, '>', (ثابت بنانشان حرف *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    نتيجو = Curl_xfer_send(معلومات, tn->subbuffer, 3, FALSE, &nwritten);
    إذا(نتيجو)
      failf(معلومات, "Sending data failed: %s", curl_easy_strerror(نتيجو));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_معلومات(معلومات, tn, (ثابت حرف *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    نتيجو = Curl_xfer_send(معلومات, tn->subbuffer + 7, 2, FALSE, &nwritten);
    إذا(نتيجو)
      failf(معلومات, "Sending data failed: %s", curl_easy_strerror(نتيجو));
    اكسر;
  }
}

ساكن فراغ rec_do(بنية Curl_easy *معلومات, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    إذا(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(معلومات, CURL_WILL, option);
      إذا(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(معلومات, tn, option);
    }
    وإلا إذا(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(معلومات, CURL_WILL, option);
      sendsuboption(معلومات, tn, option);
    }
    وإلا
      send_negotiation(معلومات, CURL_WONT, option);
    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_خالي_جايون:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_خالي_جايون;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_خالي_جايون:
      tn->us[option] = CURL_YES;
      إذا(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(معلومات, tn, option);
      }
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_خالي_جايون;
      send_negotiation(معلومات, CURL_WONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_dont(بنية Curl_easy *معلومات, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(معلومات, CURL_WONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_خالي_جايون:
      tn->us[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_خالي_جايون;
      send_negotiation(معلومات, CURL_WILL, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_خالي_جايون:
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_خالي_جايون;
      اكسر;
    }
    اكسر;
  }
}

ساكن bool str_is_nonascii(ثابت حرف *str)
{
  حرف c;
  طالما((c = *str++) != 0)
    إذا(c & 0x80)
      أرجع TRUE;

  أرجع FALSE;
}

ساكن CURLcode check_telnet_options(بنية Curl_easy *معلومات,
                                     بنية TELNET *tn)
{
  بنية curl_slist *سر;
  بنية curl_slist *beg;
  CURLcode نتيجو = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  إذا(معلومات->state.creds) {
    حرف بفر[256];
    إذا(str_is_nonascii(Curl_creds_user(معلومات->conn->creds))) {
      DEBUGF(infof(معلومات, "set a non ASCII username in telnet"));
      أرجع CURLE_BAD_FUNCTION_دليل;
    }
    curl_msnprintf(بفر, حجم(بفر), "USER,%s",
                   Curl_creds_user(معلومات->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, بفر);
    إذا(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = عدم;
      أرجع CURLE_ٻاهريون_جو_يادگيري;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  لكل(سر = معلومات->سيٽ.telnet_options; سر && !نتيجو; سر = سر->ايندڙ) {
    size_t olen;
    ثابت حرف *option = سر->معلومات;
    ثابت حرف *arg;
    ثابت حرف *sep = strchr(option, '=');
    إذا(sep) {
      olen = sep - option;
      arg = ++sep;
      إذا(str_is_nonascii(arg))
        استمر;
      بدل(olen) {
      حالة 5:
        /* Terminal type */
        إذا(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          اكسر;
        }
        نتيجو = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 8:
        /* Display variable */
        إذا(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          اكسر;
        }
        نتيجو = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 7:
        /* Environment variable */
        إذا(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          إذا(!beg) {
            نتيجو = CURLE_ٻاهريون_جو_يادگيري;
            اكسر;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        وإلا
          نتيجو = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 2:
        /* Window Size */
        إذا(curl_strnequal(option, "WS", 2)) {
          ثابت حرف *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          إذا(curlx_str_عدد(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_عدد(&p, &y, 0xffff)) {
            failf(معلومات, "Syntax error in telnet option: %s", سر->معلومات);
            نتيجو = CURLE_SETOPT_OPTION_SYNTAX;
          }
          وإلا {
            tn->subopt_wsx = (بنانشان ننڍو)x;
            tn->subopt_wsy = (بنانشان ننڍو)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        وإلا
          نتيجو = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 6:
        /* To take care or not of the 8th bit in data exchange */
        إذا(curl_strnequal(option, "BINARY", 6)) {
          ثابت حرف *p = arg;
          curl_off_t ٻٽو_option;
          إذا(!curlx_str_عدد(&p, &ٻٽو_option, 1) &&
             (ٻٽو_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ٻٽو] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ٻٽو] = CURL_NO;
          }
        }
        وإلا
          نتيجو = CURLE_UNKNOWN_OPTION;
        اكسر;
      افتراضي:
        failf(معلومات, "Unknown telnet option %s", سر->معلومات);
        نتيجو = CURLE_UNKNOWN_OPTION;
        اكسر;
      }
    }
    وإلا {
      failf(معلومات, "Syntax error in telnet option: %s", سر->معلومات);
      نتيجو = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  إذا(نتيجو) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = عدم;
  }

  أرجع نتيجو;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ساكن bool bad_option(ثابت حرف *معلومات)
{
  أرجع !معلومات || !!strchr(معلومات, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ساكن CURLcode suboption(بنية Curl_easy *معلومات, بنية TELNET *tn)
{
  بنية curl_slist *v;
  بنانشان حرف عارضي[2048];
  size_t nwritten;
  size_t len;
  CURLcode نتيجو = CURLE_OK;

  إذا(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    أرجع CURLE_OK;

  printsub(معلومات, '<', (ثابت بنانشان حرف *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  بدل(CURL_SB_GET(tn)) {
  حالة CURL_TELOPT_TTYPE:
    إذا(bad_option(tn->subopt_ttype))
      أرجع CURLE_BAD_FUNCTION_دليل;
    إذا(لڙي_ڊيگهه(tn->subopt_ttype) > 1000) {
      failf(معلومات, "Too long telnet TTYPE");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)عارضي, حجم(عارضي), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    نتيجو = Curl_xfer_send(معلومات, عارضي, len, FALSE, &nwritten);
    إذا(نتيجو) {
      failf(معلومات, "Sending data failed: %s", curl_easy_strerror(نتيجو));
      أرجع نتيجو;
    }
    printsub(معلومات, '>', &عارضي[2], len-2);
    اكسر;
  حالة CURL_TELOPT_XDISPLOC:
    إذا(bad_option(tn->subopt_xdisploc))
      أرجع CURLE_BAD_FUNCTION_دليل;
    إذا(لڙي_ڊيگهه(tn->subopt_xdisploc) > 1000) {
      failf(معلومات, "Too long telnet XDISPLOC");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)عارضي, حجم(عارضي), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    نتيجو = Curl_xfer_send(معلومات, عارضي, len, FALSE, &nwritten);
    إذا(نتيجو) {
      failf(معلومات, "Sending data failed: %s", curl_easy_strerror(نتيجو));
      أرجع نتيجو;
    }
    printsub(معلومات, '>', &عارضي[2], len - 2);
    اكسر;
  حالة CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((حرف *)عارضي, حجم(عارضي), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    لكل(v = tn->telnet_vars; v; v = v->ايندڙ) {
      size_t tmplen = (لڙي_ڊيگهه(v->معلومات) + 1);
      إذا(bad_option(v->معلومات))
        أرجع CURLE_BAD_FUNCTION_دليل;
      /* Add the variable if it fits */
      إذا(len + tmplen < حجم(عارضي) - 6) {
        ثابت حرف *s = strchr(v->معلومات, ',');
        إذا(!s)
          len += curl_msnprintf((حرف *)&عارضي[len], حجم(عارضي) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->معلومات);
        وإلا {
          size_t vlen = s - v->معلومات;
          len += curl_msnprintf((حرف *)&عارضي[len], حجم(عارضي) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (صحيح)vlen, v->معلومات, CURL_NEW_ENV_قدر, ++s);
        }
      }
    }
    curl_msnprintf((حرف *)&عارضي[len], حجم(عارضي) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    نتيجو = Curl_xfer_send(معلومات, عارضي, len, FALSE, &nwritten);
    إذا(نتيجو) {
      failf(معلومات, "Sending data failed: %s", curl_easy_strerror(نتيجو));
      أرجع نتيجو;
    }
    printsub(معلومات, '>', &عارضي[2], len - 2);
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode telrcv(بنية Curl_easy *معلومات,
                       بنية TELNET *tn,
                       ثابت بنانشان حرف *inbuf, /* Data received from
                                                      socket */
                       ssize_t ڳڻپ)              /* Number of bytes
                                                      received */
{
  بنانشان حرف c;
  CURLcode نتيجو;
  صحيح اندريون = 0;
  صحيح startwrite = -1;

#وصف_ڪريو startskipping()                                          \
  إذا(startwrite >= 0) {                                          \
    نتيجو = Curl_client_write(معلومات,                             \
                               CLIENTWRITE_BODY,                 \
                               (ثابت حرف *)&inbuf[startwrite], \
                               اندريون-startwrite);                   \
    إذا(نتيجو)                                                   \
      أرجع نتيجو;                                             \
  }                                                              \
  startwrite = -1

#وصف_ڪريو writebyte()  \
  إذا(startwrite < 0) \
    startwrite = اندريون

#وصف_ڪريو bufferflush() startskipping()

  طالما(ڳڻپ--) {
    c = inbuf[اندريون];

    بدل(tn->telrcv_state) {
    حالة CURL_TS_CR:
      tn->telrcv_state = CURL_TS_معلومات;
      إذا(c == '\0') {
        startskipping();
        اكسر;   /* Ignore \0 after CR */
      }
      writebyte();
      اكسر;

    حالة CURL_TS_معلومات:
      إذا(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        اكسر;
      }
      وإلا إذا(c == '\r')
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
        tn->telrcv_state = CURL_TS_معلومات;
        writebyte();
        اكسر;
      حالة CURL_DM:
      حالة CURL_NOP:
      حالة CURL_GA:
      افتراضي:
        tn->telrcv_state = CURL_TS_معلومات;
        printoption(معلومات, "RCVD", CURL_IAC, c);
        اكسر;
      }
      اكسر;

    حالة CURL_TS_WILL:
      printoption(معلومات, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(معلومات, tn, c);
      tn->telrcv_state = CURL_TS_معلومات;
      اكسر;

    حالة CURL_TS_WONT:
      printoption(معلومات, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(معلومات, tn, c);
      tn->telrcv_state = CURL_TS_معلومات;
      اكسر;

    حالة CURL_TS_DO:
      printoption(معلومات, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(معلومات, tn, c);
      tn->telrcv_state = CURL_TS_معلومات;
      اكسر;

    حالة CURL_TS_DONT:
      printoption(معلومات, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(معلومات, tn, c);
      tn->telrcv_state = CURL_TS_معلومات;
      اكسر;

    حالة CURL_TS_SB:
      إذا(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      وإلا
        CURL_SB_ACCUM(tn, c);
      اكسر;

    حالة CURL_TS_SE:
      إذا(c != CURL_SE) {
        إذا(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(معلومات, "telnet: suboption error");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      وإلا {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_رڪن(tn);
        نتيجو = suboption(معلومات, tn);   /* handle sub-option */
        إذا(نتيجو)
          أرجع نتيجو;
        tn->telrcv_state = CURL_TS_معلومات;
      }
      اكسر;
    }
    ++اندريون;
  }
  bufferflush();
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_پورو(بنية Curl_easy *معلومات,
                            CURLcode حالت, bool premature)
{
  (فراغ)حالت;
  (فراغ)premature;
  Curl_meta_remove(معلومات, CURL_META_TELNET_EASY);
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_do(بنية Curl_easy *معلومات, bool *پورو)
{
  CURLcode نتيجو;
  بنية connectdata *conn = معلومات->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#جيڪڏهن_وصف_ٿيل USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_ڳڻپ;
  DWORD wait_timeout;
  DWORD readfile_read;
#وإلا
  timediff_t interval_ms;
  بنية pollfd pfd[2];
  صحيح poll_cnt;
  ssize_t snread;
#جيڪڏهن_پورو
  bool keepon = TRUE;
  حرف بفر[4 * 1024];
  بنية TELNET *tn;

  *پورو = TRUE; /* unconditionally */

  نتيجو = init_telnet(معلومات);
  إذا(نتيجو)
    أرجع نتيجو;

  tn = Curl_meta_get(معلومات, CURL_META_TELNET_EASY);
  إذا(!tn)
    أرجع CURLE_FAILED_INIT;

  نتيجو = check_telnet_options(معلومات, tn);
  إذا(نتيجو)
    أرجع نتيجو;

#جيڪڏهن_وصف_ٿيل USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  إذا(event_handle == WSA_INVALID_EVENT) {
    failf(معلومات, "WSACreateEvent failed (%d)", SOCKERRNO);
    أرجع CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  إذا(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    أرجع CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_داخلا_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  إذا(GetFileType(stdin_handle) == FILE_TYPE_PIPE || معلومات->سيٽ.is_fread_سيٽ) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_ڳڻپ = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  وإلا {
    obj_ڳڻپ = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  طالما(keepon) {
    ثابت DWORD buf_سائيز = (DWORD)حجم(بفر);
    DWORD waitret = Wait_For_گڻو_Objects(obj_ڳڻپ, objs,
                                           FALSE, wait_timeout);
    بدل(waitret) {

    حالة WAIT_TIMEOUT: {
      لكل(;;) {
        إذا(معلومات->سيٽ.is_fread_سيٽ) {
          size_t n;
          /* read from user-supplied method */
          n = معلومات->state.fread_func(بفر, 1, buf_سائيز, معلومات->state.اندريون);
          إذا(n == CURL_READFUNC_ABORT) {
            failf(معلومات, "operation aborted by callback");
            keepon = FALSE;
            نتيجو = CURLE_ABORTED_BY_CALLBACK;
            اكسر;
          }

          إذا(n == CURL_READFUNC_PAUSE)
            اكسر;

          إذا(n == 0)                        /* no bytes */
            اكسر;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        وإلا {
          /* read from stdin */
          إذا(!PeekNamedPipe(stdin_handle, عدم, 0, عدم,
                            &readfile_read, عدم)) {
            keepon = FALSE;
            نتيجو = CURLE_READ_ERROR;
            اكسر;
          }

          إذا(!readfile_read)
            اكسر;

          إذا(!ReadFile(stdin_handle, بفر, buf_سائيز, &readfile_read, عدم)) {
            keepon = FALSE;
            نتيجو = CURLE_READ_ERROR;
            اكسر;
          }
        }

        نتيجو = send_telnet_معلومات(معلومات, tn, بفر, readfile_read);
        إذا(نتيجو) {
          keepon = FALSE;
          اكسر;
        }
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0 + 1: {
      إذا(!ReadFile(stdin_handle, بفر, buf_سائيز, &readfile_read, عدم)) {
        keepon = FALSE;
        نتيجو = CURLE_READ_ERROR;
        اكسر;
      }

      نتيجو = send_telnet_معلومات(معلومات, tn, بفر, readfile_read);
      إذا(نتيجو) {
        keepon = FALSE;
        اكسر;
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0: {
      events.l_ڄار_Events = 0;
      إذا(WSAEnum_ڄار_Events(sockfd, event_handle, &events) != 0) {
        صحيح sockerr = SOCKERRNO;
        إذا(sockerr != SOCKEINPROGRESS) {
          infof(معلومات, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          نتيجو = CURLE_READ_ERROR;
        }
        اكسر;
      }
      إذا(events.l_ڄار_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        نتيجو = Curl_xfer_recv(معلومات, بفر, حجم(بفر), &nread);
        /* read would have blocked. Loop again */
        إذا(نتيجو == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        وإلا إذا(نتيجو) {
          keepon = FALSE;
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا إذا(!nread) {
          keepon = FALSE;
          اكسر;
        }

        نتيجو = telrcv(معلومات, tn, (بنانشان حرف *)بفر, nread);
        إذا(نتيجو) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        إذا(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(معلومات, tn);
          tn->already_negotiated = 1;
        }
      }
      إذا(events.l_ڄار_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      اكسر;
    }
    } /* switch */

    إذا(Curl_timeleft_ms(معلومات) < 0) {
      failf(معلومات, "Time-out");
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  إذا(!WSACloseEvent(event_handle)) {
    infof(معلومات, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#وإلا
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  إذا(معلومات->سيٽ.is_fread_سيٽ) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  وإلا {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)معلومات->state.اندريون);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    إذا(pfd[1].fd < 0) {
      failf(معلومات, "cannot read input");
      نتيجو = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  طالما(keepon) {
    DEBUGF(infof(معلومات, "telnet_do, poll %d fds", poll_cnt));
    بدل(Curl_poll(pfd, (بنانشان صحيح)poll_cnt, interval_ms)) {
    حالة -1:                    /* error, stop reading */
      نتيجو = CURLE_RECV_ERROR;
      keepon = FALSE;
      استمر;
    حالة 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    افتراضي:                    /* read! */
      إذا(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        نتيجو = Curl_xfer_recv(معلومات, بفر, حجم(بفر), &nread);
        /* read would have blocked. Loop again */
        إذا(نتيجو == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        إذا(نتيجو) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          إذا(معلومات->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(معلومات, "telnet_do, unexpected ECONNRESET on recv"));
          }
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا إذا(!nread) {
          keepon = FALSE;
          اكسر;
        }

        Curl_pgrs_download_inc(معلومات, nread);
        نتيجو = telrcv(معلومات, tn, (بنانشان حرف *)بفر, nread);
        إذا(نتيجو) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        إذا(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(معلومات, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      إذا(poll_cnt == 2) {
        إذا(pfd[1].revents & POLLIN) { /* read from in file */
          snread = اقرأ(pfd[1].fd, بفر, حجم(بفر));
        }
      }
      وإلا {
        /* read from user-supplied method */
        snread = (صحيح)معلومات->state.fread_func(بفر, 1, حجم(بفر),
                                             معلومات->state.اندريون);
        إذا(snread == CURL_READFUNC_ABORT) {
          failf(معلومات, "operation aborted by callback");
          نتيجو = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          اكسر;
        }
        إذا(snread == CURL_READFUNC_PAUSE)
          اكسر;
      }

      إذا(snread > 0) {
        نتيجو = send_telnet_معلومات(معلومات, tn, بفر, snread);
        إذا(نتيجو) {
          keepon = FALSE;
          اكسر;
        }
        Curl_pgrs_upload_inc(معلومات, (size_t)snread);
      }
      وإلا إذا(snread < 0) {
        failf(معلومات, "Failed to read input");
        نتيجو = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      اكسر;
    } /* poll switch statement */

    إذا(Curl_timeleft_ms(معلومات) < 0) {
      failf(معلومات, "Time-out");
      نتيجو = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    إذا(!نتيجو) {
      نتيجو = Curl_pgrsUpdate(معلومات);
      إذا(نتيجو)
        keepon = FALSE;
    }
  }
#جيڪڏهن_پورو
  /* mark this as "no further transfer wanted" */
  Curl_xfer_تياري_nop(معلومات);

  أرجع نتيجو;
}

/*
 * TELNET protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_پورو,                          /* done */
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

#جيڪڏهن_پورو /* !CURL_DISABLE_TELNET */
