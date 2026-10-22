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
#समावेश "curl_setup.h"
#समावेश "urldata.h"
#समावेश "telnet.h"

#अगर_अघोषित CURL_DISABLE_TELNET

#अगर_घोषित HAVE_NETINET_IN_H
#समावेश <netinet/in.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NETDB_H
#समावेश <netdb.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_ARPA_INET_H
#समावेश <arpa/inet.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_NET_IF_H
#समावेश <net/if.h>
#पूर्ण_अगर
#अगर_घोषित HAVE_SYS_IOCTL_H
#समावेश <sys/ioctl.h>
#पूर्ण_अगर

#अगर_घोषित HAVE_SYS_PARAM_H
#समावेश <sys/param.h>
#पूर्ण_अगर

#समावेश "url.h"
#समावेश "transfer.h"
#समावेश "sendf.h"
#समावेश "curl_trc.h"
#समावेश "progress.h"
#समावेश "arpa_telnet.h"
#समावेश "connect.h"
#समावेश "select.h"
#समावेश "curlx/strparse.h"

#घोषणा SUBBUFSIZE 512

#घोषणा CURL_SB_CLEAR(x) (x)->subpointer = (x)->subbuffer
#घोषणा CURL_SB_TERM(x)            \
  करो {                             \
    (x)->subend = (x)->subpointer; \
    CURL_SB_CLEAR(x);              \
  } जबतक(0)
#घोषणा CURL_SB_ACCUM(x, c)                                         \
  करो {                                                              \
    अगर((x)->subpointer < ((x)->subbuffer + माप((x)->subbuffer))) \
      *(x)->subpointer++ = (c);                                     \
  } जबतक(0)

#घोषणा CURL_SB_GET(x) ((*(x)->subpointer++) & 0xff)
#घोषणा CURL_SB_LEN(x) ((x)->subend - (x)->subpointer)

/* For posterity:
#define CURL_SB_PEEK(x) (*(x)->subpointer & 0xff)
#define CURL_SB_EOF(x)  ((x)->subpointer >= (x)->subend) */

/* For negotiation compliant to RFC 1143 */
#घोषणा CURL_NO          0
#घोषणा CURL_YES         1
#घोषणा CURL_WANTYES     2
#घोषणा CURL_WANTNO      3

#घोषणा CURL_EMPTY       0
#घोषणा CURL_OPPOSITE    1

/* meta key for storing protocol meta at easy handle */
#घोषणा CURL_META_TELNET_EASY   "meta:proto:telnet:easy"

/*
 * Telnet receiver states for fsm
 */
प्रकार क्रमागत {
  CURL_TS_तथ्य = 0,
  CURL_TS_IAC,
  CURL_TS_WILL,
  CURL_TS_WONT,
  CURL_TS_DO,
  CURL_TS_DONT,
  CURL_TS_CR,
  CURL_TS_SB,   /* sub-option collection */
  CURL_TS_SE    /* looking for sub-option end */
} TelnetReceive;

काष्ठा TELNET {
  पूर्णांक please_negotiate;
  पूर्णांक already_negotiated;
  पूर्णांक us[256];
  पूर्णांक usq[256];
  पूर्णांक us_preferred[256];
  पूर्णांक him[256];
  पूर्णांक himq[256];
  पूर्णांक him_preferred[256];
  पूर्णांक subnegotiation[256];
  स्थिर अक्षर *subopt_ttype;          /* Set with suboption TTYPE */
  स्थिर अक्षर *subopt_xdisploc;       /* Set with suboption XDISPLOC */
  अचिन्हित लघु subopt_wsx;         /* Set with suboption NAWS */
  अचिन्हित लघु subopt_wsy;         /* Set with suboption NAWS */
  TelnetReceive telrcv_state;
  काष्ठा curl_slist *telnet_vars;    /* Environment variables */
  काष्ठा dynbuf out;                 /* output buffer */

  /* suboptions */
  अचिन्हित अक्षर subbuffer[SUBBUFSIZE];
  अचिन्हित अक्षर *subpointer, *subend;      /* buffer for sub-options */
};

#अगर_अघोषित CURLVERBOSE
#घोषणा printoption(a, b, c, d) Curl_nop_stmt
#अन्यथा
अटल व्योम printoption(काष्ठा Curl_easy *तथ्य,
                        स्थिर अक्षर *direction, पूर्णांक cmd, पूर्णांक option)
{
  अगर(तथ्य->set.verbose) {
    अगर(cmd == CURL_IAC) {
      अगर(CURL_TELCMD_OK(option))
        infof(तथ्य, "%s IAC %s", direction, CURL_TELCMD(option));
      अन्यथा
        infof(तथ्य, "%s IAC %d", direction, option);
    }
    अन्यथा {
      स्थिर अक्षर *fmt = (cmd == CURL_WILL) ? "WILL" :
                        (cmd == CURL_WONT) ? "WONT" :
                        (cmd == CURL_DO) ? "DO" :
                        (cmd == CURL_DONT) ? "DONT" : 0;
      अगर(fmt) {
        स्थिर अक्षर *opt;
        अगर(CURL_TELOPT_OK(option))
          opt = CURL_TELOPT(option);
        अन्यथा अगर(option == CURL_TELOPT_EXOPL)
          opt = "EXOPL";
        अन्यथा
          opt = शून्य;

        अगर(opt)
          infof(तथ्य, "%s %s %s", direction, fmt, opt);
        अन्यथा
          infof(तथ्य, "%s %s %d", direction, fmt, option);
      }
      अन्यथा
        infof(तथ्य, "%s %d %d", direction, cmd, option);
    }
  }
}
#पूर्ण_अगर /* !CURLVERBOSE */

अटल व्योम telnet_easy_dtor(स्थिर व्योम *कुंजी, माप_प्रकार klen, व्योम *entry)
{
  काष्ठा TELNET *tn = entry;
  (व्योम)कुंजी;
  (व्योम)klen;
  curl_slist_free_all(tn->telnet_vars);
  curlx_dyn_free(&tn->out);
  curlx_free(tn);
}

अटल CURLcode init_telnet(काष्ठा Curl_easy *तथ्य)
{
  काष्ठा TELNET *tn;

  tn = curlx_calloc(1, माप(काष्ठा TELNET));
  अगर(!tn)
    वापस CURLE_OUT_OF_MEMORY;

  curlx_dyn_init(&tn->out, 0xffff);

  tn->telrcv_state = CURL_TS_तथ्य;

  /* Init suboptions */
  CURL_SB_CLEAR(tn);

  /* Set the options we want by default */
  tn->us_preferred[CURL_TELOPT_SGA] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_SGA] = CURL_YES;

  /* To be compliant with previous releases of libcurl we enable this option
     by default. This behavior can be changed with the "BINARY" option in
     CURLOPT_TELNETOPTIONS */
  tn->us_preferred[CURL_TELOPT_द्विआधारी] = CURL_YES;
  tn->him_preferred[CURL_TELOPT_द्विआधारी] = CURL_YES;

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

  वापस Curl_meta_set(तथ्य, CURL_META_TELNET_EASY, tn, telnet_easy_dtor);
}

