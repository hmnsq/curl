#నిర్వచితం_కాకపోతే శీర్షిక_CURL_ARPA_TELNET_H
#నిర్వచించు శీర్షిక_CURL_ARPA_TELNET_H
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
#నిర్వచితం_కాకపోతే CURL_DISABLE_TELNET
/*
 * Telnet option defines. Add more here if in need.
 */
#నిర్వచించు CURL_TELOPT_ద్విమాన      0   /* binary 8-bit data */
#నిర్వచించు CURL_TELOPT_ECHO        1   /* echo */
#నిర్వచించు CURL_TELOPT_SGA         3   /* Suppress Go Ahead */
#నిర్వచించు CURL_TELOPT_EXOPL       255 /* EXtended OPtions List */
#నిర్వచించు CURL_TELOPT_TTYPE       24  /* Terminal TYPE */
#నిర్వచించు CURL_TELOPT_NAWS        31  /* Negotiate About Window Size */
#నిర్వచించు CURL_TELOPT_XDISPLOC    35  /* X DISPlay LOCation */
#నిర్వచించు CURL_TELOPT_NEW_ENVIRON 39  /* NEW ENVIRONment variables */

#నిర్వచించు CURL_NEW_ENV_VAR   0
#నిర్వచించు CURL_NEW_ENV_విలువ 1

#నిర్వచితమైతే CURLVERBOSE
/*
 * The telnet options represented as strings
 */
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి telnetoptions[] = {
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
#నిర్వచించు CURL_TELOPT(x) telnetoptions[x]
#లేకపోతే
#నిర్వచించు CURL_TELOPT(x) ""
#అయితే_ముగింపు

#నిర్వచించు CURL_TELOPT_గరిష్ఠ CURL_TELOPT_NEW_ENVIRON

#నిర్వచించు CURL_TELOPT_OK(x) ((x) <= CURL_TELOPT_గరిష్ఠ)

#నిర్వచించు CURL_NTELOPTS 40

/*
 * First some defines
 */
#నిర్వచించు CURL_xEOF 236 /* End Of File */
#నిర్వచించు CURL_SE   240 /* Sub negotiation End */
#నిర్వచించు CURL_NOP  241 /* No OPeration */
#నిర్వచించు CURL_DM   242 /* Data Mark */
#నిర్వచించు CURL_GA   249 /* Go Ahead, reverse the line */
#నిర్వచించు CURL_SB   250 /* SuBnegotiation */
#నిర్వచించు CURL_WILL 251 /* Our side WILL use this option */
#నిర్వచించు CURL_WONT 252 /* Our side will not use this option */
#నిర్వచించు CURL_DO   253 /* DO use this option! */
#నిర్వచించు CURL_DONT 254 /* DO NOT use this option! */
#నిర్వచించు CURL_IAC  255 /* Interpret As Command */

#నిర్వచితమైతే CURLVERBOSE
/*
 * Then those numbers represented as strings:
 */
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి telnetcmds[] = {
  "EOF",  "SUSP",  "ABORT", "EOR",  "SE",
  "NOP",  "DMARK", "BRK",   "IP",   "AO",
  "AYT",  "EC",    "EL",    "GA",   "SB",
  "WILL", "WONT",  "DO",    "DONT", "IAC"
};
#అయితే_ముగింపు

#నిర్వచించు CURL_TELCMD_MINIMUM CURL_xEOF /* the first one */
#నిర్వచించు CURL_TELCMD_గరిష్ఠ CURL_IAC  /* surprise, 255 is the last one! ;-) */

#నిర్వచించు CURL_TELQUAL_IS   0
#నిర్వచించు CURL_TELQUAL_SEND 1
#నిర్వచించు CURL_TELQUAL_INFO 2
#నిర్వచించు CURL_TELQUAL_NAME 3

#నిర్వచించు CURL_TELCMD_OK(x)                        \
  (((గుర్తులేని పూర్ణాంకం)(x) >= CURL_TELCMD_MINIMUM) && \
   ((గుర్తులేని పూర్ణాంకం)(x) <= CURL_TELCMD_గరిష్ఠ))

#నిర్వచితమైతే CURLVERBOSE
#నిర్వచించు CURL_TELCMD(x) telnetcmds[(x) - CURL_TELCMD_MINIMUM]
#లేకపోతే
#నిర్వచించు CURL_TELCMD(x) ""
#అయితే_ముగింపు

#అయితే_ముగింపు /* CURL_DISABLE_TELNET */

#అయితే_ముగింపు /* HEADER_CURL_ARPA_TELNET_H */
