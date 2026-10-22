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
#अन्तर्भाव "curl_setup.h"

#अन्तर्भाव "parsedate.h"
#अन्तर्भाव "curlx/strparse.h"
#अन्तर्भाव "curlx/strcopy.h"

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

#जइ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ध्रुव वर्ण * ध्रुव Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ध्रुव वर्ण * ध्रुव Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#जइ SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#जइ SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

संरचना tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
स्थिर ध्रुव int16_t tzone[] = {
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
स्थिर ध्रुव संरचना tzinfo tzthree[] = {
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
स्थिर ध्रुव संरचना tzinfo tzfoura[] = {
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
UNITTEST पूर्णाङ्क checkday(ध्रुव वर्ण *check, size_t len);
UNITTEST पूर्णाङ्क checkday(ध्रुव वर्ण *check, size_t len)
{
  पूर्णाङ्क day = -1;
  जइ(len < 3)
    प्रत्यागम -1; /* too short */

  विकल्प(LOWERCASE(check[1])) {
  पक्ष 'o': /* monday */
    जइ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    भङ्ग;
  पक्ष 'u': /* tuesday or sunday */
    जइ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    अण्णहा जइ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    भङ्ग;
  पक्ष 'e': /* wednesday */
    जइ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    भङ्ग;
  पक्ष 'h': /* thursday */
    जइ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    भङ्ग;
  पक्ष 'r': /* friday */
    जइ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    भङ्ग;
  पक्ष 'a': /* saturday */
    जइ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    भङ्ग;
  }
  जइ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    स्थिर ध्रुव वर्ण * ध्रुव daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    जइ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      प्रत्यागम -1;
  }
  प्रत्यागम day;
}

/* @unittest 4781 */

UNITTEST पूर्णाङ्क checkmonth(ध्रुव वर्ण *check, size_t len);
UNITTEST पूर्णाङ्क checkmonth(ध्रुव वर्ण *check, size_t len)
{
  जइ(len != 3)
    प्रत्यागम -1; /* not a month */

  विकल्प(LOWERCASE(check[2])) {
  पक्ष 'n': /* jan, jun */
    जइ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      जइ(c2 == 'a')
        प्रत्यागम 0;
      अण्णहा जइ(c2 == 'u')
        प्रत्यागम 5;
    }
    भङ्ग;
  पक्ष 'b': /* feb */
    जइ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      प्रत्यागम 1;
    भङ्ग;
  पक्ष 'r': /* mar, apr */
    जइ(LOWERCASE(check[0]) == 'm') {
      जइ(LOWERCASE(check[1]) == 'a')
        प्रत्यागम 2;
    }
    अण्णहा जइ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      प्रत्यागम 3;
    भङ्ग;
  पक्ष 'y': /* may */
    जइ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      प्रत्यागम 4;
    भङ्ग;
  पक्ष 'l': /* jul */
    जइ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      प्रत्यागम 6;
    भङ्ग;
  पक्ष 'g': /* aug */
    जइ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      प्रत्यागम 7;
    भङ्ग;
  पक्ष 'p': /* sep */
    जइ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      प्रत्यागम 8;
    भङ्ग;
  पक्ष 't': /* oct */
    जइ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      प्रत्यागम 9;
    भङ्ग;
  पक्ष 'v': /* nov */
    जइ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      प्रत्यागम 10;
    भङ्ग;
  पक्ष 'c': /* dec */
    जइ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      प्रत्यागम 11;
    भङ्ग;
  }
  प्रत्यागम -1; /* return the offset or -1, no real offset is -1 */
}

स्थिर पूर्णाङ्क tzcompare(ध्रुव शून्य *m1, ध्रुव शून्य *m2)
{
  ध्रुव संरचना tzinfo *tz1 = m1;
  ध्रुव संरचना tzinfo *tz2 = m2;
  प्रत्यागम tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST पूर्णाङ्क checktz(ध्रुव वर्ण *check, size_t len);
UNITTEST पूर्णाङ्क checktz(ध्रुव वर्ण *check, size_t len)
{
  वर्ण first;
  जइ(len > 4)
    प्रत्यागम -1;
  first = check[0];
  जइ(first < 'A')
    प्रत्यागम -1;
  जइ(len == 1) {
    /* short-cut single letter names */
    जइ((first > 'Z') || (first == 'J'))
      प्रत्यागम -1; /* no such tz */
    प्रत्यागम tzone[first - 'A'] * 60;
  }
  अण्णहा जइ(len == 2) {
    /* two-letter names */
    जइ(check[1] == 'T') {
      जइ(first == 'N')
        प्रत्यागम 11 * 60 * 60;
      अण्णहा जइ(first == 'U')
        प्रत्यागम 0;
    }
    प्रत्यागम -1; /* nope */
  }
  अण्णहा जइ(len == 3) {
    /* three-letter name */
    ध्रुव संरचना tzinfo *what;
    संरचना tzinfo find;
    जइ(first > 'Y')
      प्रत्यागम -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   परिमाण(tzthree[0]), tzcompare);
    जइ(what)
      प्रत्यागम what->offset * 60;
  }
  अण्णहा {
    /* four-letter name */
    ध्रुव संरचना tzinfo *what;
    संरचना tzinfo find;
    जइ(first > 'W')
      प्रत्यागम -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   परिमाण(tzfoura[0]), tzcompare);
    जइ(what)
      प्रत्यागम what->offset * 60;
  }
  प्रत्यागम -1;
}

