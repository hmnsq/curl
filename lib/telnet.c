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

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURL_DISABLE_TELNET

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETINET_ಒಳ_H
#ಅನ್ತರ್ಭಾವ <netinet/in.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NETDB_H
#ಅನ್ತರ್ಭಾವ <netdb.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ARPA_INET_H
#ಅನ್ತರ್ಭಾವ <arpa/inet.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NET_IF_H
#ಅನ್ತರ್ಭಾವ <net/if.h>
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
#ಅನ್ತರ್ಭಾವ <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_PARAM_H
#ಅನ್ತರ್ಭಾವ <sys/param.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "url.h"
#ಅನ್ತರ್ಭಾವ "transfer.h"
#ಅನ್ತರ್ಭಾವ "sendf.h"
#ಅನ್ತರ್ಭಾವ "curl_trc.h"
#ಅನ್ತರ್ಭಾವ "progress.h"
#ಅನ್ತರ್ಭಾವ "arpa_telnet.h"
#ಅನ್ತರ್ಭಾವ "connect.h"
#ಅನ್ತರ್ಭಾವ "select.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"

#ವ್ಯಾಖ್ಯಾನಿಸು SUBBUFSIZE 512

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SB_ಪದ(x)            \
  ಮಾಡು {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } ಆಗ(0)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SB_ACCUM(x, c)                                         \
  ಮಾಡು {                                                              \
    ಆದರೆ((x)->subpointer < ((x)->subbuffer + ಗಾತ್ರ((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } ಆಗ(0)

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_NO          0
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_YES         1
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_WANTYES     2
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_WANTNO      3

#ವ್ಯಾಖ್ಯಾನಿಸು CURL_ಖಾಲಿಗಳು       0
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#ವ್ಯಾಖ್ಯಾನಿಸು CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಎಣಿಕೆ {
  CURL_TS_ದತ್ತಾಂಶ = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

ರಚನೆ TELNET {
  ಪೂರ್ಣಾಂಕ please_negotiate;
  ಪೂರ್ಣಾಂಕ already_negotiated;
  ಪೂರ್ಣಾಂಕ us[256];
  ಪೂರ್ಣಾಂಕ usq[256];
  ಪೂರ್ಣಾಂಕ us_preferred[256];
  ಪೂರ್ಣಾಂಕ him[256];
  ಪೂರ್ಣಾಂಕ himq[256];
  ಪೂರ್ಣಾಂಕ him_preferred[256];
  ಪೂರ್ಣಾಂಕ subnegotiation[256];
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subopt_ttype;          /* Set with suboption TTYPE */
  ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ subopt_wsx;         /* Set with suboption NAWS */
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  ರಚನೆ curl_slist *telnet_vars;    /* Environment variables */
  ರಚನೆ dynbuf ಹೊರಹೋಗು;                 /* output buffer */

  /* suboptions */
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ subbuffer[SUBBUFSIZE];
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *subpointer, *subend;      /* buffer for sub-options */
};

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CURLVERBOSE
#ವ್ಯಾಖ್ಯಾನಿಸು printoption(a, b, c, d) Curl_nop_stmt
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಶೂನ್ಯ printoption(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *direction, ಪೂರ್ಣಾಂಕ cmd, ಪೂರ್ಣಾಂಕ option)
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose) {
    ಆದರೆ(cmd == CURL_IAC) {
      ಆದರೆ(CURL_TELCMD_OK(option))
        infof(ದತ್ತಾಂಶ, "%s IAC %s", direction, CURL_TELCMD(option));
      ಇಲ್ಲದಿದ್ದರೆ
        infof(ದತ್ತಾಂಶ, "%s IAC %d", direction, option);
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      ಆದರೆ(fmt) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *opt;
        ಆದರೆ(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        ಇಲ್ಲದಿದ್ದರೆ
          opt = ಖಾಲಿ;

        ಆದರೆ(opt)
          infof(ದತ್ತಾಂಶ, "%s %s %s", direction, fmt, opt);
        ಇಲ್ಲದಿದ್ದರೆ
          infof(ದತ್ತಾಂಶ, "%s %s %d", direction, fmt, option);
      }
      ಇಲ್ಲದಿದ್ದರೆ
        infof(ದತ್ತಾಂಶ, "%s %d %d", direction, cmd, option);
    }
  }
}
#ಆದರೆ_ಅಂತ್ಯ /* !CURLVERBOSE */

ಸ್ಥಿರ ಶೂನ್ಯ telnet_easy_dtor(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *ಕೀಲಿ, size_t klen, ಶೂನ್ಯ *ನಮೂದು)
{
  ರಚನೆ TELNET *tn = ನಮೂದು;
  (ಶೂನ್ಯ)ಕೀಲಿ;
  (ಶೂನ್ಯ)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->ಹೊರಹೋಗು);
  curlx_free(tn);
}

ಸ್ಥಿರ CURLcode init_telnet(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ)
{
  ರಚನೆ TELNET *tn;

  tn = curlx_calloc(1, ಗಾತ್ರ(ರಚನೆ TELNET));
  ಆದರೆ(!tn)
    ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;

  curlx_dyn_init(&tn->ಹೊರಹೋಗು, 0xffff);

  tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;

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

  ಹಿಂತಿರುಗಿ Curl_meta_ಗಣ(ದತ್ತಾಂಶ, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

ಸ್ಥಿರ ಶೂನ್ಯ send_negotiation(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ cmd, ಪೂರ್ಣಾಂಕ option)
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ buf[3];
  size_t nwritten;
  CURLcode ಫಲಿತಾಂಶ;

  buf[0] = CURL_IAC;
  buf[1] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)cmd;
  buf[2] = (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)option;

  ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, buf, 3, FALSE, &nwritten);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    failf(ದತ್ತಾಂಶ, "Sending data failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));

  printoption(ದತ್ತಾಂಶ, "SENT", cmd, option);
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_remote_option(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn,
                              ಪೂರ್ಣಾಂಕ option, ಪೂರ್ಣಾಂಕ newstate)
{
  ಆದರೆ(newstate == CURL_YES) {
    ಆಯ್ಕೆ(tn->him[option]) {
    ಸ್ಥಿತಿ CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(ದತ್ತಾಂಶ, CURL_DO, option);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_YES:
      /* Already enabled */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTNO:
      ಆಯ್ಕೆ(tn->himq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTYES:
      ಆಯ್ಕೆ(tn->himq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        /* Error: already negotiating for enable */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        tn->himq[option] = CURL_ಖಾಲಿಗಳು;
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* NO */
    ಆಯ್ಕೆ(tn->him[option]) {
    ಸ್ಥಿತಿ CURL_NO:
      /* Already disabled */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(ದತ್ತಾಂಶ, CURL_DONT, option);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTNO:
      ಆಯ್ಕೆ(tn->himq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        /* Already negotiating for NO */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        tn->himq[option] = CURL_ಖಾಲಿಗಳು;
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTYES:
      ಆಯ್ಕೆ(tn->himq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        tn->himq[option] = CURL_OPPOSITE;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_local_option(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn,
                             ಪೂರ್ಣಾಂಕ option, ಪೂರ್ಣಾಂಕ newstate)
{
  ಆದರೆ(newstate == CURL_YES) {
    ಆಯ್ಕೆ(tn->us[option]) {
    ಸ್ಥಿತಿ CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(ದತ್ತಾಂಶ, CURL_WILL, option);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_YES:
      /* Already enabled */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTNO:
      ಆಯ್ಕೆ(tn->usq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        /* Error: already queued an enable request */
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTYES:
      ಆಯ್ಕೆ(tn->usq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        /* Error: already negotiating for enable */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        tn->usq[option] = CURL_ಖಾಲಿಗಳು;
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    }
  }
  ಇಲ್ಲದಿದ್ದರೆ { /* NO */
    ಆಯ್ಕೆ(tn->us[option]) {
    ಸ್ಥಿತಿ CURL_NO:
      /* Already disabled */
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(ದತ್ತಾಂಶ, CURL_WONT, option);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTNO:
      ಆಯ್ಕೆ(tn->usq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        /* Already negotiating for NO */
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        tn->usq[option] = CURL_ಖಾಲಿಗಳು;
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_WANTYES:
      ಆಯ್ಕೆ(tn->usq[option]) {
      ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
        tn->usq[option] = CURL_OPPOSITE;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_OPPOSITE:
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;
    }
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ telnet_negotiate(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn)
{
  ಪೂರ್ಣಾಂಕ i;

  ಗಾಗಿ(i = 0; i < CURL_NTELOPTS; i++) {
    ಆದರೆ(i == CURL_TELOPT_ECHO)
      ಮುಂದುವರಿ;

    ಆದರೆ(tn->us_preferred[i] == CURL_YES)
      ಗಣ_local_option(ದತ್ತಾಂಶ, tn, i, CURL_YES);

    ಆದರೆ(tn->him_preferred[i] == CURL_YES)
      ಗಣ_remote_option(ದತ್ತಾಂಶ, tn, i, CURL_YES);
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_will(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn, ಪೂರ್ಣಾಂಕ option)
{
  ಆಯ್ಕೆ(tn->him[option]) {
  ಸ್ಥಿತಿ CURL_NO:
    ಆದರೆ(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(ದತ್ತಾಂಶ, CURL_DO, option);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      send_negotiation(ದತ್ತಾಂಶ, CURL_DONT, option);

    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_YES:
    /* Already enabled */
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTNO:
    ಆಯ್ಕೆ(tn->himq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_ಖಾಲಿಗಳು;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTYES:
    ಆಯ್ಕೆ(tn->himq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      tn->him[option] = CURL_YES;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_ಖಾಲಿಗಳು;
      send_negotiation(ದತ್ತಾಂಶ, CURL_DONT, option);
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_wont(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn, ಪೂರ್ಣಾಂಕ option)
{
  ಆಯ್ಕೆ(tn->him[option]) {
  ಸ್ಥಿತಿ CURL_NO:
    /* Already disabled */
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(ದತ್ತಾಂಶ, CURL_DONT, option);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTNO:
    ಆಯ್ಕೆ(tn->himq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      tn->him[option] = CURL_NO;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_ಖಾಲಿಗಳು;
      send_negotiation(ದತ್ತಾಂಶ, CURL_DO, option);
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTYES:
    ಆಯ್ಕೆ(tn->himq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      tn->him[option] = CURL_NO;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_ಖಾಲಿಗಳು;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ printsub(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                     ಪೂರ್ಣಾಂಕ direction,                /* '<' or '>' */
                     ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ಸೂಚಕ, /* ptr to suboption data */
                     size_t ಉದ್ದಳತೆ)                /* suboption data length */
{
  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.verbose) {
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ i = 0;
    ಆದರೆ(direction) {
      infof(ದತ್ತಾಂಶ, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      ಆದರೆ(ಉದ್ದಳತೆ >= 3) {
        ಪೂರ್ಣಾಂಕ j;

        i = ಸೂಚಕ[ಉದ್ದಳತೆ - 2];
        j = ಸೂಚಕ[ಉದ್ದಳತೆ - 1];

        ಆದರೆ(i != CURL_IAC || j != CURL_SE) {
          infof(ದತ್ತಾಂಶ, "(terminated by ");
          ಆದರೆ(CURL_TELOPT_OK(i))
            infof(ದತ್ತಾಂಶ, "%s ", CURL_TELOPT(i));
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(CURL_TELCMD_OK(i))
            infof(ದತ್ತಾಂಶ, "%s ", CURL_TELCMD(i));
          ಇಲ್ಲದಿದ್ದರೆ
            infof(ದತ್ತಾಂಶ, "%u ", i);
          ಆದರೆ(CURL_TELOPT_OK(j))
            infof(ದತ್ತಾಂಶ, "%s", CURL_TELOPT(j));
          ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(CURL_TELCMD_OK(j))
            infof(ದತ್ತಾಂಶ, "%s", CURL_TELCMD(j));
          ಇಲ್ಲದಿದ್ದರೆ
            infof(ದತ್ತಾಂಶ, "%d", j);
          infof(ದತ್ತಾಂಶ, ", not IAC SE) ");
        }
      }
      ಆದರೆ(ಉದ್ದಳತೆ >= 2)
        ಉದ್ದಳತೆ -= 2;
      ಇಲ್ಲದಿದ್ದರೆ /* bad input */
        ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ(ಉದ್ದಳತೆ <= 1) {
      infof(ದತ್ತಾಂಶ, "(Empty suboption?)");
      ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ(CURL_TELOPT_OK(ಸೂಚಕ[0])) {
      ಆಯ್ಕೆ(ಸೂಚಕ[0]) {
      ಸ್ಥಿತಿ CURL_TELOPT_TTYPE:
      ಸ್ಥಿತಿ CURL_TELOPT_XDISPLOC:
      ಸ್ಥಿತಿ CURL_TELOPT_NEW_ENVIRON:
      ಸ್ಥಿತಿ CURL_TELOPT_NAWS:
        infof(ದತ್ತಾಂಶ, "%s", CURL_TELOPT(ಸೂಚಕ[0]));
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        infof(ದತ್ತಾಂಶ, "%s (unsupported)", CURL_TELOPT(ಸೂಚಕ[0]));
        ನಿಲ್ಲಿಸು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ
      infof(ದತ್ತಾಂಶ, "%d (unknown)", ಸೂಚಕ[0]);

    ಆಯ್ಕೆ(ಸೂಚಕ[0]) {
    ಸ್ಥಿತಿ CURL_TELOPT_NAWS:
      ಆದರೆ(ಉದ್ದಳತೆ > 4)
        infof(ದತ್ತಾಂಶ, "Width: %d ; Height: %d", (ಸೂಚಕ[1] << 8) | ಸೂಚಕ[2],
              (ಸೂಚಕ[3] << 8) | ಸೂಚಕ[4]);
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:
      ಆಯ್ಕೆ(ಸೂಚಕ[1]) {
      ಸ್ಥಿತಿ CURL_TELQUAL_IS:
        infof(ದತ್ತಾಂಶ, " IS");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_TELQUAL_SEND:
        infof(ದತ್ತಾಂಶ, " SEND");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_TELQUAL_INFO:
        infof(ದತ್ತಾಂಶ, " INFO/REPLY");
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_TELQUAL_NAME:
        infof(ದತ್ತಾಂಶ, " NAME");
        ನಿಲ್ಲಿಸು;
      }

      ಆಯ್ಕೆ(ಸೂಚಕ[0]) {
      ಸ್ಥಿತಿ CURL_TELOPT_TTYPE:
      ಸ್ಥಿತಿ CURL_TELOPT_XDISPLOC:
        infof(ದತ್ತಾಂಶ, " \"%.*s\"",
              (ಪೂರ್ಣಾಂಕ)((ಉದ್ದಳತೆ > 2) ? (ಉದ್ದಳತೆ - 2) : 0), &ಸೂಚಕ[2]);
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_TELOPT_NEW_ENVIRON:
        ಆದರೆ(ಸೂಚಕ[1] == CURL_TELQUAL_IS) {
          infof(ದತ್ತಾಂಶ, " ");
          ಗಾಗಿ(i = 3; i < ಉದ್ದಳತೆ; i++) {
            ಆಯ್ಕೆ(ಸೂಚಕ[i]) {
            ಸ್ಥಿತಿ CURL_NEW_ENV_VAR:
              infof(ದತ್ತಾಂಶ, ", ");
              ನಿಲ್ಲಿಸು;
            ಸ್ಥಿತಿ CURL_NEW_ENV_ಮೌಲ್ಯ:
              infof(ದತ್ತಾಂಶ, " = ");
              ನಿಲ್ಲಿಸು;
            ಪೂರ್ವನಿಯೋಜಿತ:
              infof(ದತ್ತಾಂಶ, "%c", ಸೂಚಕ[i]);
              ನಿಲ್ಲಿಸು;
            }
          }
        }
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        ಗಾಗಿ(i = 2; i < ಉದ್ದಳತೆ; i++)
          infof(ದತ್ತಾಂಶ, " %.2x", ಸೂಚಕ[i]);
        ನಿಲ್ಲಿಸು;
      }
    }
  }
}

/* Escape and send a telnet data block */
ಸ್ಥಿರ CURLcode send_telnet_ದತ್ತಾಂಶ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                 ರಚನೆ TELNET *tn,
                                 ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ಬಫರ್, ssize_t nread)
{
  size_t i, outlen;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *outbuf;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;
  size_t ಬೈಟುಗಳು_written;
  size_t ಒಟ್ಟು_written = 0;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  ಆದರೆ(nread < 0)
    ಹಿಂತಿರುಗಿ CURLE_TOO_LARGE;

  ಆದರೆ(memchr(ಬಫರ್, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->ಹೊರಹೋಗು);

    ಗಾಗಿ(i = 0; i < (size_t)nread && !ಫಲಿತಾಂಶ; i++) {
      ಫಲಿತಾಂಶ = curlx_dyn_addn(&tn->ಹೊರಹೋಗು, &ಬಫರ್[i], 1);
      ಆದರೆ(!ಫಲಿತಾಂಶ && ((ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ)ಬಫರ್[i] == CURL_IAC))
        /* IAC is FF in hex */
        ಫಲಿತಾಂಶ = curlx_dyn_addn(&tn->ಹೊರಹೋಗು, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->ಹೊರಹೋಗು);
    outbuf = curlx_dyn_uptr(&tn->ಹೊರಹೋಗು);
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    outlen = (size_t)nread;
    outbuf = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಬಫರ್;
  }
  ಆಗ(!ಫಲಿತಾಂಶ && ಒಟ್ಟು_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    ರಚನೆ pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(ದತ್ತಾಂಶ);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    ಆದರೆ(timeout_ms < 0)
      ಹಿಂತಿರುಗಿ CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    ಆಯ್ಕೆ(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    ಸ್ಥಿತಿ -1:                    /* error, abort writing */
      ಫಲಿತಾಂಶ = CURLE_SEND_ERROR;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ 0:                     /* timeout */
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      ನಿಲ್ಲಿಸು;
    ಪೂರ್ವನಿಯೋಜಿತ:                    /* write! */
      ಬೈಟುಗಳು_written = 0;
      ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, outbuf + ಒಟ್ಟು_written,
                              outlen - ಒಟ್ಟು_written, FALSE, &ಬೈಟುಗಳು_written);
      ಒಟ್ಟು_written += ಬೈಟುಗಳು_written;
      ನಿಲ್ಲಿಸು;
    }
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
ಸ್ಥಿರ ಶೂನ್ಯ sendsuboption(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                          ರಚನೆ TELNET *tn, ಪೂರ್ಣಾಂಕ option)
{
  size_t nwritten;
  ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ x, y;
  ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *uc1, *uc2;
  CURLcode ಫಲಿತಾಂಶ;

  ಆಯ್ಕೆ(option) {
  ಸ್ಥಿತಿ CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&x;
    uc2 = (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_ಪದ(tn);
    /* data suboption is now ready */

    printsub(ದತ್ತಾಂಶ, '>', (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, tn->subbuffer, 3, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      failf(ದತ್ತಾಂಶ, "Sending data failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, tn, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, tn->subbuffer + 7, 2, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ)
      failf(ದತ್ತಾಂಶ, "Sending data failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn, ಪೂರ್ಣಾಂಕ option)
{
  ಆಯ್ಕೆ(tn->us[option]) {
  ಸ್ಥಿತಿ CURL_NO:
    ಆದರೆ(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(ದತ್ತಾಂಶ, CURL_WILL, option);
      ಆದರೆ(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(ದತ್ತಾಂಶ, tn, option);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(ದತ್ತಾಂಶ, CURL_WILL, option);
      sendsuboption(ದತ್ತಾಂಶ, tn, option);
    }
    ಇಲ್ಲದಿದ್ದರೆ
      send_negotiation(ದತ್ತಾಂಶ, CURL_WONT, option);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_YES:
    /* Already enabled */
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTNO:
    ಆಯ್ಕೆ(tn->usq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_ಖಾಲಿಗಳು;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTYES:
    ಆಯ್ಕೆ(tn->usq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      tn->us[option] = CURL_YES;
      ಆದರೆ(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(ದತ್ತಾಂಶ, tn, option);
      }
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_ಖಾಲಿಗಳು;
      send_negotiation(ದತ್ತಾಂಶ, CURL_WONT, option);
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ ಶೂನ್ಯ rec_dont(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn, ಪೂರ್ಣಾಂಕ option)
{
  ಆಯ್ಕೆ(tn->us[option]) {
  ಸ್ಥಿತಿ CURL_NO:
    /* Already disabled */
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(ದತ್ತಾಂಶ, CURL_WONT, option);
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTNO:
    ಆಯ್ಕೆ(tn->usq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      tn->us[option] = CURL_NO;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_ಖಾಲಿಗಳು;
      send_negotiation(ದತ್ತಾಂಶ, CURL_WILL, option);
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;

  ಸ್ಥಿತಿ CURL_WANTYES:
    ಆಯ್ಕೆ(tn->usq[option]) {
    ಸ್ಥಿತಿ CURL_ಖಾಲಿಗಳು:
      tn->us[option] = CURL_NO;
      ನಿಲ್ಲಿಸು;
    ಸ್ಥಿತಿ CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_ಖಾಲಿಗಳು;
      ನಿಲ್ಲಿಸು;
    }
    ನಿಲ್ಲಿಸು;
  }
}

ಸ್ಥಿರ bool str_is_nonascii(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *str)
{
  ಅಕ್ಷರ c;
  ಆಗ((c = *str++) != 0)
    ಆದರೆ(c & 0x80)
      ಹಿಂತಿರುಗಿ TRUE;

  ಹಿಂತಿರುಗಿ FALSE;
}

ಸ್ಥಿರ CURLcode check_telnet_options(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                                     ರಚನೆ TELNET *tn)
{
  ರಚನೆ curl_slist *ತಲೆ;
  ರಚನೆ curl_slist *beg;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  ಆದರೆ(ದತ್ತಾಂಶ->state.creds) {
    ಅಕ್ಷರ ಬಫರ್[256];
    ಆದರೆ(str_is_nonascii(Curl_creds_user(ದತ್ತಾಂಶ->conn->creds))) {
      DEBUGF(infof(ದತ್ತಾಂಶ, "set a non ASCII username in telnet"));
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    }
    curl_msnprintf(ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), "USER,%s",
                   Curl_creds_user(ದತ್ತಾಂಶ->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, ಬಫರ್);
    ಆದರೆ(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = ಖಾಲಿ;
      ಹಿಂತಿರುಗಿ CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  ಗಾಗಿ(ತಲೆ = ದತ್ತಾಂಶ->ಗಣ.telnet_options; ತಲೆ && !ಫಲಿತಾಂಶ; ತಲೆ = ತಲೆ->ಮುಂದಿನ) {
    size_t olen;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *option = ತಲೆ->ದತ್ತಾಂಶ;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *arg;
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *sep = strchr(option, '=');
    ಆದರೆ(sep) {
      olen = sep - option;
      arg = ++sep;
      ಆದರೆ(str_is_nonascii(arg))
        ಮುಂದುವರಿ;
      ಆಯ್ಕೆ(olen) {
      ಸ್ಥಿತಿ 5:
        /* Terminal type */
        ಆದರೆ(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          ನಿಲ್ಲಿಸು;
        }
        ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ 8:
        /* Display variable */
        ಆದರೆ(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          ನಿಲ್ಲಿಸು;
        }
        ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ 7:
        /* Environment variable */
        ಆದರೆ(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          ಆದರೆ(!beg) {
            ಫಲಿತಾಂಶ = CURLE_ಹೊರಹೋಗು_ದ_ಸ್ಮರಣೆ;
            ನಿಲ್ಲಿಸು;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ 2:
        /* Window Size */
        ಆದರೆ(curl_strnequal(option, "WS", 2)) {
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          ಆದರೆ(curlx_str_ಸಂಖ್ಯೆ(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_ಸಂಖ್ಯೆ(&p, &y, 0xffff)) {
            failf(ದತ್ತಾಂಶ, "Syntax error in telnet option: %s", ತಲೆ->ದತ್ತಾಂಶ);
            ಫಲಿತಾಂಶ = CURLE_SETOPT_OPTION_SYNTAX;
          }
          ಇಲ್ಲದಿದ್ದರೆ {
            tn->subopt_wsx = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)x;
            tn->subopt_wsy = (ಚಿಹ್ನೆರಹಿತ ಗಿಡ್ಡ)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
        ನಿಲ್ಲಿಸು;

      ಸ್ಥಿತಿ 6:
        /* To take care or not of the 8th bit in data exchange */
        ಆದರೆ(curl_strnequal(option, "BINARY", 6)) {
          ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *p = arg;
          curl_off_t ದ್ವಿಮಾನ_option;
          ಆದರೆ(!curlx_str_ಸಂಖ್ಯೆ(&p, &ದ್ವಿಮಾನ_option, 1) &&
             (ದ್ವಿಮಾನ_option != 1)) {
            tn->us_preferred[CURL_TELOPT_ದ್ವಿಮಾನ] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_ದ್ವಿಮಾನ] = CURL_NO;
          }
        }
        ಇಲ್ಲದಿದ್ದರೆ
          ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
        ನಿಲ್ಲಿಸು;
      ಪೂರ್ವನಿಯೋಜಿತ:
        failf(ದತ್ತಾಂಶ, "Unknown telnet option %s", ತಲೆ->ದತ್ತಾಂಶ);
        ಫಲಿತಾಂಶ = CURLE_UNKNOWN_OPTION;
        ನಿಲ್ಲಿಸು;
      }
    }
    ಇಲ್ಲದಿದ್ದರೆ {
      failf(ದತ್ತಾಂಶ, "Syntax error in telnet option: %s", ತಲೆ->ದತ್ತಾಂಶ);
      ಫಲಿತಾಂಶ = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  ಆದರೆ(ಫಲಿತಾಂಶ) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = ಖಾಲಿ;
  }

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
ಸ್ಥಿರ bool bad_option(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *ದತ್ತಾಂಶ)
{
  ಹಿಂತಿರುಗಿ !ದತ್ತಾಂಶ || !!strchr(ದತ್ತಾಂಶ, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
ಸ್ಥಿರ CURLcode suboption(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, ರಚನೆ TELNET *tn)
{
  ರಚನೆ curl_slist *v;
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ ತಾತ್ಕಾಲಿಕ[2048];
  size_t nwritten;
  size_t len;
  CURLcode ಫಲಿತಾಂಶ = CURLE_OK;

  ಆದರೆ(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    ಹಿಂತಿರುಗಿ CURLE_OK;

  printsub(ದತ್ತಾಂಶ, '<', (ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  ಆಯ್ಕೆ(CURL_SB_GET(tn)) {
  ಸ್ಥಿತಿ CURL_TELOPT_TTYPE:
    ಆದರೆ(bad_option(tn->subopt_ttype))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tn->subopt_ttype) > 1000) {
      failf(ದತ್ತಾಂಶ, "Too long telnet TTYPE");
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ಅಕ್ಷರ *)ತಾತ್ಕಾಲಿಕ, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, ತಾತ್ಕಾಲಿಕ, len, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Sending data failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    printsub(ದತ್ತಾಂಶ, '>', &ತಾತ್ಕಾಲಿಕ[2], len-2);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_TELOPT_XDISPLOC:
    ಆದರೆ(bad_option(tn->subopt_xdisploc))
      ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
    ಆದರೆ(ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(tn->subopt_xdisploc) > 1000) {
      failf(ದತ್ತಾಂಶ, "Too long telnet XDISPLOC");
      ಹಿಂತಿರುಗಿ CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((ಅಕ್ಷರ *)ತಾತ್ಕಾಲಿಕ, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, ತಾತ್ಕಾಲಿಕ, len, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Sending data failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    printsub(ದತ್ತಾಂಶ, '>', &ತಾತ್ಕಾಲಿಕ[2], len - 2);
    ನಿಲ್ಲಿಸು;
  ಸ್ಥಿತಿ CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((ಅಕ್ಷರ *)ತಾತ್ಕಾಲಿಕ, ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    ಗಾಗಿ(v = tn->telnet_vars; v; v = v->ಮುಂದಿನ) {
      size_t tmplen = (ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(v->ದತ್ತಾಂಶ) + 1);
      ಆದರೆ(bad_option(v->ದತ್ತಾಂಶ))
        ಹಿಂತಿರುಗಿ CURLE_BAD_FUNCTION_ವಾದ;
      /* Add the variable if it fits */
      ಆದರೆ(len + tmplen < ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ) - 6) {
        ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *s = strchr(v->ದತ್ತಾಂಶ, ',');
        ಆದರೆ(!s)
          len += curl_msnprintf((ಅಕ್ಷರ *)&ತಾತ್ಕಾಲಿಕ[len], ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->ದತ್ತಾಂಶ);
        ಇಲ್ಲದಿದ್ದರೆ {
          size_t vlen = s - v->ದತ್ತಾಂಶ;
          len += curl_msnprintf((ಅಕ್ಷರ *)&ತಾತ್ಕಾಲಿಕ[len], ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (ಪೂರ್ಣಾಂಕ)vlen, v->ದತ್ತಾಂಶ, CURL_NEW_ENV_ಮೌಲ್ಯ, ++s);
        }
      }
    }
    curl_msnprintf((ಅಕ್ಷರ *)&ತಾತ್ಕಾಲಿಕ[len], ಗಾತ್ರ(ತಾತ್ಕಾಲಿಕ) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    ಫಲಿತಾಂಶ = Curl_xfer_send(ದತ್ತಾಂಶ, ತಾತ್ಕಾಲಿಕ, len, FALSE, &nwritten);
    ಆದರೆ(ಫಲಿತಾಂಶ) {
      failf(ದತ್ತಾಂಶ, "Sending data failed: %s", curl_easy_strerror(ಫಲಿತಾಂಶ));
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
    }
    printsub(ದತ್ತಾಂಶ, '>', &ತಾತ್ಕಾಲಿಕ[2], len - 2);
    ನಿಲ್ಲಿಸು;
  }
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode telrcv(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                       ರಚನೆ TELNET *tn,
                       ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *inbuf, /* Data received from
                                                      socket */
                       ssize_t ಎಣಿಕೆ_2)              /* Number of bytes
                                                      received */
{
  ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ c;
  CURLcode ಫಲಿತಾಂಶ;
  ಪೂರ್ಣಾಂಕ ಒಳ = 0;
  ಪೂರ್ಣಾಂಕ startwrite = -1;

#ವ್ಯಾಖ್ಯಾನಿಸು startskipping()                                          \
  ಆದರೆ(startwrite >= 0) {                                          \
    ಫಲಿತಾಂಶ = Curl_client_write(ದತ್ತಾಂಶ,                             \
                               CLIENTWRITE_BODY,                 \
                               (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)&inbuf[startwrite], \
                               ಒಳ-startwrite);                   \
    ಆದರೆ(ಫಲಿತಾಂಶ)                                                   \
      ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;                                             \
  }                                                              \
  startwrite = -1

#ವ್ಯಾಖ್ಯಾನಿಸು writebyte()  \
  ಆದರೆ(startwrite < 0) \
    startwrite = ಒಳ

#ವ್ಯಾಖ್ಯಾನಿಸು bufferflush() startskipping()

  ಆಗ(ಎಣಿಕೆ_2--) {
    c = inbuf[ಒಳ];

    ಆಯ್ಕೆ(tn->telrcv_state) {
    ಸ್ಥಿತಿ CURL_TS_CR:
      tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
      ಆದರೆ(c == '\0') {
        startskipping();
        ನಿಲ್ಲಿಸು;   /* Ignore \0 after CR */
      }
      writebyte();
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_ದತ್ತಾಂಶ:
      ಆದರೆ(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        ನಿಲ್ಲಿಸು;
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      ಆಯ್ಕೆ(c) {
      ಸ್ಥಿತಿ CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_IAC:
        tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
        writebyte();
        ನಿಲ್ಲಿಸು;
      ಸ್ಥಿತಿ CURL_DM:
      ಸ್ಥಿತಿ CURL_NOP:
      ಸ್ಥಿತಿ CURL_GA:
      ಪೂರ್ವನಿಯೋಜಿತ:
        tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
        printoption(ದತ್ತಾಂಶ, "RCVD", CURL_IAC, c);
        ನಿಲ್ಲಿಸು;
      }
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_WILL:
      printoption(ದತ್ತಾಂಶ, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(ದತ್ತಾಂಶ, tn, c);
      tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_WONT:
      printoption(ದತ್ತಾಂಶ, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(ದತ್ತಾಂಶ, tn, c);
      tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_DO:
      printoption(ದತ್ತಾಂಶ, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(ದತ್ತಾಂಶ, tn, c);
      tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_DONT:
      printoption(ದತ್ತಾಂಶ, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(ದತ್ತಾಂಶ, tn, c);
      tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_SB:
      ಆದರೆ(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      ಇಲ್ಲದಿದ್ದರೆ
        CURL_SB_ACCUM(tn, c);
      ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ CURL_TS_SE:
      ಆದರೆ(c != CURL_SE) {
        ಆದರೆ(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(ದತ್ತಾಂಶ, "telnet: suboption error");
          ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_ಪದ(tn);
        ಫಲಿತಾಂಶ = suboption(ದತ್ತಾಂಶ, tn);   /* handle sub-option */
        ಆದರೆ(ಫಲಿತಾಂಶ)
          ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
        tn->telrcv_state = CURL_TS_ದತ್ತಾಂಶ;
      }
      ನಿಲ್ಲಿಸು;
    }
    ++ಒಳ;
  }
  bufferflush();
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode telnet_ಮುಗಿದ(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ,
                            CURLcode ಸ್ಥಿತಿ_2, bool premature)
{
  (ಶೂನ್ಯ)ಸ್ಥಿತಿ_2;
  (ಶೂನ್ಯ)premature;
  Curl_meta_remove(ದತ್ತಾಂಶ, CURL_META_TELNET_EASY);
  ಹಿಂತಿರುಗಿ CURLE_OK;
}

ಸ್ಥಿರ CURLcode telnet_do(ರಚನೆ Curl_easy *ದತ್ತಾಂಶ, bool *ಮುಗಿದ)
{
  CURLcode ಫಲಿತಾಂಶ;
  ರಚನೆ connectdata *conn = ದತ್ತಾಂಶ->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_ಎಣಿಕೆ;
  DWORD wait_timeout;
  DWORD readfile_read;
#ಇಲ್ಲದಿದ್ದರೆ
  timediff_t interval_ms;
  ರಚನೆ pollfd pfd[2];
  ಪೂರ್ಣಾಂಕ poll_cnt;
  ssize_t snread;
#ಆದರೆ_ಅಂತ್ಯ
  bool keepon = TRUE;
  ಅಕ್ಷರ ಬಫರ್[4 * 1024];
  ರಚನೆ TELNET *tn;

  *ಮುಗಿದ = TRUE; /* unconditionally */

  ಫಲಿತಾಂಶ = init_telnet(ದತ್ತಾಂಶ);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

  tn = Curl_meta_get(ದತ್ತಾಂಶ, CURL_META_TELNET_EASY);
  ಆದರೆ(!tn)
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;

  ಫಲಿತಾಂಶ = check_telnet_options(ದತ್ತಾಂಶ, tn);
  ಆದರೆ(ಫಲಿತಾಂಶ)
    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  ಆದರೆ(event_handle == WSA_INVALID_EVENT) {
    failf(ದತ್ತಾಂಶ, "WSACreateEvent failed (%d)", SOCKERRNO);
    ಹಿಂತಿರುಗಿ CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  ಆದರೆ(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    ಹಿಂತಿರುಗಿ CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_ಒಳಹರಿವು_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  ಆದರೆ(GetFileType(stdin_handle) == FILE_TYPE_PIPE || ದತ್ತಾಂಶ->ಗಣ.is_fread_ಗಣ) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_ಎಣಿಕೆ = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    obj_ಎಣಿಕೆ = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  ಆಗ(keepon) {
    ಸ್ಥಿರಾಂಕ DWORD buf_ಗಾತ್ರ = (DWORD)ಗಾತ್ರ(ಬಫರ್);
    DWORD waitret = Wait_For_ಅಪವರ್ತ್ಯ_Objects(obj_ಎಣಿಕೆ, objs,
                                           FALSE, wait_timeout);
    ಆಯ್ಕೆ(waitret) {

    ಸ್ಥಿತಿ WAIT_TIMEOUT: {
      ಗಾಗಿ(;;) {
        ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.is_fread_ಗಣ) {
          size_t n;
          /* read from user-supplied method */
          n = ದತ್ತಾಂಶ->state.fread_func(ಬಫರ್, 1, buf_ಗಾತ್ರ, ದತ್ತಾಂಶ->state.ಒಳ);
          ಆದರೆ(n == CURL_READFUNC_ABORT) {
            failf(ದತ್ತಾಂಶ, "operation aborted by callback");
            keepon = FALSE;
            ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
            ನಿಲ್ಲಿಸು;
          }

          ಆದರೆ(n == CURL_READFUNC_PAUSE)
            ನಿಲ್ಲಿಸು;

          ಆದರೆ(n == 0)                        /* no bytes */
            ನಿಲ್ಲಿಸು;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        ಇಲ್ಲದಿದ್ದರೆ {
          /* read from stdin */
          ಆದರೆ(!PeekNamedPipe(stdin_handle, ಖಾಲಿ, 0, ಖಾಲಿ,
                            &readfile_read, ಖಾಲಿ)) {
            keepon = FALSE;
            ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
            ನಿಲ್ಲಿಸು;
          }

          ಆದರೆ(!readfile_read)
            ನಿಲ್ಲಿಸು;

          ಆದರೆ(!ReadFile(stdin_handle, ಬಫರ್, buf_ಗಾತ್ರ, &readfile_read, ಖಾಲಿ)) {
            keepon = FALSE;
            ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
            ನಿಲ್ಲಿಸು;
          }
        }

        ಫಲಿತಾಂಶ = send_telnet_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, tn, ಬಫರ್, readfile_read);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }
      }
    }
    ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ WAIT_OBJECT_0 + 1: {
      ಆದರೆ(!ReadFile(stdin_handle, ಬಫರ್, buf_ಗಾತ್ರ, &readfile_read, ಖಾಲಿ)) {
        keepon = FALSE;
        ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
        ನಿಲ್ಲಿಸು;
      }

      ಫಲಿತಾಂಶ = send_telnet_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, tn, ಬಫರ್, readfile_read);
      ಆದರೆ(ಫಲಿತಾಂಶ) {
        keepon = FALSE;
        ನಿಲ್ಲಿಸು;
      }
    }
    ನಿಲ್ಲಿಸು;

    ಸ್ಥಿತಿ WAIT_OBJECT_0: {
      events.l_ಜಾಲ_Events = 0;
      ಆದರೆ(WSAEnum_ಜಾಲ_Events(sockfd, event_handle, &events) != 0) {
        ಪೂರ್ಣಾಂಕ sockerr = SOCKERRNO;
        ಆದರೆ(sockerr != SOCKEINPROGRESS) {
          infof(ದತ್ತಾಂಶ, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
        }
        ನಿಲ್ಲಿಸು;
      }
      ಆದರೆ(events.l_ಜಾಲ_Events & FD_READ) {
        /* read data from network */
        size_t nread;
        ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), &nread);
        /* read would have blocked. Loop again */
        ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
          ನಿಲ್ಲಿಸು;
        /* returned not-zero, this an error */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(ಫಲಿತಾಂಶ) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nread) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }

        ಫಲಿತಾಂಶ = telrcv(ದತ್ತಾಂಶ, tn, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಬಫರ್, nread);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ಆದರೆ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(ದತ್ತಾಂಶ, tn);
          tn->already_negotiated = 1;
        }
      }
      ಆದರೆ(events.l_ಜಾಲ_Events & FD_CLOSE) {
        keepon = FALSE;
      }
      ನಿಲ್ಲಿಸು;
    }
    } /* switch */

    ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
      failf(ದತ್ತಾಂಶ, "Time-out");
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  ಆದರೆ(!WSACloseEvent(event_handle)) {
    infof(ದತ್ತಾಂಶ, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#ಇಲ್ಲದಿದ್ದರೆ
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  ಆದರೆ(ದತ್ತಾಂಶ->ಗಣ.is_fread_ಗಣ) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  ಇಲ್ಲದಿದ್ದರೆ {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((FILE *)ದತ್ತಾಂಶ->state.ಒಳ);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    ಆದರೆ(pfd[1].fd < 0) {
      failf(ದತ್ತಾಂಶ, "cannot read input");
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  ಆಗ(keepon) {
    DEBUGF(infof(ದತ್ತಾಂಶ, "telnet_do, poll %d fds", poll_cnt));
    ಆಯ್ಕೆ(Curl_poll(pfd, (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)poll_cnt, interval_ms)) {
    ಸ್ಥಿತಿ -1:                    /* error, stop reading */
      ಫಲಿತಾಂಶ = CURLE_RECV_ERROR;
      keepon = FALSE;
      ಮುಂದುವರಿ;
    ಸ್ಥಿತಿ 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    ಪೂರ್ವನಿಯೋಜಿತ:                    /* read! */
      ಆದರೆ(pfd[0].revents & POLLIN) {
        /* read data from network */
        size_t nread;
        ಫಲಿತಾಂಶ = Curl_xfer_recv(ದತ್ತಾಂಶ, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್), &nread);
        /* read would have blocked. Loop again */
        ಆದರೆ(ಫಲಿತಾಂಶ == CURLE_AGAIN)
          ನಿಲ್ಲಿಸು;
        /* returned not-zero, this an error */
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          ಆದರೆ(ದತ್ತಾಂಶ->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(ದತ್ತಾಂಶ, "telnet_do, unexpected ECONNRESET on recv"));
          }
          ನಿಲ್ಲಿಸು;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(!nread) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }

        Curl_pgrs_download_inc(ದತ್ತಾಂಶ, nread);
        ಫಲಿತಾಂಶ = telrcv(ದತ್ತಾಂಶ, tn, (ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *)ಬಫರ್, nread);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        ಆದರೆ(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(ದತ್ತಾಂಶ, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      ಆದರೆ(poll_cnt == 2) {
        ಆದರೆ(pfd[1].revents & POLLIN) { /* read from in file */
          snread = ಪಠ(pfd[1].fd, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
        }
      }
      ಇಲ್ಲದಿದ್ದರೆ {
        /* read from user-supplied method */
        snread = (ಪೂರ್ಣಾಂಕ)ದತ್ತಾಂಶ->state.fread_func(ಬಫರ್, 1, ಗಾತ್ರ(ಬಫರ್),
                                             ದತ್ತಾಂಶ->state.ಒಳ);
        ಆದರೆ(snread == CURL_READFUNC_ABORT) {
          failf(ದತ್ತಾಂಶ, "operation aborted by callback");
          ಫಲಿತಾಂಶ = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }
        ಆದರೆ(snread == CURL_READFUNC_PAUSE)
          ನಿಲ್ಲಿಸು;
      }

      ಆದರೆ(snread > 0) {
        ಫಲಿತಾಂಶ = send_telnet_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, tn, ಬಫರ್, snread);
        ಆದರೆ(ಫಲಿತಾಂಶ) {
          keepon = FALSE;
          ನಿಲ್ಲಿಸು;
        }
        Curl_pgrs_upload_inc(ದತ್ತಾಂಶ, (size_t)snread);
      }
      ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ(snread < 0) {
        failf(ದತ್ತಾಂಶ, "Failed to read input");
        ಫಲಿತಾಂಶ = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      ನಿಲ್ಲಿಸು;
    } /* poll switch statement */

    ಆದರೆ(Curl_timeleft_ms(ದತ್ತಾಂಶ) < 0) {
      failf(ದತ್ತಾಂಶ, "Time-out");
      ಫಲಿತಾಂಶ = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    ಆದರೆ(!ಫಲಿತಾಂಶ) {
      ಫಲಿತಾಂಶ = Curl_pgrsUpdate(ದತ್ತಾಂಶ);
      ಆದರೆ(ಫಲಿತಾಂಶ)
        keepon = FALSE;
    }
  }
#ಆದರೆ_ಅಂತ್ಯ
  /* mark this as "no further transfer wanted" */
  Curl_xfer_ಸಿದ್ಧತೆ_nop(ದತ್ತಾಂಶ);

  ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * TELNET protocol handler.
 */
ಸ್ಥಿರಾಂಕ ರಚನೆ Curl_protocol Curl_protocol_telnet = {
  ZERO_NULL,                            /* setup_connection */
  telnet_do,                            /* do_it */
  telnet_ಮುಗಿದ,                          /* done */
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

#ಆದರೆ_ಅಂತ್ಯ /* !CURL_DISABLE_TELNET */
