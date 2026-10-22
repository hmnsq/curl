#اگر_تعریف_نشده سرآیند_CURL_ARPA_TELNET_H
#تعریف_کن سرآیند_CURL_ARPA_TELNET_H
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
#اگر_تعریف_نشده CURL_DISABLE_TELNET
/*
 * Telnet option defines. Add more here if in need.
 */
#تعریف_کن CURL_TELOPT_دودویی      0   /* binary 8-bit data */
#تعریف_کن CURL_TELOPT_ECHO        1   /* echo */
#تعریف_کن CURL_TELOPT_SGA         3   /* Suppress Go Ahead */
#تعریف_کن CURL_TELOPT_EXOPL       255 /* EXtended OPtions List */
#تعریف_کن CURL_TELOPT_TTYPE       24  /* Terminal TYPE */
#تعریف_کن CURL_TELOPT_NAWS        31  /* Negotiate About Window Size */
#تعریف_کن CURL_TELOPT_XDISPLOC    35  /* X DISPlay LOCation */
#تعریف_کن CURL_TELOPT_NEW_ENVIRON 39  /* NEW ENVIRONment variables */

#تعریف_کن CURL_NEW_ENV_VAR   0
#تعریف_کن CURL_NEW_ENV_مقدار 1

#اگر_تعریف_شده CURLVERBOSE
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
#تعریف_کن CURL_TELOPT(x) telnetoptions[x]
#وإلا
#تعریف_کن CURL_TELOPT(x) ""
#پایان_اگر

#تعریف_کن CURL_TELOPT_بیشینه CURL_TELOPT_NEW_ENVIRON

#تعریف_کن CURL_TELOPT_OK(x) ((x) <= CURL_TELOPT_بیشینه)

#تعریف_کن CURL_NTELOPTS 40

/*
 * First some defines
 */
#تعریف_کن CURL_xEOF 236 /* End Of File */
#تعریف_کن CURL_SE   240 /* Sub negotiation End */
#تعریف_کن CURL_NOP  241 /* No OPeration */
#تعریف_کن CURL_DM   242 /* Data Mark */
#تعریف_کن CURL_GA   249 /* Go Ahead, reverse the line */
#تعریف_کن CURL_SB   250 /* SuBnegotiation */
#تعریف_کن CURL_WILL 251 /* Our side WILL use this option */
#تعریف_کن CURL_WONT 252 /* Our side will not use this option */
#تعریف_کن CURL_DO   253 /* DO use this option! */
#تعریف_کن CURL_DONT 254 /* DO NOT use this option! */
#تعریف_کن CURL_IAC  255 /* Interpret As Command */

#اگر_تعریف_شده CURLVERBOSE
/*
 * Then those numbers represented as strings:
 */
ساكن ثابت حرف * ثابت telnetcmds[] = {
  "EOF",  "SUSP",  "ABORT", "EOR",  "SE",
  "NOP",  "DMARK", "BRK",   "IP",   "AO",
  "AYT",  "EC",    "EL",    "GA",   "SB",
  "WILL", "WONT",  "DO",    "DONT", "IAC"
};
#پایان_اگر

#تعریف_کن CURL_TELCMD_MINIMUM CURL_xEOF /* the first one */
#تعریف_کن CURL_TELCMD_بیشینه CURL_IAC  /* surprise, 255 is the last one! ;-) */

#تعریف_کن CURL_TELQUAL_IS   0
#تعریف_کن CURL_TELQUAL_SEND 1
#تعریف_کن CURL_TELQUAL_INFO 2
#تعریف_کن CURL_TELQUAL_NAME 3

#تعریف_کن CURL_TELCMD_OK(x)                        \
  (((بیعلامت صحيح)(x) >= CURL_TELCMD_MINIMUM) && \
   ((بیعلامت صحيح)(x) <= CURL_TELCMD_بیشینه))

#اگر_تعریف_شده CURLVERBOSE
#تعریف_کن CURL_TELCMD(x) telnetcmds[(x) - CURL_TELCMD_MINIMUM]
#وإلا
#تعریف_کن CURL_TELCMD(x) ""
#پایان_اگر

#پایان_اگر /* CURL_DISABLE_TELNET */

#پایان_اگر /* HEADER_CURL_ARPA_TELNET_H */
