#अगर_अघोषित HEADER_CURL_ARPA_TELNET_H
#घोषणा HEADER_CURL_ARPA_TELNET_H
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
#अगर_अघोषित CURL_DISABLE_TELNET
/*
 * Telnet option defines. Add more here if in need.
 */
#घोषणा CURL_TELOPT_द्विआधारी      0   /* binary 8-bit data */
#घोषणा CURL_TELOPT_ECHO        1   /* echo */
#घोषणा CURL_TELOPT_SGA         3   /* Suppress Go Ahead */
#घोषणा CURL_TELOPT_EXOPL       255 /* EXtended OPtions List */
#घोषणा CURL_TELOPT_TTYPE       24  /* Terminal TYPE */
#घोषणा CURL_TELOPT_NAWS        31  /* Negotiate About Window Size */
#घोषणा CURL_TELOPT_XDISPLOC    35  /* X DISPlay LOCation */
#घोषणा CURL_TELOPT_NEW_ENVIRON 39  /* NEW ENVIRONment variables */

#घोषणा CURL_NEW_ENV_VAR   0
#घोषणा CURL_NEW_ENV_मूल्य 1

#अगर_घोषित CURLVERBOSE
/*
 * The telnet options represented as strings
 */
अटल स्थिर अक्षर * स्थिर telnetoptions[] = {
  "BINARY",      "ECHO",           "RCP",           "SUPPRESS GO AHEAD",
  "NAME",        "STATUS",         "TIMING MARK",   "RCTE",
  "NAOL",        "NAOP",           "NAOCRD",        "NAOHTS",
  "NAOHTD",      "NAOFFD",         "NAOVTS",        "NAOVTD",
  "NAOLFD",      "EXTEND ASCII",   "LOGOUT",        "BYTE MACRO",
  "DE TERMINAL", "SUPDUP",         "SUPDUP OUTPUT", "SEND LOCATION",
  "TERM TYPE",   "END OF RECORD",  "TACACS UID",    "OUTPUT MARKING",
  "TTYLOC",      "3270 REGIME",    "X3 PAD",        "NAWS",
  "TERM SPEED",  "LFLOW",          "LINEMODE",      "XDISPLOC",
  "OLD-ENVIRON", "AUTHENTICATION", "ENCRYPT",       "NEW-ENVIRON"
};
#घोषणा CURL_TELOPT(x) telnetoptions[x]
#अन्यथा
#घोषणा CURL_TELOPT(x) ""
#पूर्ण_अगर

#घोषणा CURL_TELOPT_MAXIMUM CURL_TELOPT_NEW_ENVIRON

#घोषणा CURL_TELOPT_OK(x) ((x) <= CURL_TELOPT_MAXIMUM)

#घोषणा CURL_NTELOPTS 40

/*
 * First some defines
 */
#घोषणा CURL_xEOF 236 /* End Of File */
#घोषणा CURL_SE   240 /* Sub negotiation End */
#घोषणा CURL_NOP  241 /* No OPeration */
#घोषणा CURL_DM   242 /* Data Mark */
#घोषणा CURL_GA   249 /* Go Ahead, reverse the line */
#घोषणा CURL_SB   250 /* SuBnegotiation */
#घोषणा CURL_WILL 251 /* Our side WILL use this option */
#घोषणा CURL_WONT 252 /* Our side will not use this option */
#घोषणा CURL_DO   253 /* DO use this option! */
#घोषणा CURL_DONT 254 /* DO NOT use this option! */
#घोषणा CURL_IAC  255 /* Interpret As Command */

#अगर_घोषित CURLVERBOSE
/*
 * Then those numbers represented as strings:
 */
अटल स्थिर अक्षर * स्थिर telnetcmds[] = {
  "EOF",  "SUSP",  "ABORT", "EOR",  "SE",
  "NOP",  "DMARK", "BRK",   "IP",   "AO",
  "AYT",  "EC",    "EL",    "GA",   "SB",
  "WILL", "WONT",  "DO",    "DONT", "IAC"
};
#पूर्ण_अगर

#घोषणा CURL_TELCMD_MINIMUM CURL_xEOF /* the first one */
#घोषणा CURL_TELCMD_MAXIMUM CURL_IAC  /* surprise, 255 is the last one! ;-) */

#घोषणा CURL_TELQUAL_IS   0
#घोषणा CURL_TELQUAL_SEND 1
#घोषणा CURL_TELQUAL_INFO 2
#घोषणा CURL_TELQUAL_NAME 3

#घोषणा CURL_TELCMD_OK(x)                        \
  (((अचिन्हित पूर्णांक)(x) >= CURL_TELCMD_MINIMUM) && \
   ((अचिन्हित पूर्णांक)(x) <= CURL_TELCMD_MAXIMUM))

#अगर_घोषित CURLVERBOSE
#घोषणा CURL_TELCMD(x) telnetcmds[(x) - CURL_TELCMD_MINIMUM]
#अन्यथा
#घोषणा CURL_TELCMD(x) ""
#पूर्ण_अगर

#पूर्ण_अगर /* CURL_DISABLE_TELNET */

#पूर्ण_अगर /* HEADER_CURL_ARPA_TELNET_H */
