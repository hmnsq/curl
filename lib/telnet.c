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
#شامل "curl_setup.h"
#شامل "urldata.h"
#شامل "telnet.h"

#اگر_غیر_تعریف_شدہ CURL_DISABLE_TELNET

#اگر_تعریف_شدہ HAVE_NETINET_اندر_H
#شامل <netinet/in.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NETDB_H
#شامل <netdb.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_ARPA_INET_H
#شامل <arpa/inet.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NET_IF_H
#شامل <net/if.h>
#اگر_ختم
#اگر_تعریف_شدہ HAVE_SYS_IOCTL_H
#شامل <sys/ioctl.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SYS_PARAM_H
#شامل <sys/param.h>
#اگر_ختم

#شامل "url.h"
#شامل "transfer.h"
#شامل "sendf.h"
#شامل "curl_trc.h"
#شامل "progress.h"
#شامل "arpa_telnet.h"
#شامل "connect.h"
#شامل "select.h"
#شامل "curlx/strparse.h"

#تعریف_کرو SUBBUFSIZE 512

#تعریف_کرو CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#تعریف_کرو CURL_SB_رکن(x)            \
  کرو {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } جب(0)
#تعریف_کرو CURL_SB_ACCUM(x, c)                                         \
  کرو {                                                              \
    اگر((x)->subpointer < ((x)->subbuffer + حجم((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } جب(0)

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

#تعریف_کرو CURL_خالی_خانے       0
#تعریف_کرو CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#تعریف_کرو CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
قسمتعریف شمار {
  CURL_TS_ڈیٹا = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

ساخت TELNET {
  صحیح please_negotiate;
  صحیح already_negotiated;
  صحیح us[256];
  صحیح usq[256];
  صحیح us_preferred[256];
  صحیح him[256];
  صحیح himq[256];
  صحیح him_preferred[256];
  صحیح subnegotiation[256];
  ثابت حرف *subopt_ttype;          /* Set with suboption TTYPE */
  ثابت حرف *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  غیرعلامتی مختصر subopt_wsx;         /* Set with suboption NAWS */
  غیرعلامتی مختصر subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  ساخت curl_slist *telnet_vars;    /* Environment variables */
  ساخت dynbuf برآمد;                 /* output buffer */

  /* suboptions */
  غیرعلامتی حرف subbuffer[SUBBUFSIZE];
  غیرعلامتی حرف *subpointer, *subend;      /* buffer for sub-options */
};

#اگر_غیر_تعریف_شدہ CURLVERBOSE
#تعریف_کرو printoption(a, b, c, d) Curl_nop_stmt
#ورنہ
ساکن خالی printoption(ساخت Curl_easy *ڈیٹا,
                        ثابت حرف *direction, صحیح cmd, صحیح option)
{
  اگر(ڈیٹا->سیٹ.verbose) {
    اگر(cmd == CURL_IAC) {
      اگر(CURL_TELCMD_OK(option))
        infof(ڈیٹا, "%s IAC %s", direction, CURL_TELCMD(option));
      ورنہ
        infof(ڈیٹا, "%s IAC %d", direction, option);
    }
    ورنہ {
      ثابت حرف *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      اگر(fmt) {
        ثابت حرف *opt;
        اگر(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        ورنہ اگر(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        ورنہ
          opt = تہی;

        اگر(opt)
          infof(ڈیٹا, "%s %s %s", direction, fmt, opt);
        ورنہ
          infof(ڈیٹا, "%s %s %d", direction, fmt, option);
      }
      ورنہ
        infof(ڈیٹا, "%s %d %d", direction, cmd, option);
    }
  }
}
#اگر_ختم /* !CURLVERBOSE */

ساکن خالی telnet_easy_dtor(ثابت خالی *کلید, size_t klen, خالی *اندراج_2)
{
  ساخت TELNET *tn = اندراج_2;
  (خالی)کلید;
  (خالی)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->برآمد);
  curlx_free(tn);
}

ساکن CURLcode init_telnet(ساخت Curl_easy *ڈیٹا)
{
  ساخت TELNET *tn;

  tn = curlx_calloc(1, حجم(ساخت TELNET));
  اگر(!tn)
    واپس CURLE_برآمد_کا_حافظہ;

  curlx_dyn_init(&tn->برآمد, 0xffff);

  tn->telrcv_state = CURL_TS_ڈیٹا;

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

  واپس Curl_meta_سیٹ(ڈیٹا, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ساکن خالی send_negotiation(ساخت Curl_easy *ڈیٹا, صحیح cmd, صحیح option)
{
  غیرعلامتی حرف buf[3];
  size_t nwritten;
  CURLcode نتیجہ;

  buf[0] = CURL_IAC;
  buf[1] = (غیرعلامتی حرف)cmd;
  buf[2] = (غیرعلامتی حرف)option;

  نتیجہ = Curl_xfer_send(ڈیٹا, buf, 3, FALSE, &nwritten);
  اگر(نتیجہ)
    failf(ڈیٹا, "Sending data failed: %s", curl_easy_strerror(نتیجہ));

  printoption(ڈیٹا, "SENT", cmd, option);
}

ساکن خالی سیٹ_remote_option(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn,
                              صحیح option, صحیح newstate)
{
  اگر(newstate == CURL_YES) {
    انتخاب(tn->him[option]) {
    حالت CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(ڈیٹا, CURL_DO, option);
      توڑو;

    حالت CURL_YES:
      /* Already enabled */
      توڑو;

    حالت CURL_WANTNO:
      انتخاب(tn->himq[option]) {
      حالت CURL_خالی_خانے:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        توڑو;
      حالت CURL_OPPOSITE:
        /* Error: already queued an enable request */
        توڑو;
      }
      توڑو;

    حالت CURL_WANTYES:
      انتخاب(tn->himq[option]) {
      حالت CURL_خالی_خانے:
        /* Error: already negotiating for enable */
        توڑو;
      حالت CURL_OPPOSITE:
        tn->himq[option] = CURL_خالی_خانے;
        توڑو;
      }
      توڑو;
    }
  }
  ورنہ { /* NO */
    انتخاب(tn->him[option]) {
    حالت CURL_NO:
      /* Already disabled */
      توڑو;

    حالت CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(ڈیٹا, CURL_DONT, option);
      توڑو;

    حالت CURL_WANTNO:
      انتخاب(tn->himq[option]) {
      حالت CURL_خالی_خانے:
        /* Already negotiating for NO */
        توڑو;
      حالت CURL_OPPOSITE:
        tn->himq[option] = CURL_خالی_خانے;
        توڑو;
      }
      توڑو;

    حالت CURL_WANTYES:
      انتخاب(tn->himq[option]) {
      حالت CURL_خالی_خانے:
        tn->himq[option] = CURL_OPPOSITE;
        توڑو;
      حالت CURL_OPPOSITE:
        توڑو;
      }
      توڑو;
    }
  }
}

ساکن خالی سیٹ_local_option(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn,
                             صحیح option, صحیح newstate)
{
  اگر(newstate == CURL_YES) {
    انتخاب(tn->us[option]) {
    حالت CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(ڈیٹا, CURL_WILL, option);
      توڑو;

    حالت CURL_YES:
      /* Already enabled */
      توڑو;

    حالت CURL_WANTNO:
      انتخاب(tn->usq[option]) {
      حالت CURL_خالی_خانے:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        توڑو;
      حالت CURL_OPPOSITE:
        /* Error: already queued an enable request */
        توڑو;
      }
      توڑو;

    حالت CURL_WANTYES:
      انتخاب(tn->usq[option]) {
      حالت CURL_خالی_خانے:
        /* Error: already negotiating for enable */
        توڑو;
      حالت CURL_OPPOSITE:
        tn->usq[option] = CURL_خالی_خانے;
        توڑو;
      }
      توڑو;
    }
  }
  ورنہ { /* NO */
    انتخاب(tn->us[option]) {
    حالت CURL_NO:
      /* Already disabled */
      توڑو;

    حالت CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(ڈیٹا, CURL_WONT, option);
      توڑو;

    حالت CURL_WANTNO:
      انتخاب(tn->usq[option]) {
      حالت CURL_خالی_خانے:
        /* Already negotiating for NO */
        توڑو;
      حالت CURL_OPPOSITE:
        tn->usq[option] = CURL_خالی_خانے;
        توڑو;
      }
      توڑو;

    حالت CURL_WANTYES:
      انتخاب(tn->usq[option]) {
      حالت CURL_خالی_خانے:
        tn->usq[option] = CURL_OPPOSITE;
        توڑو;
      حالت CURL_OPPOSITE:
        توڑو;
      }
      توڑو;
    }
  }
}

ساکن خالی telnet_negotiate(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn)
{
  صحیح i;

  برائے(i = 0; i < CURL_NTELOPTS; i++) {
    اگر(i == CURL_TELOPT_ECHO)
      جاری;

    اگر(tn->us_preferred[i] == CURL_YES)
      سیٹ_local_option(ڈیٹا, tn, i, CURL_YES);

    اگر(tn->him_preferred[i] == CURL_YES)
      سیٹ_remote_option(ڈیٹا, tn, i, CURL_YES);
  }
}

ساکن خالی rec_will(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn, صحیح option)
{
  انتخاب(tn->him[option]) {
  حالت CURL_NO:
    اگر(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(ڈیٹا, CURL_DO, option);
    }
    ورنہ
      send_negotiation(ڈیٹا, CURL_DONT, option);

    توڑو;

  حالت CURL_YES:
    /* Already enabled */
    توڑو;

  حالت CURL_WANTNO:
    انتخاب(tn->himq[option]) {
    حالت CURL_خالی_خانے:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      توڑو;
    حالت CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_خالی_خانے;
      توڑو;
    }
    توڑو;

  حالت CURL_WANTYES:
    انتخاب(tn->himq[option]) {
    حالت CURL_خالی_خانے:
      tn->him[option] = CURL_YES;
      توڑو;
    حالت CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_خالی_خانے;
      send_negotiation(ڈیٹا, CURL_DONT, option);
      توڑو;
    }
    توڑو;
  }
}

ساکن خالی rec_wont(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn, صحیح option)
{
  انتخاب(tn->him[option]) {
  حالت CURL_NO:
    /* Already disabled */
    توڑو;

  حالت CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(ڈیٹا, CURL_DONT, option);
    توڑو;

  حالت CURL_WANTNO:
    انتخاب(tn->himq[option]) {
    حالت CURL_خالی_خانے:
      tn->him[option] = CURL_NO;
      توڑو;

    حالت CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_خالی_خانے;
      send_negotiation(ڈیٹا, CURL_DO, option);
      توڑو;
    }
    توڑو;

  حالت CURL_WANTYES:
    انتخاب(tn->himq[option]) {
    حالت CURL_خالی_خانے:
      tn->him[option] = CURL_NO;
      توڑو;
    حالت CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_خالی_خانے;
      توڑو;
    }
    توڑو;
  }
}

ساکن خالی printsub(ساخت Curl_easy *ڈیٹا,
                     صحیح direction,                /* '<' or '>' */
                     ثابت غیرعلامتی حرف *اشارہ_گر, /* ptr to suboption data */
                     size_t لمبائی)                /* suboption data length */
{
  اگر(ڈیٹا->سیٹ.verbose) {
    غیرعلامتی صحیح i = 0;
    اگر(direction) {
      infof(ڈیٹا, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      اگر(لمبائی >= 3) {
        صحیح j;

        i = اشارہ_گر[لمبائی - 2];
        j = اشارہ_گر[لمبائی - 1];

        اگر(i != CURL_IAC || j != CURL_SE) {
          infof(ڈیٹا, "(terminated by ");
          اگر(CURL_TELOPT_OK(i))
            infof(ڈیٹا, "%s ", CURL_TELOPT(i));
          ورنہ اگر(CURL_TELCMD_OK(i))
            infof(ڈیٹا, "%s ", CURL_TELCMD(i));
          ورنہ
            infof(ڈیٹا, "%u ", i);
          اگر(CURL_TELOPT_OK(j))
            infof(ڈیٹا, "%s", CURL_TELOPT(j));
          ورنہ اگر(CURL_TELCMD_OK(j))
            infof(ڈیٹا, "%s", CURL_TELCMD(j));
          ورنہ
            infof(ڈیٹا, "%d", j);
          infof(ڈیٹا, ", not IAC SE) ");
        }
      }
      اگر(لمبائی >= 2)
        لمبائی -= 2;
      ورنہ /* bad input */
        واپس;
    }
    اگر(لمبائی <= 1) {
      infof(ڈیٹا, "(Empty suboption?)");
      واپس;
    }

    اگر(CURL_TELOPT_OK(اشارہ_گر[0])) {
      انتخاب(اشارہ_گر[0]) {
      حالت CURL_TELOPT_TTYPE:
      حالت CURL_TELOPT_XDISPLOC:
      حالت CURL_TELOPT_NEW_ENVIRON:
      حالت CURL_TELOPT_NAWS:
        infof(ڈیٹا, "%s", CURL_TELOPT(اشارہ_گر[0]));
        توڑو;
      طےشدہ:
        infof(ڈیٹا, "%s (unsupported)", CURL_TELOPT(اشارہ_گر[0]));
        توڑو;
      }
    }
    ورنہ
      infof(ڈیٹا, "%d (unknown)", اشارہ_گر[0]);

    انتخاب(اشارہ_گر[0]) {
    حالت CURL_TELOPT_NAWS:
      اگر(لمبائی > 4)
        infof(ڈیٹا, "Width: %d ; Height: %d", (اشارہ_گر[1] << 8) | اشارہ_گر[2],
              (اشارہ_گر[3] << 8) | اشارہ_گر[4]);
      توڑو;
    طےشدہ:
      انتخاب(اشارہ_گر[1]) {
      حالت CURL_TELQUAL_IS:
        infof(ڈیٹا, " IS");
        توڑو;
      حالت CURL_TELQUAL_SEND:
        infof(ڈیٹا, " SEND");
        توڑو;
      حالت CURL_TELQUAL_INFO:
        infof(ڈیٹا, " INFO/REPLY");
        توڑو;
      حالت CURL_TELQUAL_NAME:
        infof(ڈیٹا, " NAME");
        توڑو;
      }

      انتخاب(اشارہ_گر[0]) {
      حالت CURL_TELOPT_TTYPE:
      حالت CURL_TELOPT_XDISPLOC:
        infof(ڈیٹا, " \"%.*s\"",
              (صحیح)((لمبائی > 2) ? (لمبائی - 2) : 0), &اشارہ_گر[2]);
        توڑو;
      حالت CURL_TELOPT_NEW_ENVIRON:
        اگر(اشارہ_گر[1] == CURL_TELQUAL_IS) {
          infof(ڈیٹا, " ");
          برائے(i = 3; i < لمبائی; i++) {
            انتخاب(اشارہ_گر[i]) {
            حالت CURL_NEW_ENV_VAR:
              infof(ڈیٹا, ", ");
              توڑو;
            حالت CURL_NEW_ENV_قدر:
              infof(ڈیٹا, " = ");
              توڑو;
            طےشدہ:
              infof(ڈیٹا, "%c", اشارہ_گر[i]);
              توڑو;
            }
          }
        }
        توڑو;
      طےشدہ:
        برائے(i = 2; i < لمبائی; i++)
          infof(ڈیٹا, " %.2x", اشارہ_گر[i]);
        توڑو;
      }
    }
  }
}

/* Escape and send a telnet data block */
ساکن CURLcode send_telnet_ڈیٹا(ساخت Curl_easy *ڈیٹا,
                                 ساخت TELNET *tn,
                                 ثابت حرف *بفر, ssize_t nread)
{
  size_t i, outlen;
  ثابت غیرعلامتی حرف *outbuf;
  CURLcode نتیجہ = CURLE_OK;
  size_t بائٹیں_written;
  size_t کل_written = 0;
  ساخت connectdata *conn = ڈیٹا->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  اگر(nread < 0)
    واپس CURLE_TOO_LARGE;

  اگر(memchr(بفر, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->برآمد);

    برائے(i = 0; i < (size_t)nread && !نتیجہ; i++) {
      نتیجہ = curlx_dyn_addn(&tn->برآمد, &بفر[i], 1);
      اگر(!نتیجہ && ((غیرعلامتی حرف)بفر[i] == CURL_IAC))
        /* IAC is FF in hex */
        نتیجہ = curlx_dyn_addn(&tn->برآمد, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->برآمد);
    outbuf = curlx_dyn_uptr(&tn->برآمد);
  }
  ورنہ {
    outlen = (size_t)nread;
    outbuf = (ثابت غیرعلامتی حرف *)بفر;
  }
  جب(!نتیجہ && کل_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    ساخت pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(ڈیٹا);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    اگر(timeout_ms < 0)
      واپس CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    انتخاب(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    حالت -1:                    /* error, abort writing */
      نتیجہ = CURLE_SEND_ERROR;
      توڑو;
    حالت 0:                     /* timeout */
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      توڑو;
    طےشدہ:                    /* write! */
      بائٹیں_written = 0;
      نتیجہ = Curl_xfer_send(ڈیٹا, outbuf + کل_written,
                              outlen - کل_written, FALSE, &بائٹیں_written);
      کل_written += بائٹیں_written;
      توڑو;
    }
  }

  واپس نتیجہ;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ساکن خالی sendsuboption(ساخت Curl_easy *ڈیٹا,
                          ساخت TELNET *tn, صحیح option)
{
  size_t nwritten;
  غیرعلامتی مختصر x, y;
  ثابت غیرعلامتی حرف *uc1, *uc2;
  CURLcode نتیجہ;

  انتخاب(option) {
  حالت CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ثابت غیرعلامتی حرف *)&x;
    uc2 = (ثابت غیرعلامتی حرف *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_رکن(tn);
    /* data suboption is now ready */

    printsub(ڈیٹا, '>', (ثابت غیرعلامتی حرف *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    نتیجہ = Curl_xfer_send(ڈیٹا, tn->subbuffer, 3, FALSE, &nwritten);
    اگر(نتیجہ)
      failf(ڈیٹا, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_ڈیٹا(ڈیٹا, tn, (ثابت حرف *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    نتیجہ = Curl_xfer_send(ڈیٹا, tn->subbuffer + 7, 2, FALSE, &nwritten);
    اگر(نتیجہ)
      failf(ڈیٹا, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
    توڑو;
  }
}

ساکن خالی rec_do(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn, صحیح option)
{
  انتخاب(tn->us[option]) {
  حالت CURL_NO:
    اگر(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(ڈیٹا, CURL_WILL, option);
      اگر(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(ڈیٹا, tn, option);
    }
    ورنہ اگر(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(ڈیٹا, CURL_WILL, option);
      sendsuboption(ڈیٹا, tn, option);
    }
    ورنہ
      send_negotiation(ڈیٹا, CURL_WONT, option);
    توڑو;

  حالت CURL_YES:
    /* Already enabled */
    توڑو;

  حالت CURL_WANTNO:
    انتخاب(tn->usq[option]) {
    حالت CURL_خالی_خانے:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      توڑو;
    حالت CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_خالی_خانے;
      توڑو;
    }
    توڑو;

  حالت CURL_WANTYES:
    انتخاب(tn->usq[option]) {
    حالت CURL_خالی_خانے:
      tn->us[option] = CURL_YES;
      اگر(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(ڈیٹا, tn, option);
      }
      توڑو;
    حالت CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_خالی_خانے;
      send_negotiation(ڈیٹا, CURL_WONT, option);
      توڑو;
    }
    توڑو;
  }
}

ساکن خالی rec_dont(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn, صحیح option)
{
  انتخاب(tn->us[option]) {
  حالت CURL_NO:
    /* Already disabled */
    توڑو;

  حالت CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(ڈیٹا, CURL_WONT, option);
    توڑو;

  حالت CURL_WANTNO:
    انتخاب(tn->usq[option]) {
    حالت CURL_خالی_خانے:
      tn->us[option] = CURL_NO;
      توڑو;

    حالت CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_خالی_خانے;
      send_negotiation(ڈیٹا, CURL_WILL, option);
      توڑو;
    }
    توڑو;

  حالت CURL_WANTYES:
    انتخاب(tn->usq[option]) {
    حالت CURL_خالی_خانے:
      tn->us[option] = CURL_NO;
      توڑو;
    حالت CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_خالی_خانے;
      توڑو;
    }
    توڑو;
  }
}

ساکن bool str_is_nonascii(ثابت حرف *str)
{
  حرف c;
  جب((c = *str++) != 0)
    اگر(c & 0x80)
      واپس TRUE;

  واپس FALSE;
}

ساکن CURLcode check_telnet_options(ساخت Curl_easy *ڈیٹا,
                                     ساخت TELNET *tn)
{
  ساخت curl_slist *سر;
  ساخت curl_slist *beg;
  CURLcode نتیجہ = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  اگر(ڈیٹا->state.creds) {
    حرف بفر[256];
    اگر(str_is_nonascii(Curl_creds_user(ڈیٹا->conn->creds))) {
      DEBUGF(infof(ڈیٹا, "set a non ASCII username in telnet"));
      واپس CURLE_BAD_FUNCTION_دلیل;
    }
    curl_msnprintf(بفر, حجم(بفر), "USER,%s",
                   Curl_creds_user(ڈیٹا->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, بفر);
    اگر(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = تہی;
      واپس CURLE_برآمد_کا_حافظہ;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  برائے(سر = ڈیٹا->سیٹ.telnet_options; سر && !نتیجہ; سر = سر->اگلا) {
    size_t olen;
    ثابت حرف *option = سر->ڈیٹا;
    ثابت حرف *arg;
    ثابت حرف *sep = strchr(option, '=');
    اگر(sep) {
      olen = sep - option;
      arg = ++sep;
      اگر(str_is_nonascii(arg))
        جاری;
      انتخاب(olen) {
      حالت 5:
        /* Terminal type */
        اگر(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          توڑو;
        }
        نتیجہ = CURLE_UNKNOWN_OPTION;
        توڑو;

      حالت 8:
        /* Display variable */
        اگر(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          توڑو;
        }
        نتیجہ = CURLE_UNKNOWN_OPTION;
        توڑو;

      حالت 7:
        /* Environment variable */
        اگر(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          اگر(!beg) {
            نتیجہ = CURLE_برآمد_کا_حافظہ;
            توڑو;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        ورنہ
          نتیجہ = CURLE_UNKNOWN_OPTION;
        توڑو;

      حالت 2:
        /* Window Size */
        اگر(curl_strnequal(option, "WS", 2)) {
          ثابت حرف *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          اگر(curlx_str_نمبر(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_نمبر(&p, &y, 0xffff)) {
            failf(ڈیٹا, "Syntax error in telnet option: %s", سر->ڈیٹا);
            نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;
          }
          ورنہ {
            tn->subopt_wsx = (غیرعلامتی مختصر)x;
            tn->subopt_wsy = (غیرعلامتی مختصر)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        ورنہ
          نتیجہ = CURLE_UNKNOWN_OPTION;
        توڑو;

      حالت 6:
        /* To take care or not of the 8th bit in data exchange */
        اگر(curl_strnequal(option, "BINARY", 6)) {
          ثابت حرف *p = arg;
          curl_off_t ثنائی_option;
          اگر(!curlx_str_نمبر(&p, &ثنائی_option, 1) &&
             (ثنائی_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ثنائی] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ثنائی] = CURL_NO;
          }
        }
        ورنہ
          نتیجہ = CURLE_UNKNOWN_OPTION;
        توڑو;
      طےشدہ:
        failf(ڈیٹا, "Unknown telnet option %s", سر->ڈیٹا);
        نتیجہ = CURLE_UNKNOWN_OPTION;
        توڑو;
      }
    }
    ورنہ {
      failf(ڈیٹا, "Syntax error in telnet option: %s", سر->ڈیٹا);
      نتیجہ = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  اگر(نتیجہ) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = تہی;
  }

  واپس نتیجہ;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ساکن bool bad_option(ثابت حرف *ڈیٹا)
{
  واپس !ڈیٹا || !!strchr(ڈیٹا, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ساکن CURLcode suboption(ساخت Curl_easy *ڈیٹا, ساخت TELNET *tn)
{
  ساخت curl_slist *v;
  غیرعلامتی حرف عارضی[2048];
  size_t nwritten;
  size_t len;
  CURLcode نتیجہ = CURLE_OK;

  اگر(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    واپس CURLE_OK;

  printsub(ڈیٹا, '<', (ثابت غیرعلامتی حرف *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  انتخاب(CURL_SB_GET(tn)) {
  حالت CURL_TELOPT_TTYPE:
    اگر(bad_option(tn->subopt_ttype))
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(لڑی_لمبائی(tn->subopt_ttype) > 1000) {
      failf(ڈیٹا, "Too long telnet TTYPE");
      واپس CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)عارضی, حجم(عارضی), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    نتیجہ = Curl_xfer_send(ڈیٹا, عارضی, len, FALSE, &nwritten);
    اگر(نتیجہ) {
      failf(ڈیٹا, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
      واپس نتیجہ;
    }
    printsub(ڈیٹا, '>', &عارضی[2], len-2);
    توڑو;
  حالت CURL_TELOPT_XDISPLOC:
    اگر(bad_option(tn->subopt_xdisploc))
      واپس CURLE_BAD_FUNCTION_دلیل;
    اگر(لڑی_لمبائی(tn->subopt_xdisploc) > 1000) {
      failf(ڈیٹا, "Too long telnet XDISPLOC");
      واپس CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((حرف *)عارضی, حجم(عارضی), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    نتیجہ = Curl_xfer_send(ڈیٹا, عارضی, len, FALSE, &nwritten);
    اگر(نتیجہ) {
      failf(ڈیٹا, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
      واپس نتیجہ;
    }
    printsub(ڈیٹا, '>', &عارضی[2], len - 2);
    توڑو;
  حالت CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((حرف *)عارضی, حجم(عارضی), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    برائے(v = tn->telnet_vars; v; v = v->اگلا) {
      size_t tmplen = (لڑی_لمبائی(v->ڈیٹا) + 1);
      اگر(bad_option(v->ڈیٹا))
        واپس CURLE_BAD_FUNCTION_دلیل;
      /* Add the variable if it fits */
      اگر(len + tmplen < حجم(عارضی) - 6) {
        ثابت حرف *s = strchr(v->ڈیٹا, ',');
        اگر(!s)
          len += curl_msnprintf((حرف *)&عارضی[len], حجم(عارضی) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->ڈیٹا);
        ورنہ {
          size_t vlen = s - v->ڈیٹا;
          len += curl_msnprintf((حرف *)&عارضی[len], حجم(عارضی) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (صحیح)vlen, v->ڈیٹا, CURL_NEW_ENV_قدر, ++s);
        }
      }
    }
    curl_msnprintf((حرف *)&عارضی[len], حجم(عارضی) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    نتیجہ = Curl_xfer_send(ڈیٹا, عارضی, len, FALSE, &nwritten);
    اگر(نتیجہ) {
      failf(ڈیٹا, "Sending data failed: %s", curl_easy_strerror(نتیجہ));
      واپس نتیجہ;
    }
    printsub(ڈیٹا, '>', &عارضی[2], len - 2);
    توڑو;
  }
  واپس CURLE_OK;
}

ساکن CURLcode telrcv(ساخت Curl_easy *ڈیٹا,
                       ساخت TELNET *tn,
                       ثابت غیرعلامتی حرف *inbuf, /* Data received from
                                                      socket */
                       ssize_t گنتی)              /* Number of bytes
                                                      received */
{
  غیرعلامتی حرف c;
  CURLcode نتیجہ;
  صحیح اندر = 0;
  صحیح startwrite = -1;

#تعریف_کرو startskipping()                                          \
  اگر(startwrite >= 0) {                                          \
    نتیجہ = Curl_client_write(ڈیٹا,                             \
                               CLIENTWRITE_BODY,                 \
                               (ثابت حرف *)&inbuf[startwrite], \
                               اندر-startwrite);                   \
    اگر(نتیجہ)                                                   \
      واپس نتیجہ;                                             \
  }                                                              \
  startwrite = -1

#تعریف_کرو writebyte()  \
  اگر(startwrite < 0) \
    startwrite = اندر

#تعریف_کرو bufferflush() startskipping()

  جب(گنتی--) {
    c = inbuf[اندر];

    انتخاب(tn->telrcv_state) {
    حالت CURL_TS_CR:
      tn->telrcv_state = CURL_TS_ڈیٹا;
      اگر(c == '\0') {
        startskipping();
        توڑو;   /* Ignore \0 after CR */
      }
      writebyte();
      توڑو;

    حالت CURL_TS_ڈیٹا:
      اگر(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        توڑو;
      }
      ورنہ اگر(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      توڑو;

    حالت CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      انتخاب(c) {
      حالت CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        توڑو;
      حالت CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        توڑو;
      حالت CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        توڑو;
      حالت CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        توڑو;
      حالت CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        توڑو;
      حالت CURL_IAC:
        tn->telrcv_state = CURL_TS_ڈیٹا;
        writebyte();
        توڑو;
      حالت CURL_DM:
      حالت CURL_NOP:
      حالت CURL_GA:
      طےشدہ:
        tn->telrcv_state = CURL_TS_ڈیٹا;
        printoption(ڈیٹا, "RCVD", CURL_IAC, c);
        توڑو;
      }
      توڑو;

    حالت CURL_TS_WILL:
      printoption(ڈیٹا, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(ڈیٹا, tn, c);
      tn->telrcv_state = CURL_TS_ڈیٹا;
      توڑو;

    حالت CURL_TS_WONT:
      printoption(ڈیٹا, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(ڈیٹا, tn, c);
      tn->telrcv_state = CURL_TS_ڈیٹا;
      توڑو;

    حالت CURL_TS_DO:
      printoption(ڈیٹا, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(ڈیٹا, tn, c);
      tn->telrcv_state = CURL_TS_ڈیٹا;
      توڑو;

    حالت CURL_TS_DONT:
      printoption(ڈیٹا, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(ڈیٹا, tn, c);
      tn->telrcv_state = CURL_TS_ڈیٹا;
      توڑو;

    حالت CURL_TS_SB:
      اگر(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      ورنہ
        CURL_SB_ACCUM(tn, c);
      توڑو;

    حالت CURL_TS_SE:
      اگر(c != CURL_SE) {
        اگر(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(ڈیٹا, "telnet: suboption error");
          واپس CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      ورنہ {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_رکن(tn);
        نتیجہ = suboption(ڈیٹا, tn);   /* handle sub-option */
        اگر(نتیجہ)
          واپس نتیجہ;
        tn->telrcv_state = CURL_TS_ڈیٹا;
      }
      توڑو;
    }
    ++اندر;
  }
  bufferflush();
  واپس CURLE_OK;
}

ساکن CURLcode telnet_مکمل(ساخت Curl_easy *ڈیٹا,
                            CURLcode حالت_2, bool premature)
{
  (خالی)حالت_2;
  (خالی)premature;
  Curl_meta_remove(ڈیٹا, CURL_META_TELNET_EASY);
  واپس CURLE_OK;
}

ساکن CURLcode telnet_do(ساخت Curl_easy *ڈیٹا, bool *مکمل)
{
  CURLcode نتیجہ;
  ساخت connectdata *conn = ڈیٹا->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#اگر_تعریف_شدہ USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_گنتی;
  DWORD wait_timeout;
  DWORD readfile_read;
#ورنہ
  timediff_t interval_ms;
  ساخت pollfd pfd[2];
  صحیح poll_cnt;
  ssize_t snread;
#اگر_ختم
  bool keepon = TRUE;
  حرف بفر[4 * 1024];
  ساخت TELNET *tn;

  *مکمل = TRUE; /* unconditionally */

  نتیجہ = init_telnet(ڈیٹا);
  اگر(نتیجہ)
    واپس نتیجہ;

  tn = Curl_meta_get(ڈیٹا, CURL_META_TELNET_EASY);
  اگر(!tn)
    واپس CURLE_FAILED_INIT;

  نتیجہ = check_telnet_options(ڈیٹا, tn);
  اگر(نتیجہ)
    واپس نتیجہ;

#اگر_تعریف_شدہ USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  اگر(event_handle == WSA_INVALID_EVENT) {
    failf(ڈیٹا, "WSACreateEvent failed (%d)", SOCKERRNO);
    واپس CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  اگر(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    واپس CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_داخلہ_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  اگر(GetFileType(stdin_handle) == FILE_TYPE_PIPE || ڈیٹا->سیٹ.is_fread_سیٹ) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_گنتی = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  ورنہ {
    obj_گنتی = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  جب(keepon) {
    ثابت DWORD buf_سائز = (DWORD)حجم(بفر);
    DWORD waitret = Wait_For_مضرب_Objects(obj_گنتی, objs,
                                           FALSE, wait_timeout);
    انتخاب(waitret) {

    حالت WAIT_TIMEOUT: {
      برائے(;;) {
        اگر(ڈیٹا->سیٹ.is_fread_سیٹ) {
          size_t n;
          /* read from user-supplied method */
          n = ڈیٹا->state.fread_func(بفر, 1, buf_سائز, ڈیٹا->state.اندر);
          اگر(n == CURL_READFUNC_ABORT) {
            failf(ڈیٹا, "operation aborted by callback");
            keepon = FALSE;
            نتیجہ = CURLE_ABORTED_BY_CALLBACK;
            توڑو;
          }

          اگر(n == CURL_READFUNC_PAUSE)
            توڑو;

          اگر(n == 0)                        /* no bytes */
            توڑو;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        ورنہ {
          /* read from stdin */
          اگر(!PeekNamedPipe(stdin_handle, تہی, 0, تہی,
                            &readfile_read, تہی)) {
            keepon = FALSE;
            نتیجہ = CURLE_READ_ERROR;
            توڑو;
          }

          اگر(!readfile_read)
            توڑو;

          اگر(!ReadFile(stdin_handle, بفر, buf_سائز, &readfile_read, تہی)) {
            keepon = FALSE;
            نتیجہ = CURLE_READ_ERROR;
            توڑو;
          }
        }

        نتیجہ = send_telnet_ڈیٹا(ڈیٹا, tn, بفر, readfile_read);
        اگر(نتیجہ) {
          keepon = FALSE;
          توڑو;
        }
      }
    }
    توڑو;

    حالت WAIT_OBJECT_0 + 1: {
      اگر(!ReadFile(stdin_handle, بفر, buf_سائز, &readfile_read, تہی)) {
        keepon = FALSE;
        نتیجہ = CURLE_READ_ERROR;
        توڑو;
      }

      نتیجہ = send_telnet_ڈیٹا(ڈیٹا, tn, بفر, readfile_read);
      اگر(نتیجہ) {
        keepon = FALSE;
        توڑو;
      }
    }
    توڑو;

    حالت WAIT_OBJECT_0: {
      events.l_جال_Events = 0;
      اگر(WSAEnum_جال_Events(sockfd, event_handle, &events) != 0) {
        صحیح sockerr = SOCKERRNO;
        اگر(sockerr != SOCKEINPROGRESS) {
          infof(ڈیٹا, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          نتیجہ = CURLE_READ_ERROR;
        }
        توڑو;
      }
      اگر(events.l_جال_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        نتیجہ = Curl_xfer_recv(ڈیٹا, بفر, حجم(بفر), &nread);
        /* read would have blocked. Loop again */
        اگر(نتیجہ == CURLE_AGAIN)
          توڑو;
        /* returned not-zero, this an error */
        ورنہ اگر(نتیجہ) {
          keepon = FALSE;
          توڑو;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ورنہ اگر(!nread) {
          keepon = FALSE;
          توڑو;
        }

        نتیجہ = telrcv(ڈیٹا, tn, (غیرعلامتی حرف *)بفر, nread);
        اگر(نتیجہ) {
          keepon = FALSE;
          توڑو;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        اگر(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(ڈیٹا, tn);
          tn->already_negotiated = 1;
        }
      }
      اگر(events.l_جال_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      توڑو;
    }
    } /* switch */

    اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
      failf(ڈیٹا, "Time-out");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  اگر(!WSACloseEvent(event_handle)) {
    infof(ڈیٹا, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#ورنہ
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  اگر(ڈیٹا->سیٹ.is_fread_سیٹ) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  ورنہ {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)ڈیٹا->state.اندر);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    اگر(pfd[1].fd < 0) {
      failf(ڈیٹا, "cannot read input");
      نتیجہ = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  جب(keepon) {
    DEBUGF(infof(ڈیٹا, "telnet_do, poll %d fds", poll_cnt));
    انتخاب(Curl_poll(pfd, (غیرعلامتی صحیح)poll_cnt, interval_ms)) {
    حالت -1:                    /* error, stop reading */
      نتیجہ = CURLE_RECV_ERROR;
      keepon = FALSE;
      جاری;
    حالت 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    طےشدہ:                    /* read! */
      اگر(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        نتیجہ = Curl_xfer_recv(ڈیٹا, بفر, حجم(بفر), &nread);
        /* read would have blocked. Loop again */
        اگر(نتیجہ == CURLE_AGAIN)
          توڑو;
        /* returned not-zero, this an error */
        اگر(نتیجہ) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          اگر(ڈیٹا->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(ڈیٹا, "telnet_do, unexpected ECONNRESET on recv"));
          }
          توڑو;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ورنہ اگر(!nread) {
          keepon = FALSE;
          توڑو;
        }

        Curl_pgrs_download_inc(ڈیٹا, nread);
        نتیجہ = telrcv(ڈیٹا, tn, (غیرعلامتی حرف *)بفر, nread);
        اگر(نتیجہ) {
          keepon = FALSE;
          توڑو;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        اگر(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(ڈیٹا, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      اگر(poll_cnt == 2) {
        اگر(pfd[1].revents & POLLIN) { /* read from in file */
          snread = اقرأ(pfd[1].fd, بفر, حجم(بفر));
        }
      }
      ورنہ {
        /* read from user-supplied method */
        snread = (صحیح)ڈیٹا->state.fread_func(بفر, 1, حجم(بفر),
                                             ڈیٹا->state.اندر);
        اگر(snread == CURL_READFUNC_ABORT) {
          failf(ڈیٹا, "operation aborted by callback");
          نتیجہ = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          توڑو;
        }
        اگر(snread == CURL_READFUNC_PAUSE)
          توڑو;
      }

      اگر(snread > 0) {
        نتیجہ = send_telnet_ڈیٹا(ڈیٹا, tn, بفر, snread);
        اگر(نتیجہ) {
          keepon = FALSE;
          توڑو;
        }
        Curl_pgrs_upload_inc(ڈیٹا, (size_t)snread);
      }
      ورنہ اگر(snread < 0) {
        failf(ڈیٹا, "Failed to read input");
        نتیجہ = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      توڑو;
    } /* poll switch statement */

    اگر(Curl_timeleft_ms(ڈیٹا) < 0) {
      failf(ڈیٹا, "Time-out");
      نتیجہ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    اگر(!نتیجہ) {
      نتیجہ = Curl_pgrsUpdate(ڈیٹا);
      اگر(نتیجہ)
        keepon = FALSE;
    }
  }
#اگر_ختم
  /* mark this as "no further transfer wanted" */
  Curl_xfer_تیاری_nop(ڈیٹا);

  واپس نتیجہ;
}

/*
 * TELNET protocol handler.
 */
ثابت ساخت Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_مکمل,                          /* done */
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

#اگر_ختم /* !CURL_DISABLE_TELNET */
