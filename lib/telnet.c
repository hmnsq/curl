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
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_setup.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "urldata.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "telnet.h"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURL_DISABLE_TELNET

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETINET_ਵਿੱਚ_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netinet/in.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NETDB_H
#ਅਨ੍ਤਰ੍ਭਾਵ <netdb.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ARPA_INET_H
#ਅਨ੍ਤਰ੍ਭਾਵ <arpa/inet.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NET_IF_H
#ਅਨ੍ਤਰ੍ਭਾਵ <net/if.h>
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_PARAM_H
#ਅਨ੍ਤਰ੍ਭਾਵ <sys/param.h>
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "url.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "transfer.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "sendf.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curl_trc.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "progress.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "arpa_telnet.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "connect.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "select.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "curlx/strparse.h"

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ SUBBUFSIZE 512

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SB_ਪਦ(x)            \
  ਕਰੋ {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } ਜਦੋਂ(0)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SB_ACCUM(x, c)                                         \
  ਕਰੋ {                                                              \
    ਜੇ((x)->subpointer < ((x)->subbuffer + ਆਕਾਰ((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } ਜਦੋਂ(0)

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_NO          0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_YES         1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_WANTYES     2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_WANTNO      3

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_ਖਾਲੀ_ਥਾਵਾਂ       0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਗਿਣਤੀ {
  CURL_TS_ਅੰਕੜੇ = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

ਬਣਤਰ TELNET {
  ਪੂਰਨ please_negotiate;
  ਪੂਰਨ already_negotiated;
  ਪੂਰਨ us[256];
  ਪੂਰਨ usq[256];
  ਪੂਰਨ us_preferred[256];
  ਪੂਰਨ him[256];
  ਪੂਰਨ himq[256];
  ਪੂਰਨ him_preferred[256];
  ਪੂਰਨ subnegotiation[256];
  ਅਚਲ ਅੱਖਰ *subopt_ttype;          /* Set with suboption TTYPE */
  ਅਚਲ ਅੱਖਰ *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ subopt_wsx;         /* Set with suboption NAWS */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  ਬਣਤਰ curl_slist *telnet_vars;    /* Environment variables */
  ਬਣਤਰ dynbuf ਬਾਹਰਲਾ;                 /* output buffer */

  /* suboptions */
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ subbuffer[SUBBUFSIZE];
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *subpointer, *subend;      /* buffer for sub-options */
};

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CURLVERBOSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ printoption(a, b, c, d) Curl_nop_stmt
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਖਾਲੀ printoption(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                        ਅਚਲ ਅੱਖਰ *direction, ਪੂਰਨ cmd, ਪੂਰਨ option)
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose) {
    ਜੇ(cmd == CURL_IAC) {
      ਜੇ(CURL_TELCMD_OK(option))
        infof(ਅੰਕੜੇ, "%s IAC %s", direction, CURL_TELCMD(option));
      ਨਹੀਂਤਾਂ
        infof(ਅੰਕੜੇ, "%s IAC %d", direction, option);
    }
    ਨਹੀਂਤਾਂ {
      ਅਚਲ ਅੱਖਰ *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      ਜੇ(fmt) {
        ਅਚਲ ਅੱਖਰ *opt;
        ਜੇ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        ਨਹੀਂਤਾਂ ਜੇ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        ਨਹੀਂਤਾਂ
          opt = ਨਿਰਾ;

        ਜੇ(opt)
          infof(ਅੰਕੜੇ, "%s %s %s", direction, fmt, opt);
        ਨਹੀਂਤਾਂ
          infof(ਅੰਕੜੇ, "%s %s %d", direction, fmt, option);
      }
      ਨਹੀਂਤਾਂ
        infof(ਅੰਕੜੇ, "%s %d %d", direction, cmd, option);
    }
  }
}
#ਜੇ_ਅੰਤ /* !CURLVERBOSE */

ਸਥਿਰ ਖਾਲੀ telnet_easy_dtor(ਅਚਲ ਖਾਲੀ *ਕੁੰਜੀ, size_t klen, ਖਾਲੀ *ਇੰਦਰਾਜ)
{
  ਬਣਤਰ TELNET *tn = ਇੰਦਰਾਜ;
  (ਖਾਲੀ)ਕੁੰਜੀ;
  (ਖਾਲੀ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->ਬਾਹਰਲਾ);
  curlx_free(tn);
}

ਸਥਿਰ CURLcode init_telnet(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ)
{
  ਬਣਤਰ TELNET *tn;

  tn = curlx_calloc(1, ਆਕਾਰ(ਬਣਤਰ TELNET));
  ਜੇ(!tn)
    ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;

  curlx_dyn_init(&tn->ਬਾਹਰਲਾ, 0xffff);

  tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_ਦੁਆਧਾਰੀ] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_ਦੁਆਧਾਰੀ] = CURL_YES;

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

  ਵਾਪਸ Curl_meta_ਸੈੱਟ(ਅੰਕੜੇ, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ਸਥਿਰ ਖਾਲੀ send_negotiation(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਪੂਰਨ cmd, ਪੂਰਨ option)
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ buf[3];
  size_t nwritten;
  CURLcode ਨਤੀਜਾ;

  buf[0] = CURL_IAC;
  buf[1] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)cmd;
  buf[2] = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)option;

  ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, buf, 3, FALSE, &nwritten);
  ਜੇ(ਨਤੀਜਾ)
    failf(ਅੰਕੜੇ, "Sending data failed: %s", curl_easy_strerror(ਨਤੀਜਾ));

  printoption(ਅੰਕੜੇ, "SENT", cmd, option);
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_remote_option(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn,
                              ਪੂਰਨ option, ਪੂਰਨ newstate)
{
  ਜੇ(newstate == CURL_YES) {
    ਚੋਣ(tn->him[option]) {
    ਹਾਲਤ CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(ਅੰਕੜੇ, CURL_DO, option);
      ਰੋਕੋ;

    ਹਾਲਤ CURL_YES:
      /* Already enabled */
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTNO:
      ਚੋਣ(tn->himq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ਰੋਕੋ;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTYES:
      ਚੋਣ(tn->himq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        /* Error: already negotiating for enable */
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        tn->himq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    }
  }
  ਨਹੀਂਤਾਂ { /* NO */
    ਚੋਣ(tn->him[option]) {
    ਹਾਲਤ CURL_NO:
      /* Already disabled */
      ਰੋਕੋ;

    ਹਾਲਤ CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(ਅੰਕੜੇ, CURL_DONT, option);
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTNO:
      ਚੋਣ(tn->himq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        /* Already negotiating for NO */
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        tn->himq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
        ਰੋਕੋ;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTYES:
      ਚੋਣ(tn->himq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        tn->himq[option] = CURL_OPPOSITE;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    }
  }
}

ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_local_option(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn,
                             ਪੂਰਨ option, ਪੂਰਨ newstate)
{
  ਜੇ(newstate == CURL_YES) {
    ਚੋਣ(tn->us[option]) {
    ਹਾਲਤ CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(ਅੰਕੜੇ, CURL_WILL, option);
      ਰੋਕੋ;

    ਹਾਲਤ CURL_YES:
      /* Already enabled */
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTNO:
      ਚੋਣ(tn->usq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ਰੋਕੋ;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTYES:
      ਚੋਣ(tn->usq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        /* Error: already negotiating for enable */
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        tn->usq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    }
  }
  ਨਹੀਂਤਾਂ { /* NO */
    ਚੋਣ(tn->us[option]) {
    ਹਾਲਤ CURL_NO:
      /* Already disabled */
      ਰੋਕੋ;

    ਹਾਲਤ CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(ਅੰਕੜੇ, CURL_WONT, option);
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTNO:
      ਚੋਣ(tn->usq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        /* Already negotiating for NO */
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        tn->usq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
        ਰੋਕੋ;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURL_WANTYES:
      ਚੋਣ(tn->usq[option]) {
      ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
        tn->usq[option] = CURL_OPPOSITE;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_OPPOSITE:
        ਰੋਕੋ;
      }
      ਰੋਕੋ;
    }
  }
}

ਸਥਿਰ ਖਾਲੀ telnet_negotiate(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn)
{
  ਪੂਰਨ i;

  ਲਈ(i = 0; i < CURL_NTELOPTS; i++) {
    ਜੇ(i == CURL_TELOPT_ECHO)
      ਜਾਰੀ;

    ਜੇ(tn->us_preferred[i] == CURL_YES)
      ਸੈੱਟ_local_option(ਅੰਕੜੇ, tn, i, CURL_YES);

    ਜੇ(tn->him_preferred[i] == CURL_YES)
      ਸੈੱਟ_remote_option(ਅੰਕੜੇ, tn, i, CURL_YES);
  }
}

ਸਥਿਰ ਖਾਲੀ rec_will(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn, ਪੂਰਨ option)
{
  ਚੋਣ(tn->him[option]) {
  ਹਾਲਤ CURL_NO:
    ਜੇ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(ਅੰਕੜੇ, CURL_DO, option);
    }
    ਨਹੀਂਤਾਂ
      send_negotiation(ਅੰਕੜੇ, CURL_DONT, option);

    ਰੋਕੋ;

  ਹਾਲਤ CURL_YES:
    /* Already enabled */
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTNO:
    ਚੋਣ(tn->himq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTYES:
    ਚੋਣ(tn->himq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      tn->him[option] = CURL_YES;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      send_negotiation(ਅੰਕੜੇ, CURL_DONT, option);
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  }
}

ਸਥਿਰ ਖਾਲੀ rec_wont(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn, ਪੂਰਨ option)
{
  ਚੋਣ(tn->him[option]) {
  ਹਾਲਤ CURL_NO:
    /* Already disabled */
    ਰੋਕੋ;

  ਹਾਲਤ CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(ਅੰਕੜੇ, CURL_DONT, option);
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTNO:
    ਚੋਣ(tn->himq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      tn->him[option] = CURL_NO;
      ਰੋਕੋ;

    ਹਾਲਤ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      send_negotiation(ਅੰਕੜੇ, CURL_DO, option);
      ਰੋਕੋ;
    }
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTYES:
    ਚੋਣ(tn->himq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      tn->him[option] = CURL_NO;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  }
}

ਸਥਿਰ ਖਾਲੀ printsub(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                     ਪੂਰਨ direction,                /* '<' or '>' */
                     ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਸੂਚਕ, /* ptr to suboption data */
                     size_t ਲੰਮਾਈ)                /* suboption data length */
{
  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.verbose) {
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ i = 0;
    ਜੇ(direction) {
      infof(ਅੰਕੜੇ, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      ਜੇ(ਲੰਮਾਈ >= 3) {
        ਪੂਰਨ j;

        i = ਸੂਚਕ[ਲੰਮਾਈ - 2];
        j = ਸੂਚਕ[ਲੰਮਾਈ - 1];

        ਜੇ(i != CURL_IAC || j != CURL_SE) {
          infof(ਅੰਕੜੇ, "(terminated by ");
          ਜੇ(CURL_TELOPT_OK(i))
            infof(ਅੰਕੜੇ, "%s ", CURL_TELOPT(i));
          ਨਹੀਂਤਾਂ ਜੇ(CURL_TELCMD_OK(i))
            infof(ਅੰਕੜੇ, "%s ", CURL_TELCMD(i));
          ਨਹੀਂਤਾਂ
            infof(ਅੰਕੜੇ, "%u ", i);
          ਜੇ(CURL_TELOPT_OK(j))
            infof(ਅੰਕੜੇ, "%s", CURL_TELOPT(j));
          ਨਹੀਂਤਾਂ ਜੇ(CURL_TELCMD_OK(j))
            infof(ਅੰਕੜੇ, "%s", CURL_TELCMD(j));
          ਨਹੀਂਤਾਂ
            infof(ਅੰਕੜੇ, "%d", j);
          infof(ਅੰਕੜੇ, ", not IAC SE) ");
        }
      }
      ਜੇ(ਲੰਮਾਈ >= 2)
        ਲੰਮਾਈ -= 2;
      ਨਹੀਂਤਾਂ /* bad input */
        ਵਾਪਸ;
    }
    ਜੇ(ਲੰਮਾਈ <= 1) {
      infof(ਅੰਕੜੇ, "(Empty suboption?)");
      ਵਾਪਸ;
    }

    ਜੇ(CURL_TELOPT_OK(ਸੂਚਕ[0])) {
      ਚੋਣ(ਸੂਚਕ[0]) {
      ਹਾਲਤ CURL_TELOPT_TTYPE:
      ਹਾਲਤ CURL_TELOPT_XDISPLOC:
      ਹਾਲਤ CURL_TELOPT_NEW_ENVIRON:
      ਹਾਲਤ CURL_TELOPT_NAWS:
        infof(ਅੰਕੜੇ, "%s", CURL_TELOPT(ਸੂਚਕ[0]));
        ਰੋਕੋ;
      ਮੂਲ:
        infof(ਅੰਕੜੇ, "%s (unsupported)", CURL_TELOPT(ਸੂਚਕ[0]));
        ਰੋਕੋ;
      }
    }
    ਨਹੀਂਤਾਂ
      infof(ਅੰਕੜੇ, "%d (unknown)", ਸੂਚਕ[0]);

    ਚੋਣ(ਸੂਚਕ[0]) {
    ਹਾਲਤ CURL_TELOPT_NAWS:
      ਜੇ(ਲੰਮਾਈ > 4)
        infof(ਅੰਕੜੇ, "Width: %d ; Height: %d", (ਸੂਚਕ[1] << 8) | ਸੂਚਕ[2],
              (ਸੂਚਕ[3] << 8) | ਸੂਚਕ[4]);
      ਰੋਕੋ;
    ਮੂਲ:
      ਚੋਣ(ਸੂਚਕ[1]) {
      ਹਾਲਤ CURL_TELQUAL_IS:
        infof(ਅੰਕੜੇ, " IS");
        ਰੋਕੋ;
      ਹਾਲਤ CURL_TELQUAL_SEND:
        infof(ਅੰਕੜੇ, " SEND");
        ਰੋਕੋ;
      ਹਾਲਤ CURL_TELQUAL_INFO:
        infof(ਅੰਕੜੇ, " INFO/REPLY");
        ਰੋਕੋ;
      ਹਾਲਤ CURL_TELQUAL_NAME:
        infof(ਅੰਕੜੇ, " NAME");
        ਰੋਕੋ;
      }

      ਚੋਣ(ਸੂਚਕ[0]) {
      ਹਾਲਤ CURL_TELOPT_TTYPE:
      ਹਾਲਤ CURL_TELOPT_XDISPLOC:
        infof(ਅੰਕੜੇ, " \"%.*s\"",
              (ਪੂਰਨ)((ਲੰਮਾਈ > 2) ? (ਲੰਮਾਈ - 2) : 0), &ਸੂਚਕ[2]);
        ਰੋਕੋ;
      ਹਾਲਤ CURL_TELOPT_NEW_ENVIRON:
        ਜੇ(ਸੂਚਕ[1] == CURL_TELQUAL_IS) {
          infof(ਅੰਕੜੇ, " ");
          ਲਈ(i = 3; i < ਲੰਮਾਈ; i++) {
            ਚੋਣ(ਸੂਚਕ[i]) {
            ਹਾਲਤ CURL_NEW_ENV_VAR:
              infof(ਅੰਕੜੇ, ", ");
              ਰੋਕੋ;
            ਹਾਲਤ CURL_NEW_ENV_ਮੁੱਲ:
              infof(ਅੰਕੜੇ, " = ");
              ਰੋਕੋ;
            ਮੂਲ:
              infof(ਅੰਕੜੇ, "%c", ਸੂਚਕ[i]);
              ਰੋਕੋ;
            }
          }
        }
        ਰੋਕੋ;
      ਮੂਲ:
        ਲਈ(i = 2; i < ਲੰਮਾਈ; i++)
          infof(ਅੰਕੜੇ, " %.2x", ਸੂਚਕ[i]);
        ਰੋਕੋ;
      }
    }
  }
}

/* Escape and send a telnet data block */
ਸਥਿਰ CURLcode send_telnet_ਅੰਕੜੇ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                 ਬਣਤਰ TELNET *tn,
                                 ਅਚਲ ਅੱਖਰ *ਬਫਰ, ssize_t nread)
{
  size_t i, outlen;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *outbuf;
  CURLcode ਨਤੀਜਾ = CURLE_OK;
  size_t ਬਾਈਟਾਂ_written;
  size_t ਕੁੱਲ_written = 0;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  ਜੇ(nread < 0)
    ਵਾਪਸ CURLE_TOO_LARGE;

  ਜੇ(memchr(ਬਫਰ, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->ਬਾਹਰਲਾ);

    ਲਈ(i = 0; i < (size_t)nread && !ਨਤੀਜਾ; i++) {
      ਨਤੀਜਾ = curlx_dyn_addn(&tn->ਬਾਹਰਲਾ, &ਬਫਰ[i], 1);
      ਜੇ(!ਨਤੀਜਾ && ((ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ)ਬਫਰ[i] == CURL_IAC))
        /* IAC is FF in hex */
        ਨਤੀਜਾ = curlx_dyn_addn(&tn->ਬਾਹਰਲਾ, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->ਬਾਹਰਲਾ);
    outbuf = curlx_dyn_uptr(&tn->ਬਾਹਰਲਾ);
  }
  ਨਹੀਂਤਾਂ {
    outlen = (size_t)nread;
    outbuf = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਬਫਰ;
  }
  ਜਦੋਂ(!ਨਤੀਜਾ && ਕੁੱਲ_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    ਬਣਤਰ pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(ਅੰਕੜੇ);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    ਜੇ(timeout_ms < 0)
      ਵਾਪਸ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    ਚੋਣ(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    ਹਾਲਤ -1:                    /* error, abort writing */
      ਨਤੀਜਾ = CURLE_SEND_ERROR;
      ਰੋਕੋ;
    ਹਾਲਤ 0:                     /* timeout */
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      ਰੋਕੋ;
    ਮੂਲ:                    /* write! */
      ਬਾਈਟਾਂ_written = 0;
      ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, outbuf + ਕੁੱਲ_written,
                              outlen - ਕੁੱਲ_written, FALSE, &ਬਾਈਟਾਂ_written);
      ਕੁੱਲ_written += ਬਾਈਟਾਂ_written;
      ਰੋਕੋ;
    }
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ਸਥਿਰ ਖਾਲੀ sendsuboption(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                          ਬਣਤਰ TELNET *tn, ਪੂਰਨ option)
{
  size_t nwritten;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ x, y;
  ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *uc1, *uc2;
  CURLcode ਨਤੀਜਾ;

  ਚੋਣ(option) {
  ਹਾਲਤ CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&x;
    uc2 = (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_ਪਦ(tn);
    /* data suboption is now ready */

    printsub(ਅੰਕੜੇ, '>', (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, tn->subbuffer, 3, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      failf(ਅੰਕੜੇ, "Sending data failed: %s", curl_easy_strerror(ਨਤੀਜਾ));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_ਅੰਕੜੇ(ਅੰਕੜੇ, tn, (ਅਚਲ ਅੱਖਰ *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, tn->subbuffer + 7, 2, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ)
      failf(ਅੰਕੜੇ, "Sending data failed: %s", curl_easy_strerror(ਨਤੀਜਾ));
    ਰੋਕੋ;
  }
}

ਸਥਿਰ ਖਾਲੀ rec_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn, ਪੂਰਨ option)
{
  ਚੋਣ(tn->us[option]) {
  ਹਾਲਤ CURL_NO:
    ਜੇ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(ਅੰਕੜੇ, CURL_WILL, option);
      ਜੇ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(ਅੰਕੜੇ, tn, option);
    }
    ਨਹੀਂਤਾਂ ਜੇ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(ਅੰਕੜੇ, CURL_WILL, option);
      sendsuboption(ਅੰਕੜੇ, tn, option);
    }
    ਨਹੀਂਤਾਂ
      send_negotiation(ਅੰਕੜੇ, CURL_WONT, option);
    ਰੋਕੋ;

  ਹਾਲਤ CURL_YES:
    /* Already enabled */
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTNO:
    ਚੋਣ(tn->usq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTYES:
    ਚੋਣ(tn->usq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      tn->us[option] = CURL_YES;
      ਜੇ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(ਅੰਕੜੇ, tn, option);
      }
      ਰੋਕੋ;
    ਹਾਲਤ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      send_negotiation(ਅੰਕੜੇ, CURL_WONT, option);
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  }
}

ਸਥਿਰ ਖਾਲੀ rec_dont(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn, ਪੂਰਨ option)
{
  ਚੋਣ(tn->us[option]) {
  ਹਾਲਤ CURL_NO:
    /* Already disabled */
    ਰੋਕੋ;

  ਹਾਲਤ CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(ਅੰਕੜੇ, CURL_WONT, option);
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTNO:
    ਚੋਣ(tn->usq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      tn->us[option] = CURL_NO;
      ਰੋਕੋ;

    ਹਾਲਤ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      send_negotiation(ਅੰਕੜੇ, CURL_WILL, option);
      ਰੋਕੋ;
    }
    ਰੋਕੋ;

  ਹਾਲਤ CURL_WANTYES:
    ਚੋਣ(tn->usq[option]) {
    ਹਾਲਤ CURL_ਖਾਲੀ_ਥਾਵਾਂ:
      tn->us[option] = CURL_NO;
      ਰੋਕੋ;
    ਹਾਲਤ CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_ਖਾਲੀ_ਥਾਵਾਂ;
      ਰੋਕੋ;
    }
    ਰੋਕੋ;
  }
}

ਸਥਿਰ bool str_is_nonascii(ਅਚਲ ਅੱਖਰ *str)
{
  ਅੱਖਰ c;
  ਜਦੋਂ((c = *str++) != 0)
    ਜੇ(c & 0x80)
      ਵਾਪਸ TRUE;

  ਵਾਪਸ FALSE;
}

ਸਥਿਰ CURLcode check_telnet_options(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                                     ਬਣਤਰ TELNET *tn)
{
  ਬਣਤਰ curl_slist *ਸਿਰ;
  ਬਣਤਰ curl_slist *beg;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  ਜੇ(ਅੰਕੜੇ->state.creds) {
    ਅੱਖਰ ਬਫਰ[256];
    ਜੇ(str_is_nonascii(Curl_creds_user(ਅੰਕੜੇ->conn->creds))) {
      DEBUGF(infof(ਅੰਕੜੇ, "set a non ASCII username in telnet"));
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    }
    curl_msnprintf(ਬਫਰ, ਆਕਾਰ(ਬਫਰ), "USER,%s",
                   Curl_creds_user(ਅੰਕੜੇ->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, ਬਫਰ);
    ਜੇ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ਨਿਰਾ;
      ਵਾਪਸ CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  ਲਈ(ਸਿਰ = ਅੰਕੜੇ->ਸੈੱਟ.telnet_options; ਸਿਰ && !ਨਤੀਜਾ; ਸਿਰ = ਸਿਰ->ਅਗਲਾ) {
    size_t olen;
    ਅਚਲ ਅੱਖਰ *option = ਸਿਰ->ਅੰਕੜੇ;
    ਅਚਲ ਅੱਖਰ *arg;
    ਅਚਲ ਅੱਖਰ *sep = strchr(option, '=');
    ਜੇ(sep) {
      olen = sep - option;
      arg = ++sep;
      ਜੇ(str_is_nonascii(arg))
        ਜਾਰੀ;
      ਚੋਣ(olen) {
      ਹਾਲਤ 5:
        /* Terminal type */
        ਜੇ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          ਰੋਕੋ;
        }
        ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
        ਰੋਕੋ;

      ਹਾਲਤ 8:
        /* Display variable */
        ਜੇ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          ਰੋਕੋ;
        }
        ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
        ਰੋਕੋ;

      ਹਾਲਤ 7:
        /* Environment variable */
        ਜੇ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          ਜੇ(!beg) {
            ਨਤੀਜਾ = CURLE_ਬਾਹਰਲਾ_ਦਾ_ਸਮ੍ਰਿਤੀ;
            ਰੋਕੋ;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        ਨਹੀਂਤਾਂ
          ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
        ਰੋਕੋ;

      ਹਾਲਤ 2:
        /* Window Size */
        ਜੇ(curl_strnequal(option, "WS", 2)) {
          ਅਚਲ ਅੱਖਰ *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          ਜੇ(curlx_str_ਸੰਖਿਆ(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_ਸੰਖਿਆ(&p, &y, 0xffff)) {
            failf(ਅੰਕੜੇ, "Syntax error in telnet option: %s", ਸਿਰ->ਅੰਕੜੇ);
            ਨਤੀਜਾ = CURLE_SETOPT_OPTION_SYNTAX;
          }
          ਨਹੀਂਤਾਂ {
            tn->subopt_wsx = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)x;
            tn->subopt_wsy = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਛੋਟਾ)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        ਨਹੀਂਤਾਂ
          ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
        ਰੋਕੋ;

      ਹਾਲਤ 6:
        /* To take care or not of the 8th bit in data exchange */
        ਜੇ(curl_strnequal(option, "BINARY", 6)) {
          ਅਚਲ ਅੱਖਰ *p = arg;
          curl_off_t ਦੁਆਧਾਰੀ_option;
          ਜੇ(!curlx_str_ਸੰਖਿਆ(&p, &ਦੁਆਧਾਰੀ_option, 1) &&
             (ਦੁਆਧਾਰੀ_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ਦੁਆਧਾਰੀ] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ਦੁਆਧਾਰੀ] = CURL_NO;
          }
        }
        ਨਹੀਂਤਾਂ
          ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
        ਰੋਕੋ;
      ਮੂਲ:
        failf(ਅੰਕੜੇ, "Unknown telnet option %s", ਸਿਰ->ਅੰਕੜੇ);
        ਨਤੀਜਾ = CURLE_UNKNOWN_OPTION;
        ਰੋਕੋ;
      }
    }
    ਨਹੀਂਤਾਂ {
      failf(ਅੰਕੜੇ, "Syntax error in telnet option: %s", ਸਿਰ->ਅੰਕੜੇ);
      ਨਤੀਜਾ = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  ਜੇ(ਨਤੀਜਾ) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ਨਿਰਾ;
  }

  ਵਾਪਸ ਨਤੀਜਾ;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ਸਥਿਰ bool bad_option(ਅਚਲ ਅੱਖਰ *ਅੰਕੜੇ)
{
  ਵਾਪਸ !ਅੰਕੜੇ || !!strchr(ਅੰਕੜੇ, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ਸਥਿਰ CURLcode suboption(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, ਬਣਤਰ TELNET *tn)
{
  ਬਣਤਰ curl_slist *v;
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ ਆਰਜ਼ੀ[2048];
  size_t nwritten;
  size_t len;
  CURLcode ਨਤੀਜਾ = CURLE_OK;

  ਜੇ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ਵਾਪਸ CURLE_OK;

  printsub(ਅੰਕੜੇ, '<', (ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  ਚੋਣ(CURL_SB_GET(tn)) {
  ਹਾਲਤ CURL_TELOPT_TTYPE:
    ਜੇ(bad_option(tn->subopt_ttype))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(tn->subopt_ttype) > 1000) {
      failf(ਅੰਕੜੇ, "Too long telnet TTYPE");
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ਅੱਖਰ *)ਆਰਜ਼ੀ, ਆਕਾਰ(ਆਰਜ਼ੀ), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, ਆਰਜ਼ੀ, len, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Sending data failed: %s", curl_easy_strerror(ਨਤੀਜਾ));
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    printsub(ਅੰਕੜੇ, '>', &ਆਰਜ਼ੀ[2], len-2);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_TELOPT_XDISPLOC:
    ਜੇ(bad_option(tn->subopt_xdisploc))
      ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
    ਜੇ(ਲੜੀ_ਲੰਬਾਈ(tn->subopt_xdisploc) > 1000) {
      failf(ਅੰਕੜੇ, "Too long telnet XDISPLOC");
      ਵਾਪਸ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ਅੱਖਰ *)ਆਰਜ਼ੀ, ਆਕਾਰ(ਆਰਜ਼ੀ), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, ਆਰਜ਼ੀ, len, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Sending data failed: %s", curl_easy_strerror(ਨਤੀਜਾ));
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    printsub(ਅੰਕੜੇ, '>', &ਆਰਜ਼ੀ[2], len - 2);
    ਰੋਕੋ;
  ਹਾਲਤ CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((ਅੱਖਰ *)ਆਰਜ਼ੀ, ਆਕਾਰ(ਆਰਜ਼ੀ), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    ਲਈ(v = tn->telnet_vars; v; v = v->ਅਗਲਾ) {
      size_t tmplen = (ਲੜੀ_ਲੰਬਾਈ(v->ਅੰਕੜੇ) + 1);
      ਜੇ(bad_option(v->ਅੰਕੜੇ))
        ਵਾਪਸ CURLE_BAD_FUNCTION_ਦਲੀਲ;
      /* Add the variable if it fits */
      ਜੇ(len + tmplen < ਆਕਾਰ(ਆਰਜ਼ੀ) - 6) {
        ਅਚਲ ਅੱਖਰ *s = strchr(v->ਅੰਕੜੇ, ',');
        ਜੇ(!s)
          len += curl_msnprintf((ਅੱਖਰ *)&ਆਰਜ਼ੀ[len], ਆਕਾਰ(ਆਰਜ਼ੀ) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->ਅੰਕੜੇ);
        ਨਹੀਂਤਾਂ {
          size_t vlen = s - v->ਅੰਕੜੇ;
          len += curl_msnprintf((ਅੱਖਰ *)&ਆਰਜ਼ੀ[len], ਆਕਾਰ(ਆਰਜ਼ੀ) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (ਪੂਰਨ)vlen, v->ਅੰਕੜੇ, CURL_NEW_ENV_ਮੁੱਲ, ++s);
        }
      }
    }
    curl_msnprintf((ਅੱਖਰ *)&ਆਰਜ਼ੀ[len], ਆਕਾਰ(ਆਰਜ਼ੀ) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    ਨਤੀਜਾ = Curl_xfer_send(ਅੰਕੜੇ, ਆਰਜ਼ੀ, len, FALSE, &nwritten);
    ਜੇ(ਨਤੀਜਾ) {
      failf(ਅੰਕੜੇ, "Sending data failed: %s", curl_easy_strerror(ਨਤੀਜਾ));
      ਵਾਪਸ ਨਤੀਜਾ;
    }
    printsub(ਅੰਕੜੇ, '>', &ਆਰਜ਼ੀ[2], len - 2);
    ਰੋਕੋ;
  }
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode telrcv(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                       ਬਣਤਰ TELNET *tn,
                       ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *inbuf, /* Data received from
                                                      socket */
                       ssize_t ਗਿਣਤੀ_2)              /* Number of bytes
                                                      received */
{
  ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ c;
  CURLcode ਨਤੀਜਾ;
  ਪੂਰਨ ਵਿੱਚ = 0;
  ਪੂਰਨ startwrite = -1;

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ startskipping()                                          \
  ਜੇ(startwrite >= 0) {                                          \
    ਨਤੀਜਾ = Curl_client_write(ਅੰਕੜੇ,                             \
                               CLIENTWRITE_BODY,                 \
                               (ਅਚਲ ਅੱਖਰ *)&inbuf[startwrite], \
                               ਵਿੱਚ-startwrite);                   \
    ਜੇ(ਨਤੀਜਾ)                                                   \
      ਵਾਪਸ ਨਤੀਜਾ;                                             \
  }                                                              \
  startwrite = -1

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ writebyte()  \
  ਜੇ(startwrite < 0) \
    startwrite = ਵਿੱਚ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ bufferflush() startskipping()

  ਜਦੋਂ(ਗਿਣਤੀ_2--) {
    c = inbuf[ਵਿੱਚ];

    ਚੋਣ(tn->telrcv_state) {
    ਹਾਲਤ CURL_TS_CR:
      tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
      ਜੇ(c == '\0') {
        startskipping();
        ਰੋਕੋ;   /* Ignore \0 after CR */
      }
      writebyte();
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_ਅੰਕੜੇ:
      ਜੇ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        ਰੋਕੋ;
      }
      ਨਹੀਂਤਾਂ ਜੇ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      ਚੋਣ(c) {
      ਹਾਲਤ CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        ਰੋਕੋ;
      ਹਾਲਤ CURL_IAC:
        tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
        writebyte();
        ਰੋਕੋ;
      ਹਾਲਤ CURL_DM:
      ਹਾਲਤ CURL_NOP:
      ਹਾਲਤ CURL_GA:
      ਮੂਲ:
        tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
        printoption(ਅੰਕੜੇ, "RCVD", CURL_IAC, c);
        ਰੋਕੋ;
      }
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_WILL:
      printoption(ਅੰਕੜੇ, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(ਅੰਕੜੇ, tn, c);
      tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_WONT:
      printoption(ਅੰਕੜੇ, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(ਅੰਕੜੇ, tn, c);
      tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_DO:
      printoption(ਅੰਕੜੇ, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(ਅੰਕੜੇ, tn, c);
      tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_DONT:
      printoption(ਅੰਕੜੇ, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(ਅੰਕੜੇ, tn, c);
      tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_SB:
      ਜੇ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      ਨਹੀਂਤਾਂ
        CURL_SB_ACCUM(tn, c);
      ਰੋਕੋ;

    ਹਾਲਤ CURL_TS_SE:
      ਜੇ(c != CURL_SE) {
        ਜੇ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(ਅੰਕੜੇ, "telnet: suboption error");
          ਵਾਪਸ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      ਨਹੀਂਤਾਂ {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_ਪਦ(tn);
        ਨਤੀਜਾ = suboption(ਅੰਕੜੇ, tn);   /* handle sub-option */
        ਜੇ(ਨਤੀਜਾ)
          ਵਾਪਸ ਨਤੀਜਾ;
        tn->telrcv_state = CURL_TS_ਅੰਕੜੇ;
      }
      ਰੋਕੋ;
    }
    ++ਵਿੱਚ;
  }
  bufferflush();
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode telnet_ਪੂਰਾ(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ,
                            CURLcode ਸਥਿਤੀ, bool premature)
{
  (ਖਾਲੀ)ਸਥਿਤੀ;
  (ਖਾਲੀ)premature;
  Curl_meta_remove(ਅੰਕੜੇ, CURL_META_TELNET_EASY);
  ਵਾਪਸ CURLE_OK;
}

ਸਥਿਰ CURLcode telnet_do(ਬਣਤਰ Curl_easy *ਅੰਕੜੇ, bool *ਪੂਰਾ)
{
  CURLcode ਨਤੀਜਾ;
  ਬਣਤਰ connectdata *conn = ਅੰਕੜੇ->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_ਗਿਣਤੀ;
  DWORD wait_timeout;
  DWORD readfile_read;
#ਨਹੀਂਤਾਂ
  timediff_t interval_ms;
  ਬਣਤਰ pollfd pfd[2];
  ਪੂਰਨ poll_cnt;
  ssize_t snread;
#ਜੇ_ਅੰਤ
  bool keepon = TRUE;
  ਅੱਖਰ ਬਫਰ[4 * 1024];
  ਬਣਤਰ TELNET *tn;

  *ਪੂਰਾ = TRUE; /* unconditionally */

  ਨਤੀਜਾ = init_telnet(ਅੰਕੜੇ);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

  tn = Curl_meta_get(ਅੰਕੜੇ, CURL_META_TELNET_EASY);
  ਜੇ(!tn)
    ਵਾਪਸ CURLE_FAILED_INIT;

  ਨਤੀਜਾ = check_telnet_options(ਅੰਕੜੇ, tn);
  ਜੇ(ਨਤੀਜਾ)
    ਵਾਪਸ ਨਤੀਜਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  ਜੇ(event_handle == WSA_INVALID_EVENT) {
    failf(ਅੰਕੜੇ, "WSACreateEvent failed (%d)", SOCKERRNO);
    ਵਾਪਸ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  ਜੇ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ਵਾਪਸ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_ਨਿਵੇਸ਼_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  ਜੇ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || ਅੰਕੜੇ->ਸੈੱਟ.is_fread_ਸੈੱਟ) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_ਗਿਣਤੀ = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  ਨਹੀਂਤਾਂ {
    obj_ਗਿਣਤੀ = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  ਜਦੋਂ(keepon) {
    ਅਚਲ DWORD buf_ਮਾਪ = (DWORD)ਆਕਾਰ(ਬਫਰ);
    DWORD waitret = Wait_For_ਗੁਣਜ_Objects(obj_ਗਿਣਤੀ, objs,
                                           FALSE, wait_timeout);
    ਚੋਣ(waitret) {

    ਹਾਲਤ WAIT_TIMEOUT: {
      ਲਈ(;;) {
        ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.is_fread_ਸੈੱਟ) {
          size_t n;
          /* read from user-supplied method */
          n = ਅੰਕੜੇ->state.fread_func(ਬਫਰ, 1, buf_ਮਾਪ, ਅੰਕੜੇ->state.ਵਿੱਚ);
          ਜੇ(n == CURL_READFUNC_ABORT) {
            failf(ਅੰਕੜੇ, "operation aborted by callback");
            keepon = FALSE;
            ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
            ਰੋਕੋ;
          }

          ਜੇ(n == CURL_READFUNC_PAUSE)
            ਰੋਕੋ;

          ਜੇ(n == 0)                        /* no bytes */
            ਰੋਕੋ;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        ਨਹੀਂਤਾਂ {
          /* read from stdin */
          ਜੇ(!PeekNamedPipe(stdin_handle, ਨਿਰਾ, 0, ਨਿਰਾ,
                            &readfile_read, ਨਿਰਾ)) {
            keepon = FALSE;
            ਨਤੀਜਾ = CURLE_READ_ERROR;
            ਰੋਕੋ;
          }

          ਜੇ(!readfile_read)
            ਰੋਕੋ;

          ਜੇ(!ReadFile(stdin_handle, ਬਫਰ, buf_ਮਾਪ, &readfile_read, ਨਿਰਾ)) {
            keepon = FALSE;
            ਨਤੀਜਾ = CURLE_READ_ERROR;
            ਰੋਕੋ;
          }
        }

        ਨਤੀਜਾ = send_telnet_ਅੰਕੜੇ(ਅੰਕੜੇ, tn, ਬਫਰ, readfile_read);
        ਜੇ(ਨਤੀਜਾ) {
          keepon = FALSE;
          ਰੋਕੋ;
        }
      }
    }
    ਰੋਕੋ;

    ਹਾਲਤ WAIT_OBJECT_0 + 1: {
      ਜੇ(!ReadFile(stdin_handle, ਬਫਰ, buf_ਮਾਪ, &readfile_read, ਨਿਰਾ)) {
        keepon = FALSE;
        ਨਤੀਜਾ = CURLE_READ_ERROR;
        ਰੋਕੋ;
      }

      ਨਤੀਜਾ = send_telnet_ਅੰਕੜੇ(ਅੰਕੜੇ, tn, ਬਫਰ, readfile_read);
      ਜੇ(ਨਤੀਜਾ) {
        keepon = FALSE;
        ਰੋਕੋ;
      }
    }
    ਰੋਕੋ;

    ਹਾਲਤ WAIT_OBJECT_0: {
      events.l_ਜਾਲ_Events = 0;
      ਜੇ(WSAEnum_ਜਾਲ_Events(sockfd, event_handle, &events) != 0) {
        ਪੂਰਨ sockerr = SOCKERRNO;
        ਜੇ(sockerr != SOCKEINPROGRESS) {
          infof(ਅੰਕੜੇ, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          ਨਤੀਜਾ = CURLE_READ_ERROR;
        }
        ਰੋਕੋ;
      }
      ਜੇ(events.l_ਜਾਲ_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ), &nread);
        /* read would have blocked. Loop again */
        ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
          ਰੋਕੋ;
        /* returned not-zero, this an error */
        ਨਹੀਂਤਾਂ ਜੇ(ਨਤੀਜਾ) {
          keepon = FALSE;
          ਰੋਕੋ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ਨਹੀਂਤਾਂ ਜੇ(!nread) {
          keepon = FALSE;
          ਰੋਕੋ;
        }

        ਨਤੀਜਾ = telrcv(ਅੰਕੜੇ, tn, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਬਫਰ, nread);
        ਜੇ(ਨਤੀਜਾ) {
          keepon = FALSE;
          ਰੋਕੋ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ਜੇ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(ਅੰਕੜੇ, tn);
          tn->already_negotiated = 1;
        }
      }
      ਜੇ(events.l_ਜਾਲ_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      ਰੋਕੋ;
    }
    } /* switch */

    ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
      failf(ਅੰਕੜੇ, "Time-out");
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  ਜੇ(!WSACloseEvent(event_handle)) {
    infof(ਅੰਕੜੇ, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#ਨਹੀਂਤਾਂ
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  ਜੇ(ਅੰਕੜੇ->ਸੈੱਟ.is_fread_ਸੈੱਟ) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  ਨਹੀਂਤਾਂ {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)ਅੰਕੜੇ->state.ਵਿੱਚ);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    ਜੇ(pfd[1].fd < 0) {
      failf(ਅੰਕੜੇ, "cannot read input");
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  ਜਦੋਂ(keepon) {
    DEBUGF(infof(ਅੰਕੜੇ, "telnet_do, poll %d fds", poll_cnt));
    ਚੋਣ(Curl_poll(pfd, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)poll_cnt, interval_ms)) {
    ਹਾਲਤ -1:                    /* error, stop reading */
      ਨਤੀਜਾ = CURLE_RECV_ERROR;
      keepon = FALSE;
      ਜਾਰੀ;
    ਹਾਲਤ 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    ਮੂਲ:                    /* read! */
      ਜੇ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        ਨਤੀਜਾ = Curl_xfer_recv(ਅੰਕੜੇ, ਬਫਰ, ਆਕਾਰ(ਬਫਰ), &nread);
        /* read would have blocked. Loop again */
        ਜੇ(ਨਤੀਜਾ == CURLE_AGAIN)
          ਰੋਕੋ;
        /* returned not-zero, this an error */
        ਜੇ(ਨਤੀਜਾ) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          ਜੇ(ਅੰਕੜੇ->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(ਅੰਕੜੇ, "telnet_do, unexpected ECONNRESET on recv"));
          }
          ਰੋਕੋ;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ਨਹੀਂਤਾਂ ਜੇ(!nread) {
          keepon = FALSE;
          ਰੋਕੋ;
        }

        Curl_pgrs_download_inc(ਅੰਕੜੇ, nread);
        ਨਤੀਜਾ = telrcv(ਅੰਕੜੇ, tn, (ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *)ਬਫਰ, nread);
        ਜੇ(ਨਤੀਜਾ) {
          keepon = FALSE;
          ਰੋਕੋ;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ਜੇ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(ਅੰਕੜੇ, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      ਜੇ(poll_cnt == 2) {
        ਜੇ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = ਪਠ(pfd[1].fd, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
        }
      }
      ਨਹੀਂਤਾਂ {
        /* read from user-supplied method */
        snread = (ਪੂਰਨ)ਅੰਕੜੇ->state.fread_func(ਬਫਰ, 1, ਆਕਾਰ(ਬਫਰ),
                                             ਅੰਕੜੇ->state.ਵਿੱਚ);
        ਜੇ(snread == CURL_READFUNC_ABORT) {
          failf(ਅੰਕੜੇ, "operation aborted by callback");
          ਨਤੀਜਾ = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          ਰੋਕੋ;
        }
        ਜੇ(snread == CURL_READFUNC_PAUSE)
          ਰੋਕੋ;
      }

      ਜੇ(snread > 0) {
        ਨਤੀਜਾ = send_telnet_ਅੰਕੜੇ(ਅੰਕੜੇ, tn, ਬਫਰ, snread);
        ਜੇ(ਨਤੀਜਾ) {
          keepon = FALSE;
          ਰੋਕੋ;
        }
        Curl_pgrs_upload_inc(ਅੰਕੜੇ, (size_t)snread);
      }
      ਨਹੀਂਤਾਂ ਜੇ(snread < 0) {
        failf(ਅੰਕੜੇ, "Failed to read input");
        ਨਤੀਜਾ = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      ਰੋਕੋ;
    } /* poll switch statement */

    ਜੇ(Curl_timeleft_ms(ਅੰਕੜੇ) < 0) {
      failf(ਅੰਕੜੇ, "Time-out");
      ਨਤੀਜਾ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    ਜੇ(!ਨਤੀਜਾ) {
      ਨਤੀਜਾ = Curl_pgrsUpdate(ਅੰਕੜੇ);
      ਜੇ(ਨਤੀਜਾ)
        keepon = FALSE;
    }
  }
#ਜੇ_ਅੰਤ
  /* mark this as "no further transfer wanted" */
  Curl_xfer_ਤਿਆਰੀ_nop(ਅੰਕੜੇ);

  ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * TELNET protocol handler.
 */
ਅਚਲ ਬਣਤਰ Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_ਪੂਰਾ,                          /* done */
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

#ਜੇ_ਅੰਤ /* !CURL_DISABLE_TELNET */
