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
#అన్తర్భావ "curl_setup.h"

#అన్తర్భావ "parsedate.h"
#అన్తర్భావ "curlx/strparse.h"
#అన్తర్భావ "curlx/strcopy.h"

/* A brief summary of the date string formats this parser groks:

   RFC 2616 3.3.1

   Sun, 06 Nov 1994 08:49:37 GMT  ; RFC 822, updated by RFC 1123
   Sunday, 06-Nov-94 08:49:37 GMT ; RFC 850, obsoleted by RFC 1036
   Sun Nov  6 08:49:37 1994       ; ANSI C's asctime() format

   we support dates without week day name:

   06 Nov 1994 08:49:37 GMT
   06-Nov-94 08:49:37 GMT
   Nov  6 08:49:37 1994

   without the time zone:

   06 Nov 1994 08:49:37
   06-Nov-94 08:49:37

   weird order:

   1994 Nov 6 08:49:37  (GNU date fails)
   GMT 08:49:37 06-Nov-94 Sunday
   94 6 Nov 08:49:37    (GNU date fails)

   time left out:

   1994 Nov 6
   06-Nov-94
   Sun Nov 6 94

   unusual separators:

   1994.Nov.6
   Sun/Nov/6/94/GMT

   commonly used time zone names:

   Sun, 06 Nov 1994 08:49:37 CET
   06 Nov 1994 08:49:37 EST

   time zones specified using RFC822 style:

   Sun, 12 Sep 2004 15:05:58 -0700
   Sat, 11 Sep 2004 21:32:11 +0200

   compact numerical date strings:

   20040912 15:05:58 -0700
   20040911 +0200
 */

#ఒకవేళ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
స్థిరరాశి అక్షరం * స్థిరరాశి Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
స్థిరరాశి అక్షరం * స్థిరరాశి Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#అయితే_ముగింపు

#నిర్వచించు PARSEDATE_OK     0
#నిర్వచించు PARSEDATE_FAIL   (-1)

#నిర్వచితం_కాకపోతే CURL_DISABLE_PARSEDATE

#ఒకవేళ SIZEOF_సమయం_T < 5
#నిర్వచించు PARSEDATE_LATER  1
#అయితే_ముగింపు
#ఒకవేళ SIZEOF_సమయం_T < 5 || defined(HAVE_సమయం_T_UNSIGNED)
#నిర్వచించు PARSEDATE_SOONER 2
#అయితే_ముగింపు

నిర్మాణం tzinfo {
  uint32_t tz;
  int16_t స్థానభ్రంశం; /* +/- in minutes */
};

#నిర్వచించు DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
స్థిర స్థిరరాశి int16_t tzone[] = {
  -1 * 60,            /* Alpha */
  -2 * 60,            /* Bravo */
  -3 * 60,            /* Charlie */
  -4 * 60,            /* Delta */
  -5 * 60,            /* Echo */
  -6 * 60,            /* Foxtrot */
  -7 * 60,            /* Golf */
  -8 * 60,            /* Hotel */
  -9 * 60,            /* India */
  -1,                 /* J - not a timezone */
  -10 * 60,           /* Kilo */
  -11 * 60,           /* Lima */
  -12 * 60,           /* Mike */
  60,                 /* November */
   2 * 60,            /* Oscar */
   3 * 60,            /* Papa */
   4 * 60,            /* Quebec */
   5 * 60,            /* Romeo */
   6 * 60,            /* Sierra */
   7 * 60,            /* Tango */
   8 * 60,            /*ఏకరీతి*/
   9 * 60,            /* Victor */
  10 * 60,            /* Whiskey */
  11 * 60,            /* X-ray */
  12 * 60,            /* Yankee */
  0                   /* Zulu, zero meridian, a.k.a. UTC */
};

/* two-letter time zones

  11 * 60,              NT - Nome    spellchecker:disable-line
  0,                    UT - Universal Time

*/

