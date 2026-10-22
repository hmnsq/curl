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
#অন্তর্ভাব "curl_setup.h"

#অন্তর্ভাব "parsedate.h"
#অন্তর্ভাব "curlx/strparse.h"
#অন্তর্ভাব "curlx/strcopy.h"

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

#যদি !defined(CURL_DISABLE_PARSEDATE) || !defined(CURL_DISABLE_FTP) || \
  !defined(CURL_DISABLE_FILE) || defined(USE_GNUTLS)
/* These names are also used by FTP and FILE code */
ধ্রুব বর্ণ * ধ্রুব Curl_wkday[] = {
  "Mon", "Tue", "Wed", "Thu", "Fri", "Sat", "Sun"
};
ধ্রুব বর্ণ * ধ্রুব Curl_month[] = {
  "Jan", "Feb", "Mar", "Apr", "May", "Jun",
  "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
#endif

#define PARSEDATE_OK     0
#define PARSEDATE_FAIL   (-1)

#ifndef CURL_DISABLE_PARSEDATE

#যদি SIZEOF_TIME_T < 5
#define PARSEDATE_LATER  1
#endif
#যদি SIZEOF_TIME_T < 5 || defined(HAVE_TIME_T_UNSIGNED)
#define PARSEDATE_SOONER 2
#endif

সংরচনা tzinfo {
  uint32_t tz;
  int16_t offset; /* +/- in minutes */
};

#define DST (-60)     /* offset for daylight savings time */

/* alpha-sorted list of single-letter time zones, A - Z */
স্থির ধ্রুব int16_t tzone[] = {
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
স্থির ধ্রুব সংরচনা tzinfo tzthree[] = {
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
স্থির ধ্রুব সংরচনা tzinfo tzfoura[] = {
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
UNITTEST পূর্ণাঙ্ক checkday(ধ্রুব বর্ণ *check, size_t len);
UNITTEST পূর্ণাঙ্ক checkday(ধ্রুব বর্ণ *check, size_t len)
{
  পূর্ণাঙ্ক day = -1;
  যদি(len < 3)
    প্রত্যাগম -1; /* too short */

  বিকল্প(LOWERCASE(check[1])) {
  পক্ষ 'o': /* monday */
    যদি((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[2]) == 'n')
      day = 0;
    ভঙ্গ;
  পক্ষ 'u': /* tuesday or sunday */
    যদি((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'e')
      day = 1;
    অন্যথা যদি((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 'n')
      day = 6;
    ভঙ্গ;
  পক্ষ 'e': /* wednesday */
    যদি((LOWERCASE(check[0]) == 'w') && LOWERCASE(check[2]) == 'd')
      day = 2;
    ভঙ্গ;
  পক্ষ 'h': /* thursday */
    যদি((LOWERCASE(check[0]) == 't') && LOWERCASE(check[2]) == 'u')
      day = 3;
    ভঙ্গ;
  পক্ষ 'r': /* friday */
    যদি((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[2]) == 'i')
      day = 4;
    ভঙ্গ;
  পক্ষ 'a': /* saturday */
    যদি((LOWERCASE(check[0]) == 's') && LOWERCASE(check[2]) == 't')
      day = 5;
    ভঙ্গ;
  }
  যদি((len > 3) && (day != -1)) {
    /* when more than three letters are provided, verify the tail name case
       insensitively */
    স্থির ধ্রুব বর্ণ * ধ্রুব daysuffix[] = {
      /* without the leading three letters */
      "day", "sday", "nesday", "rsday", "day", "urday", "day"
    };

    size_t wlen = strlen(daysuffix[day]);
    যদি(((len - 3) != wlen) ||
       !curl_strnequal(&check[3], daysuffix[day], wlen))
      প্রত্যাগম -1;
  }
  প্রত্যাগম day;
}

/* @unittest 4781 */

UNITTEST পূর্ণাঙ্ক checkmonth(ধ্রুব বর্ণ *check, size_t len);
UNITTEST পূর্ণাঙ্ক checkmonth(ধ্রুব বর্ণ *check, size_t len)
{
  যদি(len != 3)
    প্রত্যাগম -1; /* not a month */

  বিকল্প(LOWERCASE(check[2])) {
  পক্ষ 'n': /* jan, jun */
    যদি(LOWERCASE(check[0]) == 'j') {
      uint8_t c2 = LOWERCASE(check[1]);
      যদি(c2 == 'a')
        প্রত্যাগম 0;
      অন্যথা যদি(c2 == 'u')
        প্রত্যাগম 5;
    }
    ভঙ্গ;
  পক্ষ 'b': /* feb */
    যদি((LOWERCASE(check[0]) == 'f') && LOWERCASE(check[1]) == 'e')
      প্রত্যাগম 1;
    ভঙ্গ;
  পক্ষ 'r': /* mar, apr */
    যদি(LOWERCASE(check[0]) == 'm') {
      যদি(LOWERCASE(check[1]) == 'a')
        প্রত্যাগম 2;
    }
    অন্যথা যদি((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'p')
      প্রত্যাগম 3;
    ভঙ্গ;
  পক্ষ 'y': /* may */
    যদি((LOWERCASE(check[0]) == 'm') && LOWERCASE(check[1]) == 'a')
      প্রত্যাগম 4;
    ভঙ্গ;
  পক্ষ 'l': /* jul */
    যদি((LOWERCASE(check[0]) == 'j') && LOWERCASE(check[1]) == 'u')
      প্রত্যাগম 6;
    ভঙ্গ;
  পক্ষ 'g': /* aug */
    যদি((LOWERCASE(check[0]) == 'a') && LOWERCASE(check[1]) == 'u')
      প্রত্যাগম 7;
    ভঙ্গ;
  পক্ষ 'p': /* sep */
    যদি((LOWERCASE(check[0]) == 's') && LOWERCASE(check[1]) == 'e')
      প্রত্যাগম 8;
    ভঙ্গ;
  পক্ষ 't': /* oct */
    যদি((LOWERCASE(check[0]) == 'o') && LOWERCASE(check[1]) == 'c')
      প্রত্যাগম 9;
    ভঙ্গ;
  পক্ষ 'v': /* nov */
    যদি((LOWERCASE(check[0]) == 'n') && LOWERCASE(check[1]) == 'o')
      প্রত্যাগম 10;
    ভঙ্গ;
  পক্ষ 'c': /* dec */
    যদি((LOWERCASE(check[0]) == 'd') && LOWERCASE(check[1]) == 'e')
      প্রত্যাগম 11;
    ভঙ্গ;
  }
  প্রত্যাগম -1; /* return the offset or -1, no real offset is -1 */
}

স্থির পূর্ণাঙ্ক tzcompare(ধ্রুব শূন্য *m1, ধ্রুব শূন্য *m2)
{
  ধ্রুব সংরচনা tzinfo *tz1 = m1;
  ধ্রুব সংরচনা tzinfo *tz2 = m2;
  প্রত্যাগম tz1->tz - tz2->tz;
}

/* return the time zone offset between GMT and the input one, in number of
   seconds or -1 if the timezone was not found/legal

   @unittest 4781
*/
UNITTEST পূর্ণাঙ্ক checktz(ধ্রুব বর্ণ *check, size_t len);
UNITTEST পূর্ণাঙ্ক checktz(ধ্রুব বর্ণ *check, size_t len)
{
  বর্ণ first;
  যদি(len > 4)
    প্রত্যাগম -1;
  first = check[0];
  যদি(first < 'A')
    প্রত্যাগম -1;
  যদি(len == 1) {
    /* short-cut single letter names */
    যদি((first > 'Z') || (first == 'J'))
      প্রত্যাগম -1; /* no such tz */
    প্রত্যাগম tzone[first - 'A'] * 60;
  }
  অন্যথা যদি(len == 2) {
    /* two-letter names */
    যদি(check[1] == 'T') {
      যদি(first == 'N')
        প্রত্যাগম 11 * 60 * 60;
      অন্যথা যদি(first == 'U')
        প্রত্যাগম 0;
    }
    প্রত্যাগম -1; /* nope */
  }
  অন্যথা যদি(len == 3) {
    /* three-letter name */
    ধ্রুব সংরচনা tzinfo *what;
    সংরচনা tzinfo find;
    যদি(first > 'Y')
      প্রত্যাগম -1;
    find.tz = MKTZ(first, check[1], check[2], 0);
    what = bsearch(&find, tzthree, CURL_ARRAYSIZE(tzthree),
                   পরিমাণ(tzthree[0]), tzcompare);
    যদি(what)
      প্রত্যাগম what->offset * 60;
  }
  অন্যথা {
    /* four-letter name */
    ধ্রুব সংরচনা tzinfo *what;
    সংরচনা tzinfo find;
    যদি(first > 'W')
      প্রত্যাগম -1;
    find.tz = MKTZ(first, check[1], check[2], check[3]);
    what = bsearch(&find, tzfoura, CURL_ARRAYSIZE(tzfoura),
                   পরিমাণ(tzfoura[0]), tzcompare);
    যদি(what)
      প্রত্যাগম what->offset * 60;
  }
  প্রত্যাগম -1;
}

স্থির শূন্য skip(ধ্রুব বর্ণ **date)
{
  /* skip everything that are not letters or digits */
  যাবত্(**date && !ISALNUM(**date))
    (*date)++;
}

/* each field is exactly -1 when unknown */
সংরচনা when {
  পূর্ণাঙ্ক wday;  /* day of the week, 0-6 (mon-sun) */
  পূর্ণাঙ্ক mon;   /* month of the year, 0-11 */
  পূর্ণাঙ্ক mday;  /* day of month, 1 - 31 */
  পূর্ণাঙ্ক dsecs; /* number of seconds into the day */
  পূর্ণাঙ্ক year;  /* year, >= 1583 */
  পূর্ণাঙ্ক tzoff; /* time zone offset in seconds */
};

/* (1969 / 4) - (1969 / 100) + (1969 / 400) = 492 - 19 + 4 = 477 */
#define LEAP_DAYS_BEFORE_1969 477
#define SECONDS_PER_DAY 86400

/*
 * time2epoch: time stamp to seconds since epoch in GMT time zone. Similar to
 * mktime but for GMT only.
 */
স্থির curl_off_t time2epoch(সংরচনা when *w)
{
  স্থির ধ্রুব পূর্ণাঙ্ক cumulative_days[12] = {
    0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334
  };
  পূর্ণাঙ্ক y = w->year - (w->mon <= 1);
  পূর্ণাঙ্ক leap_days = (y / 4) - (y / 100) + (y / 400) - LEAP_DAYS_BEFORE_1969;
  curl_off_t days = (curl_off_t)(w->year - 1970) * 365 + leap_days +
    cumulative_days[w->mon] + w->mday - 1;

  প্রত্যাগম (days * SECONDS_PER_DAY) + w->dsecs;
}

/* Returns the value of a single-digit or two-digit decimal number, return
   then pointer to after the number. The 'date' pointer is known to point to a
   digit. */
স্থির পূর্ণাঙ্ক oneortwodigit(ধ্রুব বর্ণ *date, ধ্রুব বর্ণ **endp)
{
  পূর্ণাঙ্ক অঙ্ক = date[0] - '0';
  যদি(ISDIGIT(date[1])) {
    *endp = &date[2];
    প্রত্যাগম (অঙ্ক * 10) + (date[1] - '0');
  }
  *endp = &date[1];
  প্রত্যাগম অঙ্ক;
}

/* HH:MM:SS or HH:MM and accept single-digits too */
স্থির bool match_time(ধ্রুব বর্ণ *date, সংরচনা when *w, বর্ণ **endp)
{
  ধ্রুব বর্ণ *p;
  পূর্ণাঙ্ক hh, mm, ss = 0;
  hh = oneortwodigit(date, &p);
  যদি((hh < 24) && (*p == ':') && ISDIGIT(p[1])) {
    mm = oneortwodigit(&p[1], &p);
    যদি(mm < 60) {
      যদি((*p == ':') && ISDIGIT(p[1])) {
        ss = oneortwodigit(&p[1], &p);
        যদি(ss <= 60) {
          /* valid HH:MM:SS */
          গচ্ছ match;
        }
      }
      অন্যথা {
        /* valid HH:MM */
        গচ্ছ match;
      }
    }
  }
  প্রত্যাগম FALSE; /* not a time string */
match:
  w->dsecs = (hh * 3600) + (mm * 60) + ss;
  *endp = (বর্ণ *)CURL_UNCONST(p);
  প্রত্যাগম TRUE;
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

স্থির শূন্য initwhen(সংরচনা when *w)
{
  w->wday = w->mon = w->mday = w->dsecs = w->year = w->tzoff = -1;
}

স্থির পূর্ণাঙ্ক datestring(ধ্রুব বর্ণ **datep, সংরচনা when *w)
{
  /* a name coming up */
  size_t len = 0;
  ধ্রুব বর্ণ *p = *datep;
  যাবত্(ISALPHA(p[len]) && (len < NAME_LEN))
    len++;

  যদি(len != NAME_LEN) {
    যদি(w->wday == -1) {
      w->wday = checkday(p, len);
      যদি(w->wday != -1)
        গচ্ছ found;
    }
    যদি(w->mon == -1) {
      w->mon = checkmonth(p, len);
      যদি(w->mon != -1)
        গচ্ছ found;
    }
    যদি(w->tzoff == -1) {
      /* this must be a time zone string */
      w->tzoff = checktz(p, len);
      যদি(w->tzoff != -1)
        গচ্ছ found;
    }
  }
  প্রত্যাগম PARSEDATE_FAIL; /* bad string */
found:
  *datep += len;
  প্রত্যাগম PARSEDATE_OK;
}

স্থির পূর্ণাঙ্ক datenum(ধ্রুব বর্ণ *indate, ধ্রুব বর্ণ **datep, সংরচনা when *w)
{
  /* a digit */
  unsigned পূর্ণাঙ্ক val;
  বর্ণ *end;
  ধ্রুব বর্ণ *date = *datep;

  যদি((w->dsecs == -1) && match_time(date, w, &end)) {
    /* time stamp */
    date = end;
  }
  অন্যথা {
    bool found = FALSE;
    curl_off_t lval;
    পূর্ণাঙ্ক অঙ্ক_digits = 0;
    ধ্রুব বর্ণ *p = *datep;
    যদি(curlx_str_সংখ্যা(&p, &lval, 99999999))
      প্রত্যাগম PARSEDATE_FAIL;

    /* we know num_digits cannot be larger than 8 */
    অঙ্ক_digits = (পূর্ণাঙ্ক)(p - *datep);
    val = (unsigned পূর্ণাঙ্ক)lval;

    যদি((w->tzoff == -1) &&
       (অঙ্ক_digits == 4) &&
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

    অন্যথা যদি((অঙ্ক_digits == 8) && (w->year == -1) &&
            (w->mon == -1) && (w->mday == -1)) {
      /* 8 digits, no year, month or day yet. This is YYYYMMDD */
      found = TRUE;
      w->year = val / 10000;
      w->mon = ((val % 10000) / 100) - 1; /* month is 0 - 11 */
      যদি(w->mon > 11)
        প্রত্যাগম PARSEDATE_FAIL;
      w->mday = val % 100;
    }

    যদি(!found && (w->mday == -1)) {
      যদি((val > 0) && (val < 32)) {
        w->mday = val;
        found = TRUE;
      }
    }

    যদি(!found && (w->year == -1)) {
      w->year = val;
      found = TRUE;
      যদি(w->year < 100) {
        যদি(w->year > 70)
          w->year += 1900;
        অন্যথা
          w->year += 2000;
      }
    }

    যদি(!found)
      প্রত্যাগম PARSEDATE_FAIL;

    date = p;
  }
  *datep = date;
  প্রত্যাগম PARSEDATE_OK;
}

স্থির bool datecheck(সংরচনা when *w)
{
  যদি(w->dsecs == -1)
    w->dsecs = 0; /* no time available, make it zero */

  যদি((w->mday == -1) || (w->mon == -1) || (w->year == -1) ||
    /* lacks vital info, fail */
     (w->year < 1583) ||
     /* The Gregorian calendar was introduced 1582 */
     (w->mday > 31) /* too long month */ )
    প্রত্যাগম TRUE;

  প্রত্যাগম FALSE;
}

স্থির শূন্য tzadjust(curl_off_t *tp, সংরচনা when *w)
{
  যদি(w->tzoff == -1) /* unknown tz means no offset */
    w->tzoff = 0;

  /* Add the time zone diff between local time zone and GMT. */
  যদি((w->tzoff > 0) && (*tp > (curl_off_t)(CURL_OFF_T_MAX - w->tzoff)))
    *tp = CURL_OFF_T_MAX;
  অন্যথা
    *tp += w->tzoff;
  /* this needs no minimum check since we require a year > 1582 */
}

স্থির পূর্ণাঙ্ক mktimet(curl_off_t seconds, time_t *output)
{
#যদি SIZEOF_TIME_T < 5
  যদি(seconds > TIME_T_MAX) {
    *output = TIME_T_MAX;
    প্রত্যাগম PARSEDATE_LATER;
  }
  অন্যথা যদি(seconds < TIME_T_MIN) {
    *output = TIME_T_MIN;
    প্রত্যাগম PARSEDATE_SOONER;
  }
#নোচেদ্যদি defined(HAVE_TIME_T_UNSIGNED)
  যদি(seconds < 0) {
    *output = 0;
    প্রত্যাগম PARSEDATE_SOONER;
  }
#endif
  *output = (time_t)seconds;
  প্রত্যাগম PARSEDATE_OK;
}

স্থির পূর্ণাঙ্ক parsedate(ধ্রুব বর্ণ *date, time_t *output)
{
  curl_off_t seconds = 0;
  ধ্রুব বর্ণ *indate = date; /* save the original pointer */
  পূর্ণাঙ্ক part = 0; /* max 6 parts */
  পূর্ণাঙ্ক rc = 0;
  সংরচনা when w;
  initwhen(&w);

  যাবত্(*date && (part < 6)) {
    skip(&date);

    যদি(ISALPHA(*date))
      rc = datestring(&date, &w);
    অন্যথা যদি(ISDIGIT(*date))
      rc = datenum(indate, &date, &w);
    যদি(rc)
      প্রত্যাগম rc;

    part++;
  }

  যদি(datecheck(&w))
    প্রত্যাগম PARSEDATE_FAIL;

  seconds = time2epoch(&w); /* get number of seconds */
  tzadjust(&seconds, &w); /* handle the time zone offset */
  rc = mktimet(seconds, output); /* squeeze seconds into a time_t */

  প্রত্যাগম rc;
}
#অন্যথা
/* disabled */
স্থির পূর্ণাঙ্ক parsedate(ধ্রুব বর্ণ *date, time_t *output)
{
  (শূন্য)date;
  *output = 0;
  প্রত্যাগম PARSEDATE_OK; /* a lie */
}
#endif

time_t curl_getdate(ধ্রুব বর্ণ *p, ধ্রুব time_t *unused)
{
  time_t parsed = -1;
  পূর্ণাঙ্ক rc;
  (শূন্য)unused; /* legacy argument from the past that we ignore */

  যদি(!p)
    প্রত্যাগম -1;

  rc = parsedate(p, &parsed);
  যদি(rc == PARSEDATE_OK) {
    যদি(parsed == (time_t)-1)
      /* avoid returning -1 for a working scenario */
      parsed++;
    প্রত্যাগম parsed;
  }
  /* everything else is fail */
  প্রত্যাগম -1;
}

/* Curl_getdate_capped() differs from curl_getdate() in that this will return
   TIME_T_MAX in case the parsed time value was too big, instead of an
   error. Returns non-zero on error. */

পূর্ণাঙ্ক Curl_getdate_capped(ধ্রুব বর্ণ *p, time_t *tp)
{
  পূর্ণাঙ্ক rc = parsedate(p, tp);
  প্রত্যাগম (rc == PARSEDATE_FAIL);
}