अटल व्योम send_negotiation(काष्ठा Curl_easy *तथ्य, पूर्णांक cmd, पूर्णांक option)
{
  अचिन्हित अक्षर buf[3];
  माप_प्रकार nwritten;
  CURLcode परिणाम;

  buf[0] = CURL_IAC;
  buf[1] = (अचिन्हित अक्षर)cmd;
  buf[2] = (अचिन्हित अक्षर)option;

  परिणाम = Curl_xfer_send(तथ्य, buf, 3, FALSE, &nwritten);
  अगर(परिणाम)
    failf(तथ्य, "Sending data failed: %s", curl_easy_strerror(परिणाम));

  printoption(तथ्य, "SENT", cmd, option);
}

अटल व्योम set_remote_option(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn,
                              पूर्णांक option, पूर्णांक newstate)
{
  अगर(newstate == CURL_YES) {
    चयन(tn->him[option]) {
    हाल CURL_NO:
      tn->him[option] = CURL_WANTYES;
      send_negotiation(तथ्य, CURL_DO, option);
      अवरोध;

    हाल CURL_YES:
      /* Already enabled */
      अवरोध;

    हाल CURL_WANTNO:
      चयन(tn->himq[option]) {
      हाल CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->himq[option] = CURL_OPPOSITE;
        अवरोध;
      हाल CURL_OPPOSITE:
        /* Error: already queued an enable request */
        अवरोध;
      }
      अवरोध;

    हाल CURL_WANTYES:
      चयन(tn->himq[option]) {
      हाल CURL_EMPTY:
        /* Error: already negotiating for enable */
        अवरोध;
      हाल CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        अवरोध;
      }
      अवरोध;
    }
  }
  अन्यथा { /* NO */
    चयन(tn->him[option]) {
    हाल CURL_NO:
      /* Already disabled */
      अवरोध;

    हाल CURL_YES:
      tn->him[option] = CURL_WANTNO;
      send_negotiation(तथ्य, CURL_DONT, option);
      अवरोध;

    हाल CURL_WANTNO:
      चयन(tn->himq[option]) {
      हाल CURL_EMPTY:
        /* Already negotiating for NO */
        अवरोध;
      हाल CURL_OPPOSITE:
        tn->himq[option] = CURL_EMPTY;
        अवरोध;
      }
      अवरोध;

    हाल CURL_WANTYES:
      चयन(tn->himq[option]) {
      हाल CURL_EMPTY:
        tn->himq[option] = CURL_OPPOSITE;
        अवरोध;
      हाल CURL_OPPOSITE:
        अवरोध;
      }
      अवरोध;
    }
  }
}

अटल व्योम set_local_option(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn,
                             पूर्णांक option, पूर्णांक newstate)
{
  अगर(newstate == CURL_YES) {
    चयन(tn->us[option]) {
    हाल CURL_NO:
      tn->us[option] = CURL_WANTYES;
      send_negotiation(तथ्य, CURL_WILL, option);
      अवरोध;

    हाल CURL_YES:
      /* Already enabled */
      अवरोध;

    हाल CURL_WANTNO:
      चयन(tn->usq[option]) {
      हाल CURL_EMPTY:
        /* Already negotiating for CURL_YES, queue the request */
        tn->usq[option] = CURL_OPPOSITE;
        अवरोध;
      हाल CURL_OPPOSITE:
        /* Error: already queued an enable request */
        अवरोध;
      }
      अवरोध;

    हाल CURL_WANTYES:
      चयन(tn->usq[option]) {
      हाल CURL_EMPTY:
        /* Error: already negotiating for enable */
        अवरोध;
      हाल CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        अवरोध;
      }
      अवरोध;
    }
  }
  अन्यथा { /* NO */
    चयन(tn->us[option]) {
    हाल CURL_NO:
      /* Already disabled */
      अवरोध;

    हाल CURL_YES:
      tn->us[option] = CURL_WANTNO;
      send_negotiation(तथ्य, CURL_WONT, option);
      अवरोध;

    हाल CURL_WANTNO:
      चयन(tn->usq[option]) {
      हाल CURL_EMPTY:
        /* Already negotiating for NO */
        अवरोध;
      हाल CURL_OPPOSITE:
        tn->usq[option] = CURL_EMPTY;
        अवरोध;
      }
      अवरोध;

    हाल CURL_WANTYES:
      चयन(tn->usq[option]) {
      हाल CURL_EMPTY:
        tn->usq[option] = CURL_OPPOSITE;
        अवरोध;
      हाल CURL_OPPOSITE:
        अवरोध;
      }
      अवरोध;
    }
  }
}

अटल व्योम telnet_negotiate(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn)
{
  पूर्णांक i;

  क्रम(i = 0; i < CURL_NTELOPTS; i++) {
    अगर(i == CURL_TELOPT_ECHO)
      जारी;

    अगर(tn->us_preferred[i] == CURL_YES)
      set_local_option(तथ्य, tn, i, CURL_YES);

    अगर(tn->him_preferred[i] == CURL_YES)
      set_remote_option(तथ्य, tn, i, CURL_YES);
  }
}

अटल व्योम rec_will(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn, पूर्णांक option)
{
  चयन(tn->him[option]) {
  हाल CURL_NO:
    अगर(tn->him_preferred[option] == CURL_YES) {
      tn->him[option] = CURL_YES;
      send_negotiation(तथ्य, CURL_DO, option);
    }
    अन्यथा
      send_negotiation(तथ्य, CURL_DONT, option);

    अवरोध;

  हाल CURL_YES:
    /* Already enabled */
    अवरोध;

  हाल CURL_WANTNO:
    चयन(tn->himq[option]) {
    हाल CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_NO;
      अवरोध;
    हाल CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->him[option] = CURL_YES;
      tn->himq[option] = CURL_EMPTY;
      अवरोध;
    }
    अवरोध;

  हाल CURL_WANTYES:
    चयन(tn->himq[option]) {
    हाल CURL_EMPTY:
      tn->him[option] = CURL_YES;
      अवरोध;
    हाल CURL_OPPOSITE:
      tn->him[option] = CURL_WANTNO;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(तथ्य, CURL_DONT, option);
      अवरोध;
    }
    अवरोध;
  }
}