#నిర్వచించు MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
స్థిర స్థిరరాశి నిర్మాణం tzinfo tzthree[] = {
  { MKTZ('A', 'D', 'T', '\0'),   240 + DST }, /* Atlantic Daylight */
  { MKTZ('A', 'S', 'T', '\0'),   240 },       /* Atlantic Standard */
  { MKTZ('B', 'S', 'T', '\0'),     0 + DST }, /* British Summer */
  { MKTZ('C', 'A', 'T', '\0'),   600 },       /* Central Alaska */
  { MKTZ('C', 'C', 'T', '\0'),  -480 },       /* China Coast, USSR Zone 7 */
  { MKTZ('C', 'D', 'T', '\0'),   360 + DST }, /* Central Daylight */
  { MKTZ('C', 'E', 'T', '\0'),   -60 },       /* Central European */
  { MKTZ('C', 'S', 'T', '\0'),   360 },       /* Central Standard */
  { MKTZ('E', 'D', 'T', '\0'),   300 + DST }, /* Eastern Daylight */
  { MKTZ('E', 'E', 'T', '\0'),  -120 },       /* Eastern Europe, USSR Zone 1 */
  { MKTZ('E', 'S', 'T', '\0'),   300 },       /* Eastern Standard */
  { MKTZ('F', 'S', 'T', '\0'),   -60 + DST }, /* French Summer */
  { MKTZ('F', 'W', 'T', '\0'),   -60 },       /* French Winter */
  { MKTZ('G', 'M', 'T', '\0'),     0 },       /* Greenwich Mean */
  { MKTZ('G', 'S', 'T', '\0'),  -600 },       /* Guam Standard, USSR Zone 9 */
  { MKTZ('H', 'D', 'T', '\0'),   600 + DST }, /* Hawaii Daylight */
  { MKTZ('H', 'S', 'T', '\0'),   600 },       /* Hawaii Standard */
  { MKTZ('J', 'S', 'T', '\0'),  -540 },       /* Japan Standard, USSR Zone 8 */
  { MKTZ('M', 'D', 'T', '\0'),   420 + DST }, /* Mountain Daylight */
  { MKTZ('M', 'E', 'T', '\0'),   -60 },       /* Middle European */
  { MKTZ('M', 'S', 'T', '\0'),   420 },       /* Mountain Standard */
  { MKTZ('N', 'Z', 'T', '\0'),  -720 },       /* New Zealand */
  { MKTZ('P', 'D', 'T', '\0'),   480 + DST }, /* Pacific Daylight */
  { MKTZ('P', 'S', 'T', '\0'),   480 },       /* Pacific Standard */
  { MKTZ('U', 'T', 'C', '\0'),     0 },       /* Universal (Coordinated) */
  { MKTZ('W', 'A', 'T', '\0'),    60 },       /* West Africa */
  { MKTZ('W', 'E', 'T', '\0'),     0 },       /* Western European */
  { MKTZ('Y', 'D', 'T', '\0'),   540 + DST }, /* Yukon Daylight */
  { MKTZ('Y', 'S', 'T', '\0'),   540 },       /* Yukon Standard */
};

/* four-letter time zones */
స్థిర స్థిరరాశి నిర్మాణం tzinfo tzfoura[] = {
  { MKTZ('A', 'H', 'S', 'T'),  600 },       /* Alaska-Hawaii Standard */
  { MKTZ('C', 'E', 'S', 'T'),  -60 + DST }, /* Central European Summer */
  { MKTZ('E', 'A', 'D', 'T'), -600 + DST }, /* Eastern Australian Daylight */
  { MKTZ('E', 'A', 'S', 'T'), -600 },       /* Eastern Australian Standard */
  { MKTZ('I', 'D', 'L', 'E'), -720 },       /* International Date Line East */
  { MKTZ('I', 'D', 'L', 'W'),  720 },       /* International Date Line West */
  { MKTZ('M', 'E', 'S', 'T'),  -60 + DST }, /* Middle European Summer */
  { MKTZ('M', 'E', 'S', 'Z'),  -60 + DST }, /* Middle European Summer */
  { MKTZ('M', 'E', 'W', 'T'),  -60 },       /* Middle European Winter */
  { MKTZ('N', 'Z', 'D', 'T'), -720 + DST }, /* New Zealand Daylight */
  { MKTZ('N', 'Z', 'S', 'T'), -720 },       /* New Zealand Standard */
  { MKTZ('W', 'A', 'D', 'T'), -420 + DST }, /* West Australian Daylight */
  { MKTZ('W', 'A', 'S', 'T'), -420 }, /* spellchecker:disable-line */
                               /* West Australian Standard */
};

