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

#ಅನ್ತರ್ಭಾವ "parsedate.h"
#ಅನ್ತರ್ಭಾವ "curlx/strparse.h"
#ಅನ್ತರ್ಭಾವ "curlx/strcopy.h"

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

#ಯದಿ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#ಯದಿ SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#ಯದಿ SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

ಸಂರಚನಾ tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ಸ್ಥಿರ ಧ್ರುವ int16_t tzone[] = {
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
   8 * 60,            /* Uniform */
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

#define MKTZ(a,b,c,d) (((a)<<24) | ((b)<<16) | ((c)<<8) | (d))

/* three-letter time zones */
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ tzinfo tzthree[] = {
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
ಸ್ಥಿರ ಧ್ರುವ ಸಂರಚನಾ tzinfo tzfoura[] = {
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

#define LOWERCASE(x) ((x) | 0x20)

/* returns:
   -1 no day
   0 monday - 6 sunday

   @unittest 4781
*/
UNITTEST ಪೂರ್ಣಾಙ್ಕ checkday(ಧ್ರುವ ವರ್ಣ *check, size_t len);
UNITTEST ಪೂರ್ಣಾಙ್ಕ checkday(ಧ್ರುವ ವರ್ಣ *check, size_t len)
{
  ಪೂರ್ಣಾಙ್ಕ day = -1;
  ಯದಿ(len < 3)
    ಪ್ರತ್ಯಾಗಮ -1; /* too short */

  ವಿಕಲ್ಪ(LOWERCASE(check[1])) {
  ಪಕ್ಷ 'o': /* monday */
    ಯದಿ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'u': /* tuesday or sunday */
    ಯದಿ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    ಅನ್ಯಥಾ ಯದಿ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'e': /* wednesday */
    ಯದಿ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'h': /* thursday */
    ಯದಿ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'r': /* friday */
    ಯದಿ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'a': /* saturday */
    ಯದಿ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    ಭಙ್ಗ;
  }
  ಯದಿ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    ಯದಿ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      ಪ್ರತ್ಯಾಗಮ -1;
  }
  ಪ್ರತ್ಯಾಗಮ day;
}

/* @unittest 4781 */

UNITTEST ಪೂರ್ಣಾಙ್ಕ checkmonth(ಧ್ರುವ ವರ್ಣ *check, size_t len);
UNITTEST ಪೂರ್ಣಾಙ್ಕ checkmonth(ಧ್ರುವ ವರ್ಣ *check, size_t len)
{
  ಯದಿ(len != 3)
    ಪ್ರತ್ಯಾಗಮ -1; /* not a month */

  ವಿಕಲ್ಪ(LOWERCASE(check[2])) {
  ಪಕ್ಷ 'n': /* jan, jun */
    ಯದಿ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ಯದಿ(c2 == 'a')
        ಪ್ರತ್ಯಾಗಮ 0;
      ಅನ್ಯಥಾ ಯದಿ(c2 == 'u')
        ಪ್ರತ್ಯಾಗಮ 5;
    }
    ಭಙ್ಗ;
  ಪಕ್ಷ 'b': /* feb */
    ಯದಿ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      ಪ್ರತ್ಯಾಗಮ 1;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'r': /* mar, apr */
    ಯದಿ(LOWERCASE(check[0]) == 'm') {
      ಯದಿ(LOWERCASE(check[1]) == 'a')
        ಪ್ರತ್ಯಾಗಮ 2;
    }
    ಅನ್ಯಥಾ ಯದಿ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      ಪ್ರತ್ಯಾಗಮ 3;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'y': /* may */
    ಯದಿ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      ಪ್ರತ್ಯಾಗಮ 4;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'l': /* jul */
    ಯದಿ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      ಪ್ರತ್ಯಾಗಮ 6;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'g': /* aug */
    ಯದಿ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      ಪ್ರತ್ಯಾಗಮ 7;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'p': /* sep */
    ಯದಿ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      ಪ್ರತ್ಯಾಗಮ 8;
    ಭಙ್ಗ;
  ಪಕ್ಷ 't': /* oct */
    ಯದಿ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      ಪ್ರತ್ಯಾಗಮ 9;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'v': /* nov */
    ಯದಿ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      ಪ್ರತ್ಯಾಗಮ 10;
    ಭಙ್ಗ;
  ಪಕ್ಷ 'c': /* dec */
    ಯದಿ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      ಪ್ರತ್ಯಾಗಮ 11;
    ಭಙ್ಗ;
  }
  ಪ್ರತ್ಯಾಗಮ -1; /* return the offset or -1, no real offset is -1 */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tzcompare(ಧ್ರುವ ಶೂನ್ಯ *m1, ಧ್ರುವ ಶೂನ್ಯ *m2)
{
  ಧ್ರುವ ಸಂರಚನಾ tzinfo *tz1 = m1;
  ಧ್ರುವ ಸಂರಚನಾ tzinfo *tz2 = m2;
  ಪ್ರತ್ಯಾಗಮ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST ಪೂರ್ಣಾಙ್ಕ checktz(ಧ್ರುವ ವರ್ಣ *check, size_t len);
UNITTEST ಪೂರ್ಣಾಙ್ಕ checktz(ಧ್ರುವ ವರ್ಣ *check, size_t len)
{
  ವರ್ಣ first;
  ಯದಿ(len > 4)
    ಪ್ರತ್ಯಾಗಮ -1;
  first = check[0];
  ಯದಿ(first < 'A')
    ಪ್ರತ್ಯಾಗಮ -1;
  ಯದಿ(len == 1) {
    /* short-cut single letter names */
    ಯದಿ((first > 'Z') || (first == 'J'))
      ಪ್ರತ್ಯಾಗಮ -1; /* no such tz */
    ಪ್ರತ್ಯಾಗಮ tzone[first - 'A'] * 60;
  }
  ಅನ್ಯಥಾ ಯದಿ(len == 2) {
    /* two-letter names */
    ಯದಿ(check[1] == 'T') {
      ಯದಿ(first == 'N')
        ಪ್ರತ್ಯಾಗಮ 11 * 60 * 60;
      ಅನ್ಯಥಾ ಯದಿ(first == 'U')
        ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಪ್ರತ್ಯಾಗಮ -1; /* nope */
  }
  ಅನ್ಯಥಾ ಯದಿ(len == 3) {
    /* three-letter name */
    ಧ್ರುವ ಸಂರಚನಾ tzinfo *what;
    ಸಂರಚನಾ tzinfo find;
    ಯದಿ(first > 'Y')
      ಪ್ರತ್ಯಾಗಮ -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   ಪರಿಮಾಣ(tzthree[0]), tzcompare);
    ಯದಿ(what)
      ಪ್ರತ್ಯಾಗಮ what->offset * 60;
  }
  ಅನ್ಯಥಾ {
    /* four-letter name */
    ಧ್ರುವ ಸಂರಚನಾ tzinfo *what;
    ಸಂರಚನಾ tzinfo find;
    ಯದಿ(first > 'W')
      ಪ್ರತ್ಯಾಗಮ -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   ಪರಿಮಾಣ(tzfoura[0]), tzcompare);
    ಯದಿ(what)
      ಪ್ರತ್ಯಾಗಮ what->offset * 60;
  }
  ಪ್ರತ್ಯಾಗಮ -1;
}

ಸ್ಥಿರ ಶೂನ್ಯ skip(ಧ್ರುವ ವರ್ಣ **date)
{
  /* skip everything that are not letters or digits */
  ಯಾವತ್(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ಸಂರಚನಾ when {
  ಪೂರ್ಣಾಙ್ಕ wday;  /* day of the week, 0-6 (mon-sun) */
  ಪೂರ್ಣಾಙ್ಕ mon;   /* month of the year, 0-11 */
  ಪೂರ್ಣಾಙ್ಕ mday;  /* day of month, 1 - 31 */
  ಪೂರ್ಣಾಙ್ಕ dsecs; /* number of seconds into the day */
  ಪೂರ್ಣಾಙ್ಕ year;  /* year, >= 1583 */
  ಪೂರ್ಣಾಙ್ಕ tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ಸ್ಥಿರ curl_off_t time2epoch(ಸಂರಚನಾ when *w)
{
  ಸ್ಥಿರ ಧ್ರುವ ಪೂರ್ಣಾಙ್ಕ cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  ಪೂರ್ಣಾಙ್ಕ y = w->year - (w->mon <= 1);
  ಪೂರ್ಣಾಙ್ಕ leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  ಪ್ರತ್ಯಾಗಮ (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ oneortwodigit(ಧ್ರುವ ವರ್ಣ *date, ಧ್ರುವ ವರ್ಣ **endp)
{
  ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ = date[0] - '0';
  ಯದಿ(ISDIGIT(date[1])) {
    *endp = &date[2];
    ಪ್ರತ್ಯಾಗಮ (ಅಂಕೆ * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  ಪ್ರತ್ಯಾಗಮ ಅಂಕೆ;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ಸ್ಥಿರ bool match_time(ಧ್ರುವ ವರ್ಣ *date, ಸಂರಚನಾ when *w, ವರ್ಣ **endp)
{
  ಧ್ರುವ ವರ್ಣ *p;
  ಪೂರ್ಣಾಙ್ಕ hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ಯದಿ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ಯದಿ(mm < 60) {
      ಯದಿ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ಯದಿ(ss <= 60) {
          /* valid HH:MM:SS */
          ಗಚ್ಛ match;
        }
      }
      ಅನ್ಯಥಾ {
        /* valid HH:MM */
        ಗಚ್ಛ match;
      }
    }
  }
  ಪ್ರತ್ಯಾಗಮ FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (ವರ್ಣ *)CURL_UNCONST(p);
  ಪ್ರತ್ಯಾಗಮ TRUE;
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
#define NAME_LEN 10

ಸ್ಥಿರ ಶೂನ್ಯ initwhen(ಸಂರಚನಾ when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ datestring(ಧ್ರುವ ವರ್ಣ **datep, ಸಂರಚನಾ when *w)
{
  /* a name coming up */
  size_t len = 0;
  ಧ್ರುವ ವರ್ಣ *p = *datep;
  ಯಾವತ್(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ಯದಿ(len != NAME_LEN) {
    ಯದಿ(w->wday == -1) {
      w->wday = checkday(p, len);
      ಯದಿ(w->wday != -1)
        ಗಚ್ಛ found;
    }
    ಯದಿ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ಯದಿ(w->mon != -1)
        ಗಚ್ಛ found;
    }
    ಯದಿ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ಯದಿ(w->tzoff != -1)
        ಗಚ್ಛ found;
    }
  }
  ಪ್ರತ್ಯಾಗಮ PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  ಪ್ರತ್ಯಾಗಮ PARSEDATE_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ datenum(ಧ್ರುವ ವರ್ಣ *indate, ಧ್ರುವ ವರ್ಣ **datep, ಸಂರಚನಾ when *w)
{
  /* a digit */
  unsigned ಪೂರ್ಣಾಙ್ಕ val;
  ವರ್ಣ *end;
  ಧ್ರುವ ವರ್ಣ *date = *datep;

  ಯದಿ((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  ಅನ್ಯಥಾ {
    bool found = FALSE;
    curl_off_t lval;
    ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_digits = 0;
    ಧ್ರುವ ವರ್ಣ *p = *datep;
    ಯದಿ(curlx_str_ಸಂಖ್ಯೆ(&p, &lval, 99999999))
      ಪ್ರತ್ಯಾಗಮ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    ಅಂಕೆ_digits = (ಪೂರ್ಣಾಙ್ಕ)(p - *datep);
    val = (unsigned ಪೂರ್ಣಾಙ್ಕ)lval;

    ಯದಿ((w->tzoff == -1) &&
       (ಅಂಕೆ_digits == 4) &&
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
      found = TRUE;
      w->tzoff = ((val / 100 * 60) + (val % 100)) * 60;

      /* the + and - prefix indicates the local time compared to GMT,
         this we need their reversed math to get what we want */
      w->tzoff = date[-1] == '+' ? -w->tzoff : w->tzoff;
    }

    ಅನ್ಯಥಾ ಯದಿ((ಅಂಕೆ_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ಯದಿ(w->mon > 11)
        ಪ್ರತ್ಯಾಗಮ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ಯದಿ(!found && (w->mday == -1)) {
      ಯದಿ((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    ಯದಿ(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      ಯದಿ(w->year < 100) {
        ಯದಿ(w->year > 70)
          w->year += 1900;
        ಅನ್ಯಥಾ
          w->year += 2000;
      }
    }

    ಯದಿ(!found)
      ಪ್ರತ್ಯಾಗಮ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  ಪ್ರತ್ಯಾಗಮ PARSEDATE_OK;
}

ಸ್ಥಿರ bool datecheck(ಸಂರಚನಾ when *w)
{
  ಯದಿ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ಯದಿ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    ಪ್ರತ್ಯಾಗಮ TRUE;

  ಪ್ರತ್ಯಾಗಮ FALSE;
}

ಸ್ಥಿರ ಶೂನ್ಯ tzadjust(curl_off_t *tp, ಸಂರಚನಾ when *w)
{
  ಯದಿ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ಯದಿ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  ಅನ್ಯಥಾ
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mktimet(curl_off_t seconds, time_t *output)
{
#ಯದಿ SIZEOF_TIME_T < 5
  ಯದಿ(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    ಪ್ರತ್ಯಾಗಮ PARSEDATE_LATER;
  }
  ಅನ್ಯಥಾ ಯದಿ(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    ಪ್ರತ್ಯಾಗಮ PARSEDATE_SOONER;
  }
#ನೋಚೇದ್ಯದಿ defined(HAVE_TIME_T_UNSIGNED)
  ಯದಿ(seconds < 0) {
    *output = 0;
    ಪ್ರತ್ಯಾಗಮ PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  ಪ್ರತ್ಯಾಗಮ PARSEDATE_OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parsedate(ಧ್ರುವ ವರ್ಣ *date, time_t *output)
{
  curl_off_t seconds = 0;
  ಧ್ರುವ ವರ್ಣ *indate = date; /* save the original pointer */
  ಪೂರ್ಣಾಙ್ಕ part = 0; /* max 6 parts */
  ಪೂರ್ಣಾಙ್ಕ rc = 0;
  ಸಂರಚನಾ when w;
  initwhen(&w);

  ಯಾವತ್(*date && (part < 6)) {
    skip(&date);

    ಯದಿ(ISALPHA(*date))
      rc = datestring(&date, &w);
    ಅನ್ಯಥಾ ಯದಿ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ಯದಿ(rc)
      ಪ್ರತ್ಯಾಗಮ rc;

    part++;
  }

  ಯದಿ(datecheck(&w))
    ಪ್ರತ್ಯಾಗಮ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  ಪ್ರತ್ಯಾಗಮ rc;
}
#ಅನ್ಯಥಾ
/* disabled */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ parsedate(ಧ್ರುವ ವರ್ಣ *date, time_t *output)
{
  (ಶೂನ್ಯ)date;
  *output = 0;
  ಪ್ರತ್ಯಾಗಮ PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(ಧ್ರುವ ವರ್ಣ *p, ಧ್ರುವ time_t *unused)
{
  time_t parsed = -1;
  ಪೂರ್ಣಾಙ್ಕ rc;
  (ಶೂನ್ಯ)unused; /* legacy argument from the past that we ignore */

  ಯದಿ(!p)
    ಪ್ರತ್ಯಾಗಮ -1;

  rc = parsedate(p, &parsed);
  ಯದಿ(rc == PARSEDATE_OK) {
    ಯದಿ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    ಪ್ರತ್ಯಾಗಮ parsed;
  }
  /* everything else is fail */
  ಪ್ರತ್ಯಾಗಮ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

ಪೂರ್ಣಾಙ್ಕ Curl_getdate_capped(ಧ್ರುವ ವರ್ಣ *p, time_t *tp)
{
  ಪೂರ್ಣಾಙ್ಕ rc = parsedate(p, tp);
  ಪ್ರತ್ಯಾಗಮ (rc == PARSEDATE_FAIL);
}