अटल व्योम rec_wont(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn, पूर्णांक option)
{
  चयन(tn->him[option]) {
  हाल CURL_NO:
    /* Already disabled */
    अवरोध;

  हाल CURL_YES:
    tn->him[option] = CURL_NO;
    send_negotiation(तथ्य, CURL_DONT, option);
    अवरोध;

  हाल CURL_WANTNO:
    चयन(tn->himq[option]) {
    हाल CURL_EMPTY:
      tn->him[option] = CURL_NO;
      अवरोध;

    हाल CURL_OPPOSITE:
      tn->him[option] = CURL_WANTYES;
      tn->himq[option] = CURL_EMPTY;
      send_negotiation(तथ्य, CURL_DO, option);
      अवरोध;
    }
    अवरोध;

  हाल CURL_WANTYES:
    चयन(tn->himq[option]) {
    हाल CURL_EMPTY:
      tn->him[option] = CURL_NO;
      अवरोध;
    हाल CURL_OPPOSITE:
      tn->him[option] = CURL_NO;
      tn->himq[option] = CURL_EMPTY;
      अवरोध;
    }
    अवरोध;
  }
}

अटल व्योम printsub(काष्ठा Curl_easy *तथ्य,
                     पूर्णांक direction,                /* '<' or '>' */
                     स्थिर अचिन्हित अक्षर *pointer, /* ptr to suboption data */
                     माप_प्रकार लंबाई)                /* suboption data length */
{
  अगर(तथ्य->set.verbose) {
    अचिन्हित पूर्णांक i = 0;
    अगर(direction) {
      infof(तथ्य, "%s IAC SB ", (direction == '<') ? "RCVD" : "SENT");
      अगर(लंबाई >= 3) {
        पूर्णांक j;

        i = pointer[लंबाई - 2];
        j = pointer[लंबाई - 1];

        अगर(i != CURL_IAC || j != CURL_SE) {
          infof(तथ्य, "(terminated by ");
          अगर(CURL_TELOPT_OK(i))
            infof(तथ्य, "%s ", CURL_TELOPT(i));
          अन्यथा अगर(CURL_TELCMD_OK(i))
            infof(तथ्य, "%s ", CURL_TELCMD(i));
          अन्यथा
            infof(तथ्य, "%u ", i);
          अगर(CURL_TELOPT_OK(j))
            infof(तथ्य, "%s", CURL_TELOPT(j));
          अन्यथा अगर(CURL_TELCMD_OK(j))
            infof(तथ्य, "%s", CURL_TELCMD(j));
          अन्यथा
            infof(तथ्य, "%d", j);
          infof(तथ्य, ", not IAC SE) ");
        }
      }
      अगर(लंबाई >= 2)
        लंबाई -= 2;
      अन्यथा /* bad input */
        वापस;
    }
    अगर(लंबाई <= 1) {
      infof(तथ्य, "(Empty suboption?)");
      वापस;
    }

    अगर(CURL_TELOPT_OK(pointer[0])) {
      चयन(pointer[0]) {
      हाल CURL_TELOPT_TTYPE:
      हाल CURL_TELOPT_XDISPLOC:
      हाल CURL_TELOPT_NEW_ENVIRON:
      हाल CURL_TELOPT_NAWS:
        infof(तथ्य, "%s", CURL_TELOPT(pointer[0]));
        अवरोध;
      शेष:
        infof(तथ्य, "%s (unsupported)", CURL_TELOPT(pointer[0]));
        अवरोध;
      }
    }
    अन्यथा
      infof(तथ्य, "%d (unknown)", pointer[0]);

    चयन(pointer[0]) {
    हाल CURL_TELOPT_NAWS:
      अगर(लंबाई > 4)
        infof(तथ्य, "Width: %d ; Height: %d", (pointer[1] << 8) | pointer[2],
              (pointer[3] << 8) | pointer[4]);
      अवरोध;
    शेष:
      चयन(pointer[1]) {
      हाल CURL_TELQUAL_IS:
        infof(तथ्य, " IS");
        अवरोध;
      हाल CURL_TELQUAL_SEND:
        infof(तथ्य, " SEND");
        अवरोध;
      हाल CURL_TELQUAL_INFO:
        infof(तथ्य, " INFO/REPLY");
        अवरोध;
      हाल CURL_TELQUAL_NAME:
        infof(तथ्य, " NAME");
        अवरोध;
      }

      चयन(pointer[0]) {
      हाल CURL_TELOPT_TTYPE:
      हाल CURL_TELOPT_XDISPLOC:
        infof(तथ्य, " \"%.*s\"",
              (पूर्णांक)((लंबाई > 2) ? (लंबाई - 2) : 0), &pointer[2]);
        अवरोध;
      हाल CURL_TELOPT_NEW_ENVIRON:
        अगर(pointer[1] == CURL_TELQUAL_IS) {
          infof(तथ्य, " ");
          क्रम(i = 3; i < लंबाई; i++) {
            चयन(pointer[i]) {
            हाल CURL_NEW_ENV_VAR:
              infof(तथ्य, ", ");
              अवरोध;
            हाल CURL_NEW_ENV_मूल्य:
              infof(तथ्य, " = ");
              अवरोध;
            शेष:
              infof(तथ्य, "%c", pointer[i]);
              अवरोध;
            }
          }
        }
        अवरोध;
      शेष:
        क्रम(i = 2; i < लंबाई; i++)
          infof(तथ्य, " %.2x", pointer[i]);
        अवरोध;
      }
    }
  }
}

/* Escape and send a telnet data block */
अटल CURLcode send_telnet_तथ्य(काष्ठा Curl_easy *तथ्य,
                                 काष्ठा TELNET *tn,
                                 स्थिर अक्षर *बफर, ssize_t nread)
{
  माप_प्रकार i, outlen;
  स्थिर अचिन्हित अक्षर *outbuf;
  CURLcode परिणाम = CURLE_OK;
  माप_प्रकार bytes_written;
  माप_प्रकार योग_written = 0;
  काष्ठा connectdata *conn = तथ्य->conn;

  DEBUGASSERT(tn);
  DEBUGASSERT(nread > 0);
  अगर(nread < 0)
    वापस CURLE_TOO_LARGE;

  अगर(स_प्रथम(बफर, CURL_IAC, nread)) {
    /* only use the escape buffer when necessary */
    curlx_dyn_reset(&tn->out);

    क्रम(i = 0; i < (माप_प्रकार)nread && !परिणाम; i++) {
      परिणाम = curlx_dyn_addn(&tn->out, &बफर[i], 1);
      अगर(!परिणाम && ((अचिन्हित अक्षर)बफर[i] == CURL_IAC))
        /* IAC is FF in hex */
        परिणाम = curlx_dyn_addn(&tn->out, "\xff", 1);
    }

    outlen = curlx_dyn_len(&tn->out);
    outbuf = curlx_dyn_uptr(&tn->out);
  }
  अन्यथा {
    outlen = (माप_प्रकार)nread;
    outbuf = (स्थिर अचिन्हित अक्षर *)बफर;
  }
  जबतक(!परिणाम && योग_written < outlen) {
    /* Make sure socket is writable to avoid EWOULDBLOCK condition */
    काष्ठा pollfd pfd[1];
    timediff_t timeout_ms = Curl_timeleft_ms(तथ्य);
    pfd[0].fd = conn->sock[FIRSTSOCKET];
    pfd[0].events = POLLOUT;
    अगर(timeout_ms < 0)
      वापस CURLE_OPERATION_TIMEDOUT;
    /* 0 means no timeout configured; pass -1 to poll for infinite wait */
    चयन(Curl_poll(pfd, 1, timeout_ms ? timeout_ms : -1)) {
    हाल -1:                    /* error, abort writing */
      परिणाम = CURLE_SEND_ERROR;
      अवरोध;
    हाल 0:                     /* timeout */
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      अवरोध;
    शेष:                    /* write! */
      bytes_written = 0;
      परिणाम = Curl_xfer_send(तथ्य, outbuf + योग_written,
                              outlen - योग_written, FALSE, &bytes_written);
      योग_written += bytes_written;
      अवरोध;
    }
  }

  वापस परिणाम;
}

