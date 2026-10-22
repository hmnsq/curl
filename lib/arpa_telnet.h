#جيڪڏهن_وصف_نه_ٿيل سرو_CURL_ARPA_TELNET_H
#وصف_ڪريو سرو_CURL_ARPA_TELNET_H
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
#جيڪڏهن_وصف_نه_ٿيل CURL_DISABLE_TELNET
/*
 * Telnet option defines. Add more here if in need.
 */
#وصف_ڪريو CURL_TELOPT_ٻٽو      0   /* binary 8-bit data */
#وصف_ڪريو CURL_TELOPT_ECHO        1   /* echo */
#وصف_ڪريو CURL_TELOPT_SGA         3   /* Suppress Go Ahead */
#وصف_ڪريو CURL_TELOPT_EXOPL       255 /* EXtended OPtions List */
#وصف_ڪريو CURL_TELOPT_TTYPE       24  /* Terminal TYPE */
#وصف_ڪريو CURL_TELOPT_NAWS        31  /* Negotiate About Window Size */
#وصف_ڪريو CURL_TELOPT_XDISPLOC    35  /* X DISPlay LOCation */
#وصف_ڪريو CURL_TELOPT_NEW_ENVIRON 39  /* NEW ENVIRONment variables */

#وصف_ڪريو CURL_NEW_ENV_VAR   0
#وصف_ڪريو CURL_NEW_ENV_قدر 1

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
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
#وصف_ڪريو CURL_TELOPT(x) telnetoptions[x]
#وإلا
#وصف_ڪريو CURL_TELOPT(x) ""
#جيڪڏهن_پورو

#وصف_ڪريو CURL_TELOPT_وڌ_کان_وڌ CURL_TELOPT_NEW_ENVIRON

#وصف_ڪريو CURL_TELOPT_OK(x) ((x) <= CURL_TELOPT_وڌ_کان_وڌ)

#وصف_ڪريو CURL_NTELOPTS 40

/*
 * First some defines
 */
#وصف_ڪريو CURL_xEOF 236 /* End Of File */
#وصف_ڪريو CURL_SE   240 /* Sub negotiation End */
#وصف_ڪريو CURL_NOP  241 /* No OPeration */
#وصف_ڪريو CURL_DM   242 /* Data Mark */
#وصف_ڪريو CURL_GA   249 /* Go Ahead, reverse the line */
#وصف_ڪريو CURL_SB   250 /* SuBnegotiation */
#وصف_ڪريو CURL_WILL 251 /* Our side WILL use this option */
#وصف_ڪريو CURL_WONT 252 /* Our side will not use this option */
#وصف_ڪريو CURL_DO   253 /* DO use this option! */
#وصف_ڪريو CURL_DONT 254 /* DO NOT use this option! */
#وصف_ڪريو CURL_IAC  255 /* Interpret As Command */

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
/*
 * Then those numbers represented as strings:
 */
ساكن ثابت حرف * ثابت telnetcmds[] = {
  "EOF",  "SUSP",  "ABORT", "EOR",  "SE",
  "NOP",  "DMARK", "BRK",   "IP",   "AO",
  "AYT",  "EC",    "EL",    "GA",   "SB",
  "WILL", "WONT",  "DO",    "DONT", "IAC"
};
#جيڪڏهن_پورو

#وصف_ڪريو CURL_TELCMD_MINIMUM CURL_xEOF /* the first one */
#وصف_ڪريو CURL_TELCMD_وڌ_کان_وڌ CURL_IAC  /* surprise, 255 is the last one! ;-) */

#وصف_ڪريو CURL_TELQUAL_IS   0
#وصف_ڪريو CURL_TELQUAL_SEND 1
#وصف_ڪريو CURL_TELQUAL_INFO 2
#وصف_ڪريو CURL_TELQUAL_NAME 3

#وصف_ڪريو CURL_TELCMD_OK(x)                        \
  (((بنانشان صحيح)(x) >= CURL_TELCMD_MINIMUM) && \
   ((بنانشان صحيح)(x) <= CURL_TELCMD_وڌ_کان_وڌ))

#جيڪڏهن_وصف_ٿيل CURLVERBOSE
#وصف_ڪريو CURL_TELCMD(x) telnetcmds[(x) - CURL_TELCMD_MINIMUM]
#وإلا
#وصف_ڪريو CURL_TELCMD(x) ""
#جيڪڏهن_پورو

#جيڪڏهن_پورو /* CURL_DISABLE_TELNET */

#جيڪڏهن_پورو /* HEADER_CURL_ARPA_TELNET_H */
