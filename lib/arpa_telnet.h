#که_نه_تعریف_شوی سرلیک_CURL_ARPA_TELNET_H
#تعریف_کړه سرلیک_CURL_ARPA_TELNET_H
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
#که_نه_تعریف_شوی CURL_DISABLE_TELNET
/*
 * Telnet option defines. Add more here if in need.
 */
#تعریف_کړه CURL_TELOPT_دوګونی      0   /* binary 8-bit data */
#تعریف_کړه CURL_TELOPT_ECHO        1   /* echo */
#تعریف_کړه CURL_TELOPT_SGA         3   /* Suppress Go Ahead */
#تعریف_کړه CURL_TELOPT_EXOPL       255 /* EXtended OPtions List */
#تعریف_کړه CURL_TELOPT_TTYPE       24  /* Terminal TYPE */
#تعریف_کړه CURL_TELOPT_NAWS        31  /* Negotiate About Window Size */
#تعریف_کړه CURL_TELOPT_XDISPLOC    35  /* X DISPlay LOCation */
#تعریف_کړه CURL_TELOPT_NEW_ENVIRON 39  /* NEW ENVIRONment variables */

#تعریف_کړه CURL_NEW_ENV_VAR   0
#تعریف_کړه CURL_NEW_ENV_ارزښت 1

#که_تعریف_شوی CURLVERBOSE
/*
 * The telnet options represented as strings
 */
ساكن ثابت حرف * ثابت telnetoptions[] = {
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
#تعریف_کړه CURL_TELOPT(x) telnetoptions[x]
#وإلا
#تعریف_کړه CURL_TELOPT(x) ""
#د_که_پای

#تعریف_کړه CURL_TELOPT_اعظمي CURL_TELOPT_NEW_ENVIRON

#تعریف_کړه CURL_TELOPT_OK(x) ((x) <= CURL_TELOPT_اعظمي)

#تعریف_کړه CURL_NTELOPTS 40

/*
 * First some defines
 */
#تعریف_کړه CURL_xEOF 236 /* End Of File */
#تعریف_کړه CURL_SE   240 /* Sub negotiation End */
#تعریف_کړه CURL_NOP  241 /* No OPeration */
#تعریف_کړه CURL_DM   242 /* Data Mark */
#تعریف_کړه CURL_GA   249 /* Go Ahead, reverse the line */
#تعریف_کړه CURL_SB   250 /* SuBnegotiation */
#تعریف_کړه CURL_WILL 251 /* Our side WILL use this option */
#تعریف_کړه CURL_WONT 252 /* Our side will not use this option */
#تعریف_کړه CURL_DO   253 /* DO use this option! */
#تعریف_کړه CURL_DONT 254 /* DO NOT use this option! */
#تعریف_کړه CURL_IAC  255 /* Interpret As Command */

#که_تعریف_شوی CURLVERBOSE
/*
 * Then those numbers represented as strings:
 */
ساكن ثابت حرف * ثابت telnetcmds[] = {
  "EOF",  "SUSP",  "ABORT", "EOR",  "SE",
  "NOP",  "DMARK", "BRK",   "IP",   "AO",
  "AYT",  "EC",    "EL",    "GA",   "SB",
  "WILL", "WONT",  "DO",    "DONT", "IAC"
};
#د_که_پای

#تعریف_کړه CURL_TELCMD_MINIMUM CURL_xEOF /* the first one */
#تعریف_کړه CURL_TELCMD_اعظمي CURL_IAC  /* surprise, 255 is the last one! ;-) */

#تعریف_کړه CURL_TELQUAL_IS   0
#تعریف_کړه CURL_TELQUAL_SEND 1
#تعریف_کړه CURL_TELQUAL_INFO 2
#تعریف_کړه CURL_TELQUAL_NAME 3

#تعریف_کړه CURL_TELCMD_OK(x)                        \
  (((بینښه صحيح)(x) >= CURL_TELCMD_MINIMUM) && \
   ((بینښه صحيح)(x) <= CURL_TELCMD_اعظمي))

#که_تعریف_شوی CURLVERBOSE
#تعریف_کړه CURL_TELCMD(x) telnetcmds[(x) - CURL_TELCMD_MINIMUM]
#وإلا
#تعریف_کړه CURL_TELCMD(x) ""
#د_که_پای

#د_که_پای /* CURL_DISABLE_TELNET */

#د_که_پای /* HEADER_CURL_ARPA_TELNET_H */