/*
 * sendsuboption()
 *
 * Send suboption information to the server side.
 */
अटल व्योम sendsuboption(काष्ठा Curl_easy *तथ्य,
                          काष्ठा TELNET *tn, पूर्णांक option)
{
  माप_प्रकार nwritten;
  अचिन्हित लघु x, y;
  स्थिर अचिन्हित अक्षर *uc1, *uc2;
  CURLcode परिणाम;

  चयन(option) {
  हाल CURL_TELOPT_NAWS:
    /* We prepare data to be sent */
    CURL_SB_CLEAR(tn);
    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SB);
    CURL_SB_ACCUM(tn, CURL_TELOPT_NAWS);
    /* We must deal either with little or big endian processors */
    /* Window size must be sent according to the 'network order' */
    x = htons(tn->subopt_wsx);
    y = htons(tn->subopt_wsy);
    uc1 = (स्थिर अचिन्हित अक्षर *)&x;
    uc2 = (स्थिर अचिन्हित अक्षर *)&y;
    CURL_SB_ACCUM(tn, uc1[0]);
    CURL_SB_ACCUM(tn, uc1[1]);
    CURL_SB_ACCUM(tn, uc2[0]);
    CURL_SB_ACCUM(tn, uc2[1]);

    CURL_SB_ACCUM(tn, CURL_IAC);
    CURL_SB_ACCUM(tn, CURL_SE);
    CURL_SB_TERM(tn);
    /* data suboption is now ready */

    printsub(तथ्य, '>', (स्थिर अचिन्हित अक्षर *)tn->subbuffer + 2,
             CURL_SB_LEN(tn) - 2);

    /* we send the header of the suboption... */
    परिणाम = Curl_xfer_send(तथ्य, tn->subbuffer, 3, FALSE, &nwritten);
    अगर(परिणाम)
      failf(तथ्य, "Sending data failed: %s", curl_easy_strerror(परिणाम));
    /* ... then the window size with the send_telnet_data() function
       to deal with 0xFF cases ... */
    send_telnet_तथ्य(तथ्य, tn, (स्थिर अक्षर *)tn->subbuffer + 3, 4);
    /* ... and the footer */
    परिणाम = Curl_xfer_send(तथ्य, tn->subbuffer + 7, 2, FALSE, &nwritten);
    अगर(परिणाम)
      failf(तथ्य, "Sending data failed: %s", curl_easy_strerror(परिणाम));
    अवरोध;
  }
}

अटल व्योम rec_do(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn, पूर्णांक option)
{
  चयन(tn->us[option]) {
  हाल CURL_NO:
    अगर(tn->us_preferred[option] == CURL_YES) {
      tn->us[option] = CURL_YES;
      send_negotiation(तथ्य, CURL_WILL, option);
      अगर(tn->subnegotiation[option] == CURL_YES)
        /* transmission of data option */
        sendsuboption(तथ्य, tn, option);
    }
    अन्यथा अगर(tn->subnegotiation[option] == CURL_YES) {
      /* send information to achieve this option */
      tn->us[option] = CURL_YES;
      send_negotiation(तथ्य, CURL_WILL, option);
      sendsuboption(तथ्य, tn, option);
    }
    अन्यथा
      send_negotiation(तथ्य, CURL_WONT, option);
    अवरोध;

  हाल CURL_YES:
    /* Already enabled */
    अवरोध;

  हाल CURL_WANTNO:
    चयन(tn->usq[option]) {
    हाल CURL_EMPTY:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_NO;
      अवरोध;
    हाल CURL_OPPOSITE:
      /* Error: DONT answered by WILL */
      tn->us[option] = CURL_YES;
      tn->usq[option] = CURL_EMPTY;
      अवरोध;
    }
    अवरोध;

  हाल CURL_WANTYES:
    चयन(tn->usq[option]) {
    हाल CURL_EMPTY:
      tn->us[option] = CURL_YES;
      अगर(tn->subnegotiation[option] == CURL_YES) {
        /* transmission of data option */
        sendsuboption(तथ्य, tn, option);
      }
      अवरोध;
    हाल CURL_OPPOSITE:
      tn->us[option] = CURL_WANTNO;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(तथ्य, CURL_WONT, option);
      अवरोध;
    }
    अवरोध;
  }
}

अटल व्योम rec_dont(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn, पूर्णांक option)
{
  चयन(tn->us[option]) {
  हाल CURL_NO:
    /* Already disabled */
    अवरोध;

  हाल CURL_YES:
    tn->us[option] = CURL_NO;
    send_negotiation(तथ्य, CURL_WONT, option);
    अवरोध;

  हाल CURL_WANTNO:
    चयन(tn->usq[option]) {
    हाल CURL_EMPTY:
      tn->us[option] = CURL_NO;
      अवरोध;

    हाल CURL_OPPOSITE:
      tn->us[option] = CURL_WANTYES;
      tn->usq[option] = CURL_EMPTY;
      send_negotiation(तथ्य, CURL_WILL, option);
      अवरोध;
    }
    अवरोध;

  हाल CURL_WANTYES:
    चयन(tn->usq[option]) {
    हाल CURL_EMPTY:
      tn->us[option] = CURL_NO;
      अवरोध;
    हाल CURL_OPPOSITE:
      tn->us[option] = CURL_NO;
      tn->usq[option] = CURL_EMPTY;
      अवरोध;
    }
    अवरोध;
  }
}

अटल bool str_is_nonascii(स्थिर अक्षर *str)
{
  अक्षर c;
  जबतक((c = *str++) != 0)
    अगर(c & 0x80)
      वापस TRUE;

  वापस FALSE;
}