#నిర్వచించు LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST పూర్ణాంకం checkday(స్థిరరాశి అక్షరం *check, size_t len);
UNITTEST పూర్ణాంకం checkday(స్థిరరాశి అక్షరం *check, size_t len)
{
  పూర్ణాంకం day = -1;
  ఒకవేళ(len < 3)
    తిరిగి -1; /* too short */

  ఎంపిక(LOWERCASE(check[1])) {
  స్థితి 'o': /* monday */
    ఒకవేళ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    ఆపు;
  స్థితి 'u': /* tuesday or sunday */
    ఒకవేళ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    లేకపోతే ఒకవేళ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    ఆపు;
  స్థితి 'e': /* wednesday */
    ఒకవేళ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    ఆపు;
  స్థితి 'h': /* thursday */
    ఒకవేళ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    ఆపు;
  స్థితి 'r': /* friday */
    ఒకవేళ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    ఆపు;
  స్థితి 'a': /* saturday */
    ఒకవేళ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    ఆపు;
  }
  ఒకవేళ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = అక్షరశ్రేణి_పొడవు(daysuffix[day]);
    ఒకవేళ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      తిరిగి -1;
  }
  తిరిగి day;
}

/* @unittest 4781 */

UNITTEST పూర్ణాంకం checkmonth(స్థిరరాశి అక్షరం *check, size_t len);
UNITTEST పూర్ణాంకం checkmonth(స్థిరరాశి అక్షరం *check, size_t len)
{
  ఒకవేళ(len != 3)
    తిరిగి -1; /* not a month */

  ఎంపిక(LOWERCASE(check[2])) {
  స్థితి 'n': /* jan, jun */
    ఒకవేళ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ఒకవేళ(c2 == 'a')
        తిరిగి 0;
      లేకపోతే ఒకవేళ(c2 == 'u')
        తిరిగి 5;
    }
    ఆపు;
  స్థితి 'b': /* feb */
    ఒకవేళ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      తిరిగి 1;
    ఆపు;
  స్థితి 'r': /* mar, apr */
    ఒకవేళ(LOWERCASE(check[0]) == 'm') {
      ఒకవేళ(LOWERCASE(check[1]) == 'a')
        తిరిగి 2;
    }
    లేకపోతే ఒకవేళ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      తిరిగి 3;
    ఆపు;
  స్థితి 'y': /* may */
    ఒకవేళ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      తిరిగి 4;
    ఆపు;
  స్థితి 'l': /* jul */
    ఒకవేళ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      తిరిగి 6;
    ఆపు;
  స్థితి 'g': /* aug */
    ఒకవేళ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      తిరిగి 7;
    ఆపు;
  స్థితి 'p': /* sep */
    ఒకవేళ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      తిరిగి 8;
    ఆపు;
  స్థితి 't': /* oct */
    ఒకవేళ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      తిరిగి 9;
    ఆపు;
  స్థితి 'v': /* nov */
    ఒకవేళ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      తిరిగి 10;
    ఆపు;
  స్థితి 'c': /* dec */
    ఒకవేళ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      తిరిగి 11;
    ఆపు;
  }
  తిరిగి -1; /* return the offset or -1, no real offset is -1 */
}