स्थिर शून्य skip(ध्रुव वर्ण **date)
{
  /* skip everything that are not letters or digits */
  जाव(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
संरचना when {
  पूर्णाङ्क wday;  /* day of the week, 0-6 (mon-sun) */
  पूर्णाङ्क mon;   /* month of the year, 0-11 */
  पूर्णाङ्क mday;  /* day of month, 1 - 31 */
  पूर्णाङ्क dsecs; /* number of seconds into the day */
  पूर्णाङ्क year;  /* year, >= 1583 */
  पूर्णाङ्क tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
स्थिर curl_off_t time2epoch(संरचना when *w)
{
  स्थिर ध्रुव पूर्णाङ्क cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  पूर्णाङ्क y = w->year - (w->mon <= 1);
  पूर्णाङ्क leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  प्रत्यागम (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
स्थिर पूर्णाङ्क oneortwodigit(ध्रुव वर्ण *date, ध्रुव वर्ण **endp)
{
  पूर्णाङ्क अङ्कः = date[0] - '0';
  जइ(ISDIGIT(date[1])) {
    *endp = &date[2];
    प्रत्यागम (अङ्कः * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  प्रत्यागम अङ्कः;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
स्थिर bool match_time(ध्रुव वर्ण *date, संरचना when *w, वर्ण **endp)
{
  ध्रुव वर्ण *p;
  पूर्णाङ्क hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  जइ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    जइ(mm < 60) {
      जइ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        जइ(ss <= 60) {
          /* valid HH:MM:SS */
          गच्छ match;
        }
      }
      अण्णहा {
        /* valid HH:MM */
        गच्छ match;
      }
    }
  }
  प्रत्यागम FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (वर्ण *)CURL_UNCONST(p);
  प्रत्यागम TRUE;
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

स्थिर शून्य initwhen(संरचना when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

स्थिर पूर्णाङ्क datestring(ध्रुव वर्ण **datep, संरचना when *w)
{
  /* a name coming up */
  size_t len = 0;
  ध्रुव वर्ण *p = *datep;
  जाव(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  जइ(len != NAME_LEN) {
    जइ(w->wday == -1) {
      w->wday = checkday(p, len);
      जइ(w->wday != -1)
        गच्छ found;
    }
    जइ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      जइ(w->mon != -1)
        गच्छ found;
    }
    जइ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      जइ(w->tzoff != -1)
        गच्छ found;
    }
  }
  प्रत्यागम PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  प्रत्यागम PARSEDATE_OK;
}

स्थिर पूर्णाङ्क datenum(ध्रुव वर्ण *indate, ध्रुव वर्ण **datep, संरचना when *w)
{
  /* a digit */
  unsigned पूर्णाङ्क val;
  वर्ण *end;
  ध्रुव वर्ण *date = *datep;

  जइ((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  अण्णहा {
    bool found = FALSE;
    curl_off_t lval;
    पूर्णाङ्क अङ्कः_digits = 0;
    ध्रुव वर्ण *p = *datep;
    जइ(curlx_str_सङ्ख्या(&p, &lval, 99999999))
      प्रत्यागम PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    अङ्कः_digits = (पूर्णाङ्क)(p - *datep);
    val = (unsigned पूर्णाङ्क)lval;

    जइ((w->tzoff == -1) &&
       (अङ्कः_digits == 4) &&
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

    अण्णहा जइ((अङ्कः_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      जइ(w->mon > 11)
        प्रत्यागम PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    जइ(!found && (w->mday == -1)) {
      जइ((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    जइ(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      जइ(w->year < 100) {
        जइ(w->year > 70)
          w->year += 1900;
        अण्णहा
          w->year += 2000;
      }
    }

    जइ(!found)
      प्रत्यागम PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  प्रत्यागम PARSEDATE_OK;
}

स्थिर bool datecheck(संरचना when *w)
{
  जइ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  जइ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    प्रत्यागम TRUE;

  प्रत्यागम FALSE;
}

स्थिर शून्य tzadjust(curl_off_t *tp, संरचना when *w)
{
  जइ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  जइ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  अण्णहा
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

स्थिर पूर्णाङ्क mktimet(curl_off_t seconds, time_t *output)
{
#जइ SIZEOF_TIME_T < 5
  जइ(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    प्रत्यागम PARSEDATE_LATER;
  }
  अण्णहा जइ(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    प्रत्यागम PARSEDATE_SOONER;
  }
#नोचेद्यदि defined(HAVE_TIME_T_UNSIGNED)
  जइ(seconds < 0) {
    *output = 0;
    प्रत्यागम PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  प्रत्यागम PARSEDATE_OK;
}

स्थिर पूर्णाङ्क parsedate(ध्रुव वर्ण *date, time_t *output)
{
  curl_off_t seconds = 0;
  ध्रुव वर्ण *indate = date; /* save the original pointer */
  पूर्णाङ्क part = 0; /* max 6 parts */
  पूर्णाङ्क rc = 0;
  संरचना when w;
  initwhen(&w);

  जाव(*date && (part < 6)) {
    skip(&date);

    जइ(ISALPHA(*date))
      rc = datestring(&date, &w);
    अण्णहा जइ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    जइ(rc)
      प्रत्यागम rc;

    part++;
  }

  जइ(datecheck(&w))
    प्रत्यागम PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  प्रत्यागम rc;
}
#अण्णहा
/* disabled */
स्थिर पूर्णाङ्क parsedate(ध्रुव वर्ण *date, time_t *output)
{
  (शून्य)date;
  *output = 0;
  प्रत्यागम PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(ध्रुव वर्ण *p, ध्रुव time_t *unused)
{
  time_t parsed = -1;
  पूर्णाङ्क rc;
  (शून्य)unused; /* legacy argument from the past that we ignore */

  जइ(!p)
    प्रत्यागम -1;

  rc = parsedate(p, &parsed);
  जइ(rc == PARSEDATE_OK) {
    जइ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    प्रत्यागम parsed;
  }
  /* everything else is fail */
  प्रत्यागम -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

पूर्णाङ्क Curl_getdate_capped(ध्रुव वर्ण *p, time_t *tp)
{
  पूर्णाङ्क rc = parsedate(p, tp);
  प्रत्यागम (rc == PARSEDATE_FAIL);
}