अटल CURLcode check_telnet_options(काष्ठा Curl_easy *तथ्य,
                                     काष्ठा TELNET *tn)
{
  काष्ठा curl_slist *शीर्ष;
  काष्ठा curl_slist *beg;
  CURLcode परिणाम = CURLE_OK;

  /* Add the username as an environment variable if it
     was given on the command line */
  अगर(तथ्य->state.creds) {
    अक्षर बफर[256];
    अगर(str_is_nonascii(Curl_creds_user(तथ्य->conn->creds))) {
      DEBUGF(infof(तथ्य, "set a non ASCII username in telnet"));
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    }
    curl_msnprintf(बफर, माप(बफर), "USER,%s",
                   Curl_creds_user(तथ्य->conn->creds));
    beg = curl_slist_append(tn->telnet_vars, बफर);
    अगर(!beg) {
      curl_slist_free_all(tn->telnet_vars);
      tn->telnet_vars = शून्य;
      वापस CURLE_OUT_OF_MEMORY;
    }
    tn->telnet_vars = beg;
    tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
  }

  क्रम(शीर्ष = तथ्य->set.telnet_options; शीर्ष && !परिणाम; शीर्ष = शीर्ष->अग्र) {
    माप_प्रकार olen;
    स्थिर अक्षर *option = शीर्ष->तथ्य;
    स्थिर अक्षर *arg;
    स्थिर अक्षर *sep = म_अक्षर(option, '=');
    अगर(sep) {
      olen = sep - option;
      arg = ++sep;
      अगर(str_is_nonascii(arg))
        जारी;
      चयन(olen) {
      हाल 5:
        /* Terminal type */
        अगर(curl_strnequal(option, "TTYPE", 5)) {
          tn->subopt_ttype = arg;
          tn->us_preferred[CURL_TELOPT_TTYPE] = CURL_YES;
          अवरोध;
        }
        परिणाम = CURLE_UNKNOWN_OPTION;
        अवरोध;

      हाल 8:
        /* Display variable */
        अगर(curl_strnequal(option, "XDISPLOC", 8)) {
          tn->subopt_xdisploc = arg;
          tn->us_preferred[CURL_TELOPT_XDISPLOC] = CURL_YES;
          अवरोध;
        }
        परिणाम = CURLE_UNKNOWN_OPTION;
        अवरोध;

      हाल 7:
        /* Environment variable */
        अगर(curl_strnequal(option, "NEW_ENV", 7)) {
          beg = curl_slist_append(tn->telnet_vars, arg);
          अगर(!beg) {
            परिणाम = CURLE_OUT_OF_MEMORY;
            अवरोध;
          }
          tn->telnet_vars = beg;
          tn->us_preferred[CURL_TELOPT_NEW_ENVIRON] = CURL_YES;
        }
        अन्यथा
          परिणाम = CURLE_UNKNOWN_OPTION;
        अवरोध;

      हाल 2:
        /* Window Size */
        अगर(curl_strnequal(option, "WS", 2)) {
          स्थिर अक्षर *p = arg;
          curl_off_t x = 0;
          curl_off_t y = 0;
          अगर(curlx_str_संख्या(&p, &x, 0xffff) ||
             curlx_str_single(&p, 'x') ||
             curlx_str_संख्या(&p, &y, 0xffff)) {
            failf(तथ्य, "Syntax error in telnet option: %s", शीर्ष->तथ्य);
            परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
          }
          अन्यथा {
            tn->subopt_wsx = (अचिन्हित लघु)x;
            tn->subopt_wsy = (अचिन्हित लघु)y;
            tn->us_preferred[CURL_TELOPT_NAWS] = CURL_YES;
          }
        }
        अन्यथा
          परिणाम = CURLE_UNKNOWN_OPTION;
        अवरोध;

      हाल 6:
        /* To take care or not of the 8th bit in data exchange */
        अगर(curl_strnequal(option, "BINARY", 6)) {
          स्थिर अक्षर *p = arg;
          curl_off_t द्विआधारी_option;
          अगर(!curlx_str_संख्या(&p, &द्विआधारी_option, 1) &&
             (द्विआधारी_option != 1)) {
            tn->us_preferred[CURL_TELOPT_द्विआधारी] = CURL_NO;
            tn->him_preferred[CURL_TELOPT_द्विआधारी] = CURL_NO;
          }
        }
        अन्यथा
          परिणाम = CURLE_UNKNOWN_OPTION;
        अवरोध;
      शेष:
        failf(तथ्य, "Unknown telnet option %s", शीर्ष->तथ्य);
        परिणाम = CURLE_UNKNOWN_OPTION;
        अवरोध;
      }
    }
    अन्यथा {
      failf(तथ्य, "Syntax error in telnet option: %s", शीर्ष->तथ्य);
      परिणाम = CURLE_SETOPT_OPTION_SYNTAX;
    }
  }

  अगर(परिणाम) {
    curl_slist_free_all(tn->telnet_vars);
    tn->telnet_vars = शून्य;
  }

  वापस परिणाम;
}

/* if the option contains an IAC code, it should be escaped in the output, but
   as we cannot think of any legit way to send that as part of the content we
   rather ban its use instead */
अटल bool bad_option(स्थिर अक्षर *तथ्य)
{
  वापस !तथ्य || !!म_अक्षर(तथ्य, CURL_IAC);
}

/*
 * suboption()
 *
 * Look at the sub-option buffer, and try to be helpful to the other
 * side.
 */
