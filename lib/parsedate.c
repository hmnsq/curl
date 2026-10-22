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
#ଅନ୍ତର୍ଭାଵ "curl_setup.h"

#ଅନ୍ତର୍ଭାଵ "parsedate.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strparse.h"
#ଅନ୍ତର୍ଭାଵ "curlx/strcopy.h"

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

#ଯଦି !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ଅଚଳ ଅକ୍ଷର * ଅଚଳ Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ଅଚଳ ଅକ୍ଷର * ଅଚଳ Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#ଯଦି SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#ଯଦି SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

ସଂରଚନା tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
ସ୍ଥିର ଅଚଳ int16_t tzone[] = {
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
ସ୍ଥିର ଅଚଳ ସଂରଚନା tzinfo tzthree[] = {
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
ସ୍ଥିର ଅଚଳ ସଂରଚନା tzinfo tzfoura[] = {
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
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା checkday(ଅଚଳ ଅକ୍ଷର *check, size_t len);
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା checkday(ଅଚଳ ଅକ୍ଷର *check, size_t len)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା day = -1;
  ଯଦି(len < 3)
    ଫେରନ୍ତୁ -1; /* too short */

  ବାଛ(LOWERCASE(check[1])) {
  ସ୍ଥିତି 'o': /* monday */
    ଯଦି((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'u': /* tuesday or sunday */
    ଯଦି((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    ଅନ୍ୟଥା ଯଦି((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'e': /* wednesday */
    ଯଦି((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'h': /* thursday */
    ଯଦି((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'r': /* friday */
    ଯଦି((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'a': /* saturday */
    ଯଦି((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    ଭାଙ୍ଗ;
  }
  ଯଦି((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    ଯଦି(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      ଫେରନ୍ତୁ -1;
  }
  ଫେରନ୍ତୁ day;
}

/* @unittest 4781 */

UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା checkmonth(ଅଚଳ ଅକ୍ଷର *check, size_t len);
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା checkmonth(ଅଚଳ ଅକ୍ଷର *check, size_t len)
{
  ଯଦି(len != 3)
    ଫେରନ୍ତୁ -1; /* not a month */

  ବାଛ(LOWERCASE(check[2])) {
  ସ୍ଥିତି 'n': /* jan, jun */
    ଯଦି(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      ଯଦି(c2 == 'a')
        ଫେରନ୍ତୁ 0;
      ଅନ୍ୟଥା ଯଦି(c2 == 'u')
        ଫେରନ୍ତୁ 5;
    }
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'b': /* feb */
    ଯଦି((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      ଫେରନ୍ତୁ 1;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'r': /* mar, apr */
    ଯଦି(LOWERCASE(check[0]) == 'm') {
      ଯଦି(LOWERCASE(check[1]) == 'a')
        ଫେରନ୍ତୁ 2;
    }
    ଅନ୍ୟଥା ଯଦି((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      ଫେରନ୍ତୁ 3;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'y': /* may */
    ଯଦି((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      ଫେରନ୍ତୁ 4;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'l': /* jul */
    ଯଦି((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      ଫେରନ୍ତୁ 6;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'g': /* aug */
    ଯଦି((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      ଫେରନ୍ତୁ 7;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'p': /* sep */
    ଯଦି((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      ଫେରନ୍ତୁ 8;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 't': /* oct */
    ଯଦି((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      ଫେରନ୍ତୁ 9;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'v': /* nov */
    ଯଦି((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      ଫେରନ୍ତୁ 10;
    ଭାଙ୍ଗ;
  ସ୍ଥିତି 'c': /* dec */
    ଯଦି((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      ଫେରନ୍ତୁ 11;
    ଭାଙ୍ଗ;
  }
  ଫେରନ୍ତୁ -1; /* return the offset or -1, no real offset is -1 */
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tzcompare(ଅଚଳ ଶୂନ୍ୟ *m1, ଅଚଳ ଶୂନ୍ୟ *m2)
{
  ଅଚଳ ସଂରଚନା tzinfo *tz1 = m1;
  ଅଚଳ ସଂରଚନା tzinfo *tz2 = m2;
  ଫେରନ୍ତୁ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା checktz(ଅଚଳ ଅକ୍ଷର *check, size_t len);
UNITTEST ପୂର୍ଣ୍ଣସଂଖ୍ୟା checktz(ଅଚଳ ଅକ୍ଷର *check, size_t len)
{
  ଅକ୍ଷର first;
  ଯଦି(len > 4)
    ଫେରନ୍ତୁ -1;
  first = check[0];
  ଯଦି(first < 'A')
    ଫେରନ୍ତୁ -1;
  ଯଦି(len == 1) {
    /* short-cut single letter names */
    ଯଦି((first > 'Z') || (first == 'J'))
      ଫେରନ୍ତୁ -1; /* no such tz */
    ଫେରନ୍ତୁ tzone[first - 'A'] * 60;
  }
  ଅନ୍ୟଥା ଯଦି(len == 2) {
    /* two-letter names */
    ଯଦି(check[1] == 'T') {
      ଯଦି(first == 'N')
        ଫେରନ୍ତୁ 11 * 60 * 60;
      ଅନ୍ୟଥା ଯଦି(first == 'U')
        ଫେରନ୍ତୁ 0;
    }
    ଫେରନ୍ତୁ -1; /* nope */
  }
  ଅନ୍ୟଥା ଯଦି(len == 3) {
    /* three-letter name */
    ଅଚଳ ସଂରଚନା tzinfo *what;
    ସଂରଚନା tzinfo find;
    ଯଦି(first > 'Y')
      ଫେରନ୍ତୁ -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   ଆକାର(tzthree[0]), tzcompare);
    ଯଦି(what)
      ଫେରନ୍ତୁ what->offset * 60;
  }
  ଅନ୍ୟଥା {
    /* four-letter name */
    ଅଚଳ ସଂରଚନା tzinfo *what;
    ସଂରଚନା tzinfo find;
    ଯଦି(first > 'W')
      ଫେରନ୍ତୁ -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   ଆକାର(tzfoura[0]), tzcompare);
    ଯଦି(what)
      ଫେରନ୍ତୁ what->offset * 60;
  }
  ଫେରନ୍ତୁ -1;
}

ସ୍ଥିର ଶୂନ୍ୟ skip(ଅଚଳ ଅକ୍ଷର **date)
{
  /* skip everything that are not letters or digits */
  ଯେତେବେଳେ(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ସଂରଚନା when {
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା wday;  /* day of the week, 0-6 (mon-sun) */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mon;   /* month of the year, 0-11 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା mday;  /* day of month, 1 - 31 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା dsecs; /* number of seconds into the day */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା year;  /* year, >= 1583 */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
ସ୍ଥିର curl_off_t time2epoch(ସଂରଚନା when *w)
{
  ସ୍ଥିର ଅଚଳ ପୂର୍ଣ୍ଣସଂଖ୍ୟା cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା y = w->year - (w->mon <= 1);
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  ଫେରନ୍ତୁ (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା oneortwodigit(ଅଚଳ ଅକ୍ଷର *date, ଅଚଳ ଅକ୍ଷର **endp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ = date[0] - '0';
  ଯଦି(ISDIGIT(date[1])) {
    *endp = &date[2];
    ଫେରନ୍ତୁ (ଅଙ୍କ * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  ଫେରନ୍ତୁ ଅଙ୍କ;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
ସ୍ଥିର bool match_time(ଅଚଳ ଅକ୍ଷର *date, ସଂରଚନା when *w, ଅକ୍ଷର **endp)
{
  ଅଚଳ ଅକ୍ଷର *p;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  ଯଦି((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    ଯଦି(mm < 60) {
      ଯଦି((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        ଯଦି(ss <= 60) {
          /* valid HH:MM:SS */
          ଯାଅ match;
        }
      }
      ଅନ୍ୟଥା {
        /* valid HH:MM */
        ଯାଅ match;
      }
    }
  }
  ଫେରନ୍ତୁ FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (ଅକ୍ଷର *)CURL_UNCONST(p);
  ଫେରନ୍ତୁ TRUE;
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

ସ୍ଥିର ଶୂନ୍ୟ initwhen(ସଂରଚନା when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା datestring(ଅଚଳ ଅକ୍ଷର **datep, ସଂରଚନା when *w)
{
  /* a name coming up */
  size_t len = 0;
  ଅଚଳ ଅକ୍ଷର *p = *datep;
  ଯେତେବେଳେ(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  ଯଦି(len != NAME_LEN) {
    ଯଦି(w->wday == -1) {
      w->wday = checkday(p, len);
      ଯଦି(w->wday != -1)
        ଯାଅ found;
    }
    ଯଦି(w->mon == -1) {
      w->mon = checkmonth(p, len);
      ଯଦି(w->mon != -1)
        ଯାଅ found;
    }
    ଯଦି(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      ଯଦି(w->tzoff != -1)
        ଯାଅ found;
    }
  }
  ଫେରନ୍ତୁ PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  ଫେରନ୍ତୁ PARSEDATE_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା datenum(ଅଚଳ ଅକ୍ଷର *indate, ଅଚଳ ଅକ୍ଷର **datep, ସଂରଚନା when *w)
{
  /* a digit */
  unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା val;
  ଅକ୍ଷର *end;
  ଅଚଳ ଅକ୍ଷର *date = *datep;

  ଯଦି((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  ଅନ୍ୟଥା {
    bool found = FALSE;
    curl_off_t lval;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_digits = 0;
    ଅଚଳ ଅକ୍ଷର *p = *datep;
    ଯଦି(curlx_str_ସଂଖ୍ୟା(&p, &lval, 99999999))
      ଫେରନ୍ତୁ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    ଅଙ୍କ_digits = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(p - *datep);
    val = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)lval;

    ଯଦି((w->tzoff == -1) &&
       (ଅଙ୍କ_digits == 4) &&
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

    ଅନ୍ୟଥା ଯଦି((ଅଙ୍କ_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      ଯଦି(w->mon > 11)
        ଫେରନ୍ତୁ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    ଯଦି(!found && (w->mday == -1)) {
      ଯଦି((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    ଯଦି(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      ଯଦି(w->year < 100) {
        ଯଦି(w->year > 70)
          w->year += 1900;
        ଅନ୍ୟଥା
          w->year += 2000;
      }
    }

    ଯଦି(!found)
      ଫେରନ୍ତୁ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  ଫେରନ୍ତୁ PARSEDATE_OK;
}

ସ୍ଥିର bool datecheck(ସଂରଚନା when *w)
{
  ଯଦି(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  ଯଦି((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    ଫେରନ୍ତୁ TRUE;

  ଫେରନ୍ତୁ FALSE;
}

ସ୍ଥିର ଶୂନ୍ୟ tzadjust(curl_off_t *tp, ସଂରଚନା when *w)
{
  ଯଦି(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  ଯଦି((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  ଅନ୍ୟଥା
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mktimet(curl_off_t seconds, time_t *output)
{
#ଯଦି SIZEOF_TIME_T < 5
  ଯଦି(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    ଫେରନ୍ତୁ PARSEDATE_LATER;
  }
  ଅନ୍ୟଥା ଯଦି(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    ଫେରନ୍ତୁ PARSEDATE_SOONER;
  }
#ନଚେତଯଦି defined(HAVE_TIME_T_UNSIGNED)
  ଯଦି(seconds < 0) {
    *output = 0;
    ଫେରନ୍ତୁ PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  ଫେରନ୍ତୁ PARSEDATE_OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parsedate(ଅଚଳ ଅକ୍ଷର *date, time_t *output)
{
  curl_off_t seconds = 0;
  ଅଚଳ ଅକ୍ଷର *indate = date; /* save the original pointer */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା part = 0; /* max 6 parts */
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = 0;
  ସଂରଚନା when w;
  initwhen(&w);

  ଯେତେବେଳେ(*date && (part < 6)) {
    skip(&date);

    ଯଦି(ISALPHA(*date))
      rc = datestring(&date, &w);
    ଅନ୍ୟଥା ଯଦି(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    ଯଦି(rc)
      ଫେରନ୍ତୁ rc;

    part++;
  }

  ଯଦି(datecheck(&w))
    ଫେରନ୍ତୁ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  ଫେରନ୍ତୁ rc;
}
#ଅନ୍ୟଥା
/* disabled */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା parsedate(ଅଚଳ ଅକ୍ଷର *date, time_t *output)
{
  (ଶୂନ୍ୟ)date;
  *output = 0;
  ଫେରନ୍ତୁ PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(ଅଚଳ ଅକ୍ଷର *p, ଅଚଳ time_t *unused)
{
  time_t parsed = -1;
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc;
  (ଶୂନ୍ୟ)unused; /* legacy argument from the past that we ignore */

  ଯଦି(!p)
    ଫେରନ୍ତୁ -1;

  rc = parsedate(p, &parsed);
  ଯଦି(rc == PARSEDATE_OK) {
    ଯଦି(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    ଫେରନ୍ତୁ parsed;
  }
  /* everything else is fail */
  ଫେରନ୍ତୁ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

ପୂର୍ଣ୍ଣସଂଖ୍ୟା Curl_getdate_capped(ଅଚଳ ଅକ୍ଷର *p, time_t *tp)
{
  ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc = parsedate(p, tp);
  ଫେରନ୍ତୁ (rc == PARSEDATE_FAIL);
}
