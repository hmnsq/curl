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

#اگر_تعریف_نشده CURL_DISABLE_TELNET

#اگر_تعریف_شده HAVE_NETINET_درون_H
#ضمن <netinet/in.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NETDB_H
#ضمن <netdb.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_NET_IF_H
#ضمن <net/if.h>
#پایان_اگر
#اگر_تعریف_شده HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#پایان_اگر

#اگر_تعریف_شده HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#پایان_اگر

#ضمن "url.h"
#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "arpa_telnet.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"

#تعریف_کن SUBBUFSIZE 512

#تعریف_کن CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#تعریف_کن CURL_SB_جمله(x)            \
  افعل {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } طالما(0)
#تعریف_کن CURL_SB_ACCUM(x, c)                                         \
  افعل {                                                              \
    إذا((x)->subpointer < ((x)->subbuffer + حجم((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } طالما(0)

#تعریف_کن CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#تعریف_کن CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#تعریف_کن CURL_NO          0
#تعریف_کن CURL_YES         1
#تعریف_کن CURL_WANTYES     2
#تعریف_کن CURL_WANTNO      3

#تعریف_کن CURL_خالی       0
#تعریف_کن CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#تعریف_کن CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
تعريفنوع تعداد {
  CURL_TS_داده = 0,
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
  بیعلامت کوتاه subopt_wsx;         /* Set with suboption NAWS */
  بیعلامت کوتاه subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  بنية curl_slist *telnet_vars;    /* Environment variables */
  بنية dynbuf بیرون;                 /* output buffer */

  /* suboptions */
  بیعلامت حرف subbuffer[SUBBUFSIZE];
  بیعلامت حرف *subpointer, *subend;      /* buffer for sub-options */
};

#اگر_تعریف_نشده CURLVERBOSE
#تعریف_کن printoption(a, b, c, d) Curl_nop_stmt
#وإلا
ساكن فراغ printoption(بنية Curl_easy *داده,
                        ثابت حرف *direction, صحيح cmd, صحيح option)
{
  إذا(داده->مجموعه.verbose) {
    إذا(cmd == CURL_IAC) {
      إذا(CURL_TELCMD_OK(option))
        infof(داده, "%s IAC %s", direction, CURL_TELCMD(option));
      وإلا
        infof(داده, "%s IAC %d", direction, option);
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
          infof(داده, "%s %s %s", direction, fmt, opt);
        وإلا
          infof(داده, "%s %s %d", direction, fmt, option);
      }
      وإلا
        infof(داده, "%s %d %d", direction, cmd, option);
    }
  }
}
#پایان_اگر /* !CURLVERBOSE */

ساكن فراغ telnet_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *مدخل)
{
  بنية TELNET *tn = مدخل;
  (فراغ)کلید;
  (فراغ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->بیرون);
  curlx_free(tn);
}

ساكن CURLcode init_telnet(بنية Curl_easy *داده)
{
  بنية TELNET *tn;

  tn = curlx_calloc(1, حجم(بنية TELNET));
  إذا(!tn)
    أرجع CURLE_بیرون_از_حافظه;

  curlx_dyn_init(&tn->بیرون, 0xffff);

  tn->telrcv_state = CURL_TS_داده;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_دودویی] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_دودویی] = CURL_YES;

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

  أرجع Curl_meta_مجموعه(داده, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ساكن فراغ send_negotiation(بنية Curl_easy *داده, صحيح cmd, صحيح option)
{
  بیعلامت حرف buf[3];
  size_t nwritten;
  CURLcode نتیجه;

  buf[0] = CURL_IAC;
  buf[1] = (بیعلامت حرف)cmd;
  buf[2] = (بیعلامت حرف)option;

  نتیجه = Curl_xfer_send(داده, buf, 3, FALSE, &nwritten);
  إذا(نتیجه)
    failf(داده, "Sending data failed: %s", curl_easy_strerror(نتیجه));

  printoption(داده, "SENT", cmd, option);
}

ساكن فراغ مجموعه_remote_option(بنية Curl_easy *داده, بنية TELNET *tn,
                              صحيح option, صحيح newstate)
{
  إذا(newstate == CURL_YES) {
    بدل(tn->him[option]) {
    حالة CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(داده, CURL_DO, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_خالی:
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
      حالة CURL_خالی:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_خالی;
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
      send_negotiation(داده, CURL_DONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_خالی:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_خالی;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->himq[option]) {
      حالة CURL_خالی:
        tn->himq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ مجموعه_local_option(بنية Curl_easy *داده, بنية TELNET *tn,
                             صحيح option, صحيح newstate)
{
  إذا(newstate == CURL_YES) {
    بدل(tn->us[option]) {
    حالة CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(داده, CURL_WILL, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_خالی:
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
      حالة CURL_خالی:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_خالی;
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
      send_negotiation(داده, CURL_WONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_خالی:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_خالی;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->usq[option]) {
      حالة CURL_خالی:
        tn->usq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ telnet_negotiate(بنية Curl_easy *داده, بنية TELNET *tn)
{
  صحيح i;

  لكل(i = 0; i < CURL_NTELOPTS; i++) {
    إذا(i == CURL_TELOPT_ECHO)
      استمر;

    إذا(tn->us_preferred[i] == CURL_YES)
      مجموعه_local_option(داده, tn, i, CURL_YES);

    إذا(tn->him_preferred[i] == CURL_YES)
      مجموعه_remote_option(داده, tn, i, CURL_YES);
  }
}

ساكن فراغ rec_will(بنية Curl_easy *داده, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    إذا(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(داده, CURL_DO, option);
    }
    وإلا
      send_negotiation(داده, CURL_DONT, option);

    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_خالی:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_خالی;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_خالی:
      tn->him[option] = CURL_YES;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_خالی;
      send_negotiation(داده, CURL_DONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_wont(بنية Curl_easy *داده, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(داده, CURL_DONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_خالی:
      tn->him[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_خالی;
      send_negotiation(داده, CURL_DO, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_خالی:
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_خالی;
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ printsub(بنية Curl_easy *داده,
                     صحيح direction,                /* '<' or '>' */
                     ثابت بیعلامت حرف *اشاره_گر, /* ptr to suboption data */
                     size_t درازی)                /* suboption data length */
{
  إذا(داده->مجموعه.verbose) {
    بیعلامت صحيح i = 0;
    إذا(direction) {
      infof(داده, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      إذا(درازی >= 3) {
        صحيح j;

        i = اشاره_گر[درازی - 2];
        j = اشاره_گر[درازی - 1];

        إذا(i != CURL_IAC || j != CURL_SE) {
          infof(داده, "(terminated by ");
          إذا(CURL_TELOPT_OK(i))
            infof(داده, "%s ", CURL_TELOPT(i));
          وإلا إذا(CURL_TELCMD_OK(i))
            infof(داده, "%s ", CURL_TELCMD(i));
          وإلا
            infof(داده, "%u ", i);
          إذا(CURL_TELOPT_OK(j))
            infof(داده, "%s", CURL_TELOPT(j));
          وإلا إذا(CURL_TELCMD_OK(j))
            infof(داده, "%s", CURL_TELCMD(j));
          وإلا
            infof(داده, "%d", j);
          infof(داده, ", not IAC SE) ");
        }
      }
      إذا(درازی >= 2)
        درازی -= 2;
      وإلا /* bad input */
        أرجع;
    }
    إذا(درازی <= 1) {
      infof(داده, "(Empty suboption?)");
      أرجع;
    }

    إذا(CURL_TELOPT_OK(اشاره_گر[0])) {
      بدل(اشاره_گر[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
      حالة CURL_TELOPT_NEW_ENVIRON:
      حالة CURL_TELOPT_NAWS:
        infof(داده, "%s", CURL_TELOPT(اشاره_گر[0]));
        اكسر;
      افتراضي:
        infof(داده, "%s (unsupported)", CURL_TELOPT(اشاره_گر[0]));
        اكسر;
      }
    }
    وإلا
      infof(داده, "%d (unknown)", اشاره_گر[0]);

    بدل(اشاره_گر[0]) {
    حالة CURL_TELOPT_NAWS:
      إذا(درازی > 4)
        infof(داده, "Width: %d ; Height: %d", (اشاره_گر[1] << 8) | اشاره_گر[2],
              (اشاره_گر[3] << 8) | اشاره_گر[4]);
      اكسر;
    افتراضي:
      بدل(اشاره_گر[1]) {
      حالة CURL_TELQUAL_IS:
        infof(داده, " IS");
        اكسر;
      حالة CURL_TELQUAL_SEND:
        infof(داده, " SEND");
        اكسر;
      حالة CURL_TELQUAL_INFO:
        infof(داده, " INFO/REPLY");
        اكسر;
      حالة CURL_TELQUAL_NAME:
        infof(داده, " NAME");
        اكسر;
      }

      بدل(اشاره_گر[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
        infof(داده, " \"%.*s\"",
              (صحيح)((درازی > 2) ? (درازی - 2) : 0), &اشاره_گر[2]);
        اكسر;
      حالة CURL_TELOPT_NEW_ENVIRON:
        إذا(اشاره_گر[1] == CURL_TELQUAL_IS) {
          infof(داده, " ");
          لكل(i = 3; i < درازی; i++) {
            بدل(اشاره_گر[i]) {
            حالة CURL_NEW_ENV_VAR:
              infof(داده, ", ");
              اكسر;
            حالة CURL_NEW_ENV_مقدار:
              infof(داده, " = ");
              اكسر;
            افتراضي:
              infof(داده, "%c", اشاره_گر[i]);
              اكسر;
            }
          }
        }
        اكسر;
      افتراضي:
        لكل(i = 2; i < درازی; i++)
          infof(داده, " %.2x", اشاره_گر[i]);
        اكسر;
      }
    }
  }
}

/* Escape and send a telnet data block */
ساكن CURLcode send_telnet_داده(بنية Curl_easy *داده,
                                 بنية TELNET *tn,
                                 ثابت حرف *میانگیر, ssize_t nread)
{
  size_t i, outlen;
  ثابت بیعلامت حرف *outbuf;
  CURLcode نتیجه = CURLE_OK;
  size_t بایتها_written;
  size_t مجموع_written = 0;
  بنية connectdata *conn = داده->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  إذا(nread < 0)
    أرجع CURLE_TOO_LARGE;

  إذا(memchr(میانگیر, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->بیرون);

    لكل(i = 0; i < (size_t)nread && !نتیجه; i++) {
      نتیجه = curlx_dyn_addn(&tn->بیرون, &میانگیر[i], 1);
      إذا(!نتیجه && ((بیعلامت حرف)میانگیر[i] == CURL_IAC))
        /* IAC is FF in hex */
        نتیجه = curlx_dyn_addn(&tn->بیرون, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->بیرون);
    outbuf = curlx_dyn_uptr(&tn->بیرون);
  }
  وإلا {
    outlen = (size_t)nread;
    outbuf = (ثابت بیعلامت حرف *)میانگیر;
  }
  طالما(!نتیجه && مجموع_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    بنية pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(داده);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    إذا(timeout_ms < 0)
      أرجع CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    بدل(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    حالة -1:                    /* error, abort writing */
      نتیجه = CURLE_SEND_ERROR;
      اكسر;
    حالة 0:                     /* timeout */
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    افتراضي:                    /* write! */
      بایتها_written = 0;
      نتیجه = Curl_xfer_send(داده, outbuf + مجموع_written,
                              outlen - مجموع_written, FALSE, &بایتها_written);
      مجموع_written += بایتها_written;
      اكسر;
    }
  }

  أرجع نتیجه;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ساكن فراغ sendsuboption(بنية Curl_easy *داده,
                          بنية TELNET *tn, صحيح option)
{
  size_t nwritten;
  بیعلامت کوتاه x, y;
  ثابت بیعلامت حرف *uc1, *uc2;
  CURLcode نتیجه;

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
    uc1 = (ثابت بیعلامت حرف *)&x;
    uc2 = (ثابت بیعلامت حرف *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_جمله(tn);
    /* data suboption is now ready */

    printsub(داده, '>', (ثابت بیعلامت حرف *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    نتیجه = Curl_xfer_send(داده, tn->subbuffer, 3, FALSE, &nwritten);
    إذا(نتیجه)
      failf(داده, "Sending data failed: %s", curl_easy_strerror(نتیجه));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_داده(داده, tn, (ثابت حرف *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    نتیجه = Curl_xfer_send(داده, tn->subbuffer + 7, 2, FALSE, &nwritten);
    إذا(نتیجه)
      failf(داده, "Sending data failed: %s", curl_easy_strerror(نتیجه));
    اكسر;
  }
}

ساكن فراغ rec_do(بنية Curl_easy *داده, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    إذا(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(داده, CURL_WILL, option);
      إذا(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(داده, tn, option);
    }
    وإلا إذا(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(داده, CURL_WILL, option);
      sendsuboption(داده, tn, option);
    }
    وإلا
      send_negotiation(داده, CURL_WONT, option);
    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_خالی:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_خالی;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_خالی:
      tn->us[option] = CURL_YES;
      إذا(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(داده, tn, option);
      }
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_خالی;
      send_negotiation(داده, CURL_WONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_dont(بنية Curl_easy *داده, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(داده, CURL_WONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_خالی:
      tn->us[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_خالی;
      send_negotiation(داده, CURL_WILL, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_خالی:
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_خالی;
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

ساكن CURLcode check_telnet_options(بنية Curl_easy *داده,
                                     بنية TELNET *tn)
{
  بنية curl_slist *سر;
  بنية curl_slist *beg;
  CURLcode نتیجه = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  إذا(داده->state.creds) {
    حرف میانگیر[256];
    إذا(str_is_nonascii(Curl_creds_user(داده->conn->creds))) {
      DEBUGF(infof(داده, "set a non ASCII username in telnet"));
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    }
    curl_msnprintf(میانگیر, حجم(میانگیر), "USER,%s",
                   Curl_creds_user(داده->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, میانگیر);
    إذا(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = عدم;
      أرجع CURLE_بیرون_از_حافظه;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  لكل(سر = داده->مجموعه.telnet_options; سر && !نتیجه; سر = سر->بعدی) {
    size_t olen;
    ثابت حرف *option = سر->داده;
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
        نتیجه = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 8:
        /* Display variable */
        إذا(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          اكسر;
        }
        نتیجه = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 7:
        /* Environment variable */
        إذا(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          إذا(!beg) {
            نتیجه = CURLE_بیرون_از_حافظه;
            اكسر;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        وإلا
          نتیجه = CURLE_UNKNOWN_OPTION;
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
            failf(داده, "Syntax error in telnet option: %s", سر->داده);
            نتیجه = CURLE_SETOPT_OPTION_SYNTAX;
          }
          وإلا {
            tn->subopt_wsx = (بیعلامت کوتاه)x;
            tn->subopt_wsy = (بیعلامت کوتاه)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        وإلا
          نتیجه = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 6:
        /* To take care or not of the 8th bit in data exchange */
        إذا(curl_strnequal(option, "BINARY", 6)) {
          ثابت حرف *p = arg;
          curl_off_t دودویی_option;
          إذا(!curlx_str_عدد(&p, &دودویی_option, 1) &&
             (دودویی_option != 1)) {
            tn->us_preferred[CURL_TELOPT_دودویی] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_دودویی] = CURL_NO;
          }
        }
        وإلا
          نتیجه = CURLE_UNKNOWN_OPTION;
        اكسر;
      افتراضي:
        failf(داده, "Unknown telnet option %s", سر->داده);
        نتیجه = CURLE_UNKNOWN_OPTION;
        اكسر;
      }
    }
    وإلا {
      failf(داده, "Syntax error in telnet option: %s", سر->داده);
      نتیجه = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  إذا(نتیجه) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = عدم;
  }

  أرجع نتیجه;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ساكن bool bad_option(ثابت حرف *داده)
{
  أرجع !داده || !!strchr(داده, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ساكن CURLcode suboption(بنية Curl_easy *داده, بنية TELNET *tn)
{
  بنية curl_slist *v;
  بیعلامت حرف موقت[2048];
  size_t nwritten;
  size_t len;
  CURLcode نتیجه = CURLE_OK;

  إذا(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    أرجع CURLE_OK;

  printsub(داده, '<', (ثابت بیعلامت حرف *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  بدل(CURL_SB_GET(tn)) {
  حالة CURL_TELOPT_TTYPE:
    إذا(bad_option(tn->subopt_ttype))
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    إذا(طول_رشته(tn->subopt_ttype) > 1000) {
      failf(داده, "Too long telnet TTYPE");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)موقت, حجم(موقت), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    نتیجه = Curl_xfer_send(داده, موقت, len, FALSE, &nwritten);
    إذا(نتیجه) {
      failf(داده, "Sending data failed: %s", curl_easy_strerror(نتیجه));
      أرجع نتیجه;
    }
    printsub(داده, '>', &موقت[2], len-2);
    اكسر;
  حالة CURL_TELOPT_XDISPLOC:
    إذا(bad_option(tn->subopt_xdisploc))
      أرجع CURLE_BAD_FUNCTION_آرگومان;
    إذا(طول_رشته(tn->subopt_xdisploc) > 1000) {
      failf(داده, "Too long telnet XDISPLOC");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)موقت, حجم(موقت), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    نتیجه = Curl_xfer_send(داده, موقت, len, FALSE, &nwritten);
    إذا(نتیجه) {
      failf(داده, "Sending data failed: %s", curl_easy_strerror(نتیجه));
      أرجع نتیجه;
    }
    printsub(داده, '>', &موقت[2], len - 2);
    اكسر;
  حالة CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((حرف *)موقت, حجم(موقت), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    لكل(v = tn->telnet_vars; v; v = v->بعدی) {
      size_t tmplen = (طول_رشته(v->داده) + 1);
      إذا(bad_option(v->داده))
        أرجع CURLE_BAD_FUNCTION_آرگومان;
      /* Add the variable if it fits */
      إذا(len + tmplen < حجم(موقت) - 6) {
        ثابت حرف *s = strchr(v->داده, ',');
        إذا(!s)
          len += curl_msnprintf((حرف *)&موقت[len], حجم(موقت) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->داده);
        وإلا {
          size_t vlen = s - v->داده;
          len += curl_msnprintf((حرف *)&موقت[len], حجم(موقت) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (صحيح)vlen, v->داده, CURL_NEW_ENV_مقدار, ++s);
        }
      }
    }
    curl_msnprintf((حرف *)&موقت[len], حجم(موقت) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    نتیجه = Curl_xfer_send(داده, موقت, len, FALSE, &nwritten);
    إذا(نتیجه) {
      failf(داده, "Sending data failed: %s", curl_easy_strerror(نتیجه));
      أرجع نتیجه;
    }
    printsub(داده, '>', &موقت[2], len - 2);
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode telrcv(بنية Curl_easy *داده,
                       بنية TELNET *tn,
                       ثابت بیعلامت حرف *inbuf, /* Data received from
                                                      socket */
                       ssize_t شمار)              /* Number of bytes
                                                      received */
{
  بیعلامت حرف c;
  CURLcode نتیجه;
  صحيح درون = 0;
  صحيح startwrite = -1;

#تعریف_کن startskipping()                                          \
  إذا(startwrite >= 0) {                                          \
    نتیجه = Curl_client_write(داده,                             \
                               CLIENTWRITE_BODY,                 \
                               (ثابت حرف *)&inbuf[startwrite], \
                               درون-startwrite);                   \
    إذا(نتیجه)                                                   \
      أرجع نتیجه;                                             \
  }                                                              \
  startwrite = -1

#تعریف_کن writebyte()  \
  إذا(startwrite < 0) \
    startwrite = درون

#تعریف_کن bufferflush() startskipping()

  طالما(شمار--) {
    c = inbuf[درون];

    بدل(tn->telrcv_state) {
    حالة CURL_TS_CR:
      tn->telrcv_state = CURL_TS_داده;
      إذا(c == '\0') {
        startskipping();
        اكسر;   /* Ignore \0 after CR */
      }
      writebyte();
      اكسر;

    حالة CURL_TS_داده:
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
        tn->telrcv_state = CURL_TS_داده;
        writebyte();
        اكسر;
      حالة CURL_DM:
      حالة CURL_NOP:
      حالة CURL_GA:
      افتراضي:
        tn->telrcv_state = CURL_TS_داده;
        printoption(داده, "RCVD", CURL_IAC, c);
        اكسر;
      }
      اكسر;

    حالة CURL_TS_WILL:
      printoption(داده, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(داده, tn, c);
      tn->telrcv_state = CURL_TS_داده;
      اكسر;

    حالة CURL_TS_WONT:
      printoption(داده, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(داده, tn, c);
      tn->telrcv_state = CURL_TS_داده;
      اكسر;

    حالة CURL_TS_DO:
      printoption(داده, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(داده, tn, c);
      tn->telrcv_state = CURL_TS_داده;
      اكسر;

    حالة CURL_TS_DONT:
      printoption(داده, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(داده, tn, c);
      tn->telrcv_state = CURL_TS_داده;
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
          failf(داده, "telnet: suboption error");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      وإلا {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_جمله(tn);
        نتیجه = suboption(داده, tn);   /* handle sub-option */
        إذا(نتیجه)
          أرجع نتیجه;
        tn->telrcv_state = CURL_TS_داده;
      }
      اكسر;
    }
    ++درون;
  }
  bufferflush();
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_انجامشده(بنية Curl_easy *داده,
                            CURLcode وضعیت, bool premature)
{
  (فراغ)وضعیت;
  (فراغ)premature;
  Curl_meta_remove(داده, CURL_META_TELNET_EASY);
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_do(بنية Curl_easy *داده, bool *انجامشده)
{
  CURLcode نتیجه;
  بنية connectdata *conn = داده->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#اگر_تعریف_شده USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_شمار;
  DWORD wait_timeout;
  DWORD readfile_read;
#وإلا
  timediff_t interval_ms;
  بنية pollfd pfd[2];
  صحيح poll_cnt;
  ssize_t snread;
#پایان_اگر
  bool keepon = TRUE;
  حرف میانگیر[4 * 1024];
  بنية TELNET *tn;

  *انجامشده = TRUE; /* unconditionally */

  نتیجه = init_telnet(داده);
  إذا(نتیجه)
    أرجع نتیجه;

  tn = Curl_meta_get(داده, CURL_META_TELNET_EASY);
  إذا(!tn)
    أرجع CURLE_FAILED_INIT;

  نتیجه = check_telnet_options(داده, tn);
  إذا(نتیجه)
    أرجع نتیجه;

#اگر_تعریف_شده USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  إذا(event_handle == WSA_INVALID_EVENT) {
    failf(داده, "WSACreateEvent failed (%d)", SOCKERRNO);
    أرجع CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  إذا(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    أرجع CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_درون_داد_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  إذا(GetFileType(stdin_handle) == FILE_TYPE_PIPE || داده->مجموعه.is_fread_مجموعه) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_شمار = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  وإلا {
    obj_شمار = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  طالما(keepon) {
    ثابت DWORD buf_اندازه = (DWORD)حجم(میانگیر);
    DWORD waitret = Wait_For_مضرب_Objects(obj_شمار, objs,
                                           FALSE, wait_timeout);
    بدل(waitret) {

    حالة WAIT_TIMEOUT: {
      لكل(;;) {
        إذا(داده->مجموعه.is_fread_مجموعه) {
          size_t n;
          /* read from user-supplied method */
          n = داده->state.fread_func(میانگیر, 1, buf_اندازه, داده->state.درون);
          إذا(n == CURL_READFUNC_ABORT) {
            failf(داده, "operation aborted by callback");
            keepon = FALSE;
            نتیجه = CURLE_ABORTED_BY_CALLBACK;
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
            نتیجه = CURLE_READ_ERROR;
            اكسر;
          }

          إذا(!readfile_read)
            اكسر;

          إذا(!ReadFile(stdin_handle, میانگیر, buf_اندازه, &readfile_read, عدم)) {
            keepon = FALSE;
            نتیجه = CURLE_READ_ERROR;
            اكسر;
          }
        }

        نتیجه = send_telnet_داده(داده, tn, میانگیر, readfile_read);
        إذا(نتیجه) {
          keepon = FALSE;
          اكسر;
        }
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0 + 1: {
      إذا(!ReadFile(stdin_handle, میانگیر, buf_اندازه, &readfile_read, عدم)) {
        keepon = FALSE;
        نتیجه = CURLE_READ_ERROR;
        اكسر;
      }

      نتیجه = send_telnet_داده(داده, tn, میانگیر, readfile_read);
      إذا(نتیجه) {
        keepon = FALSE;
        اكسر;
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0: {
      events.l_شبکه_Events = 0;
      إذا(WSAEnum_شبکه_Events(sockfd, event_handle, &events) != 0) {
        صحيح sockerr = SOCKERRNO;
        إذا(sockerr != SOCKEINPROGRESS) {
          infof(داده, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          نتیجه = CURLE_READ_ERROR;
        }
        اكسر;
      }
      إذا(events.l_شبکه_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        نتیجه = Curl_xfer_recv(داده, میانگیر, حجم(میانگیر), &nread);
        /* read would have blocked. Loop again */
        إذا(نتیجه == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        وإلا إذا(نتیجه) {
          keepon = FALSE;
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا إذا(!nread) {
          keepon = FALSE;
          اكسر;
        }

        نتیجه = telrcv(داده, tn, (بیعلامت حرف *)میانگیر, nread);
        إذا(نتیجه) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        إذا(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(داده, tn);
          tn->already_negotiated = 1;
        }
      }
      إذا(events.l_شبکه_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      اكسر;
    }
    } /* switch */

    إذا(Curl_timeleft_ms(داده) < 0) {
      failf(داده, "Time-out");
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  إذا(!WSACloseEvent(event_handle)) {
    infof(داده, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#وإلا
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  إذا(داده->مجموعه.is_fread_مجموعه) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  وإلا {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)داده->state.درون);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    إذا(pfd[1].fd < 0) {
      failf(داده, "cannot read input");
      نتیجه = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  طالما(keepon) {
    DEBUGF(infof(داده, "telnet_do, poll %d fds", poll_cnt));
    بدل(Curl_poll(pfd, (بیعلامت صحيح)poll_cnt, interval_ms)) {
    حالة -1:                    /* error, stop reading */
      نتیجه = CURLE_RECV_ERROR;
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
        نتیجه = Curl_xfer_recv(داده, میانگیر, حجم(میانگیر), &nread);
        /* read would have blocked. Loop again */
        إذا(نتیجه == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        إذا(نتیجه) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          إذا(داده->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(داده, "telnet_do, unexpected ECONNRESET on recv"));
          }
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا إذا(!nread) {
          keepon = FALSE;
          اكسر;
        }

        Curl_pgrs_download_inc(داده, nread);
        نتیجه = telrcv(داده, tn, (بیعلامت حرف *)میانگیر, nread);
        إذا(نتیجه) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        إذا(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(داده, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      إذا(poll_cnt == 2) {
        إذا(pfd[1].revents & POLLIN) { /* read from in file */
          snread = اقرأ(pfd[1].fd, میانگیر, حجم(میانگیر));
        }
      }
      وإلا {
        /* read from user-supplied method */
        snread = (صحيح)داده->state.fread_func(میانگیر, 1, حجم(میانگیر),
                                             داده->state.درون);
        إذا(snread == CURL_READFUNC_ABORT) {
          failf(داده, "operation aborted by callback");
          نتیجه = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          اكسر;
        }
        إذا(snread == CURL_READFUNC_PAUSE)
          اكسر;
      }

      إذا(snread > 0) {
        نتیجه = send_telnet_داده(داده, tn, میانگیر, snread);
        إذا(نتیجه) {
          keepon = FALSE;
          اكسر;
        }
        Curl_pgrs_upload_inc(داده, (size_t)snread);
      }
      وإلا إذا(snread < 0) {
        failf(داده, "Failed to read input");
        نتیجه = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      اكسر;
    } /* poll switch statement */

    إذا(Curl_timeleft_ms(داده) < 0) {
      failf(داده, "Time-out");
      نتیجه = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    إذا(!نتیجه) {
      نتیجه = Curl_pgrsUpdate(داده);
      إذا(نتیجه)
        keepon = FALSE;
    }
  }
#پایان_اگر
  /* mark this as "no further transfer wanted" */
  Curl_xfer_راهاندازی_nop(داده);

  أرجع نتیجه;
}

/*
 * TELNET protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_انجامشده,                          /* done */
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

#پایان_اگر /* !CURL_DISABLE_TELNET */
