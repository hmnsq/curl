#यदि_अपरिभाषितम् शीर्षकम्_CURL_ARPA_TELNET_H
#परिभाषय शीर्षकम्_CURL_ARPA_TELNET_H
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
#यदि_अपरिभाषितम् CURL_DISABLE_TELNET
/*
 * Telnet option defines. Add more here if in need.
 */
#परिभाषय CURL_TELOPT_द्विमान      0   /* binary 8-bit data */
#परिभाषय CURL_TELOPT_ECHO        1   /* echo */
#परिभाषय CURL_TELOPT_SGA         3   /* Suppress Go Ahead */
#परिभाषय CURL_TELOPT_EXOPL       255 /* EXtended OPtions List */
#परिभाषय CURL_TELOPT_TTYPE       24  /* Terminal TYPE */
#परिभाषय CURL_TELOPT_NAWS        31  /* Negotiate About Window Size */
#परिभाषय CURL_TELOPT_XDISPLOC    35  /* X DISPlay LOCation */
#परिभाषय CURL_TELOPT_NEW_ENVIRON 39  /* NEW ENVIRONment variables */

#परिभाषय CURL_NEW_ENV_VAR   0
#परिभाषय CURL_NEW_ENV_मानम् 1

#यदि_परिभाषितम् CURLVERBOSE
/*
 * The telnet options represented as strings
 */
स्थिर ध्रुव वर्ण * ध्रुव telnetoptions[] = {
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
#परिभाषय CURL_TELOPT(x) telnetoptions[x]
#अन्यथा
#परिभाषय CURL_TELOPT(x) ""
#यदि_अन्तः

#परिभाषय CURL_TELOPT_अधिकतमम् CURL_TELOPT_NEW_ENVIRON

#परिभाषय CURL_TELOPT_OK(x) ((x) <= CURL_TELOPT_अधिकतमम्)

#परिभाषय CURL_NTELOPTS 40

/*
 * First some defines
 */
#परिभाषय CURL_xEOF 236 /* End Of File */
#परिभाषय CURL_SE   240 /* Sub negotiation End */
#परिभाषय CURL_NOP  241 /* No OPeration */
#परिभाषय CURL_DM   242 /* Data Mark */
#परिभाषय CURL_GA   249 /* Go Ahead, reverse the line */
#परिभाषय CURL_SB   250 /* SuBnegotiation */
#परिभाषय CURL_WILL 251 /* Our side WILL use this option */
#परिभाषय CURL_WONT 252 /* Our side will not use this option */
#परिभाषय CURL_DO   253 /* DO use this option! */
#परिभाषय CURL_DONT 254 /* DO NOT use this option! */
#परिभाषय CURL_IAC  255 /* Interpret As Command */

#यदि_परिभाषितम् CURLVERBOSE
/*
 * Then those numbers represented as strings:
 */
स्थिर ध्रुव वर्ण * ध्रुव telnetcmds[] = {
  "EOF",  "SUSP",  "ABORT", "EOR",  "SE",
  "NOP",  "DMARK", "BRK",   "IP",   "AO",
  "AYT",  "EC",    "EL",    "GA",   "SB",
  "WILL", "WONT",  "DO",    "DONT", "IAC"
};
#यदि_अन्तः

#परिभाषय CURL_TELCMD_MINIMUM CURL_xEOF /* the first one */
#परिभाषय CURL_TELCMD_अधिकतमम् CURL_IAC  /* surprise, 255 is the last one! ;-) */

#परिभाषय CURL_TELQUAL_IS   0
#परिभाषय CURL_TELQUAL_SEND 1
#परिभाषय CURL_TELQUAL_INFO 2
#परिभाषय CURL_TELQUAL_NAME 3

#परिभाषय CURL_TELCMD_OK(x)                        \
  (((अचिह्नित पूर्णाङ्क)(x) >= CURL_TELCMD_MINIMUM) && \
   ((अचिह्नित पूर्णाङ्क)(x) <= CURL_TELCMD_अधिकतमम्))

#यदि_परिभाषितम् CURLVERBOSE
#परिभाषय CURL_TELCMD(x) telnetcmds[(x) - CURL_TELCMD_MINIMUM]
#अन्यथा
#परिभाषय CURL_TELCMD(x) ""
#यदि_अन्तः

#यदि_अन्तः /* CURL_DISABLE_TELNET */

#यदि_अन्तः /* HEADER_CURL_ARPA_TELNET_H */