अटल CURLcode suboption(काष्ठा Curl_easy *तथ्य, काष्ठा TELNET *tn)
{
  काष्ठा curl_slist *v;
  अचिन्हित अक्षर अस्थायी[2048];
  माप_प्रकार nwritten;
  माप_प्रकार len;
  CURLcode परिणाम = CURLE_OK;

  अगर(!CURL_SB_LEN(tn)) /* ignore empty suboption */
    वापस CURLE_OK;

  printsub(तथ्य, '<', (स्थिर अचिन्हित अक्षर *)tn->subbuffer,
           CURL_SB_LEN(tn) + 2);
  चयन(CURL_SB_GET(tn)) {
  हाल CURL_TELOPT_TTYPE:
    अगर(bad_option(tn->subopt_ttype))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(म_माप(tn->subopt_ttype) > 1000) {
      failf(तथ्य, "Too long telnet TTYPE");
      वापस CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((अक्षर *)अस्थायी, माप(अस्थायी), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_TTYPE,
                         CURL_TELQUAL_IS, tn->subopt_ttype, CURL_IAC,
                         CURL_SE);
    परिणाम = Curl_xfer_send(तथ्य, अस्थायी, len, FALSE, &nwritten);
    अगर(परिणाम) {
      failf(तथ्य, "Sending data failed: %s", curl_easy_strerror(परिणाम));
      वापस परिणाम;
    }
    printsub(तथ्य, '>', &अस्थायी[2], len-2);
    अवरोध;
  हाल CURL_TELOPT_XDISPLOC:
    अगर(bad_option(tn->subopt_xdisploc))
      वापस CURLE_BAD_FUNCTION_ARGUMENT;
    अगर(म_माप(tn->subopt_xdisploc) > 1000) {
      failf(तथ्य, "Too long telnet XDISPLOC");
      वापस CURLE_SEND_ERROR;
    }
    len = curl_msnprintf((अक्षर *)अस्थायी, माप(अस्थायी), "%c%c%c%c%s%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_XDISPLOC,
                         CURL_TELQUAL_IS, tn->subopt_xdisploc, CURL_IAC,
                         CURL_SE);
    परिणाम = Curl_xfer_send(तथ्य, अस्थायी, len, FALSE, &nwritten);
    अगर(परिणाम) {
      failf(तथ्य, "Sending data failed: %s", curl_easy_strerror(परिणाम));
      वापस परिणाम;
    }
    printsub(तथ्य, '>', &अस्थायी[2], len - 2);
    अवरोध;
  हाल CURL_TELOPT_NEW_ENVIRON:
    len = curl_msnprintf((अक्षर *)अस्थायी, माप(अस्थायी), "%c%c%c%c",
                         CURL_IAC, CURL_SB, CURL_TELOPT_NEW_ENVIRON,
                         CURL_TELQUAL_IS);
    क्रम(v = tn->telnet_vars; v; v = v->अग्र) {
      माप_प्रकार tmplen = (म_माप(v->तथ्य) + 1);
      अगर(bad_option(v->तथ्य))
        वापस CURLE_BAD_FUNCTION_ARGUMENT;
      /* Add the variable if it fits */
      अगर(len + tmplen < माप(अस्थायी) - 6) {
        स्थिर अक्षर *s = म_अक्षर(v->तथ्य, ',');
        अगर(!s)
          len += curl_msnprintf((अक्षर *)&अस्थायी[len], माप(अस्थायी) - len,
                                "%c%s", CURL_NEW_ENV_VAR, v->तथ्य);
        अन्यथा {
          माप_प्रकार vlen = s - v->तथ्य;
          len += curl_msnprintf((अक्षर *)&अस्थायी[len], माप(अस्थायी) - len,
                                "%c%.*s%c%s", CURL_NEW_ENV_VAR,
                                (पूर्णांक)vlen, v->तथ्य, CURL_NEW_ENV_मूल्य, ++s);
        }
      }
    }
    curl_msnprintf((अक्षर *)&अस्थायी[len], माप(अस्थायी) - len,
                   "%c%c", CURL_IAC, CURL_SE);
    len += 2;
    परिणाम = Curl_xfer_send(तथ्य, अस्थायी, len, FALSE, &nwritten);
    अगर(परिणाम) {
      failf(तथ्य, "Sending data failed: %s", curl_easy_strerror(परिणाम));
      वापस परिणाम;
    }
    printsub(तथ्य, '>', &अस्थायी[2], len - 2);
    अवरोध;
  }
  वापस CURLE_OK;
}

अटल CURLcode telrcv(काष्ठा Curl_easy *तथ्य,
                       काष्ठा TELNET *tn,
                       स्थिर अचिन्हित अक्षर *inbuf, /* Data received from
                                                      socket */
                       ssize_t गिनती)              /* Number of bytes
                                                      received */
{
  अचिन्हित अक्षर c;
  CURLcode परिणाम;
  पूर्णांक in = 0;
  पूर्णांक startwrite = -1;

#घोषणा startskipping()                                          \
  अगर(startwrite >= 0) {                                          \
    परिणाम = Curl_client_write(तथ्य,                             \
                               CLIENTWRITE_BODY,                 \
                               (स्थिर अक्षर *)&inbuf[startwrite], \
                               in-startwrite);                   \
    अगर(परिणाम)                                                   \
      वापस परिणाम;                                             \
  }                                                              \
  startwrite = -1

#घोषणा writebyte()  \
  अगर(startwrite < 0) \
    startwrite = in

#घोषणा bufferflush() startskipping()

  जबतक(गिनती--) {
    c = inbuf[in];

    चयन(tn->telrcv_state) {
    हाल CURL_TS_CR:
      tn->telrcv_state = CURL_TS_तथ्य;
      अगर(c == '\0') {
        startskipping();
        अवरोध;   /* Ignore \0 after CR */
      }
      writebyte();
      अवरोध;

    हाल CURL_TS_तथ्य:
      अगर(c == CURL_IAC) {
        tn->telrcv_state = CURL_TS_IAC;
        startskipping();
        अवरोध;
      }
      अन्यथा अगर(c == '\r')
        tn->telrcv_state = CURL_TS_CR;
      writebyte();
      अवरोध;

    हाल CURL_TS_IAC:
      DEBUGASSERT(startwrite < 0);
      चयन(c) {
      हाल CURL_WILL:
        tn->telrcv_state = CURL_TS_WILL;
        अवरोध;
      हाल CURL_WONT:
        tn->telrcv_state = CURL_TS_WONT;
        अवरोध;
      हाल CURL_DO:
        tn->telrcv_state = CURL_TS_DO;
        अवरोध;
      हाल CURL_DONT:
        tn->telrcv_state = CURL_TS_DONT;
        अवरोध;
      हाल CURL_SB:
        CURL_SB_CLEAR(tn);
        tn->telrcv_state = CURL_TS_SB;
        अवरोध;
      हाल CURL_IAC:
        tn->telrcv_state = CURL_TS_तथ्य;
        writebyte();
        अवरोध;
      हाल CURL_DM:
      हाल CURL_NOP:
      हाल CURL_GA:
      शेष:
        tn->telrcv_state = CURL_TS_तथ्य;
        printoption(तथ्य, "RCVD", CURL_IAC, c);
        अवरोध;
      }
      अवरोध;

    हाल CURL_TS_WILL:
      printoption(तथ्य, "RCVD", CURL_WILL, c);
      tn->please_negotiate = 1;
      rec_will(तथ्य, tn, c);
      tn->telrcv_state = CURL_TS_तथ्य;
      अवरोध;

    हाल CURL_TS_WONT:
      printoption(तथ्य, "RCVD", CURL_WONT, c);
      tn->please_negotiate = 1;
      rec_wont(तथ्य, tn, c);
      tn->telrcv_state = CURL_TS_तथ्य;
      अवरोध;

    हाल CURL_TS_DO:
      printoption(तथ्य, "RCVD", CURL_DO, c);
      tn->please_negotiate = 1;
      rec_do(तथ्य, tn, c);
      tn->telrcv_state = CURL_TS_तथ्य;
      अवरोध;

    हाल CURL_TS_DONT:
      printoption(तथ्य, "RCVD", CURL_DONT, c);
      tn->please_negotiate = 1;
      rec_dont(तथ्य, tn, c);
      tn->telrcv_state = CURL_TS_तथ्य;
      अवरोध;

    हाल CURL_TS_SB:
      अगर(c == CURL_IAC)
        tn->telrcv_state = CURL_TS_SE;
      अन्यथा
        CURL_SB_ACCUM(tn, c);
      अवरोध;

    हाल CURL_TS_SE:
      अगर(c != CURL_SE) {
        अगर(c != CURL_IAC) {
          /*
           * This is an error. We only expect to get "IAC IAC" or "IAC SE".
           * Several things may have happened. An IAC was not doubled, the IAC
           * SE was left off, or another option got inserted into the
           * suboption are all possibilities.
           */
          failf(तथ्य, "telnet: suboption error");
          वापस CURLE_RECV_ERROR;
        }
        CURL_SB_ACCUM(tn, c);
        tn->telrcv_state = CURL_TS_SB;
      }
      अन्यथा {
        CURL_SB_ACCUM(tn, CURL_IAC);
        CURL_SB_ACCUM(tn, CURL_SE);
        tn->subpointer -= 2;
        CURL_SB_TERM(tn);
        परिणाम = suboption(तथ्य, tn);   /* handle sub-option */
        अगर(परिणाम)
          वापस परिणाम;
        tn->telrcv_state = CURL_TS_तथ्य;
      }
      अवरोध;
    }
    ++in;
  }
  bufferflush();
  वापस CURLE_OK;
}

अटल CURLcode telnet_done(काष्ठा Curl_easy *तथ्य,
                            CURLcode status, bool premature)
{
  (व्योम)status;
  (व्योम)premature;
  Curl_meta_remove(तथ्य, CURL_META_TELNET_EASY);
  वापस CURLE_OK;
}

अटल CURLcode telnet_do(काष्ठा Curl_easy *तथ्य, bool *done)
{
  CURLcode परिणाम;
  काष्ठा connectdata *conn = तथ्य->conn;
  curl_socket_t sockfd = conn->sock[FIRSTSOCKET];
#अगर_घोषित USE_WINSOCK
  WSAEVENT event_handle;
  WSANETWORKEVENTS events;
  HANDLE stdin_handle;
  HANDLE objs[2];
  DWORD obj_गिनती;
  DWORD wait_timeout;
  DWORD readfile_read;
#अन्यथा
  timediff_t interval_ms;
  काष्ठा pollfd pfd[2];
  पूर्णांक poll_cnt;
  ssize_t snread;
#पूर्ण_अगर
  bool keepon = TRUE;
  अक्षर बफर[4 * 1024];
  काष्ठा TELNET *tn;

  *done = TRUE; /* unconditionally */

  परिणाम = init_telnet(तथ्य);
  अगर(परिणाम)
    वापस परिणाम;

  tn = Curl_meta_get(तथ्य, CURL_META_TELNET_EASY);
  अगर(!tn)
    वापस CURLE_FAILED_INIT;

  परिणाम = check_telnet_options(तथ्य, tn);
  अगर(परिणाम)
    वापस परिणाम;

#अगर_घोषित USE_WINSOCK
  /* We want to wait for both stdin and the socket. Since
   * the select() function in Winsock only works on sockets
   * we have to use the WaitForMultipleObjects() call.
   */

  /* First, create a sockets event object */
  event_handle = WSACreateEvent();
  अगर(event_handle == WSA_INVALID_EVENT) {
    failf(तथ्य, "WSACreateEvent failed (%d)", SOCKERRNO);
    वापस CURLE_FAILED_INIT;
  }

  /* Tell Winsock what events we want to listen to */
  अगर(WSAEventSelect(sockfd, event_handle, FD_READ | FD_CLOSE)) {
    WSACloseEvent(event_handle);
    वापस CURLE_RECV_ERROR;
  }

  /* Then get the Windows file handle for stdin */
  stdin_handle = GetStdHandle(STD_निवेश_HANDLE);

  /* Create the list of objects to wait for */
  objs[0] = event_handle;
  objs[1] = stdin_handle;

  /* If stdin_handle is a pipe, use PeekNamedPipe() method to check it,
     else use the old WaitForMultipleObjects() way */
  अगर(GetFileType(stdin_handle) == FILE_TYPE_PIPE || तथ्य->set.is_fread_set) {
    /* Do not wait for stdin_handle, wait for event_handle */
    obj_गिनती = 1;
    /* Check stdin_handle per 100 milliseconds */
    wait_timeout = 100;
  }
  अन्यथा {
    obj_गिनती = 2;
    wait_timeout = 1000;
  }

  /* Keep on listening and act on events */
  जबतक(keepon) {
    स्थिर DWORD buf_आकार = (DWORD)माप(बफर);
    DWORD waitret = WaitForMultipleObjects(obj_गिनती, objs,
                                           FALSE, wait_timeout);
    चयन(waitret) {

    हाल WAIT_TIMEOUT: {
      क्रम(;;) {
        अगर(तथ्य->set.is_fread_set) {
          माप_प्रकार n;
          /* read from user-supplied method */
          n = तथ्य->state.fread_func(बफर, 1, buf_आकार, तथ्य->state.in);
          अगर(n == CURL_READFUNC_ABORT) {
            failf(तथ्य, "operation aborted by callback");
            keepon = FALSE;
            परिणाम = CURLE_ABORTED_BY_CALLBACK;
            अवरोध;
          }

          अगर(n == CURL_READFUNC_PAUSE)
            अवरोध;

          अगर(n == 0)                        /* no bytes */
            अवरोध;

          /* fall through with number of bytes read */
          readfile_read = (DWORD)n;
        }
        अन्यथा {
          /* read from stdin */
          अगर(!PeekNamedPipe(stdin_handle, शून्य, 0, शून्य,
                            &readfile_read, शून्य)) {
            keepon = FALSE;
            परिणाम = CURLE_READ_ERROR;
            अवरोध;
          }

          अगर(!readfile_read)
            अवरोध;

          अगर(!ReadFile(stdin_handle, बफर, buf_आकार, &readfile_read, शून्य)) {
            keepon = FALSE;
            परिणाम = CURLE_READ_ERROR;
            अवरोध;
          }
        }

        परिणाम = send_telnet_तथ्य(तथ्य, tn, बफर, readfile_read);
        अगर(परिणाम) {
          keepon = FALSE;
          अवरोध;
        }
      }
    }
    अवरोध;

    हाल WAIT_OBJECT_0 + 1: {
      अगर(!ReadFile(stdin_handle, बफर, buf_आकार, &readfile_read, शून्य)) {
        keepon = FALSE;
        परिणाम = CURLE_READ_ERROR;
        अवरोध;
      }

      परिणाम = send_telnet_तथ्य(तथ्य, tn, बफर, readfile_read);
      अगर(परिणाम) {
        keepon = FALSE;
        अवरोध;
      }
    }
    अवरोध;

    हाल WAIT_OBJECT_0: {
      events.lNetworkEvents = 0;
      अगर(WSAEnumNetworkEvents(sockfd, event_handle, &events) != 0) {
        पूर्णांक sockerr = SOCKERRNO;
        अगर(sockerr != SOCKEINPROGRESS) {
          infof(तथ्य, "WSAEnumNetworkEvents failed (%d)", sockerr);
          keepon = FALSE;
          परिणाम = CURLE_READ_ERROR;
        }
        अवरोध;
      }
      अगर(events.lNetworkEvents & FD_READ) {
        /* read data from network */
        माप_प्रकार nread;
        परिणाम = Curl_xfer_recv(तथ्य, बफर, माप(बफर), &nread);
        /* read would have blocked. Loop again */
        अगर(परिणाम == CURLE_AGAIN)
          अवरोध;
        /* returned not-zero, this an error */
        अन्यथा अगर(परिणाम) {
          keepon = FALSE;
          अवरोध;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा अगर(!nread) {
          keepon = FALSE;
          अवरोध;
        }

        परिणाम = telrcv(तथ्य, tn, (अचिन्हित अक्षर *)बफर, nread);
        अगर(परिणाम) {
          keepon = FALSE;
          अवरोध;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        अगर(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(तथ्य, tn);
          tn->already_negotiated = 1;
        }
      }
      अगर(events.lNetworkEvents & FD_CLOSE) {
        keepon = FALSE;
      }
      अवरोध;
    }
    } /* switch */

    अगर(Curl_timeleft_ms(तथ्य) < 0) {
      failf(तथ्य, "Time-out");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }
  }

  /* We called WSACreateEvent, so call WSACloseEvent */
  अगर(!WSACloseEvent(event_handle)) {
    infof(तथ्य, "WSACloseEvent failed (%d)", SOCKERRNO);
  }
#अन्यथा
  pfd[0].fd = sockfd;
  pfd[0].events = POLLIN;

  अगर(तथ्य->set.is_fread_set) {
    poll_cnt = 1;
    interval_ms = 100; /* poll user-supplied read function */
  }
  अन्यथा {
    /* really using fread, so infile is a FILE* */
    pfd[1].fd = fileno((खाता *)तथ्य->state.in);
    pfd[1].events = POLLIN;
    poll_cnt = 2;
    interval_ms = 1 * 1000;
    अगर(pfd[1].fd < 0) {
      failf(तथ्य, "cannot read input");
      परिणाम = CURLE_RECV_ERROR;
      keepon = FALSE;
    }
  }

  जबतक(keepon) {
    DEBUGF(infof(तथ्य, "telnet_do, poll %d fds", poll_cnt));
    चयन(Curl_poll(pfd, (अचिन्हित पूर्णांक)poll_cnt, interval_ms)) {
    हाल -1:                    /* error, stop reading */
      परिणाम = CURLE_RECV_ERROR;
      keepon = FALSE;
      जारी;
    हाल 0:                     /* timeout */
      pfd[0].revents = 0;
      pfd[1].revents = 0;
      FALLTHROUGH();
    शेष:                    /* read! */
      अगर(pfd[0].revents & POLLIN) {
        /* read data from network */
        माप_प्रकार nread;
        परिणाम = Curl_xfer_recv(तथ्य, बफर, माप(बफर), &nread);
        /* read would have blocked. Loop again */
        अगर(परिणाम == CURLE_AGAIN)
          अवरोध;
        /* returned not-zero, this an error */
        अगर(परिणाम) {
          keepon = FALSE;
          /* In test 1452, macOS sees a ECONNRESET sometimes? Is this the
           * telnet test server not shutting down the socket in a clean way?
           * Seems to be timing related, happens more on slow debug build */
          अगर(तथ्य->state.os_errno == SOCKECONNRESET) {
            DEBUGF(infof(तथ्य, "telnet_do, unexpected ECONNRESET on recv"));
          }
          अवरोध;
        }
        /* returned zero but actually received 0 or less here,
           the server closed the connection and we bail out */
        अन्यथा अगर(!nread) {
          keepon = FALSE;
          अवरोध;
        }

        Curl_pgrs_download_inc(तथ्य, nread);
        परिणाम = telrcv(तथ्य, tn, (अचिन्हित अक्षर *)बफर, nread);
        अगर(परिणाम) {
          keepon = FALSE;
          अवरोध;
        }

        /* Negotiate if the peer has started negotiating,
           otherwise do not. We do not want to speak telnet with
           non-telnet servers, like POP or SMTP. */
        अगर(tn->please_negotiate && !tn->already_negotiated) {
          telnet_negotiate(तथ्य, tn);
          tn->already_negotiated = 1;
        }
      }

      snread = 0;
      अगर(poll_cnt == 2) {
        अगर(pfd[1].revents & POLLIN) { /* read from in file */
          snread = पढ़ो(pfd[1].fd, बफर, माप(बफर));
        }
      }
      अन्यथा {
        /* read from user-supplied method */
        snread = (पूर्णांक)तथ्य->state.fread_func(बफर, 1, माप(बफर),
                                             तथ्य->state.in);
        अगर(snread == CURL_READFUNC_ABORT) {
          failf(तथ्य, "operation aborted by callback");
          परिणाम = CURLE_ABORTED_BY_CALLBACK;
          keepon = FALSE;
          अवरोध;
        }
        अगर(snread == CURL_READFUNC_PAUSE)
          अवरोध;
      }

      अगर(snread > 0) {
        परिणाम = send_telnet_तथ्य(तथ्य, tn, बफर, snread);
        अगर(परिणाम) {
          keepon = FALSE;
          अवरोध;
        }
        Curl_pgrs_upload_inc(तथ्य, (माप_प्रकार)snread);
      }
      अन्यथा अगर(snread < 0) {
        failf(तथ्य, "Failed to read input");
        परिणाम = CURLE_READ_ERROR;
        keepon = FALSE;
      }

      अवरोध;
    } /* poll switch statement */

    अगर(Curl_timeleft_ms(तथ्य) < 0) {
      failf(तथ्य, "Time-out");
      परिणाम = CURLE_OPERATION_TIMEDOUT;
      keepon = FALSE;
    }

    अगर(!परिणाम) {
      परिणाम = Curl_pgrsUpdate(तथ्य);
      अगर(परिणाम)
        keepon = FALSE;
    }
  }
#पूर्ण_अगर
  /* mark this as "no further transfer wanted" */
  Curl_xfer_setup_nop(तथ्य);

  वापस परिणाम;
}

/*
 * TELNET protocol handler.
 */
स्थिर काष्ठा Curl_protocol Curl_protocol_telnet = {
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

#पूर्ण_अगर /* !CURL_DISABLE_TELNET */
