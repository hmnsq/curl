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
#അന്തര്ഭാവ "curl_setup.h"

#അന്തര്ഭാവ "parsedate.h"
#അന്തര്ഭാവ "curlx/strparse.h"
#അന്തര്ഭാവ "curlx/strcopy.h"

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

#എങ്കിൽ !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
സ്ഥിരം അക്ഷരം * സ്ഥിരം Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
സ്ഥിരം അക്ഷരം * സ്ഥിരം Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#എങ്കിൽ SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#എങ്കിൽ SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

ഘടന tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
സ്ഥിതം സ്ഥിരം int16_t tzone[] = {
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
സ്ഥിതം സ്ഥിരം ഘടന tzinfo tzthree[] = {
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
സ്ഥിതം സ്ഥിരം ഘടന tzinfo tzfoura[] = {
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
UNITTEST പൂർണ്ണസംഖ്യ checkday(സ്ഥിരം അക്ഷരം *check, size_t len);
UNITTEST പൂർണ്ണസംഖ്യ checkday(സ്ഥിരം അക്ഷരം *check, size_t len)
{
  പൂർണ്ണസംഖ്യ day = -1;
  എങ്കിൽ(len < 3)
    തിരികെ -1; /* too short */

  തിരഞ്ഞെടുപ്പ്(LOWERCASE(check[1])) {
  അവസ്ഥ 'o': /* monday */
    എങ്കിൽ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    നിർത്തുക;
  അവസ്ഥ 'u': /* tuesday or sunday */
    എങ്കിൽ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    അല്ലെങ്കിൽ എങ്കിൽ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    നിർത്തുക;
  അവസ്ഥ 'e': /* wednesday */
    എങ്കിൽ((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    നിർത്തുക;
  അവസ്ഥ 'h': /* thursday */
    എങ്കിൽ((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    നിർത്തുക;
  അവസ്ഥ 'r': /* friday */
    എങ്കിൽ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    നിർത്തുക;
  അവസ്ഥ 'a': /* saturday */
    എങ്കിൽ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    നിർത്തുക;
  }
  എങ്കിൽ((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    എങ്കിൽ(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      തിരികെ -1;
  }
  തിരികെ day;
}

/* @unittest 4781 */

UNITTEST പൂർണ്ണസംഖ്യ checkmonth(സ്ഥിരം അക്ഷരം *check, size_t len);
UNITTEST പൂർണ്ണസംഖ്യ checkmonth(സ്ഥിരം അക്ഷരം *check, size_t len)
{
  എങ്കിൽ(len != 3)
    തിരികെ -1; /* not a month */

  തിരഞ്ഞെടുപ്പ്(LOWERCASE(check[2])) {
  അവസ്ഥ 'n': /* jan, jun */
    എങ്കിൽ(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      എങ്കിൽ(c2 == 'a')
        തിരികെ 0;
      അല്ലെങ്കിൽ എങ്കിൽ(c2 == 'u')
        തിരികെ 5;
    }
    നിർത്തുക;
  അവസ്ഥ 'b': /* feb */
    എങ്കിൽ((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      തിരികെ 1;
    നിർത്തുക;
  അവസ്ഥ 'r': /* mar, apr */
    എങ്കിൽ(LOWERCASE(check[0]) == 'm') {
      എങ്കിൽ(LOWERCASE(check[1]) == 'a')
        തിരികെ 2;
    }
    അല്ലെങ്കിൽ എങ്കിൽ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      തിരികെ 3;
    നിർത്തുക;
  അവസ്ഥ 'y': /* may */
    എങ്കിൽ((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      തിരികെ 4;
    നിർത്തുക;
  അവസ്ഥ 'l': /* jul */
    എങ്കിൽ((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      തിരികെ 6;
    നിർത്തുക;
  അവസ്ഥ 'g': /* aug */
    എങ്കിൽ((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      തിരികെ 7;
    നിർത്തുക;
  അവസ്ഥ 'p': /* sep */
    എങ്കിൽ((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      തിരികെ 8;
    നിർത്തുക;
  അവസ്ഥ 't': /* oct */
    എങ്കിൽ((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      തിരികെ 9;
    നിർത്തുക;
  അവസ്ഥ 'v': /* nov */
    എങ്കിൽ((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      തിരികെ 10;
    നിർത്തുക;
  അവസ്ഥ 'c': /* dec */
    എങ്കിൽ((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      തിരികെ 11;
    നിർത്തുക;
  }
  തിരികെ -1; /* return the offset or -1, no real offset is -1 */
}

സ്ഥിതം പൂർണ്ണസംഖ്യ tzcompare(സ്ഥിരം ശൂന്യം *m1, സ്ഥിരം ശൂന്യം *m2)
{
  സ്ഥിരം ഘടന tzinfo *tz1 = m1;
  സ്ഥിരം ഘടന tzinfo *tz2 = m2;
  തിരികെ tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST പൂർണ്ണസംഖ്യ checktz(സ്ഥിരം അക്ഷരം *check, size_t len);
UNITTEST പൂർണ്ണസംഖ്യ checktz(സ്ഥിരം അക്ഷരം *check, size_t len)
{
  അക്ഷരം first;
  എങ്കിൽ(len > 4)
    തിരികെ -1;
  first = check[0];
  എങ്കിൽ(first < 'A')
    തിരികെ -1;
  എങ്കിൽ(len == 1) {
    /* short-cut single letter names */
    എങ്കിൽ((first > 'Z') || (first == 'J'))
      തിരികെ -1; /* no such tz */
    തിരികെ tzone[first - 'A'] * 60;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(len == 2) {
    /* two-letter names */
    എങ്കിൽ(check[1] == 'T') {
      എങ്കിൽ(first == 'N')
        തിരികെ 11 * 60 * 60;
      അല്ലെങ്കിൽ എങ്കിൽ(first == 'U')
        തിരികെ 0;
    }
    തിരികെ -1; /* nope */
  }
  അല്ലെങ്കിൽ എങ്കിൽ(len == 3) {
    /* three-letter name */
    സ്ഥിരം ഘടന tzinfo *what;
    ഘടന tzinfo find;
    എങ്കിൽ(first > 'Y')
      തിരികെ -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   വലുപ്പം(tzthree[0]), tzcompare);
    എങ്കിൽ(what)
      തിരികെ what->offset * 60;
  }
  അല്ലെങ്കിൽ {
    /* four-letter name */
    സ്ഥിരം ഘടന tzinfo *what;
    ഘടന tzinfo find;
    എങ്കിൽ(first > 'W')
      തിരികെ -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   വലുപ്പം(tzfoura[0]), tzcompare);
    എങ്കിൽ(what)
      തിരികെ what->offset * 60;
  }
  തിരികെ -1;
}

സ്ഥിതം ശൂന്യം skip(സ്ഥിരം അക്ഷരം **date)
{
  /* skip everything that are not letters or digits */
  ആകുമ്പോൾ(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
ഘടന when {
  പൂർണ്ണസംഖ്യ wday;  /* day of the week, 0-6 (mon-sun) */
  പൂർണ്ണസംഖ്യ mon;   /* month of the year, 0-11 */
  പൂർണ്ണസംഖ്യ mday;  /* day of month, 1 - 31 */
  പൂർണ്ണസംഖ്യ dsecs; /* number of seconds into the day */
  പൂർണ്ണസംഖ്യ year;  /* year, >= 1583 */
  പൂർണ്ണസംഖ്യ tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
സ്ഥിതം curl_off_t time2epoch(ഘടന when *w)
{
  സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  പൂർണ്ണസംഖ്യ y = w->year - (w->mon <= 1);
  പൂർണ്ണസംഖ്യ leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  തിരികെ (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
സ്ഥിതം പൂർണ്ണസംഖ്യ oneortwodigit(സ്ഥിരം അക്ഷരം *date, സ്ഥിരം അക്ഷരം **endp)
{
  പൂർണ്ണസംഖ്യ അക്കം = date[0] - '0';
  എങ്കിൽ(ISDIGIT(date[1])) {
    *endp = &date[2];
    തിരികെ (അക്കം * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  തിരികെ അക്കം;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
സ്ഥിതം bool match_time(സ്ഥിരം അക്ഷരം *date, ഘടന when *w, അക്ഷരം **endp)
{
  സ്ഥിരം അക്ഷരം *p;
  പൂർണ്ണസംഖ്യ hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  എങ്കിൽ((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    എങ്കിൽ(mm < 60) {
      എങ്കിൽ((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        എങ്കിൽ(ss <= 60) {
          /* valid HH:MM:SS */
          പോകുക match;
        }
      }
      അല്ലെങ്കിൽ {
        /* valid HH:MM */
        പോകുക match;
      }
    }
  }
  തിരികെ FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (അക്ഷരം *)CURL_UNCONST(p);
  തിരികെ TRUE;
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

സ്ഥിതം ശൂന്യം initwhen(ഘടന when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ datestring(സ്ഥിരം അക്ഷരം **datep, ഘടന when *w)
{
  /* a name coming up */
  size_t len = 0;
  സ്ഥിരം അക്ഷരം *p = *datep;
  ആകുമ്പോൾ(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  എങ്കിൽ(len != NAME_LEN) {
    എങ്കിൽ(w->wday == -1) {
      w->wday = checkday(p, len);
      എങ്കിൽ(w->wday != -1)
        പോകുക found;
    }
    എങ്കിൽ(w->mon == -1) {
      w->mon = checkmonth(p, len);
      എങ്കിൽ(w->mon != -1)
        പോകുക found;
    }
    എങ്കിൽ(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      എങ്കിൽ(w->tzoff != -1)
        പോകുക found;
    }
  }
  തിരികെ PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  തിരികെ PARSEDATE_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ datenum(സ്ഥിരം അക്ഷരം *indate, സ്ഥിരം അക്ഷരം **datep, ഘടന when *w)
{
  /* a digit */
  unsigned പൂർണ്ണസംഖ്യ val;
  അക്ഷരം *end;
  സ്ഥിരം അക്ഷരം *date = *datep;

  എങ്കിൽ((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  അല്ലെങ്കിൽ {
    bool found = FALSE;
    curl_off_t lval;
    പൂർണ്ണസംഖ്യ അക്കം_digits = 0;
    സ്ഥിരം അക്ഷരം *p = *datep;
    എങ്കിൽ(curlx_str_സംഖ്യ(&p, &lval, 99999999))
      തിരികെ PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    അക്കം_digits = (പൂർണ്ണസംഖ്യ)(p - *datep);
    val = (unsigned പൂർണ്ണസംഖ്യ)lval;

    എങ്കിൽ((w->tzoff == -1) &&
       (അക്കം_digits == 4) &&
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

    അല്ലെങ്കിൽ എങ്കിൽ((അക്കം_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      എങ്കിൽ(w->mon > 11)
        തിരികെ PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    എങ്കിൽ(!found && (w->mday == -1)) {
      എങ്കിൽ((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    എങ്കിൽ(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      എങ്കിൽ(w->year < 100) {
        എങ്കിൽ(w->year > 70)
          w->year += 1900;
        അല്ലെങ്കിൽ
          w->year += 2000;
      }
    }

    എങ്കിൽ(!found)
      തിരികെ PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  തിരികെ PARSEDATE_OK;
}

സ്ഥിതം bool datecheck(ഘടന when *w)
{
  എങ്കിൽ(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  എങ്കിൽ((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    തിരികെ TRUE;

  തിരികെ FALSE;
}

സ്ഥിതം ശൂന്യം tzadjust(curl_off_t *tp, ഘടന when *w)
{
  എങ്കിൽ(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  എങ്കിൽ((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  അല്ലെങ്കിൽ
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

സ്ഥിതം പൂർണ്ണസംഖ്യ mktimet(curl_off_t seconds, time_t *output)
{
#എങ്കിൽ SIZEOF_TIME_T < 5
  എങ്കിൽ(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    തിരികെ PARSEDATE_LATER;
  }
  അല്ലെങ്കിൽ എങ്കിൽ(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    തിരികെ PARSEDATE_SOONER;
  }
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_TIME_T_UNSIGNED)
  എങ്കിൽ(seconds < 0) {
    *output = 0;
    തിരികെ PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  തിരികെ PARSEDATE_OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ parsedate(സ്ഥിരം അക്ഷരം *date, time_t *output)
{
  curl_off_t seconds = 0;
  സ്ഥിരം അക്ഷരം *indate = date; /* save the original pointer */
  പൂർണ്ണസംഖ്യ part = 0; /* max 6 parts */
  പൂർണ്ണസംഖ്യ rc = 0;
  ഘടന when w;
  initwhen(&w);

  ആകുമ്പോൾ(*date && (part < 6)) {
    skip(&date);

    എങ്കിൽ(ISALPHA(*date))
      rc = datestring(&date, &w);
    അല്ലെങ്കിൽ എങ്കിൽ(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    എങ്കിൽ(rc)
      തിരികെ rc;

    part++;
  }

  എങ്കിൽ(datecheck(&w))
    തിരികെ PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  തിരികെ rc;
}
#അല്ലെങ്കിൽ
/* disabled */
സ്ഥിതം പൂർണ്ണസംഖ്യ parsedate(സ്ഥിരം അക്ഷരം *date, time_t *output)
{
  (ശൂന്യം)date;
  *output = 0;
  തിരികെ PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(സ്ഥിരം അക്ഷരം *p, സ്ഥിരം time_t *unused)
{
  time_t parsed = -1;
  പൂർണ്ണസംഖ്യ rc;
  (ശൂന്യം)unused; /* legacy argument from the past that we ignore */

  എങ്കിൽ(!p)
    തിരികെ -1;

  rc = parsedate(p, &parsed);
  എങ്കിൽ(rc == PARSEDATE_OK) {
    എങ്കിൽ(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    തിരികെ parsed;
  }
  /* everything else is fail */
  തിരികെ -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

പൂർണ്ണസംഖ്യ Curl_getdate_capped(സ്ഥിരം അക്ഷരം *p, time_t *tp)
{
  പൂർണ്ണസംഖ്യ rc = parsedate(p, tp);
  തിരികെ (rc == PARSEDATE_FAIL);
}
