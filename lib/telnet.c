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

#جے_بنا_تعریف CURL_DISABLE_TELNET

#جے_تعریف_شدہ HAVE_NETINET_وچ_H
#ضمن <netinet/in.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NETDB_H
#ضمن <netdb.h>
#جے_انت
#جے_تعریف_شدہ HAVE_ARPA_INET_H
#ضمن <arpa/inet.h>
#جے_انت
#جے_تعریف_شدہ HAVE_NET_IF_H
#ضمن <net/if.h>
#جے_انت
#جے_تعریف_شدہ HAVE_SYS_IOCTL_H
#ضمن <sys/ioctl.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PARAM_H
#ضمن <sys/param.h>
#جے_انت

#ضمن "url.h"
#ضمن "transfer.h"
#ضمن "sendf.h"
#ضمن "curl_trc.h"
#ضمن "progress.h"
#ضمن "arpa_telnet.h"
#ضمن "connect.h"
#ضمن "select.h"
#ضمن "curlx/strparse.h"

#تعریف_کرو SUBBUFSIZE 512

#تعریف_کرو CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#تعریف_کرو CURL_SB_رکن(x)            \
  افعل {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } طالما(0)
#تعریف_کرو CURL_SB_ACCUM(x, c)                                         \
  افعل {                                                              \
    إذا((x)->subpointer < ((x)->subbuffer + حجم((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } طالما(0)

#تعریف_کرو CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#تعریف_کرو CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#تعریف_کرو CURL_NO          0
#تعریف_کرو CURL_YES         1
#تعریف_کرو CURL_WANTYES     2
#تعریف_کرو CURL_WANTNO      3

#تعریف_کرو CURL_خالی_تھاواں       0
#تعریف_کرو CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
تعريفنوع تعداد {
  CURL_TS_انکڑے = 0,
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
  بنانشانی چھوٹا subopt_wsx;         /* Set with suboption NAWS */
  بنانشانی چھوٹا subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  بنية curl_slist *telnet_vars;    /* Environment variables */
  بنية dynbuf باہرلا;                 /* output buffer */

  /* suboptions */
  بنانشانی حرف subbuffer[SUBBUFSIZE];
  بنانشانی حرف *subpointer, *subend;      /* buffer for sub-options */
};

#جے_بنا_تعریف CURLVERBOSE
#تعریف_کرو printoption(a, b, c, d) Curl_nop_stmt
#وإلا
ساكن فراغ printoption(بنية Curl_easy *انکڑے,
                        ثابت حرف *direction, صحيح cmd, صحيح option)
{
  إذا(انکڑے->سیٹ.verbose) {
    إذا(cmd == CURL_IAC) {
      إذا(CURL_TELCMD_OK(option))
        infof(انکڑے, "%s IAC %s", direction, CURL_TELCMD(option));
      وإلا
        infof(انکڑے, "%s IAC %d", direction, option);
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
          infof(انکڑے, "%s %s %s", direction, fmt, opt);
        وإلا
          infof(انکڑے, "%s %s %d", direction, fmt, option);
      }
      وإلا
        infof(انکڑے, "%s %d %d", direction, cmd, option);
    }
  }
}
#جے_انت /* !CURLVERBOSE */

ساكن فراغ telnet_easy_dtor(ثابت فراغ *کلید, size_t klen, فراغ *اندراج)
{
  بنية TELNET *tn = اندراج;
  (فراغ)کلید;
  (فراغ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->باہرلا);
  curlx_free(tn);
}

ساكن CURLcode init_telnet(بنية Curl_easy *انکڑے)
{
  بنية TELNET *tn;

  tn = curlx_calloc(1, حجم(بنية TELNET));
  إذا(!tn)
    أرجع CURLE_باہرلا_دا_حافظہ;

  curlx_dyn_init(&tn->باہرلا, 0xffff);

  tn->telrcv_state = CURL_TS_انکڑے;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_ثنائی] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ثنائی] = CURL_YES;

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

  أرجع Curl_meta_سیٹ(انکڑے, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ساكن فراغ send_negotiation(بنية Curl_easy *انکڑے, صحيح cmd, صحيح option)
{
  بنانشانی حرف buf[3];
  size_t nwritten;
  CURLcode نتیجہ;

  buf[0] = CURL_IAC;
  buf[1] = (بنانشانی حرف)cmd;
  buf[2] = (بنانشانی حرف)option;

  نتیجہ = Curl_xfer_send(انکڑے, buf, 3, FALSE, &nwritten);
  إذا(نتیجہ)
    failf(انکڑے, "Sending data failed: %s", curl_easy_strerror(نتیجہ));

  printoption(انکڑے, "SENT", cmd, option);
}

ساكن فراغ سیٹ_remote_option(بنية Curl_easy *انکڑے, بنية TELNET *tn,
                              صحيح option, صحيح newstate)
{
  إذا(newstate == CURL_YES) {
    بدل(tn->him[option]) {
    حالة CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(انکڑے, CURL_DO, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_خالی_تھاواں:
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
      حالة CURL_خالی_تھاواں:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_خالی_تھاواں;
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
      send_negotiation(انکڑے, CURL_DONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->himq[option]) {
      حالة CURL_خالی_تھاواں:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->himq[option] = CURL_خالی_تھاواں;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->himq[option]) {
      حالة CURL_خالی_تھاواں:
        tn->himq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ سیٹ_local_option(بنية Curl_easy *انکڑے, بنية TELNET *tn,
                             صحيح option, صحيح newstate)
{
  إذا(newstate == CURL_YES) {
    بدل(tn->us[option]) {
    حالة CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(انکڑے, CURL_WILL, option);
      اكسر;

    حالة CURL_YES:
      /* Already enabled */
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_خالی_تھاواں:
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
      حالة CURL_خالی_تھاواں:
        /* Error: already negotiating for enable */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_خالی_تھاواں;
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
      send_negotiation(انکڑے, CURL_WONT, option);
      اكسر;

    حالة CURL_WANTNO:
      بدل(tn->usq[option]) {
      حالة CURL_خالی_تھاواں:
        /* Already negotiating for NO */
        اكسر;
      حالة CURL_OPPOSITE:
        tn->usq[option] = CURL_خالی_تھاواں;
        اكسر;
      }
      اكسر;

    حالة CURL_WANTYES:
      بدل(tn->usq[option]) {
      حالة CURL_خالی_تھاواں:
        tn->usq[option] = CURL_OPPOSITE;
        اكسر;
      حالة CURL_OPPOSITE:
        اكسر;
      }
      اكسر;
    }
  }
}

ساكن فراغ telnet_negotiate(بنية Curl_easy *انکڑے, بنية TELNET *tn)
{
  صحيح i;

  لكل(i = 0; i < CURL_NTELOPTS; i++) {
    إذا(i == CURL_TELOPT_ECHO)
      استمر;

    إذا(tn->us_preferred[i] == CURL_YES)
      سیٹ_local_option(انکڑے, tn, i, CURL_YES);

    إذا(tn->him_preferred[i] == CURL_YES)
      سیٹ_remote_option(انکڑے, tn, i, CURL_YES);
  }
}

ساكن فراغ rec_will(بنية Curl_easy *انکڑے, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    إذا(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(انکڑے, CURL_DO, option);
    }
    وإلا
      send_negotiation(انکڑے, CURL_DONT, option);

    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_خالی_تھاواں:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_خالی_تھاواں;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_خالی_تھاواں:
      tn->him[option] = CURL_YES;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_خالی_تھاواں;
      send_negotiation(انکڑے, CURL_DONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_wont(بنية Curl_easy *انکڑے, بنية TELNET *tn, صحيح option)
{
  بدل(tn->him[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(انکڑے, CURL_DONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->himq[option]) {
    حالة CURL_خالی_تھاواں:
      tn->him[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_خالی_تھاواں;
      send_negotiation(انکڑے, CURL_DO, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->himq[option]) {
    حالة CURL_خالی_تھاواں:
      tn->him[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_خالی_تھاواں;
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ printsub(بنية Curl_easy *انکڑے,
                     صحيح direction,                /* '<' or '>' */
                     ثابت بنانشانی حرف *اشارہ_گر, /* ptr to suboption data */
                     size_t لمائی)                /* suboption data length */
{
  إذا(انکڑے->سیٹ.verbose) {
    بنانشانی صحيح i = 0;
    إذا(direction) {
      infof(انکڑے, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      إذا(لمائی >= 3) {
        صحيح j;

        i = اشارہ_گر[لمائی - 2];
        j = اشارہ_گر[لمائی - 1];

        إذا(i != CURL_IAC || j != CURL_SE) {
          infof(انکڑے, "(terminated by ");
          إذا(CURL_TELOPT_OK(i))
            infof(انکڑے, "%s ", CURL_TELOPT(i));
          وإلا إذا(CURL_TELCMD_OK(i))
            infof(انکڑے, "%s ", CURL_TELCMD(i));
          وإلا
            infof(انکڑے, "%u ", i);
          إذا(CURL_TELOPT_OK(j))
            infof(انکڑے, "%s", CURL_TELOPT(j));
          وإلا إذا(CURL_TELCMD_OK(j))
            infof(انکڑے, "%s", CURL_TELCMD(j));
          وإلا
            infof(انکڑے, "%d", j);
          infof(انکڑے, ", not IAC SE) ");
        }
      }
      إذا(لمائی >= 2)
        لمائی -= 2;
      وإلا /* bad input */
        أرجع;
    }
    إذا(لمائی <= 1) {
      infof(انکڑے, "(Empty suboption?)");
      أرجع;
    }

    إذا(CURL_TELOPT_OK(اشارہ_گر[0])) {
      بدل(اشارہ_گر[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
      حالة CURL_TELOPT_NEW_ENVIRON:
      حالة CURL_TELOPT_NAWS:
        infof(انکڑے, "%s", CURL_TELOPT(اشارہ_گر[0]));
        اكسر;
      افتراضي:
        infof(انکڑے, "%s (unsupported)", CURL_TELOPT(اشارہ_گر[0]));
        اكسر;
      }
    }
    وإلا
      infof(انکڑے, "%d (unknown)", اشارہ_گر[0]);

    بدل(اشارہ_گر[0]) {
    حالة CURL_TELOPT_NAWS:
      إذا(لمائی > 4)
        infof(انکڑے, "Width: %d ; Height: %d", (اشارہ_گر[1] << 8) | اشارہ_گر[2],
              (اشارہ_گر[3] << 8) | اشارہ_گر[4]);
      اكسر;
    افتراضي:
      بدل(اشارہ_گر[1]) {
      حالة CURL_TELQUAL_IS:
        infof(انکڑے, " IS");
        اكسر;
      حالة CURL_TELQUAL_SEND:
        infof(انکڑے, " SEND");
        اكسر;
      حالة CURL_TELQUAL_INFO:
        infof(انکڑے, " INFO/REPLY");
        اكسر;
      حالة CURL_TELQUAL_NAME:
        infof(انکڑے, " NAME");
        اكسر;
      }

      بدل(اشارہ_گر[0]) {
      حالة CURL_TELOPT_TTYPE:
      حالة CURL_TELOPT_XDISPLOC:
        infof(انکڑے, " \"%.*s\"",
              (صحيح)((لمائی > 2) ? (لمائی - 2) : 0), &اشارہ_گر[2]);
        اكسر;
      حالة CURL_TELOPT_NEW_ENVIRON:
        إذا(اشارہ_گر[1] == CURL_TELQUAL_IS) {
          infof(انکڑے, " ");
          لكل(i = 3; i < لمائی; i++) {
            بدل(اشارہ_گر[i]) {
            حالة CURL_NEW_ENV_VAR:
              infof(انکڑے, ", ");
              اكسر;
            حالة CURL_NEW_ENV_قدر:
              infof(انکڑے, " = ");
              اكسر;
            افتراضي:
              infof(انکڑے, "%c", اشارہ_گر[i]);
              اكسر;
            }
          }
        }
        اكسر;
      افتراضي:
        لكل(i = 2; i < لمائی; i++)
          infof(انکڑے, " %.2x", اشارہ_گر[i]);
        اكسر;
      }
    }
  }
}

/* Escape and send a telnet data block */
ساكن CURLcode send_telnet_انکڑے(بنية Curl_easy *انکڑے,
                                 بنية TELNET *tn,
                                 ثابت حرف *بفر, ssize_t nread)
{
  size_t i, outlen;
  ثابت بنانشانی حرف *outbuf;
  CURLcode نتیجہ = CURLE_OK;
  size_t بائٹاں_written;
  size_t کل_written = 0;
  بنية connectdata *conn = انکڑے->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  إذا(nread < 0)
    أرجع CURLE_TOO_LARGE;

  إذا(memchr(بفر, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->باہرلا);

    لكل(i = 0; i < (size_t)nread && !نتیجہ; i++) {
      نتیجہ = curlx_dyn_addn(&tn->باہرلا, &بفر[i], 1);
      إذا(!نتیجہ && ((بنانشانی حرف)بفر[i] == CURL_IAC))
        /* IAC is FF in hex */
        نتیجہ = curlx_dyn_addn(&tn->باہرلا, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->باہرلا);
    outbuf = curlx_dyn_uptr(&tn->باہرلا);
  }
  وإلا {
    outlen = (size_t)nread;
    outbuf = (ثابت بنانشانی حرف *)بفر;
  }
  طالما(!نتیجہ && کل_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    بنية pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(انکڑے);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    إذا(timeout_ms < 0)
      أرجع CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    بدل(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    حالة -1:                    /* error, abort writing */
      نتیجہ = CURLE_SEND_ERROR;
      اكسر;
    حالة 0:                     /* timeout */
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      اكسر;
    افتراضي:                    /* write! */
      بائٹاں_written = 0;
      نتیجہ = Curl_xfer_send(انکڑے, outbuf + کل_written,
                              outlen - کل_written, FALSE, &بائٹاں_written);
      کل_written += بائٹاں_written;
      اكسر;
    }
  }

  أرجع نتیجہ;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ساكن فراغ sendsuboption(بنية Curl_easy *انکڑے,
                          بنية TELNET *tn, صحيح option)
{
  size_t nwritten;
  بنانشانی چھوٹا x, y;
  ثابت بنانشانی حرف *uc1, *uc2;
  CURLcode نتیجہ;

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
    uc1 = (ثابت بنانشانی حرف *)&x;
    uc2 = (ثابت بنانشانی حرف *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_رکن(tn);
    /* data suboption is now ready */

    printsub(انکڑے, '>', (ثابت بنانشانی حرف *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    نتیجہ = Curl_xfer_send(انکڑے, tn->subbuffer, 3, FALSE, &nwritten);
    إذا(نتیجہ)
      failf(انکڑے, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_انکڑے(انکڑے, tn, (ثابت حرف *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    نتیجہ = Curl_xfer_send(انکڑے, tn->subbuffer + 7, 2, FALSE, &nwritten);
    إذا(نتیجہ)
      failf(انکڑے, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
    اكسر;
  }
}

ساكن فراغ rec_do(بنية Curl_easy *انکڑے, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    إذا(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(انکڑے, CURL_WILL, option);
      إذا(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(انکڑے, tn, option);
    }
    وإلا إذا(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(انکڑے, CURL_WILL, option);
      sendsuboption(انکڑے, tn, option);
    }
    وإلا
      send_negotiation(انکڑے, CURL_WONT, option);
    اكسر;

  حالة CURL_YES:
    /* Already enabled */
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_خالی_تھاواں:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_خالی_تھاواں;
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_خالی_تھاواں:
      tn->us[option] = CURL_YES;
      إذا(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(انکڑے, tn, option);
      }
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_خالی_تھاواں;
      send_negotiation(انکڑے, CURL_WONT, option);
      اكسر;
    }
    اكسر;
  }
}

ساكن فراغ rec_dont(بنية Curl_easy *انکڑے, بنية TELNET *tn, صحيح option)
{
  بدل(tn->us[option]) {
  حالة CURL_NO:
    /* Already disabled */
    اكسر;

  حالة CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(انکڑے, CURL_WONT, option);
    اكسر;

  حالة CURL_WANTNO:
    بدل(tn->usq[option]) {
    حالة CURL_خالی_تھاواں:
      tn->us[option] = CURL_NO;
      اكسر;

    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_خالی_تھاواں;
      send_negotiation(انکڑے, CURL_WILL, option);
      اكسر;
    }
    اكسر;

  حالة CURL_WANTYES:
    بدل(tn->usq[option]) {
    حالة CURL_خالی_تھاواں:
      tn->us[option] = CURL_NO;
      اكسر;
    حالة CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_خالی_تھاواں;
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

ساكن CURLcode check_telnet_options(بنية Curl_easy *انکڑے,
                                     بنية TELNET *tn)
{
  بنية curl_slist *سر;
  بنية curl_slist *beg;
  CURLcode نتیجہ = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  إذا(انکڑے->state.creds) {
    حرف بفر[256];
    إذا(str_is_nonascii(Curl_creds_user(انکڑے->conn->creds))) {
      DEBUGF(infof(انکڑے, "set a non ASCII username in telnet"));
      أرجع CURLE_BAD_FUNCTION_دلیل;
    }
    curl_msnprintf(بفر, حجم(بفر), "USER,%s",
                   Curl_creds_user(انکڑے->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, بفر);
    إذا(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = عدم;
      أرجع CURLE_باہرلا_دا_حافظہ;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  لكل(سر = انکڑے->سیٹ.telnet_options; سر && !نتیجہ; سر = سر->اگلا) {
    size_t olen;
    ثابت حرف *option = سر->انکڑے;
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
        نتیجہ = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 8:
        /* Display variable */
        إذا(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          اكسر;
        }
        نتیجہ = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 7:
        /* Environment variable */
        إذا(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          إذا(!beg) {
            نتیجہ = CURLE_باہرلا_دا_حافظہ;
            اكسر;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        وإلا
          نتیجہ = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 2:
        /* Window Size */
        إذا(curl_strnequal(option, "WS", 2)) {
          ثابت حرف *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          إذا(curlx_str_نمبر(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_نمبر(&p, &y, 0xffff)) {
            failf(انکڑے, "Syntax error in telnet option: %s", سر->انکڑے);
            نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;
          }
          وإلا {
            tn->subopt_wsx = (بنانشانی چھوٹا)x;
            tn->subopt_wsy = (بنانشانی چھوٹا)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        وإلا
          نتیجہ = CURLE_UNKNOWN_OPTION;
        اكسر;

      حالة 6:
        /* To take care or not of the 8th bit in data exchange */
        إذا(curl_strnequal(option, "BINARY", 6)) {
          ثابت حرف *p = arg;
          curl_off_t ثنائی_option;
          إذا(!curlx_str_نمبر(&p, &ثنائی_option, 1) &&
             (ثنائی_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ثنائی] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ثنائی] = CURL_NO;
          }
        }
        وإلا
          نتیجہ = CURLE_UNKNOWN_OPTION;
        اكسر;
      افتراضي:
        failf(انکڑے, "Unknown telnet option %s", سر->انکڑے);
        نتیجہ = CURLE_UNKNOWN_OPTION;
        اكسر;
      }
    }
    وإلا {
      failf(انکڑے, "Syntax error in telnet option: %s", سر->انکڑے);
      نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  إذا(نتیجہ) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = عدم;
  }

  أرجع نتیجہ;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ساكن bool bad_option(ثابت حرف *انکڑے)
{
  أرجع !انکڑے || !!strchr(انکڑے, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ساكن CURLcode suboption(بنية Curl_easy *انکڑے, بنية TELNET *tn)
{
  بنية curl_slist *v;
  بنانشانی حرف عارضی[2048];
  size_t nwritten;
  size_t len;
  CURLcode نتیجہ = CURLE_OK;

  إذا(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    أرجع CURLE_OK;

  printsub(انکڑے, '<', (ثابت بنانشانی حرف *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  بدل(CURL_SB_GET(tn)) {
  حالة CURL_TELOPT_TTYPE:
    إذا(bad_option(tn->subopt_ttype))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(لڑی_لمبائی(tn->subopt_ttype) > 1000) {
      failf(انکڑے, "Too long telnet TTYPE");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)عارضی, حجم(عارضی), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    نتیجہ = Curl_xfer_send(انکڑے, عارضی, len, FALSE, &nwritten);
    إذا(نتیجہ) {
      failf(انکڑے, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
      أرجع نتیجہ;
    }
    printsub(انکڑے, '>', &عارضی[2], len-2);
    اكسر;
  حالة CURL_TELOPT_XDISPLOC:
    إذا(bad_option(tn->subopt_xdisploc))
      أرجع CURLE_BAD_FUNCTION_دلیل;
    إذا(لڑی_لمبائی(tn->subopt_xdisploc) > 1000) {
      failf(انکڑے, "Too long telnet XDISPLOC");
      أرجع CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)عارضی, حجم(عارضی), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    نتیجہ = Curl_xfer_send(انکڑے, عارضی, len, FALSE, &nwritten);
    إذا(نتیجہ) {
      failf(انکڑے, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
      أرجع نتیجہ;
    }
    printsub(انکڑے, '>', &عارضی[2], len - 2);
    اكسر;
  حالة CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((حرف *)عارضی, حجم(عارضی), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    لكل(v = tn->telnet_vars; v; v = v->اگلا) {
      size_t tmplen = (لڑی_لمبائی(v->انکڑے) + 1);
      إذا(bad_option(v->انکڑے))
        أرجع CURLE_BAD_FUNCTION_دلیل;
      /* Add the variable if it fits */
      إذا(len + tmplen < حجم(عارضی) - 6) {
        ثابت حرف *s = strchr(v->انکڑے, ',');
        إذا(!s)
          len += curl_msnprintf((حرف *)&عارضی[len], حجم(عارضی) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->انکڑے);
        وإلا {
          size_t vlen = s - v->انکڑے;
          len += curl_msnprintf((حرف *)&عارضی[len], حجم(عارضی) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (صحيح)vlen, v->انکڑے, CURL_NEW_ENV_قدر, ++s);
        }
      }
    }
    curl_msnprintf((حرف *)&عارضی[len], حجم(عارضی) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    نتیجہ = Curl_xfer_send(انکڑے, عارضی, len, FALSE, &nwritten);
    إذا(نتیجہ) {
      failf(انکڑے, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
      أرجع نتیجہ;
    }
    printsub(انکڑے, '>', &عارضی[2], len - 2);
    اكسر;
  }
  أرجع CURLE_OK;
}

ساكن CURLcode telrcv(بنية Curl_easy *انکڑے,
                       بنية TELNET *tn,
                       ثابت بنانشانی حرف *inbuf, /* Data received from
                                                      socket */
                       ssize_t گنتی)              /* Number of bytes
                                                      received */
{
  بنانشانی حرف c;
  CURLcode نتیجہ;
  صحيح وچ = 0;
  صحيح startwrite = -1;

#تعریف_کرو startskipping()                                          \
  إذا(startwrite >= 0) {                                          \
    نتیجہ = Curl_client_write(انکڑے,                             \
                               CLIENTWRITE_BODY,                 \
                               (ثابت حرف *)&inbuf[startwrite], \
                               وچ-startwrite);                   \
    إذا(نتیجہ)                                                   \
      أرجع نتیجہ;                                             \
  }                                                              \
  startwrite = -1

#تعریف_کرو writebyte()  \
  إذا(startwrite < 0) \
    startwrite = وچ

#تعریف_کرو bufferflush() startskipping()

  طالما(گنتی--) {
    c = inbuf[وچ];

    بدل(tn->telrcv_state) {
    حالة CURL_TS_CR:
      tn->telrcv_state = CURL_TS_انکڑے;
      إذا(c == '\0') {
        startskipping();
        اكسر;   /* Ignore \0 after CR */
      }
      writebyte();
      اكسر;

    حالة CURL_TS_انکڑے:
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
        tn->telrcv_state = CURL_TS_انکڑے;
        writebyte();
        اكسر;
      حالة CURL_DM:
      حالة CURL_NOP:
      حالة CURL_GA:
      افتراضي:
        tn->telrcv_state = CURL_TS_انکڑے;
        printoption(انکڑے, "RCVD", CURL_IAC, c);
        اكسر;
      }
      اكسر;

    حالة CURL_TS_WILL:
      printoption(انکڑے, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(انکڑے, tn, c);
      tn->telrcv_state = CURL_TS_انکڑے;
      اكسر;

    حالة CURL_TS_WONT:
      printoption(انکڑے, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(انکڑے, tn, c);
      tn->telrcv_state = CURL_TS_انکڑے;
      اكسر;

    حالة CURL_TS_DO:
      printoption(انکڑے, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(انکڑے, tn, c);
      tn->telrcv_state = CURL_TS_انکڑے;
      اكسر;

    حالة CURL_TS_DONT:
      printoption(انکڑے, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(انکڑے, tn, c);
      tn->telrcv_state = CURL_TS_انکڑے;
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
          failf(انکڑے, "telnet: suboption error");
          أرجع CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      وإلا {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_رکن(tn);
        نتیجہ = suboption(انکڑے, tn);   /* handle sub-option */
        إذا(نتیجہ)
          أرجع نتیجہ;
        tn->telrcv_state = CURL_TS_انکڑے;
      }
      اكسر;
    }
    ++وچ;
  }
  bufferflush();
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_پورا(بنية Curl_easy *انکڑے,
                            CURLcode حالت, bool premature)
{
  (فراغ)حالت;
  (فراغ)premature;
  Curl_meta_remove(انکڑے, CURL_META_TELNET_EASY);
  أرجع CURLE_OK;
}

ساكن CURLcode telnet_do(بنية Curl_easy *انکڑے, bool *پورا)
{
  CURLcode نتیجہ;
  بنية connectdata *conn = انکڑے->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#جے_تعریف_شدہ USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_گنتی;
  DWORD wait_timeout;
  DWORD readfile_read;
#وإلا
  timediff_t interval_ms;
  بنية pollfd pfd[2];
  صحيح poll_cnt;
  ssize_t snread;
#جے_انت
  bool keepon = TRUE;
  حرف بفر[4 * 1024];
  بنية TELNET *tn;

  *پورا = TRUE; /* unconditionally */

  نتیجہ = init_telnet(انکڑے);
  إذا(نتیجہ)
    أرجع نتیجہ;

  tn = Curl_meta_get(انکڑے, CURL_META_TELNET_EASY);
  إذا(!tn)
    أرجع CURLE_FAILED_INIT;

  نتیجہ = check_telnet_options(انکڑے, tn);
  إذا(نتیجہ)
    أرجع نتیجہ;

#جے_تعریف_شدہ USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  إذا(event_handle == WSA_INVALID_EVENT) {
    failf(انکڑے, "WSACreateEvent failed (%d)", SOCKERRNO);
    أرجع CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  إذا(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    أرجع CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_داخلہ_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  إذا(GetFileType(stdin_handle) == FILE_TYPE_PIPE || انکڑے->سیٹ.is_fread_سیٹ) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_گنتی = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  وإلا {
    obj_گنتی = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  طالما(keepon) {
    ثابت DWORD buf_ناپ = (DWORD)حجم(بفر);
    DWORD waitret = Wait_For_مضرب_Objects(obj_گنتی, objs,
                                           FALSE, wait_timeout);
    بدل(waitret) {

    حالة WAIT_TIMEOUT: {
      لكل(;;) {
        إذا(انکڑے->سیٹ.is_fread_سیٹ) {
          size_t n;
          /* read from user-supplied method */
          n = انکڑے->state.fread_func(بفر, 1, buf_ناپ, انکڑے->state.وچ);
          إذا(n == CURL_READFUNC_ABORT) {
            failf(انکڑے, "operation aborted by callback");
            keepon = FALSE;
            نتیجہ = CURLE_ABORTED_BY_CALLBACK;
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
            نتیجہ = CURLE_READ_ERROR;
            اكسر;
          }

          إذا(!readfile_read)
            اكسر;

          إذا(!ReadFile(stdin_handle, بفر, buf_ناپ, &readfile_read, عدم)) {
            keepon = FALSE;
            نتیجہ = CURLE_READ_ERROR;
            اكسر;
          }
        }

        نتیجہ = send_telnet_انکڑے(انکڑے, tn, بفر, readfile_read);
        إذا(نتیجہ) {
          keepon = FALSE;
          اكسر;
        }
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0 + 1: {
      إذا(!ReadFile(stdin_handle, بفر, buf_ناپ, &readfile_read, عدم)) {
        keepon = FALSE;
        نتیجہ = CURLE_READ_ERROR;
        اكسر;
      }

      نتیجہ = send_telnet_انکڑے(انکڑے, tn, بفر, readfile_read);
      إذا(نتیجہ) {
        keepon = FALSE;
        اكسر;
      }
    }
    اكسر;

    حالة WAIT_OBJECT_0: {
      events.l_جال_Events = 0;
      إذا(WSAEnum_جال_Events(sockfd, event_handle, &events) != 0) {
        صحيح sockerr = SOCKERRNO;
        إذا(sockerr != SOCKEINPROGRESS) {
          infof(انکڑے, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          نتیجہ = CURLE_READ_ERROR;
        }
        اكسر;
      }
      إذا(events.l_جال_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        نتیجہ = Curl_xfer_recv(انکڑے, بفر, حجم(بفر), &nread);
        /* read would have blocked. Loop again */
        إذا(نتیجہ == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        وإلا إذا(نتیجہ) {
          keepon = FALSE;
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا إذا(!nread) {
          keepon = FALSE;
          اكسر;
        }

        نتیجہ = telrcv(انکڑے, tn, (بنانشانی حرف *)بفر, nread);
        إذا(نتیجہ) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        إذا(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(انکڑے, tn);
          tn->already_negotiated = 1;
        }
      }
      إذا(events.l_جال_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      اكسر;
    }
    } /* switch */

    إذا(Curl_timeleft_ms(انکڑے) < 0) {
      failf(انکڑے, "Time-out");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  إذا(!WSACloseEvent(event_handle)) {
    infof(انکڑے, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#وإلا
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  إذا(انکڑے->سیٹ.is_fread_سیٹ) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  وإلا {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)انکڑے->state.وچ);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    إذا(pfd[1].fd < 0) {
      failf(انکڑے, "cannot read input");
      نتیجہ = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  طالما(keepon) {
    DEBUGF(infof(انکڑے, "telnet_do, poll %d fds", poll_cnt));
    بدل(Curl_poll(pfd, (بنانشانی صحيح)poll_cnt, interval_ms)) {
    حالة -1:                    /* error, stop reading */
      نتیجہ = CURLE_RECV_ERROR;
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
        نتیجہ = Curl_xfer_recv(انکڑے, بفر, حجم(بفر), &nread);
        /* read would have blocked. Loop again */
        إذا(نتیجہ == CURLE_AGAIN)
          اكسر;
        /* returned not-zero, this an error */
        إذا(نتیجہ) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          إذا(انکڑے->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(انکڑے, "telnet_do, unexpected ECONNRESET on recv"));
          }
          اكسر;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        وإلا إذا(!nread) {
          keepon = FALSE;
          اكسر;
        }

        Curl_pgrs_download_inc(انکڑے, nread);
        نتیجہ = telrcv(انکڑے, tn, (بنانشانی حرف *)بفر, nread);
        إذا(نتیجہ) {
          keepon = FALSE;
          اكسر;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        إذا(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(انکڑے, tn);
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
        snread = (صحيح)انکڑے->state.fread_func(بفر, 1, حجم(بفر),
                                             انکڑے->state.وچ);
        إذا(snread == CURL_READFUNC_ABORT) {
          failf(انکڑے, "operation aborted by callback");
          نتیجہ = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          اكسر;
        }
        إذا(snread == CURL_READFUNC_PAUSE)
          اكسر;
      }

      إذا(snread > 0) {
        نتیجہ = send_telnet_انکڑے(انکڑے, tn, بفر, snread);
        إذا(نتیجہ) {
          keepon = FALSE;
          اكسر;
        }
        Curl_pgrs_upload_inc(انکڑے, (size_t)snread);
      }
      وإلا إذا(snread < 0) {
        failf(انکڑے, "Failed to read input");
        نتیجہ = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      اكسر;
    } /* poll switch statement */

    إذا(Curl_timeleft_ms(انکڑے) < 0) {
      failf(انکڑے, "Time-out");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    إذا(!نتیجہ) {
      نتیجہ = Curl_pgrsUpdate(انکڑے);
      إذا(نتیجہ)
        keepon = FALSE;
    }
  }
#جے_انت
  /* mark this as "no further transfer wanted" */
  Curl_xfer_تیاری_nop(انکڑے);

  أرجع نتیجہ;
}

/*
 * TELNET protocol handler.
 */
ثابت بنية Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_پورا,                          /* done */
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

#جے_انت /* !CURL_DISABLE_TELNET */