స్థిర పూర్ణాంకం tzcompare(స్థిరరాశి శూన్య *m1, స్థిరరాశి శూన్య *m2)
{
  స్థిరరాశి నిర్మాణం tzinfo *tz1 = m1;
  స్థిరరాశి నిర్మాణం tzinfo *tz2 = m2;
  తిరిగి tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST పూర్ణాంకం checktz(స్థిరరాశి అక్షరం *check, size_t len);
UNITTEST పూర్ణాంకం checktz(స్థిరరాశి అక్షరం *check, size_t len)
{
  అక్షరం మొదటి;
  ఒకవేళ(len > 4)
    తిరిగి -1;
  మొదటి = check[0];
  ఒకవేళ(మొదటి < 'A')
    తిరిగి -1;
  ఒకవేళ(len == 1) {
    /* short-cut single letter names */
    ఒకవేళ((మొదటి > 'Z') || (మొదటి == 'J'))
      తిరిగి -1; /* no such tz */
    తిరిగి tzone[మొదటి - 'A'] * 60;
  }
  లేకపోతే ఒకవేళ(len == 2) {
    /* two-letter names */
    ఒకవేళ(check[1] == 'T') {
      ఒకవేళ(మొదటి == 'N')
        తిరిగి 11 * 60 * 60;
      లేకపోతే ఒకవేళ(మొదటి == 'U')
        తిరిగి 0;
    }
    తిరిగి -1; /* nope */
  }
  లేకపోతే ఒకవేళ(len == 3) {
    /* three-letter name */
    స్థిరరాశి నిర్మాణం tzinfo *what;
    నిర్మాణం tzinfo కనుగొను;
    ఒకవేళ(మొదటి > 'Y')
      తిరిగి -1;
    కనుగొను.tz = MKTZ(మొదటి, check[1], check[2], 0);
    what = bsearch(&కనుగొను, tzthree, CURL_ARRAYSIZE(tzthree),
                   పరిమాణం(tzthree[0]), tzcompare);
    ఒకవేళ(what)
      తిరిగి what->స్థానభ్రంశం * 60;
  }
  లేకపోతే {
    /* four-letter name */
    స్థిరరాశి నిర్మాణం tzinfo *what;
    నిర్మాణం tzinfo కనుగొను;
    ఒకవేళ(మొదటి > 'W')
      తిరిగి -1;
    కనుగొను.tz = MKTZ(మొదటి, check[1], check[2], check[3]);
    what = bsearch(&కనుగొను, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   పరిమాణం(tzfoura[0]), tzcompare);
    ఒకవేళ(what)
      తిరిగి what->స్థానభ్రంశం * 60;
  }
  తిరిగి -1;
}

స్థిర శూన్య skip(స్థిరరాశి అక్షరం **date)
{
  /* skip everything that are not letters or digits */
  అయితే(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
నిర్మాణం when {
  పూర్ణాంకం wday;  /* day of the week, 0-6 (mon-sun) */
  పూర్ణాంకం mon;   /* month of the year, 0-11 */
  పూర్ణాంకం mday;  /* day of month, 1 - 31 */
  పూర్ణాంకం dsecs; /* number of seconds into the day */
  పూర్ణాంకం year;  /* year, >= 1583 */
  పూర్ణాంకం tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#నిర్వచించు LEAP_DAYS_BEFORE_1969 477
#నిర్వచించు SECONDS_చొప్పున_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
స్థిర curl_off_t time2epoch(నిర్మాణం when *w)
{
  స్థిర స్థిరరాశి పూర్ణాంకం cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  పూర్ణాంకం y = w->year - (w->mon <= 1);
  పూర్ణాంకం leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  తిరిగి (days * SECONDS_చొప్పున_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
స్థిర పూర్ణాంకం oneortwodigit(స్థిరరాశి అక్షరం *date, స్థిరరాశి అక్షరం **endp)
{
  పూర్ణాంకం అంకె = date[0] - '0';
  ఒకవేళ(ISDIGIT(date[1])) {
    *endp = &date[2];
    తిరిగి (అంకె * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  తిరిగి అంకె;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
స్థిర bool సరిపోలిక_సమయం(స్థిరరాశి అక్షరం *date, నిర్మాణం when *w, అక్షరం **endp)
{
  స్థిరరాశి అక్షరం *p;
  పూర్ణాంకం hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ఒకవేళ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ఒకవేళ(mm < 60) {
      ఒకవేళ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ఒకవేళ(ss <= 60) {
          /* valid HH:MM:SS */
          వెళ్ళు సరిపోలిక;
        }
      }
      లేకపోతే {
        /* valid HH:MM */
        వెళ్ళు సరిపోలిక;
      }
    }
  }
  తిరిగి FALSE; /* not a time string */
సరిపోలిక:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (అక్షరం *)CURL_UNCONST(p);
  తిరిగి TRUE;
}

/*
 * parsedate()
 *
 * Returns:
 *
 * PARSEDATE_OK     - a fine conversion
 * PARSEDATE_FAIL   - failed to convert
 * PARSEDATE_LATER  - time overflow at the far end of time_t
 * PARSEDATE_SOONER - time underflow at the low end of time_t
 */

/* Wednesday is the longest name this parser knows about */
#నిర్వచించు NAME_LEN 10

స్థిర శూన్య initwhen(నిర్మాణం when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

స్థిర పూర్ణాంకం datestring(స్థిరరాశి అక్షరం **datep, నిర్మాణం when *w)
{
  /* a name coming up */
  size_t len = 0;
  స్థిరరాశి అక్షరం *p = *datep;
  అయితే(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ఒకవేళ(len != NAME_LEN) {
    ఒకవేళ(w->wday == -1) {
      w->wday = checkday(p, len);
      ఒకవేళ(w->wday != -1)
        వెళ్ళు దొరికింది;
    }
    ఒకవేళ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ఒకవేళ(w->mon != -1)
        వెళ్ళు దొరికింది;
    }
    ఒకవేళ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ఒకవేళ(w->tzoff != -1)
        వెళ్ళు దొరికింది;
    }
  }
  తిరిగి PARSEDATE_FAIL; /* bad string */
దొరికింది:
  *datep += len;
  తిరిగి PARSEDATE_OK;
}

స్థిర పూర్ణాంకం datenum(స్థిరరాశి అక్షరం *indate, స్థిరరాశి అక్షరం **datep, నిర్మాణం when *w)
{
  /* a digit */
  గుర్తులేని పూర్ణాంకం val;
  అక్షరం *చివర;
  స్థిరరాశి అక్షరం *date = *datep;

  ఒకవేళ((w->dsecs == -1) && సరిపోలిక_సమయం(date, w, &చివర)) {
    /* time stamp */
    date = చివర;
  }
  లేకపోతే {
    bool దొరికింది = FALSE;
    curl_off_t lval;
    పూర్ణాంకం అంకె_digits = 0;
    స్థిరరాశి అక్షరం *p = *datep;
    ఒకవేళ(curlx_str_సంఖ్య(&p, &lval, 99999999))
      తిరిగి PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    అంకె_digits = (పూర్ణాంకం)(p - *datep);
    val = (గుర్తులేని పూర్ణాంకం)lval;

    ఒకవేళ((w->tzoff == -1) &&
       (అంకె_digits == 4) &&
       (val <= 1400) &&
       (indate < date) &&
       (date[-1] == '+' || date[-1] == '-')) {
      /* four digits and a value less than or equal to 1400 (to take into
         account all sorts of funny time zone diffs) and it is preceded
         with a plus or minus. This is a time zone indication. 1400 is
         picked since +1300 is frequently used and +1400 is mentioned as
         an edge number in the document "ISO C 200X Proposal: Timezone
         Functions" at http://david.tribble.com/text/c0xtimezone.html If
         anyone has a more authoritative source for the exact maximum time
         zone offsets, please speak up! */
      దొరికింది = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    లేకపోతే ఒకవేళ((అంకె_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      దొరికింది = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ఒకవేళ(w->mon > 11)
        తిరిగి PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ఒకవేళ(!దొరికింది && (w->mday == -1)) {
      ఒకవేళ((val > 0) && (val < 32)) {
        w->mday = val;
        దొరికింది = TRUE;
      }
    }

    ఒకవేళ(!దొరికింది && (w->year == -1)) {
      w->year = val;
      దొరికింది = TRUE;
      ఒకవేళ(w->year < 100) {
        ఒకవేళ(w->year > 70)
          w->year += 1900;
        లేకపోతే
          w->year += 2000;
      }
    }

    ఒకవేళ(!దొరికింది)
      తిరిగి PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  తిరిగి PARSEDATE_OK;
}

స్థిర bool datecheck(నిర్మాణం when *w)
{
  ఒకవేళ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ఒకవేళ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    తిరిగి TRUE;

  తిరిగి FALSE;
}

స్థిర శూన్య tzadjust(curl_off_t *tp, నిర్మాణం when *w)
{
  ఒకవేళ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ఒకవేళ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  లేకపోతే
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

స్థిర పూర్ణాంకం mktimet(curl_off_t seconds, time_t *output)
{
#ఒకవేళ SIZEOF_సమయం_T < 5
  ఒకవేళ(seconds > సమయం_T_MAX) {
    *output = సమయం_T_MAX;
    తిరిగి PARSEDATE_LATER;
  }
  లేకపోతే ఒకవేళ(seconds < సమయం_T_MIN) {
    *output = సమయం_T_MIN;
    తిరిగి PARSEDATE_SOONER;
  }
#లేదావేళ defined(HAVE_సమయం_T_UNSIGNED)
  ఒకవేళ(seconds < 0) {
    *output = 0;
    తిరిగి PARSEDATE_SOONER;
  }
#అయితే_ముగింపు
  *output = (time_t)seconds;
  తిరిగి PARSEDATE_OK;
}

స్థిర పూర్ణాంకం parsedate(స్థిరరాశి అక్షరం *date, time_t *output)
{
  curl_off_t seconds = 0;
  స్థిరరాశి అక్షరం *indate = date; /* save the original pointer */
  పూర్ణాంకం part = 0; /* max 6 parts */
  పూర్ణాంకం rc = 0;
  నిర్మాణం when w;
  initwhen(&w);

  అయితే(*date && (part < 6)) {
    skip(&date);

    ఒకవేళ(ISALPHA(*date))
      rc = datestring(&date, &w);
    లేకపోతే ఒకవేళ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ఒకవేళ(rc)
      తిరిగి rc;

    part++;
  }

  ఒకవేళ(datecheck(&w))
    తిరిగి PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  తిరిగి rc;
}
#లేకపోతే
/* disabled */
స్థిర పూర్ణాంకం parsedate(స్థిరరాశి అక్షరం *date, time_t *output)
{
  (శూన్య)date;
  *output = 0;
  తిరిగి PARSEDATE_OK; /* a lie */
}
#అయితే_ముగింపు

time_t curl_getdate(స్థిరరాశి అక్షరం *p, స్థిరరాశి time_t *వాడని)
{
  time_t parsed = -1;
  పూర్ణాంకం rc;
  (శూన్య)వాడని; /* legacy argument from the past that we ignore */

  ఒకవేళ(!p)
    తిరిగి -1;

  rc = parsedate(p, &parsed);
  ఒకవేళ(rc == PARSEDATE_OK) {
    ఒకవేళ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    తిరిగి parsed;
  }
  /* everything else is fail */
  తిరిగి -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

పూర్ణాంకం Curl_getdate_capped(స్థిరరాశి అక్షరం *p, time_t *tp)
{
  పూర్ణాంకం rc = parsedate(p, tp);
  తిరిగి (rc == PARSEDATE_FAIL);
}
